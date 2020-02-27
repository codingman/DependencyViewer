#include "modules_window.h"

#include "main.h"
#include "modules_window_impl.h"

#include "../nogui/cassert_my.h"

#include <utility>


modules_window::modules_window() noexcept :
	m_hwnd()
{
}

modules_window::modules_window(HWND const& parent) :
	modules_window()
{
	assert(parent != nullptr);
	m_hwnd = CreateWindowExW(0, modules_window_impl::get_class_atom(), nullptr, WS_CLIPCHILDREN | WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, parent, nullptr, get_instance(), nullptr);
	assert(m_hwnd != nullptr);
}

modules_window::modules_window(modules_window&& other) noexcept :
	modules_window()
{
	swap(other);
}

modules_window& modules_window::operator=(modules_window&& other) noexcept
{
	swap(other);
	return *this;
}

modules_window::~modules_window()
{
}

void modules_window::swap(modules_window& other) noexcept
{
	using std::swap;
	swap(m_hwnd, other.m_hwnd);
}

void modules_window::init()
{
	modules_window_impl::init();
}

void modules_window::deinit()
{
	modules_window_impl::deinit();
}

HWND const& modules_window::get_hwnd() const
{
	return m_hwnd;
}