#pragma once


#include "tree_window.h"

#include "../nogui/string_converter.h"

#include "../nogui/windows_my.h"


struct file_info;
struct htreeitem_t;
typedef htreeitem_t* htreeitem;


class tree_window_impl
{
private:
	tree_window_impl() noexcept = delete;
	tree_window_impl(HWND const& self);
	tree_window_impl(tree_window_impl const&) = delete;
	tree_window_impl(tree_window_impl&&) noexcept = delete;
	tree_window_impl& operator=(tree_window_impl const&) = delete;
	tree_window_impl& operator=(tree_window_impl&&) noexcept = delete;
	~tree_window_impl();
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
	LRESULT on_wm_setfullpaths(WPARAM const& wparam, LPARAM const& lparam);
	LRESULT on_wm_setonitemchanged(WPARAM const& wparam, LPARAM const& lparam);
	void on_getdispinfow(NMHDR& nmhdr);
	void refresh();
	void refresh_r(file_info* const& fi, htreeitem const& parent_ti);
	void refresh_e(file_info* const& fi, htreeitem const& parent_ti);
	void repaint();
private:
	static ATOM g_class;
private:
	HWND const m_self;
	HWND m_tree_view;
	file_info* m_fi;
	bool m_fullpaths;
	string_converter m_string_converter;
	tree_window::onitemchanged_fn_t m_onitemchanged_fn;
	tree_window::onitemchanged_ctx_t m_onitemchanged_ctx;
};
