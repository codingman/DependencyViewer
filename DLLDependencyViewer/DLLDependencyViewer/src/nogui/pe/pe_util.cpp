#include "pe_util.h"

#include "../assert.h"
#include "mz.h"

#include <algorithm>


std::uint32_t pe_find_object_in_raw(void const* const& fd, int const& /*file_size*/, std::uint32_t const& obj_va, std::uint32_t const& obj_size, pe_section_header const*& sct)
{
	char const* const file_data = static_cast<char const*>(fd);
	pe_dos_header const& dos_hdr = *reinterpret_cast<pe_dos_header const*>(file_data + 0);
	pe_coff_full_32_64 const& coff_hdr = *reinterpret_cast<pe_coff_full_32_64 const*>(file_data + dos_hdr.m_pe_offset);
	bool const is_32 = coff_hdr.m_32.m_standard.m_signature == s_pe_coff_optional_sig_32;
	std::uint32_t const data_dir_cnt = is_32 ? coff_hdr.m_32.m_windows.m_data_directory_count : coff_hdr.m_64.m_windows.m_data_directory_count;
	std::uint32_t const sect_tbl_cnt = is_32 ? coff_hdr.m_32.m_coff.m_section_count : coff_hdr.m_64.m_coff.m_section_count;
	pe_section_header const* const sect_tbl = reinterpret_cast<pe_section_header const*>(file_data + dos_hdr.m_pe_offset + (is_32 ? sizeof(pe_coff_full_32) : sizeof(pe_coff_full_64)) + data_dir_cnt * sizeof(pe_data_directory));
	for(std::uint32_t i = 0; i != sect_tbl_cnt; ++i)
	{
		pe_section_header const& sect = sect_tbl[i];
		if(obj_va >= sect.m_virtual_address && obj_va < sect.m_virtual_address + sect.m_raw_size)
		{
			std::uint32_t const offset_iniside_sect = obj_va - sect.m_virtual_address;
			std::uint32_t const obj_raw = sect.m_raw_ptr + offset_iniside_sect;
			WARN_M_R(obj_raw + obj_size <= sect.m_raw_ptr + sect.m_raw_size, L"Object does not fin in section raw size.", 0);
			sct = &sect;
			return obj_raw;
		}
	}
	WARN_M_R(false, L"Object not found in any section.", 0);
}

bool pe_parse_string(void const* const fd, int const file_size, std::uint32_t const str_rva, pe_string& str_out)
{
	char const* const file_data = static_cast<char const*>(fd);
	WARN_M_R(str_rva != 0, L"Invalid string.", false);
	pe_section_header const* sct;
	std::uint32_t const str_raw = pe_find_object_in_raw(file_data, file_size, str_rva, 2, sct);
	WARN_M_R(str_raw != 0, L"Could not find string in any section.", false);
	char const* const str = reinterpret_cast<char const*>(file_data + str_raw);
	static constexpr const std::uint32_t s_str_len_max = 32 * 1024;
	std::uint32_t const str_len_max = std::min<std::uint32_t>(s_str_len_max, sct->m_raw_ptr + sct->m_raw_size - str_raw);
	auto const str_end = std::find(str, str + str_len_max, '\0');
	WARN_M_R(str_end != str + str_len_max, L"Could not find string length.", false);
	std::uint16_t const str_len = static_cast<std::uint16_t>(str_end - str);
	WARN_M_R(str_len >= 1, L"String is too short.", false);
	WARN_M_R(pe_is_ascii(str, str_len), L"String is not in ASCII.", false);
	str_out.m_str = str;
	str_out.m_len = str_len;
	return true;
}

bool pe_is_ascii(char const* const& str, int const& len)
{
	return std::all_of(str, str + len, [](char const& e){ return e >= 32 && e <= 126; });
}
