/*******************************************************************************
* タイトル:		カメラ処理
* プログラム名:	camera.cpp
*******************************************************************************/
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "camera.h"
#include "input.h"
#include "debugProc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	CAM_POS_V_X				(0.0f)			// カメラの視点初期位置(X座標)
#define	CAM_POS_V_Y				(20.0f)			// カメラの視点初期位置(Y座標)
#define	CAM_POS_V_Z				(-20.0f)		// カメラの視点初期位置(Z座標)
#define	CAM_POS_R_X				(0.0f)			// カメラの注視点初期位置(X座標)
#define	CAM_POS_R_Y				(0.0f)			// カメラの注視点初期位置(Y座標)
#define	CAM_POS_R_Z				(0.0f)			// カメラの注視点初期位置(Z座標)

#define	VIEW_ANGLE				(D3DXToRadian(45.0f))							// ビュー平面の視野角
#define	VIEW_ASPECT				((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比
#define	VIEW_NEAR_Z				(10.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z				(10000.0f)										// ビュー平面のFarZ値
#define	VALUE_MOVE_CAMERA		(0.2f)											// カメラの移動量
#define	VALUE_ROTATE_CAMERA		(D3DX_PI * 0.002f)								// カメラの回転量
#define	VALUE_ADJUST_DISTANCE	(2.0f)											// 視点と注視点の距離調整量
#define HALF_OF_PI				(D3DX_PI / 2)

#define	DISTANCE_CAMERA_R (30)					// モデルの視線の先までの距離

#define	CHASE_HEIGHT_V (CAM_POS_V_Y)			// 追跡時の視点の高さ
#define	CHASE_HEIGHT_R (CAM_POS_R_Y)			// 追跡時の注視点の高さ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
CAMERA g_camera;		// カメラ情報

CAMERA_MODE g_cameraMode = RELEASE_CAMERA;
/*******************************************************************************
関数名:	HRESULT InitCamera(void)
引数:	なし
戻り値:	HRESULT型
説明:	カメラの初期化
*******************************************************************************/
HRESULT InitCamera(void)
{
	// 視点初期化
	g_camera.vPosV = D3DXVECTOR3(CAM_POS_V_X,CAM_POS_V_Y,CAM_POS_V_Z);
	// 注視点初期化
	g_camera.vPosR = D3DXVECTOR3(CAM_POS_R_X,CAM_POS_R_Y,CAM_POS_R_Z);
	// 上方向ベクトル初期化
	g_camera.vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	// 視点の目的位置
	g_camera.vPosVDest = g_camera.vPosV;
	// 注視点の目的位置
	g_camera.vPosRDest = g_camera.vPosR;
	// カメラ向き（回転角）の初期化
	g_camera.vRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	// 視点と注視点の距離を求める
	g_camera.fDistance = sqrt(powf(g_camera.vPosR.x - g_camera.vPosV.x, 2.0f) +  powf(g_camera.vPosR.y - g_camera.vPosV.y, 2.0f) + powf(g_camera.vPosR.z - g_camera.vPosV.z, 2.0f));
	// 始めの視点と注視点の距離
	g_camera.fInitialDistance = g_camera.fDistance;
	// 視点の高さを初期化
	g_camera.fChaseHeightV = CHASE_HEIGHT_V;
	// 注視点の高さを初期化
	g_camera.fChaseHeightR = CHASE_HEIGHT_R;
	// 始めの視点と注視点が作る直角三角形の角度の初期化
	g_camera.fInitialSin = fabs((g_camera.vPosR.y - g_camera.vPosV.y) / g_camera.fInitialDistance);
	g_camera.fInitialCos = fabs((g_camera.vPosR.z - g_camera.vPosV.z) / g_camera.fInitialDistance);

	return S_OK;
}

//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{

}

//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{
	g_camera.fDistance = sqrt(powf(g_camera.vPosR.x - g_camera.vPosV.x, 2.0f) +  powf(g_camera.vPosR.y - g_camera.vPosV.y, 2.0f) + powf(g_camera.vPosR.z - g_camera.vPosV.z, 2.0f));

	// Camera Select
	if(GetKeyboardPress(DIK_F1))
	{
		g_cameraMode = DEBUG_CAMERA;
	}
	if(GetKeyboardPress(DIK_F2))
	{
		g_cameraMode = RELEASE_CAMERA;
		InitCamera();
	}


	if(g_cameraMode == DEBUG_CAMERA)
	{
		g_camera.fDistanceXZ = sqrt(powf(g_camera.vPosR.x - g_camera.vPosV.x, 2.0f) + powf(g_camera.vPosR.z - g_camera.vPosV.z, 2.0f));
		g_camera.vecVR = D3DXVECTOR3(g_camera.vPosR.x - g_camera.vPosV.x, 0.0f, g_camera.vPosR.z - g_camera.vPosV.z) / g_camera.fDistanceXZ;
		g_camera.vecVerToVR = D3DXVECTOR3(-(g_camera.vPosR.z - g_camera.vPosV.z), 0.0f, g_camera.vPosR.x - g_camera.vPosV.x) / g_camera.fDistanceXZ;
	
		/* 注視点操作 */
		// 左旋回(視点を中心に注視点が回る)
		if(GetKeyboardPress(DIK_J))
		{
			g_camera.vRot.y -= VALUE_ROTATE_CAMERA;
			if(g_camera.vRot.y < -D3DX_PI)
			{
				g_camera.vRot.y += D3DX_PI * 2;
			}
			g_camera.vPosR.x = g_camera.fDistanceXZ * sinf(g_camera.vRot.y) + g_camera.vPosV.x;
			g_camera.vPosR.z = g_camera.fDistanceXZ * cosf(g_camera.vRot.y) + g_camera.vPosV.z;
		}
		// 右旋回(視点を中心に注視点が回る)
		if(GetKeyboardPress(DIK_L))
		{
			g_camera.vRot.y += VALUE_ROTATE_CAMERA;
			if(g_camera.vRot.y > D3DX_PI)
			{
				g_camera.vRot.y -= D3DX_PI * 2;
			}
			g_camera.vPosR.x = g_camera.fDistanceXZ * sinf(g_camera.vRot.y) + g_camera.vPosV.x;
			g_camera.vPosR.z = g_camera.fDistanceXZ * cosf(g_camera.vRot.y) + g_camera.vPosV.z;
		}
		// 注視点上移動
		if(GetKeyboardPress(DIK_I))
		{
			g_camera.vPosR.y += VALUE_MOVE_CAMERA;
		}
		// 注視点下移動
		if(GetKeyboardPress(DIK_K))
		{
			g_camera.vPosR.y -= VALUE_MOVE_CAMERA;
		}
	
		// 視点平行移動
		// 上移動
		if(GetKeyboardPress(DIK_T))
		{
			g_camera.vPosR += g_camera.vecVR * VALUE_MOVE_CAMERA;
			g_camera.vPosV += g_camera.vecVR * VALUE_MOVE_CAMERA;
		}
		// 下移動
		if(GetKeyboardPress(DIK_G))
		{
			g_camera.vPosR -= g_camera.vecVR * VALUE_MOVE_CAMERA;
			g_camera.vPosV -= g_camera.vecVR * VALUE_MOVE_CAMERA;
		}
		// 右移動
		if(GetKeyboardPress(DIK_H))
		{
			g_camera.vPosR -= g_camera.vecVerToVR * VALUE_MOVE_CAMERA;
			g_camera.vPosV -= g_camera.vecVerToVR * VALUE_MOVE_CAMERA;
		}
		// 左移動
		if(GetKeyboardPress(DIK_F))
		{
			g_camera.vPosR += g_camera.vecVerToVR * VALUE_MOVE_CAMERA;
			g_camera.vPosV += g_camera.vecVerToVR * VALUE_MOVE_CAMERA;
		}
	}
}

//=============================================================================
// カメラの設定処理
//=============================================================================
void SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&g_camera.mtxView);
	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&g_camera.mtxView, &g_camera.vPosV, &g_camera.vPosR, &g_camera.vecU);
	// ビューマトリックスの設定
	pDevice -> SetTransform(D3DTS_VIEW, &g_camera.mtxView);
	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&g_camera.mtxProjection);
	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&g_camera.mtxProjection, VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);
	// プロジェクションマトリックスの設定(透視変換の設定)
	pDevice -> SetTransform(D3DTS_PROJECTION, &g_camera.mtxProjection);
}

/*******************************************************************************
関数名:	CAMERA *GetCamera(void)
説明:	カメラのポインタを取得
*******************************************************************************/
CAMERA *GetCamera(void)
{
	return &g_camera;
}
