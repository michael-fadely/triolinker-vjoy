#include <iostream>
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include <hidsdi.h>
#include <SetupAPI.h>
#include <vGenInterface.h>

using namespace std;

struct Handle
{
	HANDLE handle;

	Handle(HANDLE handle)
	{
		this->handle = handle;
	}

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
	Start = 0x0002,
	Up    = 0x0010,
	Right = 0x0020,
	Down  = 0x0040,
	Left  = 0x0080,
	DPad  = Up | Right | Down | Left,
	Y     = 0x0100,
	B     = 0x0200,
	A     = 0x0400,
	X     = 0x0800,
	LT    = 0x1000,
	RT    = 0x2000
};

void main()
{
	HDEVICE hDev;
	auto r = AcquireDev(1, DevType::vJoy, &hDev);

	if (r != 0)
	{
		cout << "idk fam" << endl;
		return;
	}

	Handle trio(findTrio());

	if (trio.handle == nullptr)
	{
		cout << "Unable to detect Trio Linker." << endl;
		return;
	}

	cout << "Trio Linker detected." << endl;
	
	HIDP_CAPS caps = {};
	PHIDP_PREPARSED_DATA ptr = nullptr;

	if (!HidD_GetPreparsedData(trio.handle, &ptr))
	{
		cout << "HidD_GetPreparsedData failed." << endl;
		return;
	}

	HidP_GetCaps(ptr, &caps);
	HidD_FreePreparsedData(ptr);

	cout << "InputReportByteLength: " << caps.InputReportByteLength << endl;
	system("pause");

	DWORD dummy;
	vector<uint8_t> buffer(caps.InputReportByteLength);

	while (ReadFile(trio.handle, buffer.data(), static_cast<DWORD>(buffer.size()), &dummy, nullptr))
	{
		uint16_t buttons = *(uint16_t*)&buffer[1];
		uint8_t x = buffer[3];
		uint8_t y = buffer[4];

		SetDevAxis(hDev, 1, 100.0f * (x / 255.0f));
		SetDevAxis(hDev, 2, 100.0f * (y / 255.0f));

		SetDevButton(hDev, 1, !!(buttons & TrioDreamcast::A));
		SetDevButton(hDev, 2, !!(buttons & TrioDreamcast::B));
		SetDevButton(hDev, 3, !!(buttons & TrioDreamcast::X));
		SetDevButton(hDev, 4, !!(buttons & TrioDreamcast::Y));
		SetDevButton(hDev, 8, !!(buttons & TrioDreamcast::Start));

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

			case Right|Down:
				SetDevPov(hDev, 1, 135.0f);
				break;

			case Down:
				SetDevPov(hDev, 1, 180.0f);
				break;

			case Down|Left:
				SetDevPov(hDev, 1, 225.0f);
				break;

			case Left:
				SetDevPov(hDev, 1, 270.0f);
				break;

			case Left|Up:
				SetDevPov(hDev, 1, 315.0f);
				break;

		}
	}

	cout << "Failed to read data from device." << endl;
	RelinquishDev(hDev);
}

wstring getDevicePath(HDEVINFO handle, SP_DEVICE_INTERFACE_DATA* interface)
{
	DWORD size = 0;

	SetupDiGetDeviceInterfaceDetail(handle, interface, nullptr, 0, &size, nullptr);

	auto detail = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(offsetof(SP_DEVICE_INTERFACE_DETAIL_DATA, DevicePath) + size + sizeof(TCHAR)));
	detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	bool success = SetupDiGetDeviceInterfaceDetail(handle, interface, detail, size, &size, nullptr);

	wstring result;

	if (success)
	{
		result = move(wstring(detail->DevicePath));
	}

	free(detail);
	return result;
}

HANDLE findTrio()
{
	GUID guid = {};
	HidD_GetHidGuid(&guid);

	HDEVINFO devInfoSet = SetupDiGetClassDevs(&guid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (devInfoSet == reinterpret_cast<HDEVINFO>(-1))
	{
		return nullptr;
	}

	SP_DEVINFO_DATA info = {};
	info.cbSize = sizeof(SP_DEVINFO_DATA);

	for (size_t i = 0; SetupDiEnumDeviceInfo(devInfoSet, static_cast<DWORD>(i), &info); i++)
	{
		SP_DEVICE_INTERFACE_DATA interfaceData = {};
		interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		for (size_t j = 0; SetupDiEnumDeviceInterfaces(devInfoSet, &info, &guid,
			static_cast<DWORD>(j), &interfaceData); j++)
		{
			wstring path(move(getDevicePath(devInfoSet, &interfaceData)));

			auto handle = CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
				nullptr, OPEN_EXISTING, 0, nullptr);

			if (handle == nullptr || handle == reinterpret_cast<HANDLE>(-1))
			{
				continue;
			}

			// ReSharper disable once CppInitializedValueIsAlwaysRewritten
			Handle guard(handle);

			HIDD_ATTRIBUTES attributes = {};
			if (!HidD_GetAttributes(handle, &attributes))
			{
				continue;
			}

			if (attributes.VendorID != 0x7701 || attributes.ProductID != 0x0003)
			{
				continue;
			}

			guard.handle = nullptr;
			return handle;
		}
	}

	return nullptr;
}