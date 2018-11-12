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
	Maschine,
	Star,
	Search,
	Volume,
	Swing,
	Tempo,
	Plugin,
	Sampling,
	Left,
	Right,
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
	Record,
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
	Mute
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
		std::cout << "Reading button...\n";
		processButtons(input);
	}
	else if (input && input[0] == 0x02) // Too many pad messages, need to skip some...
	{
		processPads(input);
	}
	
	return true;
}
void MaschineMikroMK3::processButtons(const Transfer& input_)
{
	//bool shiftPressed(isButtonPressed(input_, Button::Shift));
	//m_buttonStates[static_cast<unsigned>(Button::Shift)] = shiftPressed;
	//std::cout << "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX.\n";
	//Device::Button changedButton(Device::Button::Unknown);
	bool buttonPressed(false);

	for (int i = 0; i < kMikroMK3_buttonsDataSize - 1; i++) // Skip the last byte (encoder value)
	{
		continue;
	}
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
