/*******************************************************************************
* タイトル:		入力処理ヘッダーファイル
* プログラム名:	input.h
*******************************************************************************/
#ifndef _INPUT_H_
#define _INPUT_H_

#include "main.h"

/*******************************************************************************
* 列挙体定義
*******************************************************************************/
typedef enum
{
	CURSOR_HAND = 0,
	CURSOR_JELLYFISH,
	MAX_CURSOR
} CURSOR;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// キーボード関連
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd);
void UninitKeyboard(void);
void UpdateKeyboard(void);
bool GetKeyboardPress(int nkey);
bool GetKeyboardTrigger(int nkey);
bool GetKeyboardRelease(int nkey);
bool GetKeyboardRepeat(int nkey);

// マウス関連
HRESULT InitMouse(HINSTANCE hInstance, HWND hWnd);
void UninitMouse(void);
void UpdateMouse(void);
void SetCursor(CURSOR Cursor);
HCURSOR *GetpHandleCursor(void);
bool GetMouseLeftPress(void);
bool GetMouseLeftTrigger(void);
bool GetMouseLeftRelease(void);
bool GetMouseRightPress(void);
bool GetMouseRightTrigger(void);
bool GetMouseRightRelease(void);
bool GetMouseCenterPress(void);
bool GetMouseCenterTrigger(void);
POINT GetMouseScreenPos(void);
POINT GetMouseClientPos(void);
long GetMouseAxisX(void);
long GetMouseAxisY(void);
long GetMouseAxisZ(void);

// ジョイパッド関連
HRESULT InitJoypad(HINSTANCE hInstance, HWND hWnd);
void UninitJoypad(void);
void UpdateJoypad(void);
BOOL CALLBACK EnumJoyCallback(const DIDEVICEINSTANCE* lpddi, VOID* pvRef);

#endif