#include "Window.h"

bool DXWindow::Init()
{
	// Window class
	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = &DXWindow::OnWindowMessage;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandleW(nullptr);
	wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"D3D12ToyRendererCls";
	wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
	
	m_wndClass = RegisterClassExW(&wcex);
	if (m_wndClass == 0)
	{
		return false;
	}

	// place window on current screen
	POINT cursorPos{ 0,0 };
	GetCursorPos(&cursorPos);
	HMONITOR monitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monitorInfo{};
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfoW(monitor, &monitorInfo);

	// Window
	m_window = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
		(LPCWSTR)m_wndClass, L"D3D12ToyRenderer", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		monitorInfo.rcWork.left + 100, monitorInfo.rcWork.top + 100, 1920, 1080,
		nullptr, nullptr, wcex.hInstance, nullptr);

	if (m_window == nullptr)
		return false;

	// Describe swap chain
	DXGI_SWAP_CHAIN_DESC1 swd{};
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fswd{};

	swd.Width = 1920;
	swd.Height = 1080;
	swd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swd.Stereo = false;
	swd.SampleDesc.Count = 1; // 1 pixel per pixel sampling
	swd.SampleDesc.Quality = 0; // NO MSAA
	swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT; // be able to write it on the gpu using back buffer and output the result to the Render target
	swd.BufferCount = GetFrameCount(); // 2 buffers, one is displayed and the other is written to and they are exchanged, 3 buffers for vsync usage
	swd.Scaling = DXGI_SCALING_STRETCH;
	swd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	fswd.Windowed = true;

	// Swap chain creation
	auto& factory = DXContext::Get().GetDXGIFactory();
	ComPointer<IDXGISwapChain1> sc1;

	factory->CreateSwapChainForHwnd(DXContext::Get().GetCommandQueue(), m_window, &swd, &fswd, nullptr, &sc1);

	if (!sc1.QueryInterface(m_swapChain))
	{
		return false;
	}

	// Get buffers
	if (!GetBuffers())
	{
		return false;
	}

	return true;
}

void DXWindow::Shutdown()
{
	ReleaseBuffers();

	m_swapChain.Release();

	if (m_window)
	{
		DestroyWindow(m_window);
	}

	if (m_wndClass)
	{
		UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandleW(nullptr));
	}
}

void DXWindow::Update()
{
	MSG msg;
	while (PeekMessageW(&msg, m_window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void DXWindow::Present()
{
	m_swapChain->Present(1, 0);
}

void DXWindow::Resize()
{
	ReleaseBuffers();

	RECT clientRect;
	// Get the window client rect(width,height)
	if (GetClientRect(m_window, &clientRect))
	{
		m_width = clientRect.right - clientRect.left;
		m_height = clientRect.bottom - clientRect.top;

		m_swapChain->ResizeBuffers(GetFrameCount(), m_width, m_height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING); //DXGI_FORMAT_UNKNOWN keeps the old format
		m_shouldResize = false;
	}

	GetBuffers();
}

void DXWindow::SetFullscreen(bool enabled)
{
	// Update window styling
	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	DWORD exStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;

	if (enabled)
	{
		style = WS_POPUP | WS_VISIBLE;
		exStyle = WS_EX_APPWINDOW;
	}

	SetWindowLongW(m_window, GWL_STYLE, style);
	SetWindowLongW(m_window, GWL_EXSTYLE, exStyle);
	
	// Adjust window size
	if (enabled)
	{
		HMONITOR monitor = MonitorFromWindow(m_window, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo{};
		monitorInfo.cbSize = sizeof(monitorInfo);
		if (GetMonitorInfoW(monitor, &monitorInfo))
		{
			SetWindowPos(m_window, nullptr, 
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_NOZORDER
			);
		}
	}
	else
	{
		ShowWindow(m_window, SW_MAXIMIZE);
	}

	m_isFullscreen = enabled;
}

bool DXWindow::GetBuffers()
{
	for (size_t i = 0; i < FrameCount; ++i)
	{
		if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i]))))
		{
			return false;
		}
	}

	return true;
}

void DXWindow::ReleaseBuffers()
{
	for (size_t i = 0; i < FrameCount; ++i)
	{
		m_buffers[i].Release();
	}
}

LRESULT CALLBACK DXWindow::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
			if (wParam == VK_F11)
			{
				Get().SetFullscreen(!Get().IsFullscreen());
			}
			break;
		case WM_SIZE:
			// lParam is 0 if window is minimized
			if (lParam && HIWORD(lParam) != Get().m_height || LOWORD(lParam) != Get().m_width)
			{
				Get().m_shouldResize = true;
			}
			break;
		case WM_CLOSE:
			Get().m_shouldClose = true;
			return 0;
	}

	return DefWindowProcW(wnd, msg, wParam, lParam);
}
