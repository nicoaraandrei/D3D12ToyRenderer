#pragma once

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>

class DXDebugLayer
{
	public:
		bool Init();
		void Shutdown();

	private:
		#ifdef _DEBUG
		ComPointer<ID3D12Debug> m_d3d12Debug;
		ComPointer<IDXGIDebug1> m_dxgiDebug;
		#endif

	// Singleton debug layer
	public:
		DXDebugLayer(const DXDebugLayer&) = delete;
		DXDebugLayer& operator=(const DXDebugLayer&) = delete;

		inline static DXDebugLayer& Get()
		{
			static DXDebugLayer instance;
			return instance;
		}
	private:
		DXDebugLayer() = default;
};