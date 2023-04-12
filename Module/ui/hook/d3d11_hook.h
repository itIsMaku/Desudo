#pragma once
#include <d3d11.h>

#include "../../fx/event.h"

namespace ui_hook
{
extern bool exclusiveInput;

class D3D11Hook
{
private:
	bool m_menuOpen{ true };

	static HRESULT(*Present_orig)(IDXGISwapChain* swapchain, UINT syncInterval, UINT flags);
	static LRESULT(*WndProc_orig)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	static HRESULT Present_hook(IDXGISwapChain* swapchain, UINT syncInterval, UINT flags);

	static LRESULT WndProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	static D3D11Hook& get_instance();

	bool is_open();

	void set_open(bool open);

	void start_hook();
};
} // ui_hook