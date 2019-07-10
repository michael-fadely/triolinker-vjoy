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

enum TrioDreamcast : uint16_t
{
	Y     = 0x0001,
	B     = 0x0002,
	A     = 0x0004,
	X     = 0x0008,
	LT    = 0x0010,
	RT    = 0x0020,
	Z     = 0x0040,
	C     = 0x0080,
	Start = 0x0200,
	Up    = 0x1000,
	Right = 0x2000,
	Down  = 0x4000,
	Left  = 0x8000,
	DPad  = Up | Right | Down | Left,
};

int main(int argc, char** argv)
{
	CopyFileA("default.ini", "config.ini", true);

	const IniFile config("config.ini");

	const  bool hide          = config.getBool("",  "HideWindow",    true);
	const  bool xinput        = config.getBool("",  "XInput",        true);
	const  bool unlinkDpad    = config.getBool("",  "UnlinkDPad",    true);
	const  bool dPadAsButtons = config.getBool("",  "DPadAsButtons", false);
	const float defaultX      = config.getFloat("", "DefaultX",      50.0f);
	const float defaultY      = config.getFloat("", "DefaultY",      50.0f);

	const DevType devType = xinput ? DevType::vXbox : DevType::vJoy;

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
		std::cout << "Unable to detect Trio Linker." << std::endl;
		return -2;
	}

	std::cout << "Trio Linker detected." << std::endl;

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
		const uint16_t buttons = *reinterpret_cast<uint16_t*>(&buffer[1]);

		const auto x1 = buffer[3];
		const auto y1 = buffer[4];
		const auto x2 = buffer[5];
		const auto y2 = buffer[6];

		// The adapter outputs analog data when the d-pad is pressed,
		// so just ignore that and center the axis.
		if (unlinkDpad && buttons & TrioDreamcast::DPad)
		{
			SetDevAxis(hDev, 1, defaultX);
			SetDevAxis(hDev, 2, defaultY);
		}
		else
		{
			SetDevAxis(hDev, 1, 100.0f * (static_cast<float>(x1) / 255.0f));
			SetDevAxis(hDev, 2, 100.0f * (static_cast<float>(y1) / 255.0f));
		}

		SetDevAxis(hDev, 4, 100.0f * (static_cast<float>(x2) / 255.0f));
		SetDevAxis(hDev, 5, 100.0f * (static_cast<float>(y2) / 255.0f));

		SetDevButton(hDev, 1, !!(buttons & TrioDreamcast::A));
		SetDevButton(hDev, 2, !!(buttons & TrioDreamcast::B));
		SetDevButton(hDev, 3, !!(buttons & TrioDreamcast::X));
		SetDevButton(hDev, 4, !!(buttons & TrioDreamcast::Y));
		SetDevButton(hDev, 5, !!(buttons & TrioDreamcast::LT));
		SetDevButton(hDev, 6, !!(buttons & TrioDreamcast::RT));
		SetDevButton(hDev, 7, !!(buttons & TrioDreamcast::C));
		SetDevButton(hDev, 8, !!(buttons & TrioDreamcast::Start));
		SetDevButton(hDev, 9, !!(buttons & TrioDreamcast::Z));

		if (!dPadAsButtons)
		{
			switch (buttons & TrioDreamcast::DPad)
			{
				default:
					SetDevPov(hDev, 1, -1.0f);
					break;

				case Up:
					SetDevPov(hDev, 1, 0.0f);
					break;

				case Up | Right:
					SetDevPov(hDev, 1, 45.0f);
					break;

				case Right:
					SetDevPov(hDev, 1, 90.0f);
					break;

				case Right | Down:
					SetDevPov(hDev, 1, 135.0f);
					break;

				case Down:
					SetDevPov(hDev, 1, 180.0f);
					break;

				case Down | Left:
					SetDevPov(hDev, 1, 225.0f);
					break;

				case Left:
					SetDevPov(hDev, 1, 270.0f);
					break;

				case Left | Up:
					SetDevPov(hDev, 1, 315.0f);
					break;
			}
		}
		else
		{
			SetDevButton(hDev, 10, !!(buttons & TrioDreamcast::Up));
			SetDevButton(hDev, 11, !!(buttons & TrioDreamcast::Down));
			SetDevButton(hDev, 12, !!(buttons & TrioDreamcast::Left));
			SetDevButton(hDev, 13, !!(buttons & TrioDreamcast::Right));
		}
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

			constexpr auto vendorID  = 0x7701;
			constexpr auto productID = 0x0003;

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
