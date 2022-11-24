#pragma once

#include <d3d11.h>	// マイクロソフト提供のDirectX11ヘッダーファイルをインクルード

// COMオブジェクトをNULLチェックしてからReleaseする
#define COM_SAFE_RELEASE(o)   if(NULL!=o){o->Release();o=NULL;}

class CDirect3D
{
public:
	~CDirect3D();

	//=============================
	// 取得関係
	//=============================

	// Direct3D11 Device*を返す
	ID3D11Device* GetDevice();

	// Direct3D11 Immidiate Context*を返す
	ID3D11DeviceContext* GetContext();

	// Direct3D11 SwapChain*を返す
	IDXGISwapChain* GetSwapChain();

	// Direct3D11 RenderTargetView*を返す
	ID3D11RenderTargetView* GetRenderTargetView();

	//=============================
	// 処理関係
	//=============================

	// DirextX11の初期化を担当する関数
	HRESULT Initialize(HWND hWnd);

	// Direct3D関連で作成したオブジェクトをすべて開放する
	void Release();

	// 指定したファイル名のテクスチャファイル（画像ファイル）を読み込む
	HRESULT LoadTexture(const char* pFileName, ID3D11ShaderResourceView** pOupSRV);

	void GetScreenSize(UINT* width, UINT* height);
	void GetScreenSize(float* width, float* height);

private:
	// DirectX関連
	ID3D11Device*           mp_d3d11_device;
	ID3D11DeviceContext*    mp_immediatecontext;
	IDXGISwapChain*         mp_swapchain;
	ID3D11RenderTargetView* mp_render_target_view;
	ID3D11Texture2D*        mp_depth_stencil_texture;
	ID3D11DepthStencilView* mp_depth_stencil_view;
	ID3D11InputLayout*      mp_input_layout;
	ID3D11VertexShader*     mp_vertex_shader;
	ID3D11PixelShader*      mp_pixel_shader;
	ID3D11SamplerState*     mp_sampler;
	ID3D11BlendState*	    mp_blend_alpha_enable;

	// スクリーンサイズ
	UINT mp_screen_width;	// 横幅
	UINT mp_screen_height;	// 縦幅

	HWND m_hwnd;				// ウィンドウハンドル

};

