/*
        ##########    Copyright (C) 2018 styyle14
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include "devices/ni/MaschineMikroMK3.h"
#include "cabl/comm/Driver.h"
#include "cabl/comm/Transfer.h"
#include "cabl/util/Functions.h"

#include <thread>

#include "cabl/gfx/TextDisplay.h"
#include "gfx/displays/NullCanvas.h"

//!\todo delete debug includes
#include <iomanip>
#include <iostream>

//--------------------------------------------------------------------------------------------------

namespace
{
const uint8_t kMikroMK3_epDisplay = 0x08;
const uint8_t kMikroMK3_epOut = 0x01;
const uint8_t kMikroMK3_epInput = 0x81;
const unsigned kMikroMK3_padThreshold = 200;
} // namespace

//--------------------------------------------------------------------------------------------------

namespace sl
{
namespace cabl
{

MaschineMikroMK3::MaschineMikroMK3() : m_padsStatus(0), m_isDirtyLeds(false)
{
}

void MaschineMikroMK3::initDisplay() const
{
	//!\todo set backlight
	return;
}

void MaschineMikroMK3::init()
{
	initLeds();
	initDisplay();
	m_display.white();
}

bool MaschineMikroMK3::isButtonPressed(Button button_) const noexcept
{
	uint8_t buttonPos = static_cast<uint8_t>(button_);
	return ((m_buttons[buttonPos >> 3] & (1 << (buttonPos % 8))) != 0);
}
bool MaschineMikroMK3::isButtonPressed(const Transfer& transfer_, Button button_) const noexcept
{
	uint8_t buttonPos = static_cast<uint8_t>(button_);
	return ((transfer_[1 + (buttonPos >> 3)] & (1 << (buttonPos % 8))) != 0);
}


Canvas* MaschineMikroMK3::graphicDisplay(size_t displayIndex_)
{
	static NullCanvas s_dummyDisplay;
	if (displayIndex_ > 0)
	{
		return &s_dummyDisplay;
	}

	return &m_display;
}


bool MaschineMikroMK3::tick()
{
	bool success = true;
	if (m_isDirtyLeds)
	{
		success &= sendLeds();
	}
	success &= read();

	return success;
}

bool MaschineMikroMK3::read()
{
	Transfer input;
	if (!readFromDeviceHandle(input, kMikroMK3_epInput))
	{
		return false;
	}
	else if (input && input[0] == 0x01)
	{
		processReport0x01(input);
	}
	else if (input && input[0] == 0x02)
	{
		processPads(input);
	}
	
	return true;
}


Device::Button MaschineMikroMK3::deviceButton(Button btn_) const noexcept
{
#define M_BTN_CASE(idBtn) \
	case Button::idBtn:     \
		return Device::Button::idBtn

	switch (btn_)
	{	
		M_BTN_CASE(NativeInstruments);
		M_BTN_CASE(Star);
		M_BTN_CASE(Search);
		M_BTN_CASE(Volume);
		M_BTN_CASE(Swing);
		M_BTN_CASE(Tempo);
		M_BTN_CASE(PlugIn);
		M_BTN_CASE(Sampling);
		M_BTN_CASE(BrowseLeft);
		M_BTN_CASE(BrowseRight);
		M_BTN_CASE(Pitch);
		M_BTN_CASE(Mod);
		M_BTN_CASE(Perform);
		M_BTN_CASE(Notes);
		M_BTN_CASE(Group);
		M_BTN_CASE(Auto);
		M_BTN_CASE(Lock);
		M_BTN_CASE(NoteRepeat);
		M_BTN_CASE(Restart);
		M_BTN_CASE(Erase);
		M_BTN_CASE(Tap);
		M_BTN_CASE(Follow);
		M_BTN_CASE(Play);
		M_BTN_CASE(Rec);
		M_BTN_CASE(Stop);
		M_BTN_CASE(Shift);
		M_BTN_CASE(FixedVel);
		M_BTN_CASE(MM3PadMode);
		M_BTN_CASE(Keyboard);
		M_BTN_CASE(Chords);
		M_BTN_CASE(Step);
		M_BTN_CASE(Scene);
		M_BTN_CASE(Pattern);
		M_BTN_CASE(Events);
		M_BTN_CASE(Variation);
		M_BTN_CASE(Duplicate);
		M_BTN_CASE(Select);
		M_BTN_CASE(Solo);
		M_BTN_CASE(Mute);
		M_BTN_CASE(MainEncoder);
		M_BTN_CASE(TouchEncoderMain);
		default:
		{
			return Device::Button::Unknown;
		}
	}
}

MaschineMikroMK3::Led MaschineMikroMK3::led(Device::Button btn_) const noexcept
{
#define M_DEV_BTN_CASE(idBtn) \
	case Device::Button::idBtn:     \
		return Led::idBtn

	switch (btn_)
	{	
		M_DEV_BTN_CASE(NativeInstruments);
		M_DEV_BTN_CASE(Star);
		M_DEV_BTN_CASE(Search);
		M_DEV_BTN_CASE(Volume);
		M_DEV_BTN_CASE(Swing);
		M_DEV_BTN_CASE(Tempo);
		M_DEV_BTN_CASE(PlugIn);
		M_DEV_BTN_CASE(Sampling);
		M_DEV_BTN_CASE(BrowseLeft);
		M_DEV_BTN_CASE(BrowseRight);
		M_DEV_BTN_CASE(Pitch);
		M_DEV_BTN_CASE(Mod);
		M_DEV_BTN_CASE(Perform);
		M_DEV_BTN_CASE(Notes);
		M_DEV_BTN_CASE(Group);
		M_DEV_BTN_CASE(Auto);
		M_DEV_BTN_CASE(Lock);
		M_DEV_BTN_CASE(NoteRepeat);
		M_DEV_BTN_CASE(Restart);
		M_DEV_BTN_CASE(Erase);
		M_DEV_BTN_CASE(Tap);
		M_DEV_BTN_CASE(Follow);
		M_DEV_BTN_CASE(Play);
		M_DEV_BTN_CASE(Rec);
		M_DEV_BTN_CASE(Stop);
		M_DEV_BTN_CASE(Shift);
		M_DEV_BTN_CASE(FixedVel);
		M_DEV_BTN_CASE(MM3PadMode);
		M_DEV_BTN_CASE(Keyboard);
		M_DEV_BTN_CASE(Chords);
		M_DEV_BTN_CASE(Step);
		M_DEV_BTN_CASE(Scene);
		M_DEV_BTN_CASE(Pattern);
		M_DEV_BTN_CASE(Events);
		M_DEV_BTN_CASE(Variation);
		M_DEV_BTN_CASE(Duplicate);
		M_DEV_BTN_CASE(Select);
		M_DEV_BTN_CASE(Solo);
		M_DEV_BTN_CASE(Mute);
		M_DEV_BTN_CASE(Pad13);
		M_DEV_BTN_CASE(Pad14);
		M_DEV_BTN_CASE(Pad15);
		M_DEV_BTN_CASE(Pad16);
		M_DEV_BTN_CASE(Pad9);
		M_DEV_BTN_CASE(Pad10);
		M_DEV_BTN_CASE(Pad11);
		M_DEV_BTN_CASE(Pad12);
		M_DEV_BTN_CASE(Pad5);
		M_DEV_BTN_CASE(Pad6);
		M_DEV_BTN_CASE(Pad7);
		M_DEV_BTN_CASE(Pad8);
		M_DEV_BTN_CASE(Pad1);
		M_DEV_BTN_CASE(Pad2);
		M_DEV_BTN_CASE(Pad3);
		M_DEV_BTN_CASE(Pad4);
		M_DEV_BTN_CASE(SmartstripLed01);
		M_DEV_BTN_CASE(SmartstripLed02);
		M_DEV_BTN_CASE(SmartstripLed03);
		M_DEV_BTN_CASE(SmartstripLed04);
		M_DEV_BTN_CASE(SmartstripLed05);
		M_DEV_BTN_CASE(SmartstripLed06);
		M_DEV_BTN_CASE(SmartstripLed07);
		M_DEV_BTN_CASE(SmartstripLed08);
		M_DEV_BTN_CASE(SmartstripLed09);
		M_DEV_BTN_CASE(SmartstripLed10);
		M_DEV_BTN_CASE(SmartstripLed11);
		M_DEV_BTN_CASE(SmartstripLed12);
		M_DEV_BTN_CASE(SmartstripLed13);
		M_DEV_BTN_CASE(SmartstripLed14);
		M_DEV_BTN_CASE(SmartstripLed15);
		M_DEV_BTN_CASE(SmartstripLed16);
		M_DEV_BTN_CASE(SmartstripLed17);
		M_DEV_BTN_CASE(SmartstripLed18);
		M_DEV_BTN_CASE(SmartstripLed19);
		M_DEV_BTN_CASE(SmartstripLed20);
		M_DEV_BTN_CASE(SmartstripLed21);
		M_DEV_BTN_CASE(SmartstripLed22);
		M_DEV_BTN_CASE(SmartstripLed23);
		M_DEV_BTN_CASE(SmartstripLed24);
		M_DEV_BTN_CASE(SmartstripLed25);
		default:
		{
			return Led::Unknown;
		}
	}
}

void MaschineMikroMK3::processButtons(const Transfer& input_)
{
	bool shiftPressed(isButtonPressed(input_, Button::Shift));
	m_buttonStates[static_cast<unsigned>(Button::Shift)] = shiftPressed;
	Device::Button changedButton(Device::Button::Unknown);
	bool buttonPressed(false);

	for (uint8_t btn = 0; btn < kMikroMK3_nButtons; btn++)
	{
		Button currentButton(static_cast<Button>(btn));
		if (currentButton == Button::Shift)
		{
			continue;
		}
		buttonPressed = isButtonPressed(input_, currentButton);
		if (buttonPressed != m_buttonStates[btn])
		{
			m_buttonStates[btn] = buttonPressed;
			changedButton = deviceButton(currentButton);
			if (changedButton != Device::Button::Unknown)
			{
				buttonChanged(changedButton, buttonPressed, shiftPressed);
			}
		}
	}
}

void MaschineMikroMK3::processEncoder(const Transfer& input_)
{
	uint8_t currentEncoderValue = input_.data()[kMikroMK3_messageTypeDataSize + kMikroMK3_buttonsDataSize];
	
	if (m_encoderValue != currentEncoderValue)
	{
		bool valueIncreased = ((m_encoderValue < currentEncoderValue) || ((m_encoderValue == 0x0f) && (currentEncoderValue == 0x00)))
			&& (!((m_encoderValue == 0x0) && (currentEncoderValue == 0x0f)));
		m_encoderValue = currentEncoderValue;
		encoderChanged(0, valueIncreased, m_buttonStates[static_cast<unsigned>(Button::Shift)]);
		if (valueIncreased)
		{
			magic++;
		}
		else
		{
			magic--;
		}
		for(auto& led : m_leds)
		{
			std::cout << "Magic is " << unsigned(magic) << "\n";
			led = magic;
			m_isDirtyLeds = true;
		}
	}
}

void MaschineMikroMK3::processSmartstrip(const Transfer& input_)
{
	uint8_t smartstripOffset= kMikroMK3_messageTypeDataSize + kMikroMK3_buttonsDataSize + kMikroMK3_EncodersDataSize;
	uint16_t timeMs = input_[smartstripOffset] | (input_[smartstripOffset + 1] << 8);
	
	uint16_t touchLeftVal = input_[smartstripOffset + 2] | (input_[smartstripOffset + 3] << 8);
	if (touchLeftVal != 0 && m_touchstripValues[0] != touchLeftVal)
	{
		m_touchstripValues[0] = touchLeftVal;
		controlChanged(0, unsigned(touchLeftVal * 1.275), m_buttonStates[static_cast<uint8_t>(Button::Shift)]);
	}
	uint16_t touchRightVal = input_[smartstripOffset + 4] | (input_[smartstripOffset + 5] << 8);
	if (touchRightVal != 0 && m_touchstripValues[1] != touchRightVal)
	{
		m_touchstripValues[1] = touchRightVal;
		controlChanged(1, unsigned(touchRightVal * 1.275), m_buttonStates[static_cast<uint8_t>(Button::Shift)]);
	}
}

void MaschineMikroMK3::processReport0x01(const Transfer& input_)
{
	processButtons(input_);
	processEncoder(input_);
	processSmartstrip(input_);
}

void MaschineMikroMK3::processPads(const Transfer& input_)
{
	unsigned pad=input_[1];
	unsigned pad_value=((input_[2] & 0x0f) << 8) + input_[3];
	if (pad_value > kMikroMK3_padThreshold)
	{
		m_padsStatus[pad] = true;
		keyChanged(pad, pad_value >> 4, m_buttonStates[static_cast<uint8_t>(Button::Shift)]);
	} else if (m_padsStatus[pad])
	{
		m_padsStatus[pad] = false;
		keyChanged(pad, 0, m_buttonStates[static_cast<uint8_t>(Button::Shift)]);
	}
}

void MaschineMikroMK3::processReport0x02(const Transfer& input_)
{
	processPads(input_);
}

bool MaschineMikroMK3::sendFrame()
{
	uint8_t yOffset = 0;
	for (int chunk = 0; chunk < 4; chunk++, yOffset += 2)
	{
		const uint8_t* ptr = m_display.buffer() + (chunk * 256);
		if (!writeToDeviceHandle(
			Transfer({0xE0, 0x00, 0x00, yOffset, 0x00, 0x80, 0x00, 0x02, 0x00}, ptr, 256),
			kMikroMK3_epDisplay))
		{
			return false;
		}
	}
	m_display.resetDirtyFlags();
	return true;
}

void MaschineMikroMK3::initLeds()
{
	for(auto& led : m_leds)
	{
		led = 0;
	}
	magic=0;
}

bool MaschineMikroMK3::sendLeds()
{
	if (!writeToDeviceHandle(Transfer({0x80}, &m_leds[0], 80), kMikroMK3_epOut))
	{
		return false;
	}
	m_isDirtyLeds = false;
	return true;
}

void MaschineMikroMK3::setButtonLed(Device::Button btn_, const Color& color_)
{
	setLedImpl(led(btn_), color_);
}

void MaschineMikroMK3::setKeyLed(unsigned pad_, const Color&){ }

bool MaschineMikroMK3::isRGBLed(Led led_) const noexcept{
	return false;
}

void MaschineMikroMK3::setLedImpl(Led led_, const Color& color_)
{
	uint8_t ledIndex = static_cast<uint8_t>(led_);

	if (Led::Unknown == led_)
	{
		return;
	}

	uint8_t currentVal = m_leds[ledIndex];
	uint8_t newVal = color_.mono();
	//if (isRGBLed(led_))
	//{
		//uint8_t newVal = color_.mono();
	//}
	//else
	//{
		//uint8_t newVal = color_.mono();
	//}
	m_leds[ledIndex] = newVal;
	m_isDirtyLeds = m_isDirtyLeds || (currentVal != newVal);
}
} // namespace cabl
} // namespace sl
