#include <iostream>

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>

#include <DxDebug/DXDebugLayer.h>

int main()
{
	DXDebugLayer::Get().Init();

	DXDebugLayer::Get().Shutdown();
}