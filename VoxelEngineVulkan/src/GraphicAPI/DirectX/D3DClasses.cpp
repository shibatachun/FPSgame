#include "D3DClasses.h"

D3D11::D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}

D3D11::D3DClass::D3DClass(const D3DClass&)
{
}

D3D11::D3DClass::~D3DClass()
{
}

bool D3D11::D3DClass::Initialize(int screenWidth, int scrfeenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{

	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;


	// Store the vsync setting.
	m_vsync_enabled = vsync;
	return false;
}

void D3D11::D3DClass::Shutdown()
{
}

void D3D11::D3DClass::BeginScene(float, float, float, float)
{
}

void D3D11::D3DClass::EndScene()
{
}

ID3D11Device* D3D11::D3DClass::GetDevice()
{
	return nullptr;
}

ID3D11DeviceContext* D3D11::D3DClass::GetDeviceContext()
{
	return nullptr;
}

void D3D11::D3DClass::GetProjectionMatrix(XMMATRIX&)
{
}

void D3D11::D3DClass::GetWorldMatrix(XMMATRIX&)
{
}

void D3D11::D3DClass::GetOrthoMatrix(XMMATRIX&)
{
}

void D3D11::D3DClass::GetVideoCardInfo(char*, int&)
{
}

void D3D11::D3DClass::SetBackBufferRenderTarget()
{
}

void D3D11::D3DClass::ResetViewport()
{
}

