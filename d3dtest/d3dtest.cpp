#include "stdafx.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#pragma comment(lib, "D3D10_1.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "dwrite.lib")
///////////////**************new**************////////////////////
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
///////////////**************new**************////////////////////

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>
///////////////**************new**************////////////////////
#include <dinput.h>
///////////////**************new**************////////////////////
#include <vector>
//全局描述符
IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;

//索引缓冲
ID3D11Buffer* squareIndexBuffer;

//深度值-20170927
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;

//着色器
ID3D11Buffer* squareVertBuffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3D11PixelShader* D2D_PS;
ID3D10Blob* D2D_PS_Buffer;
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

ID3D11Buffer* cbPerFrameBuffer;

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
///////////////**************new**************////////////////////
IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;
///////////////**************new**************////////////////////
std::wstring printText;
/////
LPCTSTR WndClassName = L"firstwindow";
HWND hwnd = NULL;
HRESULT hr;

int Width  = 1920;
int Height = 1200;

///////////////**************new**************////////////////////
DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

float rotx = 0;
float rotz = 0;
float scaleX = 1.0f;
float scaleY = 1.0f;

XMMATRIX Rotationx;
XMMATRIX Rotationz;
///////////////**************new**************////////////////////
///四个空间以及相机属性
XMMATRIX WVP;
//立方体
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
///////////////**************new**************////////////////////
XMVECTOR DefaultForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);

XMMATRIX camRotationMatrix;
XMMATRIX groundWorld;

float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;
///////////////**************new**************////////////////////
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
///////////////**************new**************////////////////////
void UpdateCamera();
///////////////**************new**************////////////////////
void RenderText(std::wstring text, int inInt);
//void RenderText(std::wstring text);

void StartTimer();
double GetTime();
double GetFrameTime();


// 初始化窗口
bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed);

//初始化消息循环函数
int messageloop();
//初始化窗口回调过程。Windows API是事件驱动型的编程模型。在该函数中捕获Windows消息，比如一个按键按下（也叫事件）以及程序操作流程。
///////////////**************new**************////////////////////
bool InitDirectInput(HINSTANCE hInstance);
void DetectInput(double time);
///////////////**************new**************////////////////////
/************************************New Stuff****************************************************/
int NumFaces = 0;
int NumVertices = 0;

struct HeightMapInfo{        // 高度图结构体
    int terrainWidth;        // 高度图宽度
    int terrainHeight;        // 高度图高度（长度）
    XMFLOAT3 *heightMap;    // 保存地形顶点位置的数组
};

bool HeightMapLoad(char* filename, HeightMapInfo &hminfo);
/*************************************************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

///new
//创建效果常量缓冲的结构体
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


//顶点结构体以及顶点布局（输入布局）

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

//主函数，传入应用程序句柄hInstance,前一个应用程序句柄hPrevInstance，传给函数处理的命令行lpCmdLine以及窗口显示方式的nShowCmd
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	//创建并注册窗口
	if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true))
	{
		MessageBox(0, L"Window Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	/////new
	if (!InitializeDirect3d11App(hInstance)) // 初始化D3D
	{
		MessageBox(0, L"Direct3D Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if(!InitScene())	//Initialize our scene
	{
		MessageBox(0, L"Scene Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	///////////////**************new**************////////////////////
	if(!InitDirectInput(hInstance))
	{
		MessageBox(0, L"Direct Input Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}
	///////////////**************new**************////////////////////

	messageloop();
	CleanUp();
	//ReleaseObjects();

	return 0;
}
// windowed 若为true则为窗口模式显示，若为false则为全屏模式显示
bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed)
{
    typedef struct _WNDCLASS {
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
    } WNDCLASS;

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX); //window类的大小
	/********windows类风格
	*CS_CLASSDC 一个使用该类创建的在所有窗口间共享的设备上下文
	*CS_DBLCLKS 在窗口上使能双击功能
	*CS_HREDRAW 若窗口的宽度有改变或者窗口水平地移动，窗口将会刷新
	*CS_NOCLOSE 窗口菜单上禁止关闭选项
	*CS_OWNDC   为每个窗口创建自己的设备上下文。正好与CS_CLASSDC相反
	*CS_PARENTDC 这会设置创建的子窗口的剪裁四边形到父窗口，这允许子窗口能够在父窗口上绘画
	*CS_VERDRAW 若在窗口的高度或窗口在垂直方向有移动窗口会重绘
	**/
	wc.style = CS_HREDRAW | CS_VREDRAW;
	//lpfnWndProc是一个指向处理窗口消息函数的指针，设置窗口处理函数的函数名WndProc
	wc.lpfnWndProc = WndProc;
	//cbClsExtra是WNDCLASSEX之后额外申请的字节数
	wc.cbClsExtra = NULL;
	//cbWndExtra指定窗口实例之后所申请的字节数
	wc.cbWndExtra = NULL;
	//当前窗口应用程序的句柄，通过给函数GetModuleHandle()函数第一个参数传入NULL可获取当前窗口应用程序。
	wc.hInstance = hInstance;

	//hIcon用来指定窗口标题栏左上角的图标。以下是一些标准图标：
	/*
	*IDI_APPLICATION 默认应用程序图标
	*IDI_HAND 手形状的图标
	*IDI_EXCLAMATION 感叹号图标
	*IDI_INFORMATION 星号图标
	*IDI_QUESTION 问号图标
	*IDI_WINLOGO 若使用的是XP则是默认应用程序图标，否则是窗口logo
	*/
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	/*定义光标图标
	*IDC_APPSTARTING 标准箭头以及小型沙漏光标
	*IDC_ARROW 标准箭头光标
	*IDC_CROSS 十字线光标
	*IDC_HAND 手型光标
	*IDC_NO 斜线圈光标
	*IDC_WAIT 沙漏光标
	*/
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//hbrBackground是一个刷子的句柄，可使得背景黑色。
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//附加到窗口的菜单名字，不需要的话设置为NULL
	wc.lpszMenuName = NULL;
	//对类进行命名
	wc.lpszClassName = WndClassName;
	//指定任务栏的图标，使用上面的IDI＿图标
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	//注册类。若失败则会获得一个错误，若成功，则继续创建窗口
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error registering class",
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}
	//创建窗口
	hwnd = CreateWindowEx(
		NULL, 
		WndClassName, 
		L"heightmap",
		WS_OVERLAPPEDWINDOW, 
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hwnd)
	{
        MessageBox(NULL, L"Error creating window",
            L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	//BOOL ShowWindow(HWND hWnd, int nCmdShow);
	//BOOL UpdateWindow(HWND hWnd);

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);// 发送WM_PAINT消息到窗口过程，若窗口客户区没有任何东西要显示，则不发送消息。返回true，继续运行到mainloop中去。

	return true;
}

bool InitializeDirect3d11App(HINSTANCE hInstance)
{
	//声明缓冲
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//声明交换链
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	///////////////**************new**************////////////////////
	swapChainDesc.Windowed = true; 
	///////////////**************new**************////////////////////
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//创建DXGI factory来枚举显卡
	IDXGIFactory1 *DXGIFactory;
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&DXGIFactory);

	//使用第一个显卡
	IDXGIAdapter1 *Adapter;
	hr = DXGIFactory->EnumAdapters1(0, &Adapter);
	DXGIFactory->Release();

	//创建D3D11设备和交换链
	//hr = D3D11C

	//创建交换链
    hr = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        NULL, NULL,    D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);

	//初始化D2D D3D10.1和DirectWrite
	InitD2D_D3D101_DWrite(Adapter);

	//释放Adapter接口
	Adapter->Release();

	//创建后缓冲
    hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&BackBuffer11 );
    hr = d3d11Device->CreateRenderTargetView( BackBuffer11, NULL, &renderTargetView );


	//创建深度模板缓冲
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = Width;
	depthStencilDesc.Height = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; //绑定到OM
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//创建深度模板视图
	d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);


	return true;
}

bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter)
{
	//创建D3D101设备
	hr = D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &d3d101Device);

	//创建共享纹理，D3D101将会渲染它
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
	
	//为共享纹理获取key互斥量（为D3D11）
	hr = sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&keyedMutex11);

	//获取共享句柄需要在D3D10.1中打开共享纹理
	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;

	hr = sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void **)&sharedResource10);
	hr = sharedResource10->GetSharedHandle(&sharedHandle10);
	sharedResource10->Release();

	//在D3D10.1中为共享纹理打开界面
	IDXGISurface1 *sharedSurface10;
	hr = d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void **)(&sharedSurface10));
	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&keyedMutex10);

	//创建D2D factory
	ID2D1Factory *D2DFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void **)&D2DFactory);
	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;
	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
	hr = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &D2DRenderTarget);

	sharedSurface10->Release();
	D2DFactory->Release();

	//创建立体彩色画笔绘制一些东西
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

/************************************New Stuff****************************************************/
bool HeightMapLoad(char* filename, HeightMapInfo &hminfo)
{
    FILE *filePtr;                            // 指向文件的当前位置
    BITMAPFILEHEADER bitmapFileHeader;        // 保存文件信息的结构体
    BITMAPINFOHEADER bitmapInfoHeader;        // 保存图片信息的结构体
    int imageSize, index;
    unsigned char height;

    // 打开文件
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return 0;

    // 读取位图头部
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1,filePtr);

    // 读取信息头部
    fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

    // 获取图片宽度和高度（长度）
    hminfo.terrainWidth = bitmapInfoHeader.biWidth;
    hminfo.terrainHeight = bitmapInfoHeader.biHeight;

    // 图片尺寸字节大小，3表示每个像素的RBG字节数
    imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;

    // 初始化保存图片数据的数组
    unsigned char* bitmapImage = new unsigned char[imageSize];

    // 将文件指针设置为图片数据的起始位置
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    // 在bitmapImage中保存图片数据
    fread(bitmapImage, 1, imageSize, filePtr);

    // 关闭文件
    fclose(filePtr);

    //初始化高度图heightMap数组(保存地形图顶点)
    hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainHeight];

    // 使用灰度图，所以3个rgb值是一样的，但是只需要一个来表示高度
    // 所以使用这个k来跳过接下来的图片数据中的两个分量 (读取R而跳过BG)
    int k=0;

    // 将height除以下面的heightFactor来淡化地形高度,否则地形会看起来
    // 非常的高低不平
    float heightFactor = 10.0f;

    // 将图片数据读进heightMap数组中
    for(int j=0; j< hminfo.terrainHeight; j++)
    {
        for(int i=0; i< hminfo.terrainWidth; i++)
        {
            height = bitmapImage[k];

            index = ( hminfo.terrainHeight * j) + i;

            hminfo.heightMap[index].x = (float)i;
            hminfo.heightMap[index].y = (float)height / heightFactor;
            hminfo.heightMap[index].z = (float)j;

            k+=3;
        }
    }

    delete [] bitmapImage;
    bitmapImage = 0;

    return true;
}
/*************************************************************************************************/
bool InitDirectInput(HINSTANCE hInstance)
{
    hr = DirectInput8Create(hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&DirectInput,
        NULL); 

    hr = DirectInput->CreateDevice(GUID_SysKeyboard,
        &DIKeyboard,
        NULL);

    hr = DirectInput->CreateDevice(GUID_SysMouse,
        &DIMouse,
        NULL);

    hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    hr = DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    hr = DIMouse->SetDataFormat(&c_dfDIMouse);
    hr = DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

    return true;
}

///////////////**************new**************////////////////////
void UpdateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
	camTarget = XMVector3Normalize(camTarget);

///////////////**************new**************////////////////////
    /*
    // First-Person Camera
    XMMATRIX RotateYTempMatrix;
    RotateYTempMatrix = XMMatrixRotationY(camYaw);

    camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
    camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
    camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);*/
    
    //自由视角相机
    camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
    camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
    camUp = XMVector3Cross(camForward, camRight);
///////////////**************new**************////////////////////

	camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;	

	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );
}
void DetectInput(double time)
{
    DIMOUSESTATE mouseCurrState;

    BYTE keyboardState[256];

    DIKeyboard->Acquire();
    DIMouse->Acquire();

    DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

    DIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

    if(keyboardState[DIK_ESCAPE] & 0x80)
        PostMessage(hwnd, WM_DESTROY, 0, 0);

	float speed = 25.0f * time;

	if(keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= speed;
	}
	if(keyboardState[DIK_D] & 0x80)
	{
		moveLeftRight += speed;
	}
	if(keyboardState[DIK_W] & 0x80)
	{
		moveBackForward += speed;
	}
	if(keyboardState[DIK_S] & 0x80)
	{
		moveBackForward -= speed;
	}
	if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseLastState.lX * 0.001f;

		camPitch += mouseCurrState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}

	UpdateCamera();


    return;
}
///////////////**************new**************////////////////////
void CleanUp()
{
	///////////////**************new**************////////////////////
	SwapChain->SetFullscreenState(false, NULL);
	PostMessage(hwnd, WM_DESTROY, 0, 0);
	///////////////**************new**************////////////////////
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

	//释放不裁剪对象
//	noCull->Release();
	//释放混合对象

	Transparency->Release();
	CCWcullMode->Release();
	CWcullMode->Release();
	
	//释放线框
	//WireFrame->Release();

	d3d101Device->Release();
	keyedMutex11->Release();
	keyedMutex10->Release();
	D2DRenderTarget->Release();
	Brush->Release();
    BackBuffer11->Release();
	sharedTex11->Release();
	DWriteFactory->Release();
    TextFormat->Release();
	d2dTexture->Release();

	/// new
	cbPerFrameBuffer->Release();
    ///////////////**************new**************////////////////////
    DIKeyboard->Unacquire();
    DIMouse->Unacquire();
    DirectInput->Release();
    ///////////////**************new**************////////////////////
}


void InitD2DScreenTexture()
{
	//创建顶点缓冲
	Vertex v[] =
	{
		//字体面
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f),
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f),
		Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),
	};

	DWORD indices[] = {
		//字体面
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

	//从纹理D2D，创建一个着色器资源视图
	//因此，能够使用它来创建一个矩形纹理，用于覆盖场景
	d3d11Device->CreateShaderResourceView(sharedTex11, NULL, &d2dTexture);

}

//void ReleaseObjects()
//{
//释放创建的COM对象
//	SwapChain->Release();
//	d3d11Device->Release();
//	d3d11DevCon->Release();
//}

bool InitScene()
{
	//
	InitD2DScreenTexture();
	//编译着色器
	hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);
	/// new
	hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "D2D_PS", "ps_4_0", 0, 0, 0, &D2D_PS_Buffer, 0, 0);

	//创建着色器对象
	hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
	///new
	hr = d3d11Device->CreatePixelShader(D2D_PS_Buffer->GetBufferPointer(), D2D_PS_Buffer->GetBufferSize(), NULL, &D2D_PS);

	//设置顶点和像素着色器
	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);

    ///////////////**************new**************////////////////////
	light.dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
	light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


    /************************************New Stuff****************************************************/
    HeightMapInfo hmInfo;
    HeightMapLoad("heightmap.bmp", hmInfo);        // 导入heightmap并将它保存进hmInfo

    int cols = hmInfo.terrainWidth;
    int rows = hmInfo.terrainHeight;

    //创建网格
    NumVertices = rows * cols;
    NumFaces  = (rows-1)*(cols-1)*2;

    std::vector<Vertex> v(NumVertices);

    for(DWORD i = 0; i < rows; ++i)
    {
        for(DWORD j = 0; j < cols; ++j)
        {
            v[i*cols+j].pos = hmInfo.heightMap[i*cols+j];
            v[i*cols+j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
        }
    }

    std::vector<DWORD> indices(NumFaces * 3);

    int k = 0;
    int texUIndex = 0;
    int texVIndex = 0;
    for(DWORD i = 0; i < rows-1; i++)
    {
        for(DWORD j = 0; j < cols-1; j++)
        {
            indices[k]   = i*cols+j;        // Bottom left of quad
            v[i*cols+j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 1.0f);

            indices[k+1] = i*cols+j+1;        // Bottom right of quad
            v[i*cols+j+1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

            indices[k+2] = (i+1)*cols+j;    // Top left of quad
            v[(i+1)*cols+j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);


            indices[k+3] = (i+1)*cols+j;    // Top left of quad
            v[(i+1)*cols+j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

            indices[k+4] = i*cols+j+1;        // Bottom right of quad
            v[i*cols+j+1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

            indices[k+5] = (i+1)*cols+j+1;    // Top right of quad
            v[(i+1)*cols+j+1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 0.0f);

            k += 6; // next quad

            texUIndex++;
        }
        texUIndex = 0;
        texVIndex++;
    }

    //////////////////////计算法线///////////////////////////
    //使用法线平均法为每个顶点计算法线
    std::vector<XMFLOAT3> tempNormal;

    //规范化和非规范化法线
    XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

    //用于从顶点位置获取向量
    float vecX, vecY, vecZ;

    //三角形的两个边
    XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    //计算面法线
    for(int i = 0; i < NumFaces; ++i)
    {
        //获取向量用于描述三角形的一个边(edge 0,2)
        vecX = v[indices[(i*3)]].pos.x - v[indices[(i*3)+2]].pos.x;
        vecY = v[indices[(i*3)]].pos.y - v[indices[(i*3)+2]].pos.y;
        vecZ = v[indices[(i*3)]].pos.z - v[indices[(i*3)+2]].pos.z;        
        edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //创建第一个边

        //获取向量用于描述三角形的另一个边(edge 2,1)
        vecX = v[indices[(i*3)+2]].pos.x - v[indices[(i*3)+1]].pos.x;
        vecY = v[indices[(i*3)+2]].pos.y - v[indices[(i*3)+1]].pos.y;
        vecZ = v[indices[(i*3)+2]].pos.z - v[indices[(i*3)+1]].pos.z;        
        edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //创建第二个边

        //叉乘两个边向量得到非规范化面法线
        XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
        tempNormal.push_back(unnormalized);            //保存非规范化法线(为法线平均法而做)
    }

    //计算顶点法线 (法线平均法)
    XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    int facesUsing = 0;
    float tX;
    float tY;
    float tZ;

    //遍历每个顶点
    for(int i = 0; i < NumVertices; ++i)
    {
        //检测使用该顶点的三角形
        for(int j = 0; j < NumFaces; ++j)
        {
            if(indices[j*3] == i ||
                indices[(j*3)+1] == i ||
                indices[(j*3)+2] == i)
            {
                tX = XMVectorGetX(normalSum) + tempNormal[j].x;
                tY = XMVectorGetY(normalSum) + tempNormal[j].y;
                tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

                normalSum = XMVectorSet(tX, tY, tZ, 0.0f);    //若有面正在使用顶点,则将非规范化面法线添加到normalSum
                facesUsing++;
            }
        }

        //normalSum除以共享顶点的面数得到实际法线
        normalSum = normalSum / facesUsing;

        //规范化normalSum向量
        normalSum = XMVector3Normalize(normalSum);

        //将法线保存进当前顶点内
        v[i].normal.x = XMVectorGetX(normalSum);
        v[i].normal.y = XMVectorGetY(normalSum);
        v[i].normal.z = XMVectorGetZ(normalSum);

        //为下一个顶点清除normalSum和facesUsing
        normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        facesUsing = 0;
    }
    /*************************************************************************************************/
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * NumFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
	d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof( Vertex ) * NumVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = &v[0];
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

	//设置顶点缓冲
//	UINT stride = sizeof(Vertex);
//	UINT offset = 0;
//	d3d11DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &stride, &offset);

	//创建输入布局
    hr = d3d11Device->CreateInputLayout( layout, numElements, VS_Buffer->GetBufferPointer(), 
		VS_Buffer->GetBufferSize(), &vertLayout);

	//设置输入布局
	d3d11DevCon->IASetInputLayout(vertLayout);
	
	//设置图元拓扑
	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//创建视口
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;
	
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//设置视口
	d3d11DevCon->RSSetViewports(1, &viewport);

	//创建缓冲用来发送到效果文件的cbuffer
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	//创建缓冲用于每帧发送cbuffer到着色器文件
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrame);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

    hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);

	//相机信息
	//相机信息
	camPosition = XMVectorSet( 0.0f, 5.0f, -8.0f, 0.0f );
	//camPosition = XMVectorSet(0.0f, 0.0f, -0.5f, 0.0f);
	camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//设置视图矩阵
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);

	//设置投影矩阵
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
	//加载图像纹理
	//hr = 
//#if 1
	hr = D3DX11CreateShaderResourceViewFromFile( d3d11Device, L"grass.jpg",
	NULL, NULL, &CubesTexture, NULL);

	//配置采样状态
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//创建采样状态
	hr = d3d11Device->CreateSamplerState(&sampDesc, &CubesTexSamplerState);


	d3d11Device->CreateBlendState(&blendDesc, &Transparency);

	//创建逆时针和顺时针状态
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
	//Reset cube1World
	groundWorld = XMMatrixIdentity();

    //定义地形的世界空间矩阵
	///////////////**************new**************////////////////////
    Scale = XMMatrixScaling( 10.0f, 10.0f, 10.0f );
    Translation = XMMatrixTranslation( -100.0f, -100.0f, -100.0f );
    /************************************New Stuff****************************************************/

    //使用转换设置地形的世界空间
    groundWorld = Scale * Translation;
}

///////////////**************new**************////////////////////
void RenderText(std::wstring text, int inInt)
//void RenderText(std::wstring text)
{
	//释放D3D11设备
    d3d11DevCon->PSSetShader(D2D_PS, 0, 0);
	keyedMutex11->ReleaseSync(0);

	//使用D3D10.1设备
	keyedMutex10->AcquireSync(0, 5);

	//绘制D2D内容
	D2DRenderTarget->BeginDraw();
	//清空D2D背景色
	D2DRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	//创建字符串
	std::wostringstream printString;
    ///////////////**************new**************////////////////////
    printString << text << inInt;
   //	printString << text;
    ///////////////**************new**************////////////////////
    printText = printString.str();

	//设置字体颜色
    D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);

	//设置D2D绘制要用到的画笔颜色
	Brush->SetColor(FontColor);

	//创建D2D渲染区域
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, Width, Height);

	//绘制文本
	D2DRenderTarget->DrawText(
		printText.c_str(),
		wcslen(printText.c_str()),
        TextFormat,
		layoutRect,
		Brush
		);

	D2DRenderTarget->EndDraw();

	//释放D3D10.1设备
	keyedMutex10->ReleaseSync(1);

	//使用D3D11设备
	keyedMutex11->AcquireSync(1, 5);

	//使用着色器资源表示d2d渲染目标来创建一个矩形纹理，该矩形是被渲染进屏幕空间的。使用α混合以便整个D2D
	//渲染目标的背景为不可见的，且只有使用D2D绘制的东西才可见（文本）。

	//为D2D渲染目标纹理对象设置混合状态
	d3d11DevCon->OMSetBlendState(Transparency, NULL, 0xffffffff);
   //Set d2d's pixel shader so lighting calculations are not done
  //  d3d11DevCon->PSSetShader(D2D_PS, 0, 0);
	//设置d2d索引缓冲
 
	d3d11DevCon->IASetIndexBuffer(d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//设置d2d顶点缓冲
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevCon->IASetVertexBuffers(0, 1, &d2dVertBuffer, &stride, &offset);

	WVP = XMMatrixIdentity();
	///new
//	cbPerObj.World = XMMatrixTranspose(WVP);

	cbPerObj.WVP = XMMatrixTranspose(WVP);
	d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	d3d11DevCon->PSSetShaderResources(0, 1, &d2dTexture);
	d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

	d3d11DevCon->RSSetState(CWcullMode);

	//画第二个立方体
	d3d11DevCon->DrawIndexed(6, 0, 0);

}

void DrawScene()
{
	//将更新的颜色填充后缓冲
//	D3DXCOLOR bgColor(red, green, blue, 1.0f);
	float bgColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);

	//刷新深度模板视图
	d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	//new
	constbuffPerFrame.light = light;
	d3d11DevCon->UpdateSubresource(cbPerFrameBuffer, 0, NULL, &constbuffPerFrame, 0, 0);
	d3d11DevCon->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);
	//复位顶点和像素着色器
//	d3d11DevCon->VSSetShader(VS, 0, 0);
//	d3d11DevCon->PSSetShader(PS, 0, 0);

	//使能默认光栅化状态
//	d3d11DevCon->RSSetState(NULL);


	//绘制使用背面裁剪的对象
	//关闭背面裁剪
   // d3d11DevCon->RSSetState(noCull);
	d3d11DevCon->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );

	d3d11DevCon->OMSetBlendState(0, 0, 0xffffffff);
	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);

    //Set the cubes index buffer
	//设置立方体的索引缓冲
	d3d11DevCon->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//设置立方体的顶点缓冲
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &stride, &offset);

	//设置WVP矩阵并将它送到效果文件中的常量缓冲中
	WVP = groundWorld * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);	
	cbPerObj.World = XMMatrixTranspose(groundWorld);	
	d3d11DevCon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	d3d11DevCon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
	d3d11DevCon->PSSetShaderResources( 0, 1, &CubesTexture );
	d3d11DevCon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

	d3d11DevCon->RSSetState(CCWcullMode);
    d3d11DevCon->DrawIndexed( NumFaces * 3, 0, 0 );

	RenderText(L"FPS: ", fps);

	//Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}

int messageloop(){
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));//清除结构体被设为NULL。

	while (true)
	{
		//使用PeekMessage()检查是否有消息传进来
		/*LPMSG lpMsg 消息结构体的指针
		*HWND hWnd 发送消息的窗口句柄。若设为NULL，那么它会从当前程序中接收来自任何一个窗口的消息
		*UINT wMsgFilterMin 指定消息范围内第一个要检查的消息的值。若wMsgFilterMin和wMsgFilterMax都设为0，那么PeekMessage将会检查素有的消息
		*UINT wMsgFilterMax 指定消息范围内最后一个要检测的消息的值
		*UINT wRemoveMsg 指定消息的处理方式。若设置为PM_REMOVE，则在读取之后会被删除
		*/
		BOOL PeekMessageL( 
            LPMSG lpMsg,
            HWND hWnd,
            UINT wMsgFilterMin,
            UINT wMsgFilterMax,
            UINT wRemoveMsg
            );

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
                break;
			//若消息为窗口消息，则解析并分发它。TranslateMessage()将会让窗口做一些解析，类似键盘的虚拟键值转换到字符形式。
			//而DispatchMessage()则发送消息到窗口过程WndProc。
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else //若没有窗口消息，则运行游戏
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
		
            ///////////////**************new**************////////////////////
            DetectInput(frameTime);
            ///////////////**************new**************////////////////////
            UpdateScene(frameTime);
			DrawScene();

		}
	}
	return msg.wParam;
}

//窗口消息处理函数
//HWND hwnd 获取消息的窗口句柄
//UINT msg 消息的内容
/*
*WM_ACTIVE 当窗口激活时发送的消息
*WM_CLOSE 当窗口关闭时发送的消息
*WM_CREATE 当窗口创建时发送的消息
*WM_DESTROY 当窗口销毁时发送的消息
*/
//wParam和lParam时消息的额外信息。使用wParam来检测键盘输入消息
LRESULT CALLBACK WndProc(HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	
	// 这是事件检测消息的地方，若escape键被按下，会显示一个消息框，询问是否真的退出。若点击yes，则程序关闭。若不点击，则消息框关闭。若消息包含WM_DESTROY
	// 则意味着窗口正在被销毁，返回0并且程序关闭
	switch (msg)
	{
	case WM_KEYDOWN:
        if( wParam == VK_ESCAPE ){
				DestroyWindow(hwnd);
			}
			return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd,
        msg,
        wParam,
        lParam);
}