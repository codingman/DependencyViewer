#include "int_to_string.h"

#include <array>
#include <cwchar>
#include <cassert>


void ordinal_to_string(std::uint16_t const ordinal, std::wstring& str)
{
	static_assert(sizeof(ordinal) == sizeof(unsigned short int), "");
	std::array<wchar_t, 32> buff;
	int const formatted = std::swprintf(buff.data(), buff.size(), L"%hu (0x%04hx)", static_cast<unsigned short int>(ordinal), static_cast<unsigned short int>(ordinal));
	assert(formatted >= 0);
	str.assign(buff.data(), buff.data() + formatted);
}

void rva_to_string(std::uint32_t const rva, std::wstring& str)
{
	static_assert(sizeof(rva) == sizeof(unsigned int), "");
	std::array<wchar_t, 32> buff;
	int const formatted = std::swprintf(buff.data(), buff.size(), L"0x%08x", static_cast<unsigned int>(rva));
	assert(formatted >= 0);
	str.assign(buff.data(), buff.data() + formatted);
}