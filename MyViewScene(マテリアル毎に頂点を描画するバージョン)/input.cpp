/*******************************************************************************
* タイトル:		入力処理
* プログラム名:	input.cpp
*******************************************************************************/
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "Input.h"
#include "DebugProc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_KEY_MAX (256)
#define LIMIT_COUNT_REPEAT (20) // リピートカウンタリミット

#define	NUM_MOUSE_BUTTON_MAX	(8)		// マウスのボタン最大数
#define	MOUSE_BUTTON_LEFT		(0)		// マウスの左ボタン
#define	MOUSE_BUTTON_RIGHT		(1)		// マウスの右ボタン
#define	MOUSE_BUTTON_CENTER		(2)		// マウスの中央ボタン

#define MAX_CONTROLER			(2)		// コントローラ最大数(使う数に応じて変更すること)

/*******************************************************************************
* 構造体定義
*******************************************************************************/

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/
HRESULT InitInput(HINSTANCE hInstance, HWND hWnd);
void UninitInput(void);
void UpdateInput(void);

/*******************************************************************************
* グローバル変数
*******************************************************************************/
LPDIRECTINPUT8			g_pDInput = NULL;					// DirectInputインターフェースへのポインタ
LPDIRECTINPUTDEVICE8	g_pDIDevKeyboard = NULL;			// 入力デバイス(キーボード)へのポインタ
LPDIRECTINPUTDEVICE8	g_pDevMouse = NULL;					// 入力デバイス(マウス)へのポインタ
// キーボード情報
BYTE					g_aKeyState[NUM_KEY_MAX];			// キーボードの状態を受け取るワーク
BYTE					g_aKeyStateTrigger[NUM_KEY_MAX];	// キーボードの状態を受け取るワーク
BYTE					g_aKeyStateRelease[NUM_KEY_MAX];	// キーボードの状態を受け取るワーク
BYTE					g_aKeyStateRepeat[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
BYTE					g_aKeyStateRepeatCnt[NUM_KEY_MAX];	// キーボードのリピートカウンタ
// マウス情報
DIMOUSESTATE2			g_mouseState;						// マウスの入力情報ワーク
DIMOUSESTATE2			g_mouseStateTrigger;				// マウスのトリガー情報ワーク
DIMOUSESTATE2			g_mouseStateRelease;				// マウスのリリース情報ワーク
POINT					g_mouseScreenPos;
POINT					g_mouseClientPos;
POINT					g_mouseWindowPos;
HCURSOR					g_hCursor[MAX_CURSOR];
bool					g_bClipCursor = true;				// マウスをウインドウ内に留めるフラグ
// パッド情報
DIJOYSTATE				g_joyState[MAX_CONTROLER];			// ジョイパッド情報配列
DIJOYSTATE				g_joyStatePrev[MAX_CONTROLER];		// ジョイパッド情報配列（直前）
LPDIRECTINPUTDEVICE8	g_pDIDevJoypad[MAX_CONTROLER];		// IDirectInputDevice8インターフェースへのポインタ(ジョイパッド)
int						g_nJoypadNum = 0;					// 見つかったジョイパッドの数
BYTE					g_joyStateTrigger[MAX_CONTROLER][32];

/*******************************************************************************
関数名:	HRESULT InitInput(HINSTANCE hInstance, HWND hWnd)
引数:	HINSTANCE hInstance, HWND hWnd
戻り値:	HRESULT型	S_OK:正常終了
					E_FAIL:初期化失敗
説明:	inputの初期化
*******************************************************************************/
HRESULT InitInput(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT hr;

	if(g_pDInput == NULL) // !g_pDInput
	{
		// DirectInputオブジェクトの作成
		hr = DirectInput8Create(hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&g_pDInput,NULL);
		
		return hr;
	}

	return S_OK;
}
/*******************************************************************************
関数名:	void UninitInput(void)
引数:	なし
戻り値:	なし
説明:	inputの終了処理
*******************************************************************************/
void UninitInput(void)
{
	if(g_pDInput != NULL)
	{// DirectInputオブジェクトの開放
		g_pDInput -> Release();
		g_pDInput = NULL;	
	}
}
/*******************************************************************************
関数名:	void UpdateInput(void)
引数:	なし
戻り値:	なし
説明:	inputの更新処理
*******************************************************************************/
void UpdateInput(void)
{
}
/*******************************************************************************
関数名:	HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd)
引数:	HINSTANCE hInstance, HWND hWnd
戻り値:	HRESULT型
説明:	Keyboardの初期化処理
*******************************************************************************/
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT hr;

	// 入力処理の初期化
	hr = InitInput(hInstance, hWnd);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "DirectInputオブジェクトが作れねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// デバイスオブジェクトを作成
	hr = g_pDInput -> CreateDevice(GUID_SysKeyboard,&g_pDIDevKeyboard,NULL);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = g_pDIDevKeyboard -> SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = g_pDIDevKeyboard -> SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	g_pDIDevKeyboard -> Acquire();
	
	return S_OK;
}
/*******************************************************************************
関数名:	void UninitKeyboard(void)
引数:	なし
戻り値:	なし
説明:	Keyboardの終了処理
*******************************************************************************/
void UninitKeyboard(void)
{
	if(g_pDIDevKeyboard != NULL)
	{// 入力デバイス(キーボード)の開放
		// キーボードへのアクセス権を開放(入力制御終了)
		g_pDIDevKeyboard -> Unacquire();
		g_pDIDevKeyboard -> Release();
		g_pDIDevKeyboard = NULL;
		
	}

	// 入力処理の終了処理
	UninitInput();
}
/*******************************************************************************
関数名:	void UpdateKeyboard(void)
引数:	なし
戻り値:	なし
説明:	Keyboardの更新処理
*******************************************************************************/
void UpdateKeyboard(void)
{
	BYTE aKeyState[NUM_KEY_MAX];
	// デバイスからデータを取得
	if(SUCCEEDED(g_pDIDevKeyboard -> GetDeviceState(sizeof(aKeyState), aKeyState)))
	{
		for(int nCntkey = 0; nCntkey < NUM_KEY_MAX; nCntkey++)
		{
			// キートリガー リリース情報を生成
			g_aKeyStateTrigger[nCntkey] = (g_aKeyState[nCntkey] ^ aKeyState[nCntkey]) & aKeyState[nCntkey];
			g_aKeyStateRelease[nCntkey] = (g_aKeyState[nCntkey] ^ aKeyState[nCntkey]) & ~aKeyState[nCntkey];
			// リピート情報を生成
			if(aKeyState[nCntkey])
			{
				if(g_aKeyStateRepeatCnt[nCntkey] < LIMIT_COUNT_REPEAT)
				{
					g_aKeyStateRepeatCnt[nCntkey]++;
					if(g_aKeyStateRepeatCnt[nCntkey] == 1 || g_aKeyStateRepeatCnt[nCntkey] >= LIMIT_COUNT_REPEAT)
					{// キーを押し始めた最初のフレーム、または一定時間経過で情報ON
						g_aKeyStateRepeat[nCntkey] = aKeyState[nCntkey];
					}
					else
					{
						g_aKeyStateRepeat[nCntkey] = 0;
					}
				}
			}
			else
			{
				g_aKeyStateRepeatCnt[nCntkey] = 0;
				g_aKeyStateRepeat[nCntkey] = 0;
			}
			// キープレス情報を保存
			g_aKeyState[nCntkey] = aKeyState[nCntkey];
		}
	}
	else
	{
		g_pDIDevKeyboard -> Acquire();
	}
}
/*******************************************************************************
関数名:	bool GetKeyboardPress(int nkey)
引数:	int nkey
戻り値:	bool型
説明:	Keyboardのプレス状態の取得
*******************************************************************************/
bool GetKeyboardPress(int nkey)
{
	return (g_aKeyState[nkey] & 0x80 ? true:false);
}
/*******************************************************************************
関数名:	bool GetKeyboardTrigger(int nkey)
引数:	int nkey
戻り値:	bool型
説明:	Keyboardのトリガー状態の取得
*******************************************************************************/
bool GetKeyboardTrigger(int nkey)
{
	return (g_aKeyStateTrigger[nkey] & 0x80 ? true:false);
}
/*******************************************************************************
関数名:	bool GetKeyboardRelease(int nkey)
引数:	int nkey
戻り値:	bool型
説明:	Keyboardのリリース状態の取得
*******************************************************************************/
bool GetKeyboardRelease(int nkey)
{
	return (g_aKeyStateRelease[nkey] & 0x80 ? true:false);
}
/*******************************************************************************
関数名:	bool GetKeyboardRepeat(int nkey)
引数:	int nkey
戻り値:	bool型
説明:	Keyboardのリピート状態の取得
*******************************************************************************/
bool GetKeyboardRepeat(int nkey)
{
	return (g_aKeyStateRepeat[nkey] & 0x80 ? true:false);
}
//=============================================================================
// マウスの初期化
//=============================================================================
HRESULT InitMouse(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT hr;

	// 入力処理の初期化
	hr = InitInput(hInstance, hWnd);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "DirectInputオブジェクトが作れねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// デバイスオブジェクトを作成
	hr = g_pDInput->CreateDevice(GUID_SysMouse, &g_pDevMouse, NULL);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "マウスがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = g_pDevMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "マウスのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = g_pDevMouse->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "マウスの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// デバイスのプロパティを設定
	{
		DIPROPDWORD dipdw;

		dipdw.diph.dwSize = sizeof(dipdw);
		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = DIPROPAXISMODE_REL;

		hr = g_pDevMouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "マウスのデバイスのプロパティを設定できませんでした。", "警告！", MB_ICONWARNING);
			return hr;
		}
	}

	// マウスへのアクセス権を獲得(入力制御開始)
	g_pDevMouse->Acquire();

	/* マウスカーソル画像の読み込み */
	{
		g_hCursor[CURSOR_HAND] = LoadCursorFromFile("data/MOUSE/MouseCursorHand.cur");
		g_hCursor[CURSOR_JELLYFISH] = LoadCursorFromFile("data/MOUSE/MouseCursorJellyfish.cur");
	}
	return S_OK;
}

//=============================================================================
// マウスの終了処理
//=============================================================================
void UninitMouse(void)
{
	// 入力処理の終了処理
	UninitInput();

	if(g_pDevMouse)
	{
		// マウスへのアクセス権を開放
		g_pDevMouse->Unacquire();

		g_pDevMouse->Release();
		g_pDevMouse = NULL;
	}
}

//=============================================================================
// マウスの更新処理
//=============================================================================
void UpdateMouse(void)
{
	DIMOUSESTATE2 mouseState;
	HWND hWnd = GetHandleWindow();
	POINT mousePos;

	// マウスの現在の座標を取得する
	GetCursorPos(&mousePos);
	// マウスのスクリーン座標を代入
	g_mouseScreenPos = mousePos;
	// マウスのスクリーン座標をクライアント座標へ変換
	ScreenToClient(hWnd, &mousePos);
	// マウスのクライアント座標を代入
	g_mouseClientPos = mousePos;
	PrintDebugProc("[マウスX座標 : (%d)]\n",g_mouseClientPos.x);
	PrintDebugProc("[マウスY座標 : (%d)]\n\n",g_mouseClientPos.y);
	
#if 0
	// ゲーム中マウス内を画面内に固定する処理
	if(g_bClipCursor)
	{
		POINT ClientOrigin = {0,0};
		RECT Rect;
		ClientToScreen(hWnd,  &ClientOrigin);
		Rect.left = ClientOrigin.x;
		Rect.right = Rect.left + SCREEN_WIDTH - GetSystemMetrics(SM_CXDLGFRAME) * 4;
		Rect.top = ClientOrigin.y;
		Rect.bottom = Rect.top + SCREEN_HEIGHT - GetSystemMetrics(SM_CXDLGFRAME) * 4;
		ClipCursor(&Rect);
	}
	if(GetKeyboardPress(DIK_LCONTROL) && GetKeyboardTrigger(DIK_SPACE))
	{
		if(g_bClipCursor)
		{
			g_bClipCursor = false;
			ClipCursor(NULL);
		}
		else
		{
			g_bClipCursor = true;
		}
	}
#endif

	// デバイスからデータを取得
	if(SUCCEEDED(g_pDevMouse->GetDeviceState(sizeof(mouseState), &mouseState)))
	{
		g_mouseStateTrigger.lX = ((g_mouseState.lX ^ mouseState.lX) & mouseState.lX);
		g_mouseStateTrigger.lY = ((g_mouseState.lY ^ mouseState.lY) & mouseState.lY);
		g_mouseStateTrigger.lZ = ((g_mouseState.lZ ^ mouseState.lZ) & mouseState.lZ);

		for(int nCntKey = 0; nCntKey < NUM_MOUSE_BUTTON_MAX; nCntKey++)
		{
			g_mouseStateTrigger.rgbButtons[nCntKey] = ((g_mouseState.rgbButtons[nCntKey] ^ mouseState.rgbButtons[nCntKey]) & mouseState.rgbButtons[nCntKey]);
			g_mouseStateRelease.rgbButtons[nCntKey] = ((g_mouseState.rgbButtons[nCntKey] ^ mouseState.rgbButtons[nCntKey]) & ~mouseState.rgbButtons[nCntKey]);
		}

		g_mouseState = mouseState;
	}
	else
	{
		// マウスへのアクセス権を獲得(入力制御開始)
		g_pDevMouse->Acquire();
	}
}
/*******************************************************************************
関数名:	void SetCursor(CURSOR Cursor)
引数:	CURSOR Cursor
戻り値:	なし
説明:	カーソルをセット
*******************************************************************************/
void SetCursor(CURSOR Cursor)
{
	HWND hWnd;
	hWnd = GetHandleWindow();
	SetClassLong(hWnd, GCL_HCURSOR, (LONG)g_hCursor[Cursor]);
}
//=============================================================================
// マウスカーソル画像ハンドル取得
//=============================================================================
HCURSOR *GetpHandleCursor(void)
{
	return &g_hCursor[0];
}
//=============================================================================
// マウスデータ取得(左プレス)
//=============================================================================
bool GetMouseLeftPress(void)
{
	return (g_mouseState.rgbButtons[MOUSE_BUTTON_LEFT] & 0x80 ? true: false);
}

//=============================================================================
// マウスデータ取得(左トリガー)
//=============================================================================
bool GetMouseLeftTrigger(void)
{
	return (g_mouseStateTrigger.rgbButtons[MOUSE_BUTTON_LEFT] & 0x80 ? true: false);
}

//=============================================================================
// マウスデータ取得(左リリース)
//=============================================================================
bool GetMouseLeftRelease(void)
{
	return (g_mouseStateRelease.rgbButtons[MOUSE_BUTTON_LEFT] & 0x80 ? true: false);
}

//=============================================================================
// マウスデータ取得(右プレス)
//=============================================================================
bool GetMouseRightPress(void)
{
	return (g_mouseState.rgbButtons[MOUSE_BUTTON_RIGHT] & 0x80 ? true: false);
}

//=============================================================================
// マウスデータ取得(右トリガー)
//=============================================================================
bool GetMouseRightTrigger(void)
{
	return (g_mouseStateTrigger.rgbButtons[MOUSE_BUTTON_RIGHT] & 0x80 ? true: false);
}

//=============================================================================
// マウスデータ取得(右リリース)
//=============================================================================
bool GetMouseRightRelease(void)
{
	return (g_mouseStateRelease.rgbButtons[MOUSE_BUTTON_RIGHT] & 0x80 ? true: false);
}

//=============================================================================
// マウスデータ取得(中央プレス)
//=============================================================================
bool GetMouseCenterPress(void)
{
	return (g_mouseState.rgbButtons[MOUSE_BUTTON_CENTER] & 0x80 ? true: false);
}

//=============================================================================
// マウスデータ取得(中央トリガー)
//=============================================================================
bool GetMouseCenterTrigger(void)
{
	return (g_mouseState.rgbButtons[MOUSE_BUTTON_CENTER] & 0x80 ? true: false);
}

//=============================================================================
// マウスデータ取得(Ｘ軸移動)
//=============================================================================
long GetMouseAxisX(void)
{
	return g_mouseState.lX;
}
//=============================================================================
// マウスのスクリーン座標取得
//=============================================================================
POINT GetMouseScreenPos(void)
{
	return g_mouseScreenPos;
}
//=============================================================================
// マウスのクライアント座標取得
//=============================================================================
POINT GetMouseClientPos(void)
{
	return g_mouseClientPos;
}
//=============================================================================
// マウスデータ取得(Ｙ軸移動)
//=============================================================================
long GetMouseAxisY(void)
{
	return g_mouseState.lY;
}

//=============================================================================
// マウスデータ取得(Ｚ軸移動)
//=============================================================================
long GetMouseAxisZ(void)
{
	return g_mouseState.lZ;
}

//=============================================================================
// ジョイパッドの初期化
//=============================================================================
HRESULT InitJoypad(HINSTANCE hInstance, HWND hWnd)
{
	int nLoop;

	HRESULT hr;

	// 入力処理の初期化
	hr = InitInput(hInstance, hWnd);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "DirectInputオブジェクトが作れねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 初期化
	for(nLoop = 0; nLoop < MAX_CONTROLER; nLoop++)
		g_pDIDevJoypad[nLoop] = NULL;

	// デバイスオブジェクトを作成(接続されているジョイパッドを列挙する)
	if(FAILED(g_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyCallback, NULL, DIEDFL_ATTACHEDONLY)))
		return E_FAIL;

	// ジョイパッドの数だけ処理
	for(nLoop = 0; nLoop < MAX_CONTROLER; nLoop++)
	{
		// ジョイパッドがない場合はすっ飛ばす
		if(g_pDIDevJoypad[nLoop] == NULL)
			continue;

		// データフォーマットの設定
		if(FAILED(g_pDIDevJoypad[nLoop]->SetDataFormat(&c_dfDIJoystick)))
			return E_FAIL;

		// 協調レベルの設定
		if(FAILED(g_pDIDevJoypad[nLoop]->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
			return E_FAIL;

		// デバイスへの入力制御開始
		g_pDIDevJoypad[nLoop]->Acquire();	
	}

	return S_OK;
}

//=============================================================================
// ジョイパッド問い合わせ用コールバック関数
//=============================================================================
BOOL CALLBACK EnumJoyCallback(const DIDEVICEINSTANCE* lpddi, VOID* pvRef)
{
	DIDEVCAPS	diDevCaps;			// デバイス情報

	// ジョイパッド用デバイスオブジェクトを作成
	if(FAILED(g_pDInput->CreateDevice(lpddi->guidInstance, &g_pDIDevJoypad[g_nJoypadNum], NULL)))
		return DIENUM_CONTINUE;		// 列挙を続ける

	// ジョイパッドの能力を調べる
	diDevCaps.dwSize = sizeof(DIDEVCAPS);
	if(FAILED(g_pDIDevJoypad[g_nJoypadNum]->GetCapabilities(&diDevCaps)))
	{
		if(g_pDIDevJoypad[g_nJoypadNum])
			g_pDIDevJoypad[g_nJoypadNum]->Release();
		g_pDIDevJoypad[g_nJoypadNum] = NULL;
		return DIENUM_CONTINUE;		// 列挙を続ける
	}

	// 規定数に達したら終了
	g_nJoypadNum++;
	if(g_nJoypadNum == MAX_CONTROLER)
		return DIENUM_STOP;			// 列挙を終了する
	else
		return DIENUM_CONTINUE;		// 列挙を続ける
}

//=============================================================================
// ジョイパッドの終了処理
//=============================================================================
void UninitJoypad(void)
{
	for(int nLoop = 0; nLoop < MAX_CONTROLER; nLoop++)
		SAFE_RELEASE(g_pDIDevJoypad[nLoop]);
}

//=============================================================================
// ジョイパッドの更新処理
//=============================================================================
void UpdateJoypad(void)
{
	int nLoop;
	
	for(nLoop = 0; nLoop < MAX_CONTROLER; nLoop++)
	{
		// 直前ジョイパッド情報のバックアップ
		g_joyStatePrev[nLoop] = g_joyState[nLoop];

		if(g_pDIDevJoypad[nLoop])
		{
			// デバイスからデータを取得
			if(FAILED(g_pDIDevJoypad[nLoop]->GetDeviceState(sizeof(DIJOYSTATE), &g_joyState[nLoop])))
				g_pDIDevJoypad[nLoop]->Acquire();
		}

		// トリガー情報計算
		for( int i = 0; i < 32; i++ )
		{
			if( g_joyState[nLoop].rgbButtons[i] & 0x80 && 
				!(g_joyStatePrev[nLoop].rgbButtons[i] & 0x80) )
			{
				g_joyStateTrigger[nLoop][i] = 0x80;
			} else {
				g_joyStateTrigger[nLoop][i] = 0x00;
			}
		}
	}
}
