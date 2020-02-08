#include "modules_view.h"

#include "main.h"
#include "main_window.h"

#include "../nogui/scope_exit.h"
#include "../nogui/utils.h"

#include <cassert>
#include <iterator>

#include "../nogui/my_windows.h"

#include <commctrl.h>


enum class e_modules_column
{
	e_name,
	e_path,
};
static constexpr wchar_t const* const s_modules_headers[] =
{
	L"name",
	L"path",
};


modules_view::modules_view(HWND const parent, main_window& mw) :
	m_hwnd(CreateWindowExW(WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE, WC_LISTVIEWW, nullptr, WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDATA, 0, 0, 0, 0, parent, nullptr, get_instance(), nullptr)),
	m_main_window(mw)
{
	assert(parent != nullptr);
	assert(m_hwnd != nullptr);
	static constexpr unsigned const extended_lv_styles = LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER;
	LRESULT const set_ex_style = SendMessageW(m_hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, extended_lv_styles, extended_lv_styles);
	for(int i = 0; i != static_cast<int>(std::size(s_modules_headers)); ++i)
	{
		LVCOLUMNW cl;
		cl.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		cl.fmt = LVCFMT_LEFT;
		cl.cx = 200;
		cl.pszText = const_cast<LPWSTR>(s_modules_headers[i]);
		cl.cchTextMax = 0;
		cl.iSubItem = i;
		cl.iImage = 0;
		cl.iOrder = i;
		cl.cxMin = 0;
		cl.cxDefault = 0;
		cl.cxIdeal = 0;
		auto const inserted = SendMessageW(m_hwnd, LVM_INSERTCOLUMNW, i, reinterpret_cast<LPARAM>(&cl));
		assert(inserted != -1 && inserted == i);
	}
}

modules_view::~modules_view()
{
}

HWND const& modules_view::get_hwnd() const
{
	return m_hwnd;
}

void modules_view::on_notify(NMHDR& nmhdr)
{
	switch(nmhdr.code)
	{
		case LVN_GETDISPINFOW:
		{
			on_getdispinfow(nmhdr);
		}
		break;
	}
}

void modules_view::refresh()
{
	LRESULT const redr_off = SendMessageW(m_hwnd, WM_SETREDRAW, FALSE, 0);
	LRESULT const deleted = SendMessageW(m_hwnd, LVM_DELETEALLITEMS, 0, 0);
	assert(deleted == TRUE);
	auto const redr_on = mk::make_scope_exit([&]()
	{
		LRESULT const redr_on = SendMessageW(m_hwnd, WM_SETREDRAW, TRUE, 0);
		repaint();
	});

	auto const& count = m_main_window.m_mo.m_modules_list.m_count;
	LRESULT const set_count = SendMessageW(m_hwnd, LVM_SETITEMCOUNT, count, 0);
	assert(set_count != 0);
}

void modules_view::repaint()
{
	BOOL const redrawn = RedrawWindow(m_hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
	assert(redrawn != 0);
}

void modules_view::on_getdispinfow(NMHDR& nmhdr)
{
	NMLVDISPINFOW& nm = reinterpret_cast<NMLVDISPINFOW&>(nmhdr);
	int const row = nm.item.iItem;
	int const col = nm.item.iSubItem;
	assert(col >= 0 && col <= static_cast<int>(e_modules_column::e_path));
	auto const ecol = static_cast<e_modules_column>(col);
	if((nm.item.mask & LVIF_TEXT) != 0)
	{
		switch(ecol)
		{
			case e_modules_column::e_name:
			{
				nm.item.pszText = const_cast<wchar_t*>(on_get_col_name(row));
			}
			break;
			case e_modules_column::e_path:
			{
				nm.item.pszText = const_cast<wchar_t*>(on_get_col_path(row));
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}
	}
}

wchar_t const* modules_view::on_get_col_name(int const& row)
{
	assert(row < m_main_window.m_mo.m_modules_list.m_count);
	file_info const* const fi = m_main_window.m_mo.m_modules_list.m_list[row];
	assert(!fi->m_orig_instance);
	assert(fi->m_file_path);
	wchar_t const* const name = find_file_name(begin(fi->m_file_path), size(fi->m_file_path));
	assert(name != begin(fi->m_file_path));
	return name;
}

wchar_t const* modules_view::on_get_col_path(int const& row)
{
	assert(row < m_main_window.m_mo.m_modules_list.m_count);
	file_info const* const fi = m_main_window.m_mo.m_modules_list.m_list[row];
	assert(!fi->m_orig_instance);
	assert(fi->m_file_path);
	return fi->m_file_path.m_string->m_str;
}