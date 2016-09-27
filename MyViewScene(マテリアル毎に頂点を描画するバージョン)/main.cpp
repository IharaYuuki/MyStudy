/*******************************************************************************
* タイトル:		メイン処理
* プログラム名:	main.cpp
*******************************************************************************/
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "input.h"
#include "camera.h"
#include "light.h"
#include "debugProc.h"
#include "myFbxImporter.h"
#include "object.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"								// ウインドウのクラス名
#define WINDOW_NAME		"MyViewScene[マテリアル別描画]"			// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
#ifdef _DEBUG
void UpdateFPS(void);
#endif

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECT3D9			g_pD3D = NULL;			// Direct3Dオブジェクト
LPDIRECT3DDEVICE9	g_pD3DDevice = NULL;	// デバイスオブジェクト(描画に必要)

HWND		g_hWnd;							// 親ウィンドウハンドル
HINSTANCE	g_hInstance;					// インスタンスハンドル
SCREEN		g_Screen = { SCREEN_WIDTH, SCREEN_HEIGHT,1.0f, 1.0f };

#ifdef _DEBUG
int			g_nCountFPS;					// FPSカウンタ
#endif

/*******************************************************************************
関数名:	int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
引数:	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow
戻り値:	正常終了: int型の 0
説明:	メイン関数
*******************************************************************************/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;
	
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	// クラスの作成
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};

	//HWND hWnd;
	MSG msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	g_hWnd = CreateWindowEx(0,
						CLASS_NAME,				// 登録されているクラス名
						WINDOW_NAME,
						WS_OVERLAPPEDWINDOW,
						0,//CW_USEDEFAULT,
						0,//CW_USEDEFAULT,
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
						NULL,
						NULL,
						hInstance,
						NULL);					// WM_CREATEでLPARAMに渡したい値。渡したいデータがないときは、NULL。
												// WM_CREATEは、ウインドウが生成されたときに１回だけ実行される。

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, g_hWnd, TRUE)))
	{
		return -1;
	}

	//フレームカウント初期化
	timeBeginPeriod(1);				// 分解能を設定
	dwExecLastTime = 
	dwFPSLastTime = timeGetTime();
	dwCurrentTime =
	dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	// メッセージループ
	while(1)
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg); // 押されているキーボードの情報を、アプリケーションが取得できる形に変換
				DispatchMessage(&msg);	// 取得したメッセージを、ウインドウプロシージャに送る
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();
			if((dwCurrentTime - dwFPSLastTime) >= 500)	// 0.5秒ごとに実行
			{
#ifdef _DEBUG
				g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
#endif
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}

			if((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;

				// 更新処理
				Update();

				// 描画処理
				Draw();

				dwFrameCount++;
			}
		}
	}
	
	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	timeEndPeriod(1);				// 分解能を戻す

	return (int)msg.wParam;
}

/*******************************************************************************
関数名:	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
引数:	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
戻り値:	正常終了: int型の 0
説明:	メイン関数
*******************************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		default:
			break;
		}
		break;
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/*******************************************************************************
関数名:	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
引数:	hInstance : インスタンスのハンドル
		hWnd      : ウインドウのハンドル
		bWindow   : ウインドウモードにするかどうか
戻り値:	HRESULT型	S_OK:正常終了
					E_FAIL:初期化失敗
説明:	DirectX 初期化関数
*******************************************************************************/
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DPRESENT_PARAMETERS d3dpp;
    D3DDISPLAYMODE d3ddm;

	// Direct3Dオブジェクトの作成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
    if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// ワークをゼロクリア
	d3dpp.BackBufferCount			= 1;						// バックバッファの数
	d3dpp.BackBufferWidth			= SCREEN_WIDTH;				// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight			= SCREEN_HEIGHT;			// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat			= d3ddm.Format;				// バックバッファフォーマットはディスプレイモードに合わせて使う
	d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;	// 映像信号に同期してフリップする
	d3dpp.Windowed					= bWindow;					// ウィンドウモード
	d3dpp.EnableAutoDepthStencil	= TRUE;						// デプスバッファ（Ｚバッファ）とステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;				// デプスバッファとして16bitを使う

	if(bWindow)
	{// ウィンドウモード
		d3dpp.FullScreen_RefreshRateInHz = 0;								// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// インターバル
	}
	else
	{// フルスクリーンモード
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;			// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;		// インターバル
	}

	// デバイスオブジェクトの生成
	// [デバイス作成制御]<描画>と<頂点処理>をハードウェアで行なう
	if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
									D3DDEVTYPE_HAL, 
									hWnd, 
									D3DCREATE_HARDWARE_VERTEXPROCESSING, 
									&d3dpp, &g_pD3DDevice)))
	{
		// 上記の設定が失敗したら
		// [デバイス作成制御]<描画>をハードウェアで行い、<頂点処理>はCPUで行なう
		if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
										D3DDEVTYPE_HAL, 
										hWnd, 
										D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
										&d3dpp, &g_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// [デバイス作成制御]<描画>と<頂点処理>をCPUで行なう
			if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
											D3DDEVTYPE_REF,
											hWnd, 
											D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
											&d3dpp, &g_pD3DDevice)))
			{
				// 初期化失敗
				return E_FAIL;
			}
		}
	}

	// レンダーステートパラメータの設定
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面をカリング D3DCULL_CCW
	g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						// Zバッファを使用
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドを行う
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定

	// サンプラーステートパラメータの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(U値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(V値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// テクスチャ縮小フィルタモードを設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// テクスチャ拡大フィルタモードを設定

	// テクスチャステージステートの設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// アルファブレンディング処理を設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 最初のアルファ引数
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// ２番目のアルファ引数
	
	// ウインドウサイズの初期化
	InitWindowSize();
	// 入力の初期化
	InitKeyboard(hInstance, hWnd);
	InitMouse(hInstance, hWnd);
	// デバッグプロックの初期化
	InitDebugProc();

	InitCamera();
	InitLight();

	// FBXファイルから情報を読み込む
	MyFbxImporter::GetFbxInfo();
	Object::CreateObject();
	
	return S_OK;
}

/*******************************************************************************
関数名:	void Uninit(void)
引数:	なし
戻り値:	なし
説明:	終了処理
*******************************************************************************/
void Uninit(void)
{
	if(g_pD3DDevice != NULL)
	{// デバイスオブジェクトの開放
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	if(g_pD3D != NULL)
	{// Direct3Dオブジェクトの開放
		g_pD3D->Release();
		g_pD3D = NULL;
	}

	// 入力の終了処理
	UninitKeyboard();
	UninitMouse();
	// デバッグプロックの終了処理
	UninitDebugProc();

	// FBXファイル情報を保存しているメモリを開放
	MyFbxFile::ReleaseAll();
	Object::ReleaseAll();
}
/*******************************************************************************
関数名:	void Update(void)
引数:	なし
戻り値:	なし
説明:	更新処理
*******************************************************************************/
void Update(void)
{
	// FPS更新
#ifdef _DEBUG
	UpdateFPS();
#endif
	// ウィンドウサイズ更新
	//UpdateWindowSize();
	// 入力の更新処理
	UpdateKeyboard();
	UpdateMouse();
	// デバッグプロックの更新処理
	UpdateDebugProc();

	UpdateCamera();
	Object::UpdateAll();
}
/*******************************************************************************
関数名:	void Draw(void)
引数:	なし
戻り値:	なし
説明:	描画処理
*******************************************************************************/
void Draw(void)
{
	// バックバッファ＆Ｚバッファのクリア
	g_pD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(30, 30, 30, 0), 1.0f, 0);
	
	// 描画の開始
	if(SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		SetCamera();
		Object::DrawAll();

#ifdef _DEBUG
		// デバッグ用ステータスを表示
		DrawDebugProc();
#endif
		// 描画の終了
		g_pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}
/*******************************************************************************
関数名:	LPDIRECT3DDEVICE9 GetDevice(void)
引数:	なし
戻り値:	LPDIRECT3DDEVICE9型	g_pD3DDevice
説明:	デバイスの取得
*******************************************************************************/
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}
/*******************************************************************************
関数名:	HWND GetHandleWindow(void)
引数:	なし
戻り値:	HWND型	g_hWnd
説明:	ウィンドウのハンドルの取得
*******************************************************************************/
HWND GetHandleWindow(void)
{
	return g_hWnd;
}
/*******************************************************************************
関数名:	HINSTANCE GetHandleInstance(void)
引数:	なし
戻り値:	HINSTANCE型	g_hInstance
説明:	アプリのインスタンスの取得
*******************************************************************************/
HINSTANCE GetHandleInstance(void)
{
	return g_hInstance;
}
#ifdef _DEBUG
/*******************************************************************************
関数名:	void DrawFPS(void)
引数:	なし
戻り値:	なし
説明:	FPS表示処理
*******************************************************************************/
void UpdateFPS(void)
{
	PrintDebugProc("FPS:%d\n", g_nCountFPS);
}
#endif
/*******************************************************************************
関数名:	void InitWindowSize(void)
引数:	なし
戻り値:	なし
説明:	ウィンドウサイズの初期化
*******************************************************************************/
void InitWindowSize(void)
{
	RECT RectClient,RectWindow;

	GetClientRect(g_hWnd,&RectClient);
	GetWindowRect(g_hWnd,&RectWindow);

	int nNewWidth = (RectWindow.right - RectWindow.left) - (RectClient.right - RectClient.left) + g_Screen.fWidth;
	int nNewHeight = (RectWindow.bottom - RectWindow.top) - (RectClient.bottom - RectClient.top) + g_Screen.fHeight;

	// API関数（ウインドウサイズを設定）
	SetWindowPos(g_hWnd, NULL, 0, 0, nNewWidth, nNewHeight, SWP_NOMOVE | SWP_NOZORDER);

	g_Screen.fWidth = (float)RectClient.right - (float)RectClient.left;
	g_Screen.fHeight = (float)RectClient.bottom - (float)RectClient.top;
	g_Screen.fRatioNewWidthToOldWidth = g_Screen.fWidth / SCREEN_WIDTH;
	g_Screen.fRatioNewHeightToOldHeight = g_Screen.fHeight / SCREEN_HEIGHT;
}
/*******************************************************************************
関数名:	void UpdateWindowSize(void)
説明:	ウィンドウサイズの更新
*******************************************************************************/
void UpdateWindowSize(void)
{
	RECT RectClient,RectWindow;

	GetClientRect(g_hWnd,&RectClient);
	GetWindowRect(g_hWnd,&RectWindow);

#if 0
	PrintDebugProc("ウィンドウサイズとクライアントサイズの幅の差＝%d\n",(RectWindow.right - RectWindow.left) - (RectClient.right - RectClient.left));
	PrintDebugProc("ウィンドウサイズとクライアントサイズの高さの差＝%d\n",(RectWindow.bottom - RectWindow.top) - (RectClient.bottom - RectClient.top));
	PrintDebugProc("GetSystemMetrics(SM_CXDLGFRAME) * 2の値＝%d\n",GetSystemMetrics(SM_CXDLGFRAME) * 2);
	PrintDebugProc("GetSystemMetrics(SM_CYDLGFRAME) * 2の値＝%d\n",GetSystemMetrics(SM_CYDLGFRAME) * 2);
	PrintDebugProc("GetSystemMetrics(SM_CYCAPTION)の値＝%d\n",GetSystemMetrics(SM_CYCAPTION));
#endif
	PrintDebugProc("[現在のクライアント領域]\n");
	PrintDebugProc("幅＝%d,高さ＝%d\n",(RectClient.right - RectClient.left),(RectClient.bottom - RectClient.top));
	PrintDebugProc("[現在のウィンドウ領域]\n");
	PrintDebugProc("幅＝%d,高さ＝%d\n",(RectWindow.right - RectWindow.left),(RectWindow.bottom - RectWindow.top));
	PrintDebugProc("（現在のクライアント横幅） / （クライアント横幅の初期値）＝%f\n",g_Screen.fRatioNewWidthToOldWidth);
	PrintDebugProc("（現在のクライアント縦幅） / （クライアント縦幅の初期値）＝%f\n",g_Screen.fRatioNewHeightToOldHeight);

	// API関数（ウインドウサイズを設定）
	SetWindowPos(g_hWnd, NULL, 0, 0, RectWindow.right - RectWindow.left, RectWindow.bottom - RectWindow.top, SWP_NOMOVE | SWP_NOZORDER);

	g_Screen.fWidth = (float)RectClient.right - (float)RectClient.left;
	g_Screen.fHeight = (float)RectClient.bottom - (float)RectClient.top;
	g_Screen.fRatioNewWidthToOldWidth = g_Screen.fWidth / SCREEN_WIDTH;
	g_Screen.fRatioNewHeightToOldHeight = g_Screen.fHeight / SCREEN_HEIGHT;
}
/*******************************************************************************
関数名:	SCREEN GetScreenInfo(void)
説明:	スクリーン情報の受け渡し
*******************************************************************************/
SCREEN GetScreenInfo(void)
{
	return g_Screen;
}