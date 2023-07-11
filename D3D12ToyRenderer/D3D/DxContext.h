#pragma once

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>

class DXContext
{
public:
	bool Init();
	void Shutdown();

	inline ComPointer<ID3D12Device>& GetDevice()
	{
		return m_device;
	}

	inline ComPointer<ID3D12CommandQueue> GetCommandQueue()
	{
		return m_cmdQueue;
	}

private:
	ComPointer<ID3D12Device> m_device;
	ComPointer<ID3D12CommandQueue> m_cmdQueue;

	ComPointer<ID3D12Fence> m_fence;
	UINT64 m_fenceValue = 0;

	// Singleton DirectX context
public:
	DXContext(const DXContext&) = delete;
	DXContext& operator=(const DXContext&) = delete;

	inline static DXContext& Get()
	{
		static DXContext instance;
		return instance;
	}
private:
	DXContext() = default;
};