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
		DXWindow::Get().SetFullscreen(true);
		while (!DXWindow::Get().ShouldClose())
		{
			// Process pending window events/message
			DXWindow::Get().Update();

			// Handle window resizing
			if (DXWindow::Get().ShouldResize())
			{
				DXContext::Get().Flush(DXWindow::GetFrameCount());
				DXWindow::Get().Resize();
			}

			// Begin drawing
			auto* cmdList = DXContext::Get().InitCommandList();

			// Draw stuff

			// Finish drawing and present buffer to the screen
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