#pragma once

#include <d3d11.h>	// �}�C�N���\�t�g�񋟂�DirectX11�w�b�_�[�t�@�C�����C���N���[�h

// COM�I�u�W�F�N�g��NULL�`�F�b�N���Ă���Release����
#define COM_SAFE_RELEASE(o)   if(NULL!=o){o->Release();o=NULL;}

class CDirect3D
{
public:
	~CDirect3D();

	//=============================
	// �擾�֌W
	//=============================

	// Direct3D11 Device*��Ԃ�
	ID3D11Device* GetDevice();

	// Direct3D11 Immidiate Context*��Ԃ�
	ID3D11DeviceContext* GetContext();

	// Direct3D11 SwapChain*��Ԃ�
	IDXGISwapChain* GetSwapChain();

	// Direct3D11 RenderTargetView*��Ԃ�
	ID3D11RenderTargetView* GetRenderTargetView();

	//=============================
	// �����֌W
	//=============================

	// DirextX11�̏�������S������֐�
	HRESULT Initialize(HWND hWnd);

	// Direct3D�֘A�ō쐬�����I�u�W�F�N�g�����ׂĊJ������
	void Release();

	// �w�肵���t�@�C�����̃e�N�X�`���t�@�C���i�摜�t�@�C���j��ǂݍ���
	HRESULT LoadTexture(const char* pFileName, ID3D11ShaderResourceView** pOupSRV);

	void GetScreenSize(UINT* width, UINT* height);
	void GetScreenSize(float* width, float* height);

private:
	// DirectX�֘A
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

	// �X�N���[���T�C�Y
	UINT mp_screen_width;	// ����
	UINT mp_screen_height;	// �c��

	HWND m_hwnd;				// �E�B���h�E�n���h��

};

