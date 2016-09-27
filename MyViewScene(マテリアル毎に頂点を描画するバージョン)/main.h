/*******************************************************************************
* タイトル:		メイン処理
* プログラム名:	main.h
*******************************************************************************/
#ifndef _MAIN_H_
#define _MAIN_H_

#define USE_SOUND		// サウンド使用の有無
//#undef USE_SOUND
//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <windows.h>
#include <fbxsdk.h>
#include "d3dx9.h"
#define DIRECTINPUT_VERSION (0x0800)
#include "dinput.h"
#include "input.h"

#ifdef USE_SOUND
#include "xaudio2.h"		// XAudio2関連のヘッダー
#endif

//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#if 1	// [ここを"0"にした場合、"構成プロパティ" -> "リンカ" -> "入力" -> "追加の依存ファイル"に対象ライブラリを設定する]
#pragma comment (lib, "d3d9.lib")		// 描画処理に必要
#pragma comment (lib, "d3dx9.lib")		// [d3d9.lib]の拡張ライブラリ
#pragma comment (lib, "dxguid.lib")		// DirectXコンポーネント使用に必要
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "winmm.lib")
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#if 1

#define SCREEN_WIDTH	(1760)//(1280)	// ウインドウの幅
#define SCREEN_HEIGHT	(990)//(720)	// ウインドウの高さ

#else

#define SCREEN_WIDTH	(1280)//(1760)	// ウインドウの幅
#define SCREEN_HEIGHT	(720)//(990)	// ウインドウの高さ

#endif

#define NUM_VERTEX (4)
#define NUM_POLYGON (2)

#define SAFE_RELEASE(ptr)			{ if(ptr) { (ptr)->Release(); (ptr) = NULL; } }

/*******************************************************************************
* 構造体定義
*******************************************************************************/
// 上記２Ｄポリゴン頂点フォーマットに合わせた構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標
	float		rhw;	// テクスチャのパースペクティブコレクト用
	D3DCOLOR	col;	// 反射光(頂点カラー)
	D3DXVECTOR2 uv;	// テクスチャ座標
} VERTEX_2D;

// 上記３Ｄポリゴン頂点フォーマットに合わせた構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標
	D3DXVECTOR3 nor;	// 法線ベクトル
	D3DCOLOR	col;	// 反射光
	D3DXVECTOR2 uv;	// テクスチャ座標
} VERTEX_3D;

#if 0
class CMyWindow
{
private:
	float fWidth;
	float fHeight;
	float fRatioNewWidthToOldWidth;		// （現在のクライアント横幅） / （クライアント横幅の初期値）
	float fRatioNewHeightToOldHeight;	// （現在のクライアント縦幅） / （クライアント縦幅の初期値）
public:
	void UpdateSize(void);
};
#else
typedef struct
{
	float fWidth;
	float fHeight;
	float fRatioNewWidthToOldWidth;		// （現在のクライアント横幅） / （クライアント横幅の初期値）
	float fRatioNewHeightToOldHeight;	// （現在のクライアント縦幅） / （クライアント縦幅の初期値）
} SCREEN;
#endif

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LPDIRECT3DDEVICE9 GetDevice(void);

HWND GetHandleWindow(void);
HINSTANCE GetHandleInstance(void);

void InitWindowSize(void);
void UpdateWindowSize(void);
SCREEN GetScreenInfo(void);

#endif