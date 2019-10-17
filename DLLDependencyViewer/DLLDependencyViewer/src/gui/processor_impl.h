#pragma once


#include "processor.h"

#include "../nogui/file_name.h"

#include <queue>
#include <string>
#include <unordered_map>


struct processor_impl
{
	main_type* m_mo = nullptr;
	std::queue<file_info*> m_queue;
	std::unordered_map<string const*, file_info*, string_case_insensitive_hash, string_case_insensitive_equal> m_map;
	file_name* m_file_name = nullptr;
	manifest_parser* m_manifest_parser = nullptr;
};


main_type process_impl(std::vector<std::wstring> const& file_paths);
void process_r(processor_impl& prcsr);
void process_e(processor_impl& prcsr, file_info& fi, file_info& sub_fi, string const* const& dll_name);
manifest_data process_manifest(processor_impl& prcsr, file_info const& fi);
std::pair<char const*, int> find_manifest(file_info const& fi);
void pair_imports_with_exports(file_info& fi, file_info& sub_fi);
void pair_exports_with_imports(processor_impl& prcsr, file_info& fi, file_info& sub_fi);
