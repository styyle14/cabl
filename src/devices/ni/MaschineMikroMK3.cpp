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
enum class MaschineMikroMK3::Button : uint8_t
{
	NativeInstruments,
	Star,
	Search,
	Volume,
	Swing,
	Tempo,
	PlugIn,
	Sampling,
	BrowseLeft,
	BrowseRight,
	Pitch,
	Mod,
	Perform,
	Notes,
	Group,
	Auto,
	Lock,
	NoteRepeat,
	Restart,
	Erase,
	Tap,
	Follow,
	Play,
	Rec,
	Stop,
	Shift,
	FixedVel,
	PadMode,
	Keyboard,
	Chords,
	Step,
	Scene,
	Pattern,
	Events,
	Variation,
	Duplicate,
	Select,
	Solo,
	Mute,
	MainEncoder,
	TouchEncoderMain,
	Unknown
};

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
	// Display
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

void MaschineMikroMK3::setButtonLed(Device::Button, const Color&){ }

void MaschineMikroMK3::setKeyLed(unsigned, const Color&){ }


bool MaschineMikroMK3::tick()
{
	static int state = 0;
	bool success = false;

	//!\todo enable once display dirty flag is properly set
	//if (state == 0 && m_display.dirty())
	//{
		//success = sendFrame();
	//}
	//else if (state == 1)
	//{
		//success = sendLeds();
	//}
	if (state == 2)
	{
		success = read();
	}

	if (++state >= 3)
	{
		state = 0;
	}

	return success;
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

bool MaschineMikroMK3::read()
{
	Transfer input;
	if (!readFromDeviceHandle(input, kMikroMK3_epInput))
	{
		return false;
	}
	else if (input && input[0] == 0x01)
	{
		std::cout << "Processing Report 0x01 : Buttons, Encoder, and Smartstrip\n";
		processReport0x01(input);
	}
	else if (input && input[0] == 0x02) // Too many pad messages, need to skip some...
	{
		//std::cout << "Processing Report 0x02 : Pads\n";
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
		M_BTN_CASE(PadMode);
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
	uint8_t currentEncoderValue = input_.data()[kMikroMK3_buttonsDataSize + 1];
	
	std::cout << "Current Encoder Value: " << unsigned(currentEncoderValue) << "\n";
	if (m_encoderValue != currentEncoderValue)
	{
		bool valueIncreased = ((m_encoderValue < currentEncoderValue) || ((m_encoderValue == 0x0f) && (currentEncoderValue == 0x00)))
			&& (!((m_encoderValue == 0x0) && (currentEncoderValue == 0x0f)));
		m_encoderValue = currentEncoderValue;
		encoderChanged(0, valueIncreased, m_buttonStates[static_cast<unsigned>(Button::Shift)]);
	}
}

void MaschineMikroMK3::processSmartstrip(const Transfer& input_)
{

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

} // namespace cabl
} // namespace sl
