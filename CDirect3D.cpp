// ライブラリのリンクを指定するプリプロセッサ命令
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")	
#pragma comment(lib, "DirectXTex.lib")

#include "CDirect3D.h"
#include "directxtex/include/WICTextureLoader.h"
#include "directxtex/include/DirectXTex.h"
#include <locale.h>

// 実行するPCによって下のパターンを1または2で試す。2の方がサポート率が高い？
#define DIRECT3D_INIT_PATTERN  2

#include "vs.h"
#include "ps.h"

CDirect3D::~CDirect3D()
{
	this->Release();
}

ID3D11Device * CDirect3D::GetDevice()
{
	return this->mp_d3d11_device;;
}

ID3D11DeviceContext * CDirect3D::GetContext()
{
	return this->mp_immediatecontext;
}

IDXGISwapChain * CDirect3D::GetSwapChain()
{
	return this->mp_swapchain;
}

ID3D11RenderTargetView * CDirect3D::GetRenderTargetView()
{
	return this->mp_render_target_view;
}

HRESULT CDirect3D::Initialize(HWND hWnd)
{
	HRESULT  hr;

	D3D_FEATURE_LEVEL level;
	RECT                rect;

	GetClientRect(hWnd, &rect);
	this->mp_screen_width = (UINT)(rect.right - rect.left);
	this->mp_screen_height = (UINT)(rect.bottom - rect.top);

	this->m_hwnd = hWnd;

#if DIRECT3D_INIT_PATTERN==1
	// DirectX11デバイスの作成　※デバイスはDirectX11の様々な機能を呼び出すためのオブジェクト
	UINT flags = 0;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL pLevels[] = { D3D_FEATURE_LEVEL_11_0 };


	DXGI_SWAP_CHAIN_DESC scDesc;


	ZeroMemory(&scDesc, sizeof(scDesc));



	scDesc.BufferCount = 1;
	scDesc.BufferDesc.Width = this->mp_screen_width;
	scDesc.BufferDesc.Height = this->mp_screen_height;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.OutputWindow = hWnd;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Windowed = TRUE;

	// DirectX11デバイスの作成　※デバイスはDirectX11の様々な機能を呼び出すためのオブジェクト
	hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		pLevels,
		1,
		D3D11_SDK_VERSION,
		&scDesc,
		&this->mp_swapchain,
		&this->mp_d3d11_device,
		&level,
		&this->mp_immediatecontext);

	if (FAILED(hr))
		return hr;

#elif DIRECT3D_INIT_PATTERN==2

	IDXGIFactory* factory;				// factory
	IDXGIAdapter* adapter;				// videocard
	IDXGIOutput* adapterOutput;			// monitor
	unsigned int numModes;
	unsigned int numerator = 60;			// 分子
	unsigned int denominator = 1;			// 分母
	DXGI_MODE_DESC* displayModeList;
	//	D3D11_BLEND_DESC blendStateDescription;

	//	mp_screen_width = Width;
	//	mp_screen_height = Height;

		// ドライバの種類
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,					// Windows Advanced Rasterizer
		D3D_DRIVER_TYPE_REFERENCE,
		D3D_DRIVER_TYPE_SOFTWARE,
	};
	unsigned int numDriverTypes = ARRAYSIZE(driverTypes);

	unsigned int createDeviceFlags = 0;
	//   createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

	  // 機能レベル
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,					// DirectX11.1対応GPUレベル
		D3D_FEATURE_LEVEL_11_0,					// DirectX11対応GPUレベル
		D3D_FEATURE_LEVEL_10_1,					// DirectX10.1対応GPUレベル
		D3D_FEATURE_LEVEL_10_0,					// DirectX10対応GPUレベル
		D3D_FEATURE_LEVEL_9_3,					// DirectX9.3対応GPUレベル
		D3D_FEATURE_LEVEL_9_2,					// DirectX9.2対応GPUレベル
		D3D_FEATURE_LEVEL_9_1					// Direct9.1対応GPUレベル
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	// Create a DirectX graphics interface factory
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr)) {
		return hr;
	}

	// use the factory to create an adpter for the primary graphics interface(video card)
	hr = factory->EnumAdapters(0, &adapter);
	if (FAILED(hr)) {
		return hr;
	}

	// enumerrate primary adapter output(monitor)
	hr = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(hr)) {
		return hr;
	}

	// get the number of modes that fit the DXGI_FORMAT_R8G8B8_UNORM display format forthe adapter output(monitor)
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(hr)) {
		return hr;
	}

	// create alist to hold all possible display modes for this monitor/video card combination
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) {
		return E_FAIL;
	}

	// now fill the display mode list structures
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(hr)) {
		return hr;
	}

	// now go through all the display modes and find the one that matches the screen width and height
	// when a match is found store the numerator and denominator of the refresh rate for that monitor
	for (unsigned int i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == this->mp_screen_width) {
			if (displayModeList[i].Height == this->mp_screen_height) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	delete[] displayModeList;
	displayModeList = NULL;

	adapterOutput->Release();
	adapterOutput = NULL;

	adapter->Release();
	adapter = NULL;

	factory->Release();
	factory = NULL;

	// スワップチェインの設定
	// スワップチェインとは、ウインドウへの表示ダブルバッファを管理する
	// クラス　マルチサンプリング、リフレッシュレートが設定できる
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));					// ゼロクリア
	sd.BufferCount = 1;									// バックバッファの数
	sd.BufferDesc.Width = this->mp_screen_width;						// バックバッファの幅
	sd.BufferDesc.Height = this->mp_screen_height;						// バックバッファの高さ
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// バックバッファフォーマット(R,G,B 範囲０．０から１．０)
	sd.BufferDesc.RefreshRate.Numerator = numerator;			// リフレッシュレート（分母）
	sd.BufferDesc.RefreshRate.Denominator = denominator;			// リフレッシュレート（分子）
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バックバッファの使用方法
	sd.OutputWindow = hWnd;			// 関連付けるウインドウ
	sd.SampleDesc.Count = 1;		// マルチサンプルの数
	sd.SampleDesc.Quality = 0;		// マルチサンプルのクオリティ
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// ウインドウモード・フルスクリーンモード切替用。
	// グローバル変数にするなどして、動的に切り替えられるようにするのも良い。
	BOOL fullscreen = FALSE;

	if (fullscreen) {
		sd.Windowed = FALSE;				// ウインドウモード
	}
	else {
		sd.Windowed = TRUE;				// ウインドウモード
	}
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;		// モードの自動切り替え

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		//m_DriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL,				// ディスプレイデバイスのアダプタ（ＮＵＬＬの場合最初に見つかったアダプタ）
			driverTypes[driverTypeIndex],		// デバイスドライバのタイプ
			NULL,				// ソフトウェアラスタライザを使用する場合に指定する
			createDeviceFlags,	// デバイスフラグ
			featureLevels,		// 機能レベル
			numFeatureLevels,	// 機能レベル数
			D3D11_SDK_VERSION,	// 
			&sd,				// スワップチェインの設定
			&this->mp_swapchain,		// IDXGIDwapChainインタフェース	
			&this->mp_d3d11_device,		// ID3D11Deviceインタフェース
			&level,	// サポートされている機能レベル
			&this->mp_immediatecontext);	// デバイスコンテキスト
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr)) {
		return hr;
	}

#endif

	// バックバッファ関係の設定処理
	ID3D11Texture2D* pBackBuffer;

	hr = GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
		return hr;

	hr = GetDevice()->CreateRenderTargetView(pBackBuffer, NULL, &this->mp_render_target_view);
	pBackBuffer->Release();

	if (FAILED(hr))
		return hr;

	//深度ステンシルバッファ作成
	D3D11_TEXTURE2D_DESC txDesc;

	ZeroMemory(&txDesc, sizeof(txDesc));
	txDesc.Width = this->mp_screen_width;
	txDesc.Height = this->mp_screen_height;
	txDesc.MipLevels = 1;
	txDesc.ArraySize = 1;
	txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	txDesc.SampleDesc.Count = 1;
	txDesc.SampleDesc.Quality = 0;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = 0;

	hr = GetDevice()->CreateTexture2D(&txDesc, NULL, &this->mp_depth_stencil_texture);

	if (SUCCEEDED(hr)) {
		D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));
		dsDesc.Format = txDesc.Format;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Texture2D.MipSlice = 0;
		hr = GetDevice()->CreateDepthStencilView(this->mp_depth_stencil_texture, &dsDesc, &this->mp_depth_stencil_view);
	}

	if (FAILED(hr))
		return hr;

	// レンダリングターゲットを設定　（注）今は2DなのでZバッファ無効にしておく
	GetContext()->OMSetRenderTargets(1, &this->mp_render_target_view, /*dx->pDepthStencilView*/NULL);

	//頂点シェーダー生成
	hr = GetDevice()->CreateVertexShader(&g_vs_main, sizeof(g_vs_main), NULL, &this->mp_vertex_shader);

	if (FAILED(hr))
		return hr;

	//ピクセルシェーダー生成
	hr = GetDevice()->CreatePixelShader(&g_ps_main, sizeof(g_ps_main), NULL, &this->mp_pixel_shader);

	if (FAILED(hr))
		return hr;

	//頂点レイアウト作成
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
	 { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//頂点レイアウト作成
	hr = GetDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc),
		g_vs_main, sizeof(g_vs_main), &this->mp_input_layout);

	if (FAILED(hr))
		return hr;

	//ビューポート設定
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)this->mp_screen_width;
	viewport.Height = (FLOAT)this->mp_screen_height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	GetContext()->RSSetViewports(1, &viewport);

	// テクスチャ・サンプラーの作成
	D3D11_SAMPLER_DESC smpDesc;

	ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));
	smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

	hr = GetDevice()->CreateSamplerState(&smpDesc, &this->mp_sampler);

	if (FAILED(hr))
		return hr;

	// 頂点シェーダーのパラメータセット
	GetContext()->VSSetShader(this->mp_vertex_shader, NULL, 0);

	// ピクセルシェーダーのパラメータセット
	GetContext()->PSSetShader(this->mp_pixel_shader, NULL, 0);
	GetContext()->PSSetSamplers(0, 1, &this->mp_sampler);

	// 頂点レイアウトとプリミティブタイプをセット
	GetContext()->IASetInputLayout(this->mp_input_layout);
	GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ブレンドステート設定
	D3D11_BLEND_DESC BlendDesc;

	// テクスチャ透過処理が行われるように設定
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	hr = GetDevice()->CreateBlendState(&BlendDesc, &this->mp_blend_alpha_enable);

	if (SUCCEEDED(hr)) {
		GetContext()->OMSetBlendState(this->mp_blend_alpha_enable, blendFactor, 0xffffffff);
	}

	if (FAILED(hr))
		return hr;

	return hr;
}

void CDirect3D::Release()
{
	COM_SAFE_RELEASE(this->mp_blend_alpha_enable);
	COM_SAFE_RELEASE(this->mp_swapchain);
	COM_SAFE_RELEASE(this->mp_render_target_view);
	COM_SAFE_RELEASE(this->mp_depth_stencil_texture);
	COM_SAFE_RELEASE(this->mp_depth_stencil_view);
	COM_SAFE_RELEASE(this->mp_input_layout);
	COM_SAFE_RELEASE(this->mp_vertex_shader);
	COM_SAFE_RELEASE(this->mp_pixel_shader);
	COM_SAFE_RELEASE(this->mp_sampler);
	COM_SAFE_RELEASE(this->mp_immediatecontext);
	COM_SAFE_RELEASE(this->mp_d3d11_device);
}

HRESULT CDirect3D::LoadTexture(const char * pFileName, ID3D11ShaderResourceView ** pOupSRV)
{
	wchar_t wc_path[256];

	char* localeInfo = setlocale(LC_ALL, "Japanese_Japan.932");
	mbstowcs_s(NULL, wc_path, 256, pFileName, 255);

	return DirectX::CreateWICTextureFromFile(this->GetDevice(), wc_path, NULL, pOupSRV);
}

void CDirect3D::GetScreenSize(UINT * width, UINT * height)
{
	*width = this->mp_screen_width;
	*height = this->mp_screen_height;
}

void CDirect3D::GetScreenSize(float * width, float * height)
{
	*width = static_cast<float>(this->mp_screen_width);
	*height = static_cast<float>(this->mp_screen_height);
}

