#include "tree_window_impl.h"

#include "common_controls.h"
#include "file_info_getters.h"
#include "main.h"
#include "processor.h"

#include "../nogui/cassert_my.h"
#include "../nogui/scope_exit.h"

#include "../res/resources.h"

#include "../nogui/windows_my.h"

#include <commctrl.h>


ATOM tree_window_impl::g_class;


tree_window_impl::tree_window_impl(HWND const& self) :
	m_self(self),
	m_tree_view(),
	m_fi(),
	m_fullpaths(false),
	m_string_converter(),
	m_onitemchanged_fn(),
	m_onitemchanged_ctx()
{
	assert(self != nullptr);

	DWORD const ex_style = WS_EX_CLIENTEDGE;
	wchar_t const* const class_name = WC_TREEVIEWW;
	wchar_t const* const window_name = nullptr;
	DWORD const style = (WS_VISIBLE | WS_CHILD | WS_TABSTOP) | (TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS);
	int const x_pos = 0;
	int const y_pos = 0;
	int const width = 0;
	int const height = 0;
	HWND const parent = m_self;
	HMENU const menu = nullptr;
	HINSTANCE const instance = get_instance();
	LPVOID const param = nullptr;
	m_tree_view = CreateWindowExW(ex_style, class_name, window_name, style, x_pos, y_pos, width, height, parent, menu, instance, param);
	assert(m_tree_view != nullptr);

	unsigned const extended_tv_styles = TVS_EX_DOUBLEBUFFER;
	LRESULT const style_set = SendMessageW(m_tree_view, TVM_SETEXTENDEDSTYLE, extended_tv_styles, extended_tv_styles);
	assert(style_set == S_OK);

	HIMAGELIST const tree_img_list = common_controls::ImageList_LoadImageW(get_instance(), MAKEINTRESOURCEW(s_res_icons_tree), 26, 0, CLR_DEFAULT, IMAGE_BITMAP, LR_DEFAULTCOLOR);
	assert(tree_img_list);
	LRESULT const prev_img_list = SendMessageW(m_tree_view, TVM_SETIMAGELIST, TVSIL_NORMAL, reinterpret_cast<LPARAM>(tree_img_list));
	assert(!prev_img_list);
}

tree_window_impl::~tree_window_impl()
{
}

void tree_window_impl::init()
{
	register_class();
}

void tree_window_impl::deinit()
{
	unregister_class();
}

wchar_t const* tree_window_impl::get_class_atom()
{
	assert(g_class != 0);
	return reinterpret_cast<wchar_t const*>(g_class);
}

void tree_window_impl::register_class()
{
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(wc);
	wc.style = 0;
	wc.lpfnWndProc = &class_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = get_instance();
	wc.hIcon = nullptr;
	wc.hCursor = LoadCursorW(nullptr, reinterpret_cast<wchar_t const*>(IDC_ARROW));
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"tree_window";
	wc.hIconSm = nullptr;

	assert(g_class == 0);
	g_class = RegisterClassExW(&wc);
	assert(g_class != 0);
}

void tree_window_impl::unregister_class()
{
	assert(g_class != 0);
	BOOL const unregistered = UnregisterClassW(reinterpret_cast<wchar_t const*>(g_class), get_instance());
	assert(unregistered != 0);
	g_class = 0;
}

LRESULT CALLBACK tree_window_impl::class_proc(HWND const hwnd, UINT const msg, WPARAM const wparam, LPARAM const lparam)
{
	if(msg == WM_CREATE)
	{
		LRESULT const ret = on_wm_create(hwnd, wparam, lparam);
		return ret;
	}
	else if(msg == WM_DESTROY)
	{
		LRESULT const ret = on_wm_destroy(hwnd, wparam, lparam);
		return ret;
	}
	else
	{
		LONG_PTR const self_ptr = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
		if(self_ptr != 0)
		{
			tree_window_impl* const self = reinterpret_cast<tree_window_impl*>(self_ptr);
			LRESULT const ret = self->on_message(msg, wparam, lparam);
			return ret;
		}
		else
		{
			LRESULT const ret = DefWindowProcW(hwnd, msg, wparam, lparam);
			return ret;
		}
	}
}

LRESULT tree_window_impl::on_wm_create(HWND const& hwnd, WPARAM const& wparam, LPARAM const& lparam)
{
	tree_window_impl* const self = new tree_window_impl{hwnd};
	assert((SetLastError(0), true));
	LONG_PTR const prev = SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
	assert(prev == 0 && GetLastError() == 0);

	LRESULT const ret = DefWindowProcW(hwnd, WM_CREATE, wparam, lparam);
	return ret;
}

LRESULT tree_window_impl::on_wm_destroy(HWND const& hwnd, WPARAM const& wparam, LPARAM const& lparam)
{
	LONG_PTR const prev = SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
	assert(prev != 0);
	tree_window_impl* const self = reinterpret_cast<tree_window_impl*>(prev);
	delete self;

	LRESULT const ret = DefWindowProcW(hwnd, WM_DESTROY, wparam, lparam);
	return ret;
}

LRESULT tree_window_impl::on_message(UINT const& msg, WPARAM const& wparam, LPARAM const& lparam)
{
	switch(msg)
	{
		case WM_SIZE:
		{
			LRESULT const ret = on_wm_size(wparam, lparam);
			return ret;
		}
		break;
		case WM_NOTIFY:
		{
			LRESULT const ret = on_wm_notify(wparam, lparam);
			return ret;
		}
		break;
		case static_cast<std::uint32_t>(tree_window::wm::wm_repaint):
		{
			LRESULT const ret = on_wm_repaint(wparam, lparam);
			return ret;
		}
		break;
		case static_cast<std::uint32_t>(tree_window::wm::wm_setfi):
		{
			LRESULT const ret = on_wm_setfi(wparam, lparam);
			return ret;
		}
		break;
		case static_cast<std::uint32_t>(tree_window::wm::wm_setfullpaths):
		{
			LRESULT const ret = on_wm_setfullpaths(wparam, lparam);
			return ret;
		}
		break;
		case static_cast<std::uint32_t>(tree_window::wm::wm_setonitemchanged):
		{
			LRESULT const ret = on_wm_setonitemchanged(wparam, lparam);
			return ret;
		}
		break;
		default:
		{
			LRESULT const ret = DefWindowProcW(m_self, msg, wparam, lparam);
			return ret;
		}
		break;
	}
}

LRESULT tree_window_impl::on_wm_size(WPARAM const& wparam, LPARAM const& lparam)
{
	RECT rect;
	BOOL const got = GetClientRect(m_self, &rect);
	assert(got != 0);
	assert(rect.left == 0 && rect.top == 0);
	BOOL const moved = MoveWindow(m_tree_view, 0, 0, rect.right, rect.bottom, TRUE);
	assert(moved != 0);

	LRESULT const ret = DefWindowProcW(m_self, WM_SIZE, wparam, lparam);
	return ret;
}

LRESULT tree_window_impl::on_wm_notify(WPARAM const& wparam, LPARAM const& lparam)
{
	NMHDR& nmhdr = *reinterpret_cast<NMHDR*>(lparam);
	if(nmhdr.hwndFrom == m_tree_view)
	{
		switch(nmhdr.code)
		{
			case TVN_GETDISPINFOW:
			{
				on_getdispinfow(nmhdr);
			}
			break;
			case TVN_SELCHANGEDW:
			{
				on_selchangedw(nmhdr);
			}
			break;
		}
	}

	LRESULT const ret = DefWindowProcW(m_self, WM_NOTIFY, wparam, lparam);
	return ret;
}

LRESULT tree_window_impl::on_wm_repaint(WPARAM const& wparam, LPARAM const& lparam)
{
	repaint();

	UINT const msg = static_cast<std::uint32_t>(tree_window::wm::wm_repaint);
	LRESULT const ret = DefWindowProcW(m_self, msg, wparam, lparam);
	return ret;
}

LRESULT tree_window_impl::on_wm_setfi(WPARAM const& wparam, LPARAM const& lparam)
{
	file_info* const fi = reinterpret_cast<file_info*>(lparam);
	m_fi = fi;
	refresh();

	UINT const msg = static_cast<std::uint32_t>(tree_window::wm::wm_setfi);
	LRESULT const ret = DefWindowProcW(m_self, msg, wparam, lparam);
	return ret;
}

LRESULT tree_window_impl::on_wm_setfullpaths(WPARAM const& wparam, LPARAM const& lparam)
{
	assert(wparam == 0 || wparam == 1);
	bool const fullpaths = wparam != 0;
	m_fullpaths = fullpaths;
	repaint();

	UINT const msg = static_cast<std::uint32_t>(tree_window::wm::wm_setfullpaths);
	LRESULT const ret = DefWindowProcW(m_self, msg, wparam, lparam);
	return ret;
}

LRESULT tree_window_impl::on_wm_setonitemchanged(WPARAM const& wparam, LPARAM const& lparam)
{
	static_assert(sizeof(wparam) == sizeof(tree_window::onitemchanged_fn_t), "");
	static_assert(sizeof(lparam) == sizeof(tree_window::onitemchanged_ctx_t), "");
	auto const onitemchanged_fn = reinterpret_cast<tree_window::onitemchanged_fn_t>(wparam);
	auto const onitemchanged_ctx = reinterpret_cast<tree_window::onitemchanged_ctx_t>(lparam);
	m_onitemchanged_fn = onitemchanged_fn;
	m_onitemchanged_ctx = onitemchanged_ctx;

	UINT const msg = static_cast<std::uint32_t>(tree_window::wm::wm_setonitemchanged);
	LRESULT const ret = DefWindowProcW(m_self, msg, wparam, lparam);
	return ret;
}

void tree_window_impl::on_getdispinfow(NMHDR& nmhdr)
{
	NMTVDISPINFOW& di = reinterpret_cast<NMTVDISPINFOW&>(nmhdr);
	assert(di.item.lParam != 0);
	file_info const* const fi = reinterpret_cast<file_info const*>(di.item.lParam);
	if((di.item.mask & TVIF_TEXT) != 0)
	{
		if(m_fullpaths)
		{
			wstring const str = get_fi_path(fi);
			assert(str);
			di.item.pszText = const_cast<wchar_t*>(str.m_str);
		}
		else
		{
			wstring const str = get_fi_name(fi, m_string_converter);
			assert(str);
			di.item.pszText = const_cast<wchar_t*>(str.m_str);
		}
	}
	if((di.item.mask & (TVIF_IMAGE | TVIF_SELECTEDIMAGE)) != 0)
	{
		di.item.iImage = fi->m_icon;
		di.item.iSelectedImage = di.item.iImage;
	}
}

void tree_window_impl::on_selchangedw([[maybe_unused]] NMHDR& nmhdr)
{
	auto const fn_get_fi = [&]() -> file_info const*
	{
		LRESULT const selected_ = SendMessageW(m_tree_view, TVM_GETNEXTITEM, TVGN_CARET, 0);
		if(selected_ == 0)
		{
			return nullptr;
		}
		HTREEITEM const selected = reinterpret_cast<HTREEITEM>(selected_);
		file_info const* const fi = htreeitem_2_file_info(reinterpret_cast<htreeitem>(selected));
		assert(fi);
		return fi;
	};

	file_info const* const fi = fn_get_fi();
	if(m_onitemchanged_fn)
	{
		m_onitemchanged_fn(m_onitemchanged_ctx, fi);
	}
}

void tree_window_impl::select_item(file_info const* const& fi)
{
	assert(fi);
	assert(fi->m_tree_item != nullptr);
	LRESULT const selected = SendMessageW(m_tree_view, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(fi->m_tree_item));
	assert(selected == TRUE);
}

void tree_window_impl::refresh()
{
	LRESULT const redr_off = SendMessageW(m_tree_view, WM_SETREDRAW, FALSE, 0);
	assert(redr_off == 0);
	auto const fn_redraw = mk::make_scope_exit([&]()
	{
		LRESULT const redr_on = SendMessageW(m_tree_view, WM_SETREDRAW, TRUE, 0);
		assert(redr_on == 0);
		repaint();
	});
	LRESULT const deselected = SendMessageW(m_tree_view, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(nullptr));
	assert(deselected == TRUE);
	LRESULT const deleted = SendMessageW(m_tree_view, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(TVI_ROOT));
	assert(deleted == TRUE);

	file_info* const fi = m_fi;
	if(!fi)
	{
		return;
	}
	htreeitem const parent = reinterpret_cast<htreeitem>(TVI_ROOT);
	refresh_r(fi, parent);
	std::uint16_t const n = fi->m_import_table.m_normal_dll_count + fi->m_import_table.m_delay_dll_count;
	for(std::uint16_t i = 0; i != n; ++i)
	{
		file_info* const& sub_fi = fi->m_fis + i;
		assert(sub_fi->m_tree_item != nullptr);
		LRESULT const expanded = SendMessageW(m_tree_view, TVM_EXPAND, TVE_EXPAND, reinterpret_cast<LPARAM>(sub_fi->m_tree_item));
		assert(expanded != 0);
	}
	HTREEITEM const first = reinterpret_cast<HTREEITEM>(fi->m_fis[0].m_tree_item);
	LRESULT const visibled = SendMessageW(m_tree_view, TVM_ENSUREVISIBLE, 0, reinterpret_cast<LPARAM>(first));
	LRESULT const selected = SendMessageW(m_tree_view, TVM_SELECTITEM, TVGN_CARET, reinterpret_cast<LPARAM>(first));
	assert(deselected == TRUE);
}

void tree_window_impl::refresh_r(file_info* const& fi, htreeitem const& parent_ti)
{
	if(!fi)
	{
		return;
	}
	std::uint16_t const n = fi->m_import_table.m_normal_dll_count + fi->m_import_table.m_delay_dll_count;
	for(std::uint16_t i = 0; i != n; ++i)
	{
		file_info* const& sub_fi = fi->m_fis + i;
		refresh_e(sub_fi, parent_ti);
	}
}

void tree_window_impl::refresh_e(file_info* const& fi, htreeitem const& parent_ti)
{
	TVINSERTSTRUCTW tvi;
	tvi.hParent = reinterpret_cast<HTREEITEM>(parent_ti);
	tvi.hInsertAfter = TVI_LAST;
	tvi.itemex.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
	tvi.itemex.hItem = nullptr;
	tvi.itemex.state = 0;
	tvi.itemex.stateMask = 0;
	tvi.itemex.pszText = LPSTR_TEXTCALLBACKW;
	tvi.itemex.cchTextMax = 0;
	tvi.itemex.iImage = I_IMAGECALLBACK;
	tvi.itemex.iSelectedImage = I_IMAGECALLBACK;
	tvi.itemex.cChildren = 0;
	tvi.itemex.lParam = reinterpret_cast<LPARAM>(fi);
	tvi.itemex.iIntegral = 0;
	tvi.itemex.uStateEx = 0;
	tvi.itemex.hwnd = nullptr;
	tvi.itemex.iExpandedImage = 0;
	tvi.itemex.iReserved = 0;
	LRESULT const ti_ = SendMessageW(m_tree_view, TVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&tvi));
	HTREEITEM const ti = reinterpret_cast<HTREEITEM>(ti_);
	assert(ti != nullptr);
	assert(fi->m_tree_item == nullptr);
	fi->m_tree_item = reinterpret_cast<htreeitem>(ti);
	htreeitem const parent = reinterpret_cast<htreeitem>(ti);
	refresh_r(fi, parent);
}

void tree_window_impl::repaint()
{
	BOOL const redrawn = RedrawWindow(m_tree_view, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
	assert(redrawn != 0);
}

file_info const* tree_window_impl::htreeitem_2_file_info(htreeitem const& hti)
{
	assert(hti);
	TVITEMW ti;
	ti.hItem = reinterpret_cast<HTREEITEM>(hti);
	ti.mask = TVIF_PARAM;
	LRESULT const got_item = SendMessageW(m_tree_view, TVM_GETITEMW, 0, reinterpret_cast<LPARAM>(&ti));
	assert(got_item == TRUE);
	assert(ti.lParam != 0);
	file_info const* const ret = reinterpret_cast<file_info*>(ti.lParam);
	return ret;
}
