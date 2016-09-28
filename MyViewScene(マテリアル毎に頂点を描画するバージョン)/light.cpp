/*******************************************************************************
* タイトル:		ライト処理
* プログラム名:	light.cpp
*******************************************************************************/
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "light.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_LIGHT (8)					// ライトの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
D3DLIGHT9 g_aLight[NUM_LIGHT];			// ライト情報

/*******************************************************************************
関数名:	void InitLight(void)
引数:	なし
戻り値:	なし
説明:	ライトの初期化
*******************************************************************************/
HRESULT InitLight(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXVECTOR3 vecDir;

	for(int nCntLight = 0; nCntLight < NUM_LIGHT; nCntLight++)
	{
		// D3DLIGHT9構造体を0でクリアする
		ZeroMemory(&g_aLight[nCntLight], sizeof(D3DLIGHT9));
		// ライト0のタイプの設定
		g_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;
		switch(nCntLight)
		{
		case 0:
			// ライト0の拡散光の設定
			g_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			// ライト0の方向の設定
			vecDir = D3DXVECTOR3(0.0f, -1.0f, -1.0f);
			break;
		case 1:
			g_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			vecDir = D3DXVECTOR3(-1.0f, -0.5f, 1.0f);
			break;
		case 2:
			g_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			vecDir = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			break;
		case 3:
			g_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			vecDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			break;
		case 4:
			g_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			vecDir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
			break;
		case 5:
			g_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			vecDir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		default:
			g_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			vecDir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		}
		D3DXVec3Normalize((D3DXVECTOR3 *) &g_aLight[nCntLight].Direction, &vecDir);
		// ライトのAmbientの設定
		g_aLight[nCntLight].Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		// ライト0をレンダリングパイプラインに設定
		pDevice -> SetLight(nCntLight, &g_aLight[nCntLight]);
		// ライト0を使用使用状態に
		pDevice -> LightEnable(nCntLight, TRUE);
	}
	// ライティングモードをON
	pDevice -> SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

//=============================================================================
// ライトの終了処理
//=============================================================================
void UninitLight(void)
{
}



//=============================================================================
// ライトの更新処理
//=============================================================================
void UpdateLight(void)
{
}
//=============================================================================
// ライトを取得
//=============================================================================
D3DLIGHT9 *GetLight(int num)
{
	return &g_aLight[num];
}