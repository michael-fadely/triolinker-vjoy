/**
 * SADX Mod Loader
 * INI file parser.
 */

#include "IniFile.hpp"
#include "TextConv.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>

#include "windows.h"
using std::transform;
using std::string;
using std::unordered_map;
using std::wstring;

/** IniGroup **/

/**
 * Check if the INI group has the specified key.
 * @param key Key.
 * @return True if the key exists; false if not.
 */
bool IniGroup::hasKey(const string& key) const
{
	return m_data.find(key) != m_data.end();
}

/**
 * Check if the INI group has the specified key with a non-empty value.
 * @param key Key.
 * @return True if the key exists; false if not or value is empty.
 */
bool IniGroup::hasKeyNonEmpty(const string& key) const
{
	const auto it = m_data.find(key);

	return it != m_data.end() && !it->second.empty();
}

const unordered_map<string, string>* IniGroup::data() const
{
	return &m_data;
}

/**
 * Get a string value from the INI group.
 * @param key Key.
 * @param def Default value.
 * @return String value.
 */
string IniGroup::getString(const string& key, const string& def) const
{
	const auto it = m_data.find(key);
	return it != m_data.end() ? it->second : def;
}

/**
 * Get a wide string value from the INI group.
 * INI strings are converted from UTF-8 to UTF-16.
 * @param key Key.
 * @param def Default value.
 * @return Wide string value.
 */
wstring IniGroup::getWString(const string& key, const wstring& def) const
{
	const auto it = m_data.find(key);

	if (it == m_data.end())
	{
		return def;
	}

	// Convert the string from UTF-8 to UTF-16.
	return MBStoUTF16(it->second, CP_UTF8);
}

/**
 * Get a boolean value from the INI group.
 * @param key Key.
 * @param def Default value.
 * @return Boolean value.
 */
bool IniGroup::getBool(const string& key, bool def) const
{
	const auto it = m_data.find(key);

	if (it == m_data.end())
	{
		return def;
	}

	string value = it->second;
	transform(value.begin(), value.end(), value.begin(), ::tolower);

	// TODO: Support integer values.
	return (value == "true");
}

/**
 * Get an integer value from the INI group.
 * @param key Key.
 * @param radix Radix.
 * @param def Default value.
 * @return Integer value.
 */
int IniGroup::getIntRadix(const string& key, int radix, int def) const
{
	const auto it = m_data.find(key);

	if (it == m_data.end())
	{
		return def;
	}

	return std::stoi(it->second, nullptr, radix);
}

/**
 * Get an integer value from the INI group.
 * @param key Key.
 * @param def Default value.
 * @return Integer value.
 */
int IniGroup::getInt(const string& key, int def) const
{
	return getIntRadix(key, 10, def);
}

/**
 * Get a floating-point value from the INI group.
 * @param key Key.
 * @param def Default value.
 * @return Floating-point value.
 */
float IniGroup::getFloat(const string& key, float def) const
{
	const auto it = m_data.find(key);
	
	if (it == m_data.end())
	{
		return def;
	}

	return std::stof(it->second);
}

/**
* Set a string value in the INI group.
* @param key Key.
* @param val Value.
*/
void IniGroup::setString(const string& key, const string& val)
{
	m_data[key] = val;
}

/**
* Set a wide string value in the INI group.
* INI strings are converted from UTF-8 to UTF-16.
* @param key Key.
* @param val Value.
*/
void IniGroup::setWString(const string& key, const wstring& val)
{
	m_data[key] = UTF16toMBS(val, CP_UTF8);
}

/**
* Set a boolean value in the INI group.
* @param key Key.
* @param val Value.
*/
void IniGroup::setBool(const string& key, bool val)
{
	const string str = val ? "True" : "False";
	m_data[key] = str;
}

/**
* Set an integer value in the INI group.
* @param key Key.
* @param radix Radix.
* @param value Value.
*/
void IniGroup::setIntRadix(const string& key, int radix, int value)
{
	std::array<char, sizeof(int) * 8 + 1> buf {};

#ifdef _MSC_VER
	_itoa_s(value, buf.data(), buf.size(), radix);
#else
	switch (radix)
	{
	case 8:
		snprintf(buf, buf.size(), "%o", value);
		break;
	case 16:
		snprintf(buf, buf.size(), "%x", value);
		break;
	default:
		snprintf(buf, buf.size(), "%d", value);
		break;
	}
#endif

	m_data[key] = std::string(buf.data());
}

/**
* Set an integer value in the INI group.
* @param key Key.
* @param val Value.
*/
void IniGroup::setInt(const string& key, int val)
{
	m_data[key] = std::to_string(val);
}

/**
* Set a floating-point value in the INI group.
* @param key Key.
* @param val Value.
*/
void IniGroup::setFloat(const string& key, float val)
{
	m_data[key] = std::to_string(val);
}

/**
* Remove a key from the INI group.
* @param key Key.
* @return True if key was found.
*/
bool IniGroup::removeKey(const string& key)
{
	if (hasKey(key))
	{
		m_data.erase(key);
		return true;
	}

	return false;
}

std::unordered_map<string, string>::iterator IniGroup::begin()
{
	return m_data.begin();
}

std::unordered_map<string, string>::const_iterator IniGroup::cbegin() const
{
	return m_data.cbegin();
}

std::unordered_map<string, string>::iterator IniGroup::end()
{
	return m_data.end();
}

std::unordered_map<string, string>::const_iterator IniGroup::cend() const
{
	return m_data.cend();
}

/** IniFile **/

IniFile::IniFile(const string& filename)
{
	std::ifstream f(filename);

	if (!f.is_open())
	{
		return;
	}

	load(f);
}

IniFile::IniFile(const wstring& filename)
{
	std::ifstream f(filename);

	if (!f.is_open())
	{
		return;
	}

	load(f);
}

IniFile::IniFile(std::istream& f)
{
	load(f);
}

IniFile::~IniFile()
{
	clear();
}

/**
 * Get an INI group.
 * @param section Section.
 * @return INI group, or nullptr if not found.
 */
IniGroup* IniFile::getGroup(const string& section)
{
	const auto it = m_groups.find(section);
	return it != m_groups.end() ? it->second : nullptr;
}

/**
* Get an INI group.
* @param section Section.
* @return INI group, or nullptr if not found.
*/
const IniGroup* IniFile::getGroup(const string& section) const
{
	const auto it = m_groups.find(section);
	return it != m_groups.end() ? it->second : nullptr;
}

/**
* Create an INI group, or retrieve an existing group.
* @param section Section.
* @return INI group.
*/
IniGroup* IniFile::createGroup(const string& section)
{
	const auto it = m_groups.find(section);

	if (it != m_groups.end())
	{
		return it->second;
	}

	const auto group = new IniGroup();
	m_groups[section] = group;
	return group;
}

/**
 * Check if the INI file has the specified group.
 * @param section Section.
 * @return True if the section exists; false if not.
 */
bool IniFile::hasGroup(const string& section) const
{
	return m_groups.find(section) != m_groups.end();
}

/**
 * Check if the INI file has the specified key.
 * @param section Section.
 * @param key Key.
 * @return True if the key exists; false if not.
 */
bool IniFile::hasKey(const string& section, const string& key) const
{
	const auto it = m_groups.find(section);

	if (it == m_groups.end())
	{
		return false;
	}

	return it->second->hasKey(key);
}

/**
 * Check if the INI file has the specified key with a non-empty value.
 * @param section Section.
 * @param key Key.
 * @return True if the key exists; false if not or value is empty.
 */
bool IniFile::hasKeyNonEmpty(const string& section, const string& key) const
{
	const auto it = m_groups.find(section);

	if (it == m_groups.end())
	{
		return false;
	}

	return it->second->hasKeyNonEmpty(key);
}

/**
 * Get a string value from the INI file.
 * @param section Section.
 * @param key Key.
 * @param def Default value.
 * @return String value.
 */
string IniFile::getString(const string& section, const string& key, const string& def) const
{
	const IniGroup* group = getGroup(section);
	return group ? group->getString(key, def) : def;
}

/**
 * Get a wide string value from the INI group.
 * INI strings are converted from UTF-8 to UTF-16.
 * @param section Section.
 * @param key Key.
 * @param def Default value.
 * @return Wide string value.
 */
wstring IniFile::getWString(const string& section, const string& key, const wstring& def) const
{
	const IniGroup* group = getGroup(section);
	return group ? group->getWString(key, def) : def;
}

/**
 * Get a boolean value from the INI file.
 * @param section Section.
 * @param key Key.
 * @param def Default value.
 * @return Boolean value.
 */
bool IniFile::getBool(const string& section, const string& key, bool def) const
{
	const IniGroup* group = getGroup(section);
	return group ? group->getBool(key, def) : def;
}

/**
 * Get an integer value from the INI file.
 * @param section Section.
 * @param key Key.
 * @param radix Radix.
 * @param def Default value.
 * @return Integer value.
 */
int IniFile::getIntRadix(const string& section, const string& key, int radix, int def) const
{
	const IniGroup* group = getGroup(section);
	return group ? group->getIntRadix(key, radix, def) : def;
}

/**
 * Get an integer value from the INI file.
 * @param section Section.
 * @param key Key.
 * @param def Default value.
 * @return Integer value.
 */
int IniFile::getInt(const string& section, const string& key, int def) const
{
	const IniGroup* group = getGroup(section);
	return group ? group->getInt(key, def) : def;
}

/**
 * Get a floating-point value from the INI file.
 * @param section Section.
 * @param key Key.
 * @param def Default value.
 * @return Floating-point value.
 */
float IniFile::getFloat(const string& section, const string& key, float def) const
{
	const IniGroup* group = getGroup(section);
	return group ? group->getFloat(key, def) : def;
}

/**
* Set a string value in the INI file.
* @param section Section.
* @param key Key.
* @param val Value.
*/
void IniFile::setString(const string& section, const string& key, const string& val)
{
	createGroup(section)->setString(key, val);
}

/**
* Set a wide string value in the INI file.
* INI strings are converted from UTF-8 to UTF-16.
* @param section Section.
* @param key Key.
* @param val Value.
*/
void IniFile::setWString(const string& section, const string& key, const wstring& val)
{
	createGroup(section)->setWString(key, val);
}

/**
* Set a boolean value in the INI file.
* @param section Section.
* @param key Key.
* @param val Value.
*/
void IniFile::setBool(const string& section, const string& key, bool val)
{
	createGroup(section)->setBool(key, val);
}

/**
* Set an integer value in the INI file.
* @param section Section.
* @param key Key.
* @param radix Radix.
* @param val Value.
*/
void IniFile::setIntRadix(const string& section, const string& key, int radix, int val)
{
	createGroup(section)->setIntRadix(key, radix, val);
}

/**
* Set an integer value in the INI file.
* @param section Section.
* @param key Key.
* @param val Value.
*/
void IniFile::setInt(const string& section, const string& key, int val)
{
	createGroup(section)->setInt(key, val);
}

/**
* Set a floating-point value in the INI file.
* @param section Section.
* @param key Key.
* @param val Value.
*/
void IniFile::setFloat(const string& section, const string& key, float val)
{
	createGroup(section)->setFloat(key, val);
}

/**
* Remove a section from the INI file.
* @param group Section.
* @return True if section was found.
*/
bool IniFile::removeGroup(const string& group)
{
	if (hasGroup(group))
	{
		delete m_groups[group];
		m_groups.erase(group);
		return true;
	}
	return false;
}

/**
* Remove a key from the INI file.
* @param section Section.
* @param key Key.
* @return True if key was found.
*/
bool IniFile::removeKey(const string& section, const string& key)
{
	IniGroup* group = getGroup(section);
	if (group)
		return group->removeKey(key);
	return false;
}

/**
* Save an INI file.
* @param filename Name of file to save to.
*/
void IniFile::save(const string& filename) const
{
	std::fstream f(filename, std::ios_base::out);

	if (!f.is_open())
	{
		return;
	}

	save(f);
}

/**
* Save an INI file.
* @param filename Name of file to save to.
*/
void IniFile::save(const wstring& filename) const
{
	std::fstream f(filename, std::ios_base::out);

	if (!f.is_open())
	{
		return;
	}

	save(f);
}

/**
* Save an INI file.
* @param f FILE pointer. (File is not closed after processing.)
*/
void IniFile::save(std::fstream& f) const
{
	std::list<std::pair<string, IniGroup*>> list;

	for (auto it = cbegin(); it != cend(); ++it)
	{
		if (it->first.empty())
		{
			list.push_front(std::make_pair(it->first, it->second));
		}
		else
		{
			list.emplace_back(it->first, it->second);
		}
	}

	for (const auto& gr : list)
	{
		if (!gr.first.empty())
		{
			f << "[" << escape(gr.first, true, false) << "]" << std::endl;
		}

		for (const auto& kv : *gr.second)
		{
			f << escape(kv.first, false, true) << "=" << escape(kv.second, false, false) << std::endl;
		}
	}
}

std::unordered_map<string, IniGroup*>::iterator IniFile::begin()
{
	return m_groups.begin();
}

std::unordered_map<string, IniGroup*>::const_iterator IniFile::cbegin() const
{
	return m_groups.cbegin();
}

std::unordered_map<string, IniGroup*>::iterator IniFile::end()
{
	return m_groups.end();
}

std::unordered_map<string, IniGroup*>::const_iterator IniFile::cend() const
{
	return m_groups.cend();
}

/**
 * Load an INI file.
 * Internal function; called from the constructor.
 * @param f FILE pointer. (File is not closed after processing.)
 */
void IniFile::load(std::istream& f)
{
	clear();
	f.seekg(0, std::ios_base::beg);

	// Create an empty group for default settings.
	auto curGroup = new IniGroup();
	m_groups[""] = curGroup;

	// Process the INI file.
	while (!f.eof())
	{
		std::string line;

		if (!std::getline(f, line))
		{
			break;
		}

		const auto line_len = line.length();

		bool startswithbracket = false;
		ptrdiff_t firstequals       = -1;
		ptrdiff_t endbracket        = -1;

		// String can contain escape characters, so
		// we need a string buffer.
		string sb;
		sb.reserve(line_len);

		// Process the line.
		for (size_t c = 0; c < line_len; c++)
		{
			switch (line[c])
			{
				case '\\': // escape character
					if (c + 1 >= line_len)
					{
						// Backslash at the end of the line.
						goto appendchar;
					}
					c++;
					switch (line[c])
					{
						case 'n': // line feed
							sb += '\n';
							break;
						case 'r': // carriage return
							sb += '\r';
							break;
						default: // literal character
							goto appendchar;
					}
					break;

				case '=':
					if (firstequals == -1)
						firstequals = sb.length();
					goto appendchar;

				case '[':
					if (c == 0)
						startswithbracket = true;
					goto appendchar;

				case ']':
					endbracket = sb.length();
					goto appendchar;

				case ';':	// comment character
				case '\r':	// trailing newline (CRLF)
				case '\n':	// trailing newline (LF)
					// Stop processing this line.
					c = line_len;
					break;

				default:
				appendchar:
					// Normal character. Append to the string buffer.
					sb += line[c];
					break;
			}
		}

		// Check the string buffer.
		if (startswithbracket && endbracket != -1)
		{
			// New section.
			string section = sb.substr(1, !endbracket ? std::string::npos : endbracket - 1);
			auto iter = m_groups.find(section);
			if (iter != m_groups.end())
			{
				// Section already exists.
				// Use the existing section.
				curGroup = iter->second;
			}
			else
			{
				// New section.
				curGroup = new IniGroup();
				m_groups[section] = curGroup;
			}
		}
		else if (!sb.empty())
		{
			// Key/value.
			string key;
			string value;
			if (firstequals > -1)
			{
				key   = sb.substr(0, firstequals);
				value = sb.substr(firstequals + 1);
			}
			else
			{
				key = sb;
			}

			// Store the value in the current group.
			curGroup->m_data[key] = value;
		}
	}
}

/**
 * Clear the loaded INI file.
 */
void IniFile::clear()
{
	for (auto& m_group : m_groups)
	{
		delete m_group.second;
	}

	m_groups.clear();
}

string IniFile::escape(const string& str, bool sec, bool key)
{
	string result(str);

	for (size_t i = 0; i < result.size(); i++)
	{
		switch (result[i])
		{
			case '=':
				if (key)
				{
					result.insert(i++, "\\");
				}
				break;

			case '[':
				if (key && i == 0)
				{
					result.insert(i++, "\\");
				}
				break;

			case ']':
				if (sec)
				{
					result.insert(i++, "\\");
				}
				break;

			case '\\':
			case '\n':
			case '\r':
			case ';':
				result.insert(i++, "\\");
				break;

			default:
				break;
		}
	}

	return result;
}
