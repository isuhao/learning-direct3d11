#include "stdafx.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#pragma comment(lib, "D3D10_1.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "dwrite.lib")

#include <windows.h>
#include "Resource.h"
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>
//ȫ��������
IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;

//��������
ID3D11Buffer* squareIndexBuffer;

//���ֵ-20170927
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;

//��ɫ��
ID3D11Buffer* squareVertBuffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;
ID3D11InputLayout* vertLayout;

///
ID3D11Buffer* cbPerObjectBuffer;
ID3D11BlendState* Transparency;
ID3D11RasterizerState* CCWcullMode;
ID3D11RasterizerState* CWcullMode;
ID3D11ShaderResourceView* CubesTexture;
ID3D11SamplerState* CubesTexSamplerState;

////*****new**********///
ID3D11Buffer* cbPerFrameBuffer;
ID3D11PixelShader* D2D_PS;
ID3D10Blob* D2D_PS_Buffer;
///**new******

// �����߿�
//ID3D11RasterizerState* WireFrame;

//�޲ü�
//ID3D11RasterizerState* noCull;
#if 0
//�������
ID3D11BlendState* Transparency;
ID3D11RasterizerState* CCWcullMode;
ID3D11RasterizerState* CWcullMode;

//float red = 0.0f;
//float green = 0.0f;
//float blue = 0.0f;
//int colormodr = 1;
//int colormodg = 1;
//int colormodb = 1;
#endif

ID3D10Device1 *d3d101Device;
IDXGIKeyedMutex *keyedMutex11;
IDXGIKeyedMutex *keyedMutex10;
ID2D1RenderTarget *D2DRenderTarget;
ID2D1SolidColorBrush *Brush;
ID3D11Texture2D *BackBuffer11;
ID3D11Texture2D *sharedTex11;
ID3D11Buffer *d2dVertBuffer;
ID3D11Buffer *d2dIndexBuffer;
ID3D11ShaderResourceView *d2dTexture;
IDWriteFactory *DWriteFactory;
IDWriteTextFormat *TextFormat;
std::wstring printText;
/////
LPCTSTR WndClassName = "firstwindow";
HWND hwnd = NULL;
HRESULT hr;

const int Width = 800; //���ÿ�
const int Height = 800; // ���ø�

///�ĸ��ռ��Լ��������
XMMATRIX WVP;
//������
XMMATRIX cube1World;
XMMATRIX cube2World;
//
//XMMATRIX World;
XMMATRIX camView;
XMMATRIX camProjection;

XMMATRIX d2dWorld;
XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;

//���
XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
float rot = 0.01f;

///////////////**************new**************////////////////////
double countsPerSecond = 0.0;
__int64 CounterStart = 0;

int frameCount = 0;
int fps = 0;

__int64 frameTimeOld = 0;
double frameTime;
///////////////**************new**************////////////////////
//Function Prototypes//
bool InitializeDirect3d11App(HINSTANCE hInstance);
void CleanUp();
bool InitScene();
void DrawScene();
bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter);
void InitD2DScreenTexture();
///////////////**************new**************////////////////////
void UpdateScene(double time);
//void UpdateScene();
void RenderText(std::wstring text, int inInt);
//void RenderText(std::wstring text);

void StartTimer();
double GetTime();
double GetFrameTime();


// ��ʼ������
bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed);

//��ʼ����Ϣѭ������
int messageloop();
//��ʼ�����ڻص����̡�Windows API���¼������͵ı��ģ�͡��ڸú����в���Windows��Ϣ������һ���������£�Ҳ���¼����Լ�����������̡�

LRESULT CALLBACK WndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

///new
//����Ч����������Ľṹ��
struct cbPerObject
{
	XMMATRIX WVP;
	XMMATRIX World;

};

cbPerObject cbPerObj;

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 dir;
	float pad;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;

};
Light light;

struct cbPerFrame
{
	Light light;
};

cbPerFrame constbuffPerFrame;


//����ṹ���Լ����㲼�֣����벼�֣�

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		:pos(x, y, z), texCoord(u, v), normal(nx, ny, nz){}
	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
UINT numElements = ARRAYSIZE(layout);

//������������Ӧ�ó�����hInstance,ǰһ��Ӧ�ó�����hPrevInstance���������������������lpCmdLine�Լ�������ʾ��ʽ��nShowCmd
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	//������ע�ᴰ��
	if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true))
	{
		MessageBox(0, "Window Initialization - Failed", "Error", MB_OK);
		return 0;
	}

	/////new
	if (!InitializeDirect3d11App(hInstance)) // ��ʼ��D3D
	{
		MessageBox(0, "Direct3D Initialization - Failed", "Error", MB_OK);
		return 0;
	}

	if (!InitScene())
	{
		MessageBox(0, "Scene Initialization - Failed", "Error", MB_OK);
		return 0;
	}
	
	messageloop();
	CleanUp();
	//ReleaseObjects();

	return 0;
}
// windowed ��Ϊtrue��Ϊ����ģʽ��ʾ����Ϊfalse��Ϊȫ��ģʽ��ʾ
bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed)
{
	/*typedef struct _WNDCLASS{
		UINT cbSize;
		UINT style;
		WNDPROC lpfnWndProc;
		int cbClsExtra;
		int cbWndExtra;
		HANDLE hInstance;
		HICON hIcon;
		HCURSOR hCursor;
		HBRUSH hbrBackground;
		LPCTSTR lpszMenuName;
		LPCTSTR lpszClassName;
	}WNDCLASS;
	*/
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX); //window��Ĵ�С
	/********windows����
	*CS_CLASSDC һ��ʹ�ø��ഴ���������д��ڼ乲����豸������
	*CS_DBLCLKS �ڴ�����ʹ��˫������
	*CS_HREDRAW �����ڵĿ���иı���ߴ���ˮƽ���ƶ������ڽ���ˢ��
	*CS_NOCLOSE ���ڲ˵��Ͻ�ֹ�ر�ѡ��
	*CS_OWNDC   Ϊÿ�����ڴ����Լ����豸�����ġ�������CS_CLASSDC�෴
	*CS_PARENTDC ������ô������Ӵ��ڵļ����ı��ε������ڣ��������Ӵ����ܹ��ڸ������ϻ滭
	*CS_VERDRAW ���ڴ��ڵĸ߶Ȼ򴰿��ڴ�ֱ�������ƶ����ڻ��ػ�
	**/
	wc.style = CS_HREDRAW | CS_VREDRAW;
	//lpfnWndProc��һ��ָ��������Ϣ������ָ�룬���ô��ڴ������ĺ�����WndProc
	wc.lpfnWndProc = WndProc;
	//cbClsExtra��WNDCLASSEX֮�����������ֽ���
	wc.cbClsExtra = NULL;
	//cbWndExtraָ������ʵ��֮����������ֽ���
	wc.cbWndExtra = NULL;
	//��ǰ����Ӧ�ó���ľ����ͨ��������GetModuleHandle()������һ����������NULL�ɻ�ȡ��ǰ����Ӧ�ó���
	wc.hInstance = hInstance;

	//hIcon����ָ�����ڱ��������Ͻǵ�ͼ�ꡣ������һЩ��׼ͼ�꣺
	/*
	*IDI_APPLICATION Ĭ��Ӧ�ó���ͼ��
	*IDI_HAND ����״��ͼ��
	*IDI_EXCLAMATION ��̾��ͼ��
	*IDI_INFORMATION �Ǻ�ͼ��
	*IDI_QUESTION �ʺ�ͼ��
	*IDI_WINLOGO ��ʹ�õ���XP����Ĭ��Ӧ�ó���ͼ�꣬�����Ǵ���logo
	*/
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	/*������ͼ��
	*IDC_APPSTARTING ��׼��ͷ�Լ�С��ɳ©���
	*IDC_ARROW ��׼��ͷ���
	*IDC_CROSS ʮ���߹��
	*IDC_HAND ���͹��
	*IDC_NO б��Ȧ���
	*IDC_WAIT ɳ©���
	*/
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//hbrBackground��һ��ˢ�ӵľ������ʹ�ñ�����ɫ��
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	//���ӵ����ڵĲ˵����֣�����Ҫ�Ļ�����ΪNULL
	wc.lpszMenuName = NULL;
	//�����������
	wc.lpszClassName = WndClassName;
	//ָ����������ͼ�꣬ʹ�������IDI��ͼ��
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	//ע���ࡣ��ʧ�������һ���������ɹ����������������
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Error registering class", "Error", MB_OK | MB_ICONERROR);
		return 1;
	}
	//��������
	hwnd = CreateWindowEx(
		NULL, 
		WndClassName, 
		"Rotating Cube", 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		width,
		height, 
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hwnd)
	{
		MessageBox(NULL, "Error registering class", "Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	//BOOL ShowWindow(HWND hWnd, int nCmdShow);
	//BOOL UpdateWindow(HWND hWnd);

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);// ����WM_PAINT��Ϣ�����ڹ��̣������ڿͻ���û���κζ���Ҫ��ʾ���򲻷�����Ϣ������true���������е�mainloop��ȥ��

	return true;
}

bool InitializeDirect3d11App(HINSTANCE hInstance)
{
	//��������
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//����������
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//����DXGI factory��ö���Կ�
	IDXGIFactory1 *DXGIFactory;
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&DXGIFactory);

	//ʹ�õ�һ���Կ�
	IDXGIAdapter1 *Adapter;
	hr = DXGIFactory->EnumAdapters1(0, &Adapter);
	DXGIFactory->Release();

	//����D3D11�豸�ͽ�����
	//hr = D3D11C

	//����������
	D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, 
	NULL, NULL,	D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);

	//��ʼ��D2D D3D10.1��DirectWrite
	InitD2D_D3D101_DWrite(Adapter);

	//�ͷ�Adapter�ӿ�
	Adapter->Release();

	//�����󻺳�
	ID3D11Texture2D* BackBuffer;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

	//������ȾĿ��
	d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
	BackBuffer->Release();

	//�������ģ�建��
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = Width;
	depthStencilDesc.Height = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; //�󶨵�OM
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//�������ģ����ͼ
	d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);


	return true;
}

bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter)
{
	//����D3D101�豸
	hr = D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &d3d101Device);

	//������������D3D101������Ⱦ��
	D3D11_TEXTURE2D_DESC sharedTexDesc;
	ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));

	sharedTexDesc.Width = Width;
	sharedTexDesc.Height = Height;
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// DXGI_FORMAT_R8G8B8A8_UNORM;// DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	hr = d3d11Device->CreateTexture2D(&sharedTexDesc, NULL, &sharedTex11);
	
	//Ϊ���������ȡkey��������ΪD3D11��
	hr = sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&keyedMutex11);

	//��ȡ��������Ҫ��D3D10.1�д򿪹�������
	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;

	hr = sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void **)&sharedResource10);
	hr = sharedResource10->GetSharedHandle(&sharedHandle10);
	sharedResource10->Release();

	//��D3D10.1��Ϊ��������򿪽���
	IDXGISurface1 *sharedSurface10;
	hr = d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void **)(&sharedSurface10));
	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&keyedMutex10);

	//����D2D factory
	ID2D1Factory *D2DFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void **)&D2DFactory);
	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;
	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
	hr = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &D2DRenderTarget);

	sharedSurface10->Release();
	D2DFactory->Release();

	//���������ɫ���ʻ���һЩ����
	hr = D2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &Brush);

	//DirectWrite
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), 
	reinterpret_cast<IUnknown **>(&DWriteFactory));
	hr = DWriteFactory->CreateTextFormat(
		L"Script",
		NULL,
        DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"en-us",
        &TextFormat
		);
    hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    d3d101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);    
	return true;
}

void CleanUp()
{
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();
	renderTargetView->Release();

	squareVertBuffer->Release();
	squareIndexBuffer->Release();

	//triangleVertBuffer->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	vertLayout->Release();

	depthStencilView->Release();
	depthStencilBuffer->Release();

	//
	cbPerObjectBuffer->Release();

	//�ͷŲ��ü�����
//	noCull->Release();
	//�ͷŻ�϶���
#if 1
	Transparency->Release();
	CCWcullMode->Release();
	CWcullMode->Release();
#endif	
	//�ͷ��߿�
	//WireFrame->Release();

	d3d101Device->Release();
	keyedMutex11->Release();
	keyedMutex10->Release();
	D2DRenderTarget->Release();
	Brush->Release();
//	BackBuffer11->Release();
	sharedTex11->Release();
	DWriteFactory->Release();
    TextFormat->Release();
	d2dTexture->Release();

	/// new
	cbPerFrameBuffer->Release();
}


void InitD2DScreenTexture()
{
	//�������㻺��
	Vertex v[] =
	{
		//������
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f),
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f),
		Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),
	};

	DWORD indices[] = {
		//������
		0, 1, 2,
		0, 2, 3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &d2dIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d2dVertBuffer);

	//������D2D������һ����ɫ����Դ��ͼ
	//��ˣ��ܹ�ʹ����������һ�������������ڸ��ǳ���
	d3d11Device->CreateShaderResourceView(sharedTex11, NULL, &d2dTexture);

}

//void ReleaseObjects()
//{
//�ͷŴ�����COM����
//	SwapChain->Release();
//	d3d11Device->Release();
//	d3d11DevCon->Release();
//}

bool InitScene()
{
	//
	InitD2DScreenTexture();
	//������ɫ��
	hr = D3DX11CompileFromFile("Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile("Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);
	/// new
	hr = D3DX11CompileFromFile("Effects.fx", 0, 0, "D2D_PS", "ps_4_0", 0, 0, 0, &D2D_PS_Buffer, 0, 0);

	//������ɫ������
	hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
	///new
	hr = d3d11Device->CreatePixelShader(D2D_PS_Buffer->GetBufferPointer(), D2D_PS_Buffer->GetBufferSize(), NULL, &D2D_PS);

	//���ö����������ɫ��
	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);

	light.dir = XMFLOAT3(0.25f, 0.25f, -1.0f);
	light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//�������㻺��
	Vertex v[] = 
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f),
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f),
		Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),

		// Back Face
		Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f),
		Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f),
		Vertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f),
		Vertex(-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f),

		// Top Face
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, -1.0f),
		Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f),
		Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
		Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f),

		// Bottom Face
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f),
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
		Vertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f),
		Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f),

		// Left Face
		Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f,1.0f),
		Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f),
		Vertex(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, -1.0f),
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f),

		// Right Face
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
		Vertex(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f),
		Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
		Vertex(1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f),

	};

	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 24;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

	//���ö��㻺��
//	UINT stride = sizeof(Vertex);
//	UINT offset = 0;
//	d3d11DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &stride, &offset);

	//�������벼��
	d3d11Device->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
		VS_Buffer->GetBufferSize(), &vertLayout);

	//�������벼��
	d3d11DevCon->IASetInputLayout(vertLayout);
	
	//����ͼԪ����
	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�����ӿ�
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;
	
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//�����ӿ�
	d3d11DevCon->RSSetViewports(1, &viewport);

	//���������������͵�Ч���ļ���cbuffer
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	//������������ÿ֡����cbuffer����ɫ���ļ�
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrame);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);

	//�����Ϣ
	//�����Ϣ
	camPosition = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
	//camPosition = XMVectorSet(0.0f, 0.0f, -0.5f, 0.0f);
	camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//������ͼ����
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);

	//����ͶӰ����
	camProjection = XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)Width / Height, 1.0f, 1000.0f);

	D3D11_BLEND_DESC blendDesc;
    ZeroMemory( &blendDesc, sizeof(blendDesc) );

    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory( &rtbd, sizeof(rtbd) );

    rtbd.BlendEnable             = true;
    rtbd.SrcBlend                 = D3D11_BLEND_SRC_COLOR;
    ///////////////**************new**************////////////////////
    rtbd.DestBlend                 = D3D11_BLEND_INV_SRC_ALPHA;
    ///////////////**************new**************////////////////////
    rtbd.BlendOp                 = D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha             = D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha             = D3D11_BLEND_ZERO;
    rtbd.BlendOpAlpha             = D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask     = D3D10_COLOR_WRITE_ENABLE_ALL;

    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.RenderTarget[0] = rtbd;
	//����ͼ������
	//hr = 
//#if 1
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, "Eye.png", 
	NULL, NULL, &CubesTexture, NULL);

	//���ò���״̬
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//��������״̬
	hr = d3d11Device->CreateSamplerState(&sampDesc, &CubesTexSamplerState);


	d3d11Device->CreateBlendState(&blendDesc, &Transparency);

	//������ʱ���˳ʱ��״̬
	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	hr = d3d11Device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = d3d11Device->CreateRasterizerState(&cmdesc, &CWcullMode);

	return true;
}

///////////////**************new**************////////////////////
void StartTimer()
{
    LARGE_INTEGER frequencyCount;
    QueryPerformanceFrequency(&frequencyCount);
    countsPerSecond = double(frequencyCount.QuadPart);

    QueryPerformanceCounter(&frequencyCount);
    CounterStart = frequencyCount.QuadPart;
}

double GetTime()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    return double(currentTime.QuadPart-CounterStart)/countsPerSecond;
}

double GetFrameTime()
{
    LARGE_INTEGER currentTime;
    __int64 tickCount;
    QueryPerformanceCounter(&currentTime);

    tickCount = currentTime.QuadPart-frameTimeOld;
    frameTimeOld = currentTime.QuadPart;

    if(tickCount < 0.0f)
        tickCount = 0.0f;

    return float(tickCount)/countsPerSecond;
}
///////////////**************new**************////////////////////

///////////////**************new**************////////////////////
void UpdateScene(double time)
    ///////////////**************new**************////////////////////
//void UpdateScene()
{
	// ���³�����ɫ
	//����������ת����
    rot += 1.005f*time;
  	//rot += .005f;
    if(rot > 6.28f)
		rot = 0.0f;

	//��λcube1World
	cube1World = XMMatrixIdentity();

	//����cube1������ռ����
	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Rotation = XMMatrixRotationAxis(rotaxis, rot);
	Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);

	//��ת������cube1������ռ�
	cube1World = Translation* Rotation;

	//��λcube2World
	cube2World = XMMatrixIdentity();
	
	//����cube2������ռ����
	Rotation = XMMatrixRotationAxis(rotaxis, -rot);
	Scale = XMMatrixScaling(1.3f, 1.3f, 1.3f);

	//����cube2������ռ����
	cube2World = Rotation * Scale;
}

///////////////**************new**************////////////////////
void RenderText(std::wstring text, int inInt)
//void RenderText(std::wstring text)
{
	//�ͷ�D3D11�豸
	keyedMutex11->ReleaseSync(0);

	//ʹ��D3D10.1�豸
	keyedMutex10->AcquireSync(0, 5);

	//����D2D����
	D2DRenderTarget->BeginDraw();
	//���D2D����ɫ
	D2DRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	//�����ַ���
	std::wostringstream printString;
    ///////////////**************new**************////////////////////
    printString << text << inInt;
   //	printString << text;
    ///////////////**************new**************////////////////////
    printText = printString.str();

	//����������ɫ
	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 0.0f, 0.0f,1.0f);

	//����D2D����Ҫ�õ��Ļ�����ɫ
	Brush->SetColor(FontColor);

	//����D2D��Ⱦ����
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, Width, Height);

	//�����ı�
	D2DRenderTarget->DrawText(
		printText.c_str(),
		wcslen(printText.c_str()),
        TextFormat,
		layoutRect,
		Brush
		);

	D2DRenderTarget->EndDraw();

	//�ͷ�D3D10.1�豸
	keyedMutex10->ReleaseSync(1);

	//ʹ��D3D11�豸
	keyedMutex11->AcquireSync(1, 5);

	//ʹ����ɫ����Դ��ʾd2d��ȾĿ��������һ�����������þ����Ǳ���Ⱦ����Ļ�ռ�ġ�ʹ�æ�����Ա�����D2D
	//��ȾĿ��ı���Ϊ���ɼ��ģ���ֻ��ʹ��D2D���ƵĶ����ſɼ����ı�����

	//ΪD2D��ȾĿ������������û��״̬
	d3d11DevCon->OMSetBlendState(Transparency, NULL, 0xffffffff);
   //Set d2d's pixel shader so lighting calculations are not done
    d3d11DevCon->PSSetShader(D2D_PS, 0, 0);
	//����d2d��������
 
	d3d11DevCon->IASetIndexBuffer(d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//����d2d���㻺��
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevCon->IASetVertexBuffers(0, 1, &d2dVertBuffer, &stride, &offset);

	WVP = XMMatrixIdentity();
	///new
	cbPerObj.World = XMMatrixTranspose(WVP);

	cbPerObj.WVP = XMMatrixTranspose(WVP);
	d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	d3d11DevCon->PSSetShaderResources(0, 1, &d2dTexture);
	d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

	d3d11DevCon->RSSetState(CWcullMode);

	//���ڶ���������
	d3d11DevCon->DrawIndexed(6, 0, 0);

}

void DrawScene()
{
	//�����µ���ɫ���󻺳�
//	D3DXCOLOR bgColor(red, green, blue, 1.0f);
	float bgColor[4] = {(0.0f, 0.0f, 0.0f, 0.0f)};
	d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);

	//ˢ�����ģ����ͼ
	d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	//new
	constbuffPerFrame.light = light;
	d3d11DevCon->UpdateSubresource(cbPerFrameBuffer, 0, NULL, &constbuffPerFrame, 0, 0);
	d3d11DevCon->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);
	//��λ�����������ɫ��
	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);

	//ʹ��Ĭ�Ϲ�դ��״̬
//	d3d11DevCon->RSSetState(NULL);


	//����ʹ�ñ���ü��Ķ���
	//�رձ���ü�
   // d3d11DevCon->RSSetState(noCull);
	d3d11DevCon->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );

	d3d11DevCon->OMSetBlendState(0, 0, 0xffffffff);

    //Set the cubes index buffer
	//�������������������
	d3d11DevCon->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//����������Ķ��㻺��
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &stride, &offset);

	//����WVP���󲢽����͵�Ч���ļ��еĳ���������
	WVP = cube1World * camView * camProjection;
	cbPerObj.World = XMMatrixTranspose(cube1World);
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	///
	d3d11DevCon->PSSetShaderResources(0, 1, &CubesTexture);
	d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

	//��ʱ�����������Ϊ��Ҫ�����������ȱ���Ⱦ������ǰ���������
	d3d11DevCon->RSSetState(CWcullMode);

	//���Ƶ�һ��������
	d3d11DevCon->DrawIndexed(36, 0, 0);

	//
	//d3d11DevCon->RSSetState(CWcullMode);
	//d3d11DevCon->DrawIndexed(36, 0, 0);

	//��������/��ͼ/ͶӰ��������͵�Ч���ļ��ĳ���������
	//World = XMMatrixIdentity();

	WVP = cube2World * camView * camProjection;
	cbPerObj.World = XMMatrixTranspose(cube2World);
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	///
	d3d11DevCon->PSSetShaderResources(0, 1, &CubesTexture);
	d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

	//
	d3d11DevCon->RSSetState(CWcullMode);
	//���Ƶڶ���������
	d3d11DevCon->DrawIndexed(36, 0, 0);
	//����������
	//d3d11DevCon->DrawIndexed(6, 0, 0);
	//��������
	//d3d11DevCon->Draw(3, 0);
	//
	//d3d11DevCon->RSSetState(CWcullMode);
	//d3d11DevCon->DrawIndexed(36, 0, 0);
    ///////////////**************new**************////////////////////
	//
    RenderText(L"FPS: ", fps);
	//RenderText(L"hello world");
  ///////////////**************new**************////////////////////
	//���󻺳���ֵ���Ļ
	SwapChain->Present(0, 0);
}

int messageloop(){
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));//����ṹ�屻��ΪNULL��

	while (true)
	{
		//ʹ��PeekMessage()����Ƿ�����Ϣ������
		/*LPMSG lpMsg ��Ϣ�ṹ���ָ��
		*HWND hWnd ������Ϣ�Ĵ��ھ��������ΪNULL����ô����ӵ�ǰ�����н��������κ�һ�����ڵ���Ϣ
		*UINT wMsgFilterMin ָ����Ϣ��Χ�ڵ�һ��Ҫ������Ϣ��ֵ����wMsgFilterMin��wMsgFilterMax����Ϊ0����ôPeekMessage���������е���Ϣ
		*UINT wMsgFilterMax ָ����Ϣ��Χ�����һ��Ҫ������Ϣ��ֵ
		*UINT wRemoveMsg ָ����Ϣ�Ĵ���ʽ��������ΪPM_REMOVE�����ڶ�ȡ֮��ᱻɾ��
		*/
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
                break;
			//����ϢΪ������Ϣ����������ַ�����TranslateMessage()�����ô�����һЩ���������Ƽ��̵������ֵת�����ַ���ʽ��
			//��DispatchMessage()������Ϣ�����ڹ���WndProc��
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else //��û�д�����Ϣ����������Ϸ
 ///////////////**************new**************////////////////////
		{
  			frameCount++;
 	 		if(GetTime() > 1.0f)
            {
                fps = frameCount;
                frameCount = 0;
                StartTimer();
            }    

            frameTime = GetFrameTime();
		
            UpdateScene(frameTime);		
			DrawScene();

		}
	}
	return msg.wParam;
}

//������Ϣ������
//HWND hwnd ��ȡ��Ϣ�Ĵ��ھ��
//UINT msg ��Ϣ������
/*
*WM_ACTIVE �����ڼ���ʱ���͵���Ϣ
*WM_CLOSE �����ڹر�ʱ���͵���Ϣ
*WM_CREATE �����ڴ���ʱ���͵���Ϣ
*WM_DESTROY ����������ʱ���͵���Ϣ
*/
//wParam��lParamʱ��Ϣ�Ķ�����Ϣ��ʹ��wParam��������������Ϣ
LRESULT CALLBACK WndProc(HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	// �����¼������Ϣ�ĵط�����escape�������£�����ʾһ����Ϣ��ѯ���Ƿ�����˳��������yes�������رա��������������Ϣ��رա�����Ϣ����WM_DESTROY
	// ����ζ�Ŵ������ڱ����٣�����0���ҳ���ر�
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (MessageBox(0, "Are you sure you want to exit?",
				"Really?", MB_YESNO | MB_ICONASTERISK) == IDYES)
			{
				DestroyWindow(hwnd);
			}
			return 0;

		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;

	default:
		break;
	}

	//����Ĭ�ϴ��ڹ��̺���
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}