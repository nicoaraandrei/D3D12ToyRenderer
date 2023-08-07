#pragma once

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>
#include <D3D/DxContext.h>

class DXWindow
{
	public:
		bool Init();
		void Shutdown();
		void Update();
		void Present();

		inline bool ShouldClose() const
		{
			return m_shouldClose;
		}

		static constexpr size_t GetFrameCount()
		{
			return 2;
		}

	private:
		static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		ATOM m_wndClass = 0;
		HWND m_window = nullptr;
		bool m_shouldClose = false;

		ComPointer<IDXGISwapChain4> m_swapChain;

		// Window singleton
	public:
		DXWindow(const DXWindow&) = delete;
		DXWindow& operator=(const DXWindow&) = delete;

		inline static DXWindow& Get()
		{
			static DXWindow instance;
			return instance;
		}
	private:
		DXWindow() = default;
};