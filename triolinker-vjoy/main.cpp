#include <iostream>
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include <hidsdi.h>
#include <SetupAPI.h>

#include <vGenInterface.h>

#include "IniFile.hpp"

static int vendorID = 0x7701;
static int productID = 0x0003;
static int buffer_x = 3;
static int buffer_y = 4;
static int buffer_z = 0;
static int buffer_rx = 0;
static int buffer_ry = 0;
static int buffer_rz = 0;
static int buffer_buttons1 = 1;
static int buffer_buttons2 = 2;
static int buffer_hat = 2;
static uint8_t hat_up = 0x10;
static uint8_t hat_up_right = 0x30;
static uint8_t hat_up_left = 0x90;
static uint8_t hat_down = 0x40;
static uint8_t hat_down_right = 0x60;
static uint8_t hat_down_left = 0xC0;
static uint8_t hat_left = 0x80;
static uint8_t hat_right = 0x20;
static uint8_t hat_center = 0;
static bool dPadAsButtons = false;

struct Handle
{
	HANDLE handle;

	explicit Handle(HANDLE handle)
	{
		this->handle = handle;
	}

	Handle(Handle&& other) noexcept
	{
		this->handle = other.handle;
		other.handle = nullptr;
	}

	Handle& operator=(Handle&& other) noexcept
	{
		this->handle = other.handle;
		other.handle = nullptr;
		return *this;
	}

	Handle(const Handle& other) = delete;
	Handle& operator=(const Handle& other) = delete;

	~Handle()
	{
		if (handle != nullptr)
		{
			CloseHandle(handle);
		}
	}
};

HANDLE findTrio();

enum ButtonBits : uint8_t
{
	Button0 = 0x01,
	Button1 = 0x02,
	Button2 = 0x04,
	Button3 = 0x08,
	Button4 = 0x10,
	Button5 = 0x20,
	Button6 = 0x40,
	Button7 = 0x80,
};

bool CheckIfButtonIsPad(int buffer, uint8_t button)
{
	if (buffer == buffer_hat && !dPadAsButtons)
	{
		if (hat_center & button || button == hat_up || button == hat_up_right || button == hat_right || button == hat_down_right || button == hat_down || button == hat_down_left || button == hat_left || button == hat_up_left || button == hat_center) return true;
	}
	return false;
}

int main(int argc, char** argv)
{
	CopyFileA("default.ini", "config.ini", true);

	const IniFile config("config.ini");

	const  bool hide          = config.getBool("General",  "HideWindow",    false);
	const  bool unlinkDpad    = config.getBool("General",  "UnlinkDPad",    true);
	const float defaultX      = config.getFloat("General", "DefaultX",      50.1f);
	const float defaultY      = config.getFloat("General", "DefaultY",      50.1f);
	dPadAsButtons = config.getBool("General", "DPadAsButtons", false);
	vendorID = std::stol(config.getString("General", "VendorID", "7701"), nullptr, 16);
	productID = std::stol(config.getString("General", "ProductID", "0003"), nullptr, 16);
	buffer_x = config.getInt("Buffers", "X", 3);
	buffer_y = config.getInt("Buffers", "Y", 4);
	buffer_z = config.getInt("Buffers", "Z", 0);
	buffer_rx = config.getInt("Buffers", "RX", 0);
	buffer_ry = config.getInt("Buffers", "RY", 0);
	buffer_rz = config.getInt("Buffers", "RZ", 0);
	buffer_buttons1 = config.getInt("Buffers", "Buttons1", 1);
	buffer_buttons2 = config.getInt("Buffers", "Buttons2", 2);
	buffer_hat = config.getInt("Buffers", "DPad", 2);
	hat_up = std::stol(config.getString("DPad", "DPad North", "0x10"), nullptr, 16);
	hat_down = std::stol(config.getString("DPad", "DPad South", "0x40"), nullptr, 16);
	hat_left = std::stol(config.getString("DPad", "DPad West", "0x80"), nullptr, 16);
	hat_right = std::stol(config.getString("DPad", "DPad East", "0x20"), nullptr, 16);
	hat_up_left = std::stol(config.getString("DPad", "DPad NorthWest", "0x90"), nullptr, 16);
	hat_up_right = std::stol(config.getString("DPad", "DPad NorthEast", "0x30"), nullptr, 16);
	hat_down_left = std::stol(config.getString("DPad", "DPad SouthWest", "0xC0"), nullptr, 16);
	hat_down_right = std::stol(config.getString("DPad", "DPad SouthEast", "0x60"), nullptr, 16);
	hat_center = std::stol(config.getString("DPad", "DPad Center", "0"), nullptr, 16);

	const DevType devType = DevType::vJoy;

	HDEVICE hDev;
	const auto r = AcquireDev(1, devType, &hDev);

	if (r != 0)
	{
		std::cout << "USB device not found." << std::endl;
		return -1;
	}

	const Handle trio(findTrio());

	if (trio.handle == nullptr)
	{
		std::cout << "Unable to detect device: vendor ID " << std::hex << vendorID << " product ID " << std::hex << productID << "." << std::endl;
		return -2;
	}

	std::cout << "Device found: vendor ID " << std::hex << vendorID << " product ID " << std::hex << productID << "." << std::endl;

	HIDP_CAPS caps {};
	PHIDP_PREPARSED_DATA ptr = nullptr;

	if (!HidD_GetPreparsedData(trio.handle, &ptr))
	{
		std::cout << "HidD_GetPreparsedData failed." << std::endl;
		return -3;
	}

	if (!HidP_GetCaps(ptr, &caps))
	{
		std::cout << "HidP_GetCaps failed." << std::endl;
		HidD_FreePreparsedData(ptr);
		return -4;
	}

	HidD_FreePreparsedData(ptr);

	if (hide)
	{
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	}

	DWORD dummy;
	std::vector<uint8_t> buffer(caps.InputReportByteLength);

	while (ReadFile(trio.handle, buffer.data(), static_cast<DWORD>(buffer.size()), &dummy, nullptr))
	{
		const uint8_t buttons1 = *reinterpret_cast<uint8_t*>(&buffer[buffer_buttons1]);
		const uint8_t buttons2 = *reinterpret_cast<uint8_t*>(&buffer[buffer_buttons2]);
		const uint8_t hat	   = *reinterpret_cast<uint8_t*>(&buffer[buffer_hat]);

		const auto x = buffer[buffer_x];
		const auto y = buffer[buffer_y];
		const auto z = buffer[buffer_z];
		const auto rx = buffer[buffer_rx];
		const auto ry = buffer[buffer_ry];
		const auto rz = buffer[buffer_rz];
		if (unlinkDpad && hat != hat_center)
		{
			SetDevAxis(hDev, 1, defaultX);
			SetDevAxis(hDev, 2, defaultY);
		}
		else
		{
			SetDevAxis(hDev, 1, 100.0f * (static_cast<float>(x) / 255.0f));
			SetDevAxis(hDev, 2, 100.0f * (static_cast<float>(y) / 255.0f));
		}
		SetDevAxis(hDev, 3, 100.0f * (static_cast<float>(z) / 255.0f));
		SetDevAxis(hDev, 4, 100.0f * (static_cast<float>(rx) / 255.0f));
		SetDevAxis(hDev, 5, 100.0f * (static_cast<float>(ry) / 255.0f));
		SetDevAxis(hDev, 6, 100.0f * (static_cast<float>(rz) / 255.0f));

		if (!CheckIfButtonIsPad(buffer_buttons1, Button0)) SetDevButton(hDev, 1, !!(buttons1 & Button0)); else SetDevButton(hDev, 1, 0);
		if (!CheckIfButtonIsPad(buffer_buttons1, Button1)) SetDevButton(hDev, 2, !!(buttons1 & Button1)); else SetDevButton(hDev, 2, 0);
		if (!CheckIfButtonIsPad(buffer_buttons1, Button2)) SetDevButton(hDev, 3, !!(buttons1 & Button2)); else SetDevButton(hDev, 3, 0);
		if (!CheckIfButtonIsPad(buffer_buttons1, Button3)) SetDevButton(hDev, 4, !!(buttons1 & Button3)); else SetDevButton(hDev, 4, 0);
		
		if (!CheckIfButtonIsPad(buffer_buttons1, Button4)) SetDevButton(hDev, 5, !!(buttons1 & Button4)); else SetDevButton(hDev, 5, 0);
		if (!CheckIfButtonIsPad(buffer_buttons1, Button5)) SetDevButton(hDev, 6, !!(buttons1 & Button5)); else SetDevButton(hDev, 6, 0);
		if (!CheckIfButtonIsPad(buffer_buttons1, Button6)) SetDevButton(hDev, 7, !!(buttons1 & Button6)); else SetDevButton(hDev, 7, 0);
		if (!CheckIfButtonIsPad(buffer_buttons1, Button7)) SetDevButton(hDev, 8, !!(buttons1 & Button7)); else SetDevButton(hDev, 8, 0);
		
		if (!CheckIfButtonIsPad(buffer_buttons2, Button0)) SetDevButton(hDev, 9,  !!(buttons2 & Button0)); else SetDevButton(hDev, 9, 0);
		if (!CheckIfButtonIsPad(buffer_buttons2, Button1)) SetDevButton(hDev, 10, !!(buttons2 & Button1)); else SetDevButton(hDev, 10, 0);
		if (!CheckIfButtonIsPad(buffer_buttons2, Button2)) SetDevButton(hDev, 11, !!(buttons2 & Button2)); else SetDevButton(hDev, 11, 0);
		if (!CheckIfButtonIsPad(buffer_buttons2, Button3)) SetDevButton(hDev, 12, !!(buttons2 & Button3)); else SetDevButton(hDev, 12, 0);

		if (!CheckIfButtonIsPad(buffer_buttons2, Button4)) SetDevButton(hDev, 13, !!(buttons2 & Button4)); else SetDevButton(hDev, 13, 0);
		if (!CheckIfButtonIsPad(buffer_buttons2, Button5)) SetDevButton(hDev, 14, !!(buttons2 & Button5)); else SetDevButton(hDev, 14, 0);
		if (!CheckIfButtonIsPad(buffer_buttons2, Button6)) SetDevButton(hDev, 15, !!(buttons2 & Button6)); else SetDevButton(hDev, 15, 0);
		if (!CheckIfButtonIsPad(buffer_buttons2, Button7)) SetDevButton(hDev, 16, !!(buttons2 & Button7)); else SetDevButton(hDev, 16, 0);

		if (!dPadAsButtons)
		{
			if (hat == hat_center) SetDevPov(hDev, 1, -1.0f);
			else if (hat == hat_up) SetDevPov(hDev, 1, 0.0f);
			else if (hat == hat_up_right) SetDevPov(hDev, 1, 45.0f);
			else if (hat == hat_right) SetDevPov(hDev, 1, 90.0f);
			else if (hat == hat_down_right) SetDevPov(hDev, 1, 135.0f);
			else if (hat == hat_down) SetDevPov(hDev, 1, 180.0f);
			else if (hat == hat_down_left) SetDevPov(hDev, 1, 225.0f);
			else if (hat == hat_left) SetDevPov(hDev, 1, 270.0f);
			else if (hat == hat_up_left) SetDevPov(hDev, 1, 315.0f);
			else SetDevPov(hDev, 1, -1.0f);
		}
		else SetDevPov(hDev, 1, -1.0f);
	}

	if (hide)
	{
		ShowWindow(GetConsoleWindow(), SW_SHOW);
	}

	std::cout << "Failed to read data from device." << std::endl;
	RelinquishDev(hDev);
	return 0;
}

std::wstring getDevicePath(HDEVINFO handle, SP_DEVICE_INTERFACE_DATA* interface)
{
	DWORD size = 0;

	SetupDiGetDeviceInterfaceDetail(handle, interface, nullptr, 0, &size, nullptr);

	const auto detail_size =
		offsetof(SP_DEVICE_INTERFACE_DETAIL_DATA, DevicePath)
		+ size
		+ sizeof(TCHAR);

	std::vector<uint8_t> detail_buffer(detail_size);
	auto detail = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(detail_buffer.data());
	detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	const bool success = SetupDiGetDeviceInterfaceDetail(handle, interface, detail, size, &size, nullptr);

	std::wstring result;

	if (success)
	{
		result = std::wstring(detail->DevicePath);
	}

	return result;
}

HANDLE findTrio()
{
	GUID guid {};
	HidD_GetHidGuid(&guid);

	const HDEVINFO devInfoSet = SetupDiGetClassDevs(&guid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (devInfoSet == reinterpret_cast<HDEVINFO>(-1))
	{
		return nullptr;
	}

	SP_DEVINFO_DATA info {};
	info.cbSize = sizeof(SP_DEVINFO_DATA);

	for (size_t i = 0; SetupDiEnumDeviceInfo(devInfoSet, static_cast<DWORD>(i), &info); i++)
	{
		SP_DEVICE_INTERFACE_DATA interfaceData {};
		interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		for (size_t j = 0; SetupDiEnumDeviceInterfaces(devInfoSet, &info, &guid, static_cast<DWORD>(j), &interfaceData); j++)
		{
			std::wstring path(getDevicePath(devInfoSet, &interfaceData));

			const auto handle = CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			                               nullptr, OPEN_EXISTING, 0, nullptr);

			if (handle == nullptr || handle == reinterpret_cast<HANDLE>(-1))
			{
				continue;
			}

			Handle guard(handle);

			HIDD_ATTRIBUTES attributes {};
			if (!HidD_GetAttributes(handle, &attributes))
			{
				continue;
			}

			if (attributes.VendorID != vendorID || attributes.ProductID != productID)
			{
				continue;
			}

			guard.handle = nullptr;
			return handle;
		}
	}

	return nullptr;
}
