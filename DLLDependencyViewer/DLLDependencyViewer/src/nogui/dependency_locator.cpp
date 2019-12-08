#include "dependency_locator.h"

#include "known_dlls.h"
#include "unicode.h"

#include <filesystem>


bool locate_dependency(dependency_locator& self)
{
	if(locate_dependency_sxs(self)) return true;
	if(locate_dependency_known_dlls(self)) return true;
	if(locate_dependency_application_dir(self)) return true;
	if(locate_dependency_system32(self)) return true;
	if(locate_dependency_system16(self)) return true;
	if(locate_dependency_windows(self)) return true;
	if(locate_dependency_current_dir(self)) return true;
	if(locate_dependency_environment_path(self)) return true;
	return false;
}


bool locate_dependency_sxs(dependency_locator& self)
{
	return false;
}

bool locate_dependency_known_dlls(dependency_locator& self)
{
	string_handle const& dependency = *self.m_dependency;
	std::string& tmpn = self.m_tmpn;
	self.m_tmpn.resize(size(dependency));
	std::transform(begin(dependency), end(dependency), begin(tmpn), [](auto const& e){ return to_lowercase(e); });
	string const dll_name_s{tmpn.c_str(), static_cast<int>(tmpn.size())};
	string_handle const dll_name_sh{&dll_name_s};
	std::vector<std::string> const& known_dll_names = get_known_dll_names_sorted_lowercase_ascii();
	auto const it = std::lower_bound(known_dll_names.begin(), known_dll_names.end(), dll_name_sh, [](auto const& e, auto const& v){ string const e_s{e.c_str(), static_cast<int>(e.size())}; return string_handle{&e_s} < v; });
	if(it == known_dll_names.end())
	{
		return false;
	}
	string const s{it->c_str(), static_cast<int>(it->size())};
	if(string_handle{&s} != dll_name_sh)
	{
		return false;
	}
	self.m_result = std::filesystem::path{get_knonw_dlls_path()}.append(*it);
	return true;
}

bool locate_dependency_application_dir(dependency_locator& self)
{
	return false;
}

bool locate_dependency_system32(dependency_locator& self)
{
	return false;
}

bool locate_dependency_system16(dependency_locator& self)
{
	return false;
}

bool locate_dependency_windows(dependency_locator& self)
{
	return false;
}

bool locate_dependency_current_dir(dependency_locator& self)
{
	return false;
}

bool locate_dependency_environment_path(dependency_locator& self)
{
	return false;
}
