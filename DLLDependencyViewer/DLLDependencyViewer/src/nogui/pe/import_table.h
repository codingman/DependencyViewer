#pragma once


#include "pe_util.h"

#include <cstdint>


struct pe_import_directory_entry
{
	std::uint32_t m_import_lookup_table;
	std::uint32_t m_date_time;
	std::uint32_t m_forwarder_chain;
	std::uint32_t m_name;
	std::uint32_t m_import_adress_table;
};
static_assert(sizeof(pe_import_directory_entry) == 20, "");
static_assert(sizeof(pe_import_directory_entry) == 0x14, "");

struct pe_import_directory_table
{
	pe_import_directory_entry const* m_table;
	int m_size;
	pe_section_header const* m_sct;
};


struct pe_import_lookup_entry_32
{
	std::uint32_t m_value;
};
static_assert(sizeof(pe_import_lookup_entry_32) == 4, "");
static_assert(sizeof(pe_import_lookup_entry_32) == 0x4, "");

struct pe_import_lookup_entry_64
{
	std::uint64_t m_value;
};
static_assert(sizeof(pe_import_lookup_entry_64) == 8, "");
static_assert(sizeof(pe_import_lookup_entry_64) == 0x8, "");

struct pe_import_lookup_table_32
{
	pe_import_lookup_entry_32 const* m_table;
	int m_size;
	pe_section_header const* m_sct;
};

struct pe_import_lookup_table_64
{
	pe_import_lookup_entry_64 const* m_table;
	int m_size;
	pe_section_header const* m_sct;
};

struct pe_hint_name
{
	std::uint16_t m_hint;
	pe_string m_name;
};

struct pe_delay_load_directory_entry
{
	std::uint32_t m_attributes;
	std::uint32_t m_name;
	std::uint32_t m_module_handle;
	std::uint32_t m_delay_import_address_table;
	std::uint32_t m_delay_import_name_table;
	std::uint32_t m_bound_delay_import_table;
	std::uint32_t m_unload_delay_import_table;
	std::uint32_t m_timestamp;
};
static_assert(sizeof(pe_delay_load_directory_entry) == 32, "");
static_assert(sizeof(pe_delay_load_directory_entry) == 0x20, "");

struct pe_delay_import_descriptor
{
	pe_delay_load_directory_entry const* m_table;
	int m_size;
	pe_section_header const* m_sct;
};


bool pe_parse_import_directory_table(void const* const& file_data, int const& file_size, pe_import_directory_table& idt);
bool pe_parse_import_dll_name(void const* const& file_data, int const& file_size, pe_import_directory_table const& idt, int const& idx, pe_string& str);
bool pe_parse_import_lookup_table_32(void const* const& file_data, int const& file_size, pe_import_directory_table const& idt, int const& idx, pe_import_lookup_table_32& ilt);
bool pe_parse_import_lookup_table_64(void const* const& file_data, int const& file_size, pe_import_directory_table const& idt, int const& idx, pe_import_lookup_table_64& ilt);
bool pe_parse_import_lookup_entry_32(void const* const& file_data, int const& file_size, pe_import_lookup_table_32 const& ilt, int const& idx, bool& is_ordinal);
bool pe_parse_import_lookup_entry_64(void const* const& file_data, int const& file_size, pe_import_lookup_table_64 const& ilt, int const& idx, bool& is_ordinal);
bool pe_parse_import_lookup_entry_ordinal_32(void const* const& file_data, int const& file_size, pe_import_lookup_table_32 const& ilt, int const& idx, std::uint16_t& ordinal);
bool pe_parse_import_lookup_entry_ordinal_64(void const* const& file_data, int const& file_size, pe_import_lookup_table_64 const& ilt, int const& idx, std::uint16_t& ordinal);
bool pe_parse_import_lookup_entry_hint_name_32(void const* const& file_data, int const& file_size, pe_import_lookup_table_32 const& ilt, int const& idx, pe_hint_name& hntnm);
bool pe_parse_import_lookup_entry_hint_name_64(void const* const& file_data, int const& file_size, pe_import_lookup_table_64 const& ilt, int const& idx, pe_hint_name& hntnm);
bool pe_parse_import_lookup_entry_hint_name_impl(void const* const& file_data, int const& file_size, pe_section_header const& sct, std::uint32_t const& hint_name_rva, pe_hint_name& hntnm);
bool pe_parse_delay_import_descriptor(void const* const& file_data, int const& file_size, pe_delay_import_descriptor& did);
bool pe_parse_delay_import_dll_name_32(void const* const& file_data, int const& file_size, pe_delay_import_descriptor const& did, int const& idx, pe_string& str);
bool pe_parse_delay_import_dll_name_64(void const* const& file_data, int const& file_size, pe_delay_import_descriptor const& did, int const& idx, pe_string& str);
bool pe_parse_delay_import_dll_name_impl(void const* const& file_data, int const& file_size, std::uint32_t const& delay_dll_name_rva, pe_string& str);
