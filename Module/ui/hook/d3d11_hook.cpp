#include "d3d11_hook.h"
#include "settings.h"

#include "../menus/menus.h"

#include "../../util/pe.h"
#include "../../util/pattern.h"

#include "../../config/config_manager.h"

#include "../../include/imgui/imgui_impl_dx11.h"
#include "../../include/imgui/imgui_impl_win32.h"

#include "../../include/xorstr/xorstr.hpp"

#include <mutex>
#include <format>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ui_hook
{
LRESULT(*D3D11Hook::WndProc_orig)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = nullptr;

HRESULT(*D3D11Hook::Present_orig)(IDXGISwapChain* swapchain, UINT syncInterval, UINT flags) = nullptr;

ui_hook::D3D11Hook& D3D11Hook::get_instance()
{
	static D3D11Hook instance;
	return instance;
}

LRESULT D3D11Hook::WndProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYUP)
	{
		static auto& keybindsConfig = config::Manager::get_instance().get_config(xorstr_("keybinds"));
		if (wParam == keybindsConfig.get_key<int>(xorstr_("open_key"), VK_DELETE))
			get_instance().set_open(!get_instance().is_open());
	}

	if (get_instance().is_open())
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		//return true;
	}

	return CallWindowProc(WndProc_orig, hWnd, uMsg, wParam, lParam);
}

HRESULT D3D11Hook::Present_hook(IDXGISwapChain* swapchain, UINT syncInterval, UINT flags)
{
	static ID3D11DeviceContext* deviceCtx = nullptr;
	static ID3D11RenderTargetView* renderTarget = nullptr;

	static bool hasFailed = false;
	try
	{
		if (!hasFailed && !WndProc_orig)
		{
			ID3D11Device* device = nullptr;
			swapchain->GetDevice(__uuidof(device), (void**)&device);

			if (!device)
				throw std::exception(xorstr_("Failed to get D3D11 device."));

			device->GetImmediateContext(&deviceCtx);

			if (!deviceCtx)
				throw std::exception(xorstr_("Failed to get D3D11 device context."));

			ID3D11Texture2D* buffer = nullptr;
			swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

			if (buffer)
			{
				device->CreateRenderTargetView(buffer, nullptr, &renderTarget);
				buffer->Release();
			}

			DXGI_SWAP_CHAIN_DESC desc;
			swapchain->GetDesc(&desc);

			if (!desc.OutputWindow)
				throw std::exception(xorstr_("Failed to get output window."));

			ImGui::CreateContext();

			ImGui_ImplWin32_Init(desc.OutputWindow);
			ImGui_ImplDX11_Init(device, deviceCtx);

			WndProc_orig = (WNDPROC)SetWindowLongPtrW(desc.OutputWindow, GWLP_WNDPROC, (LONG_PTR)WndProc_hook);

			load_settings();
		}
	}
	catch (const std::exception& ex)
	{
		MessageBox(nullptr, std::format(("Failed to initialize ImGui. The user interface will be unavailable.\n\n{}"), ex.what()).c_str(), ("Error"), MB_OK);
		hasFailed = true;
	}

	if (!hasFailed && get_instance().is_open())
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// ImGui::ShowDemoWindow();
		
		// ui_menus::Events::draw();
		ui_menus::LuaExecutor::draw();
		ui_menus::Resources::draw();
		// ui_menus::Debug::draw();

		ImGui::EndFrame();
		ImGui::Render();

		deviceCtx->OMSetRenderTargets(1, &renderTarget, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	return Present_orig(swapchain, syncInterval, flags);
}

void D3D11Hook::start_hook()
{
	auto searchSegment = util::get_module_segment(xorstr_("GameOverlayRenderer64.dll"), xorstr_(".text"));
	if (!searchSegment.has_value())
	{
		MessageBox(nullptr, xorstr_("Please open Steam before injecting."), xorstr_("Error"), MB_OK);
		return;
	}

	auto foundAddress = util::find_pattern(
		searchSegment->baseAddress,
		searchSegment->baseAddress + searchSegment->size,
		(uint8_t*)"\x48\x83\xc4\x00\x41\x5e\x48\xff\x25\x00\x00\x00\x00\x48\x89\x5c\x24",
		xorstr_("xxx?xxxxx????xxxx"),
		0x6
	);

	if (!foundAddress)
	{
		MessageBox(nullptr, xorstr_("Failed to find address required to draw overlay."), xorstr_("Error"), MB_OK);
		return;
	}

	uint8_t* bytes = (uint8_t*)foundAddress;
	int offset = *(int*)&bytes[3];

	uintptr_t* Present_ptr = (uintptr_t*)((foundAddress + 0x7) + offset);

	Present_orig = (decltype(Present_orig))*Present_ptr;
	*Present_ptr = (uintptr_t)Present_hook;
}

bool D3D11Hook::is_open()
{
	return m_menuOpen;
}

void D3D11Hook::set_open(bool open)
{
	m_menuOpen = open;
}
} // ui_hook