/*
        ##########    Copyright (C) 2018 styyle14
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include <array>
#include <bitset>
#include <iostream>

#include "cabl/devices/Device.h"
#include "cabl/devices/DeviceFactory.h"
#include "gfx/displays/GDisplayMaschineMikro.h"

namespace sl
{
namespace cabl
{

//--------------------------------------------------------------------------------------------------

class MaschineMikroMK3 : public Device
{
	public:
		size_t numOfGraphicDisplays() const override { return 1; }
		size_t numOfTextDisplays() const override {	return 0; }
		size_t numOfLedMatrices() const override { return 0; }
		size_t numOfLedArrays() const override { return 0; }
		
		MaschineMikroMK3();

		Canvas* graphicDisplay(size_t displayIndex_);

		void setButtonLed(Device::Button, const Color&);

		void setKeyLed(unsigned, const Color&);

	protected:
		bool tick() override;

	private:
		enum class Button : uint8_t;
		
		void init() override;
		bool read();
		
		GDisplayMaschineMikro m_display;
		bool sendFrame();
		void initDisplay() const;
  
		static constexpr uint8_t kMikroMK3_ledsDataSize = 78;
		std::array<uint8_t, kMikroMK3_ledsDataSize> m_leds;
		bool m_isDirtyLeds;
		
		static constexpr uint8_t kMikroMK3_nButtons = 41;
		static constexpr uint8_t kMikroMK3_buttonsDataSize = 6;
		std::array<uint8_t, kMikroMK3_buttonsDataSize> m_buttons;
		std::bitset<kMikroMK3_nButtons> m_buttonStates;
		void processButtons(const Transfer&);
		void processEncoder(const Transfer&);
		void processSmartstrip(const Transfer&);
		void processReport0x01(const Transfer&);
		Device::Button deviceButton(Button btn_) const noexcept;
		bool isButtonPressed(Button button) const noexcept;
		bool isButtonPressed(const Transfer&, Button button_) const noexcept;

		
		static constexpr uint8_t kMikroMK3_nPads = 16;
		static constexpr uint8_t kMikroMK3_padDataSize = 64;
		static constexpr uint8_t kMikroMK3_padsBufferSize = 16;
		unsigned m_padsData[kMikroMK3_nPads];
		std::bitset<kMikroMK3_nPads> m_padsStatus;
		void processPads(const Transfer&);
		
		uint8_t m_encoderValue;
};

//--------------------------------------------------------------------------------------------------

M_REGISTER_DEVICE_CLASS(
  MaschineMikroMK3, "Maschine Mikro MK3", DeviceDescriptor::Type::HID, 0x17CC, 0x1700);

//--------------------------------------------------------------------------------------------------

} // namespace cabl
} // namespace sl
