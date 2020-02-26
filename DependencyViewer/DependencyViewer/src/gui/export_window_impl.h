#pragma once


#include "export_window.h"

#include "../nogui/string_converter.h"

#include <cstdint>

#include "../nogui/windows_my.h"


struct file_info;
struct pe_export_table_info;


class export_window_impl
{
private:
	export_window_impl() noexcept = delete;
	export_window_impl(HWND const& self);
	export_window_impl(export_window_impl const&) = delete;
	export_window_impl(export_window_impl&&) noexcept = delete;
	export_window_impl& operator=(export_window_impl const&) = delete;
	export_window_impl& operator=(export_window_impl&&) noexcept = delete;
	~export_window_impl();
public:
	static void init();
	static void deinit();
	static wchar_t const* get_class_atom();
private:
	static void register_class();
	static void unregister_class();
	static LRESULT CALLBACK class_proc(HWND const hwnd, UINT const msg, WPARAM const wparam, LPARAM const lparam);
	static LRESULT on_wm_create(HWND const& hwnd, WPARAM const& wparam, LPARAM const& lparam);
	static LRESULT on_wm_destroy(HWND const& hwnd, WPARAM const& wparam, LPARAM const& lparam);
	LRESULT on_message(UINT const& msg, WPARAM const& wparam, LPARAM const& lparam);
	LRESULT on_wm_size(WPARAM const& wparam, LPARAM const& lparam);
	LRESULT on_wm_notify(WPARAM const& wparam, LPARAM const& lparam);
	LRESULT on_wm_repaint(WPARAM const& wparam, LPARAM const& lparam);
	LRESULT on_wm_setfi(WPARAM const& wparam, LPARAM const& lparam);
	LRESULT on_wm_setundecorate(WPARAM const& wparam, LPARAM const& lparam);
	void on_getdispinfow(NMHDR& nmhdr);
	wchar_t const* get_col_type(pe_export_table_info const& eti, std::uint16_t const exp_idx);
	wchar_t const* get_col_ordinal(pe_export_table_info const& eti, std::uint16_t const exp_idx);
	wchar_t const* get_col_hint(pe_export_table_info const& eti, std::uint16_t const exp_idx);
	wchar_t const* get_col_name(pe_export_table_info const& eti, std::uint16_t const exp_idx);
	wchar_t const* get_col_address(pe_export_table_info const& eti, std::uint16_t const exp_idx);
	std::uint8_t get_col_icon(pe_export_table_info const& eti, file_info const* const fi, std::uint16_t const exp_idx);
	void refresh();
	void repaint();
	int get_column_type_max_width();
private:
	static ATOM g_class;
	static int g_column_type_max_width;
private:
	HWND const m_self;
	HWND m_list_view;
	file_info const* m_fi;
	bool m_undecorate;
	string_converter m_string_converter;
};