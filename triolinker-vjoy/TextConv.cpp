/**
 * SADX Mod Loader
 * Text conversion functions.
 */

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>

#include "TextConv.hpp"

constexpr auto CP_SJIS = 932;

/**
 * Convert multibyte text to UTF-16.
 * @param mbs Multibyte text, null-terminated.
 * @param cp Code page.
 * @return UTF-16 text (allocated via new[]), or nullptr on error.
 */
wchar_t* MBStoUTF16(const char* mbs, unsigned int cp)
{
	const int cchWcs = MultiByteToWideChar(cp, 0, mbs, -1, nullptr, 0);

	if (cchWcs <= 0)
	{
		return nullptr;
	}

	const auto wcs = new wchar_t[cchWcs];
	MultiByteToWideChar(cp, 0, mbs, -1, wcs, cchWcs);
	return wcs;
}

/**
 * Convert UTF-16 text to multibyte.
 * @param wcs UTF-16 text, null-terminated.
 * @param cp Code page.
 * @return Multibyte text (allocated via new[]), or nullptr on error.
 */
char* UTF16toMBS(const wchar_t* wcs, unsigned int cp)
{
	const int cbMbs = WideCharToMultiByte(cp, 0, wcs, -1, nullptr,
	                                      0, nullptr, nullptr);

	if (cbMbs <= 0)
	{
		return nullptr;
	}

	const auto mbs = new char[cbMbs];
	WideCharToMultiByte(CP_UTF8, 0, wcs, -1,
	                    mbs, cbMbs, nullptr, nullptr);
	return mbs;
}

/** Convenience functions. **/

/**
 * Convert Shift-JIS text to UTF-8.
 * @param sjis Shift-JIS text, null-terminated.
 * @return UTF-8 text (allocated via new[]), or nullptr on error.
 */
char* SJIStoUTF8(const char* sjis)
{
	// Convert from Shift-JIS to UTF-16.
	wchar_t* wcs = MBStoUTF16(sjis, CP_SJIS);

	if (!wcs)
	{
		return nullptr;
	}

	// Convert from UTF-16 to UTF-8.
	char* mbs = UTF16toMBS(wcs, CP_UTF8);
	delete[] wcs;

	if (!mbs)
	{
		return nullptr;
	}

	return mbs;
}

/**
 * Convert UTF-8 text to Shift-JIS.
 * @param utf8 UTF-8 text, null-terminated.
 * @return Shift-JIS text (allocated via new[]), or nullptr on error.
 */
char* UTF8toSJIS(const char* utf8)
{
	// Convert from UTF-8 to UTF-16.
	wchar_t* wcs = MBStoUTF16(utf8, CP_UTF8);

	if (!wcs)
	{
		return nullptr;
	}

	// Convert from UTF-16 to Shift-JIS.
	char* mbs = UTF16toMBS(wcs, CP_SJIS);
	delete[] wcs;

	if (!mbs)
	{
		return nullptr;
	}

	return mbs;
}

/**
 * Convert UTF-8 text to Windows-1252.
 * @param utf8 UTF-8 text, null-terminated.
 * @return Windows-1252 text (allocated via new[]), or nullptr on error.
 */
char* UTF8to1252(const char* utf8)
{
	// Convert from UTF-8 to UTF-16.
	wchar_t* wcs = MBStoUTF16(utf8, CP_UTF8);

	if (!wcs)
	{
		return nullptr;
	}

	// Convert from UTF-16 to Shift-JIS.
	char* mbs = UTF16toMBS(wcs, 1252);
	delete[] wcs;

	if (!mbs)
	{
		return nullptr;
	}

	return mbs;
}

/** C++ wrappers. **/

/**
 * Convert multibyte text to UTF-16.
 * @param mbs Multibyte text.
 * @param cp Code page.
 * @return UTF-16 text, or empty string on error.
 */
std::wstring MBStoUTF16(const std::string& mbs, unsigned int cp)
{
	wchar_t* wcs = MBStoUTF16(mbs.c_str(), cp);

	if (!wcs)
	{
		return std::wstring();
	}

	std::wstring wstr(wcs);
	delete[] wcs;
	return wstr;
}

/**
 * Convert UTF-16 text to multibyte.
 * @param wcs UTF-16 text.
 * @param cp Code page.
 * @return Multibyte text, or empty string on error.
 */
std::string UTF16toMBS(const std::wstring& wcs, unsigned int cp)
{
	char* mbs = UTF16toMBS(wcs.c_str(), cp);

	if (!mbs)
	{
		return std::string();
	}

	std::string mstr(mbs);
	delete[] mbs;
	return mstr;
}

/** Convenience functions. **/

/**
 * Convert Shift-JIS text to UTF-8.
 * @param sjis Shift-JIS text.
 * @return UTF-8 text, or empty string on error.
 */
std::string SJIStoUTF8(const std::string& sjis)
{
	char* utf8 = SJIStoUTF8(sjis.c_str());

	if (!utf8)
	{
		return std::string();
	}

	std::string ustr(utf8);
	delete[] utf8;
	return ustr;
}

/**
 * Convert UTF-8 text to Shift-JIS.
 * @param utf8 UTF-8 text.
 * @return Shift-JIS text, or empty string on error.
 */
std::string UTF8toSJIS(const std::string& utf8)
{
	char* sjis = SJIStoUTF8(utf8.c_str());

	if (!sjis)
	{
		return std::string();
	}

	std::string jstr(sjis);
	delete[] sjis;
	return jstr;
}

/**
 * Convert UTF-8 text to Windows-1252.
 * @param utf8 UTF-8 text.
 * @return Windows-1252 text, or empty string on error.
 */
std::string UTF8to1252(const std::string& utf8)
{
	char* w1252 = SJIStoUTF8(utf8.c_str());

	if (!w1252)
	{
		return std::string();
	}

	std::string estr(w1252);
	delete[] w1252;
	return estr;
}
