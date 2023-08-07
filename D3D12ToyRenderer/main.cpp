#include <iostream>

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>
#include <Support/Window.h>

#include <DxDebug/DXDebugLayer.h>
#include <D3D/DxContext.h>

int main()
{
	DXDebugLayer::Get().Init();

	if (DXContext::Get().Init() && DXWindow::Get().Init())
	{
		while (!DXWindow::Get().ShouldClose())
		{
			DXWindow::Get().Update();

			auto* cmdList = DXContext::Get().InitCommandList();

			DXContext::Get().ExecuteCommandList();
			DXWindow::Get().Present();
		}
		// Flush the command queue before releasing swapchain
		DXContext::Get().Flush(DXWindow::GetFrameCount());

		DXWindow::Get().Shutdown();
		DXContext::Get().Shutdown();
	}

	DXDebugLayer::Get().Shutdown();
}