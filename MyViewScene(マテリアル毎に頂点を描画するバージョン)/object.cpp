//********************************************************************************************************************
// タイトル:		オブジェクト処理
// プログラム名:	object.cpp
//********************************************************************************************************************
//********************************************************************************************************************
// インクルードファイル
//********************************************************************************************************************
#include "object.h"
#include <string>
#include <list>
#include "debugProc.h"
#include "effectFile.h"
#include "camera.h"
#include "light.h"

//********************************************************************************************************************
// マクロ定義
//********************************************************************************************************************
#define RANGE_OF_PHI		(D3DX_PI) // 球のＹ軸からの角度φの範囲
#define	MAX_MESH			(10)

//********************************************************************************************************************
// インライン関数
//********************************************************************************************************************
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

//********************************************************************************************************************
// 静的メンバ変数の宣言
//********************************************************************************************************************
int Object::numAll = 0;						// オブジェクトの総数
Object *Object::object[MAX_OBJECT] = {};	// オブジェクトクラスのインスタンスへのポインタ

//********************************************************************************************************************
// プロトタイプ宣言
//********************************************************************************************************************															

//********************************************************************************************************************
// グローバル変数
//********************************************************************************************************************

//**********************************************************************************************************************************************
// 
// Objectクラスの実体
//
//**********************************************************************************************************************************************
//********************************************************************************************************************
// funcinfo：コンストラクタ
//********************************************************************************************************************
Object::Object()
{
	vtxBuff = NULL;
	idxBuff = NULL;
	tex = NULL;

	D3DXMatrixIdentity(&mtxWorld);	
	pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	
	rot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	scl = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );

	numVertex = 0;			// 頂点数
	numIndex = 0;			// インデックス数
	numPolygon = 0;			// ポリゴン数
	numUvSet = 0;			// UVセットの数
	numMaterial = 0;		// マテリアルの数
	
	vtxAry = NULL;			// 頂点バッファ
	idxAry = NULL;			// インデックスバッファ
	polygonSize = NULL;		// １ポリゴン内の頂点数
	normalAry = NULL;		// 法線バッファ
	vtxColor = NULL;		// 頂点カラー
	uvSet = NULL;			// UVセット
	material = NULL;		// マテリアル

	// 総数をインクリメント
	numAll++;
}
//********************************************************************************************************************
// funcinfo：デストラクタ
//********************************************************************************************************************
Object::~Object()
{

}
//********************************************************************************************************************
// funcinfo：初期化
//********************************************************************************************************************
HRESULT Object::Init(int importedFbx)
{
	// ダイレクトデバイスを取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// インポートしたFBXファイルへのポインタを取得
	MyFbxFile* myFbxFile = MyFbxFile::GetMyFbxFile(importedFbx);
	
	// マテリアル数を取得
	this -> numMaterial = myFbxFile -> GetNumMaterial();
	this -> material = myFbxFile -> GetMaterial();
	this -> vtxBuff = new LPDIRECT3DVERTEXBUFFER9[ this -> numMaterial ];

#if _DEBUG
	this -> numPolygon = myFbxFile -> GetNumPolygon();
	this -> numVertex = myFbxFile -> GetNumControlPoints();		
	this -> numIndex = myFbxFile -> GetNumIndex();
#endif

	// 頂点カラーとＵＶが存在するのかどうかを確認するためのGet
	this -> vtxColor = myFbxFile -> GetVertexColorAry();
	this -> uvSet = myFbxFile -> GetUvSet();

	/*this -> numUvSet = myFbxFile -> GetNumUvSet();		
	this -> vtxAry = myFbxFile -> GetControlPointsAry();	
	this -> idxAry = myFbxFile -> GetIndexAry();
	this -> normalAry = myFbxFile -> GetNormalAry();
	
	this -> polygonSize = myFbxFile -> GetPolygonSize();*/

	// マテリアル数分繰り返す
	for( int matCnt = 0; matCnt < this -> numMaterial; matCnt++)
	{
		// オブジェクトの頂点バッファを作成
		if(FAILED(pDevice -> CreateVertexBuffer(sizeof(VERTEX_3D) * this -> material[matCnt].numPolygonVertex,
												D3DUSAGE_WRITEONLY,
												FVF_VERTEX_3D,
												D3DPOOL_MANAGED,
												&this -> vtxBuff[matCnt],
												NULL)))
		{
			return E_FAIL;
		}

		{//頂点バッファの中身を埋める
			VERTEX_3D *vtx;

			// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
			this->vtxBuff[matCnt]->Lock(0,0,(void**)&vtx,0);

			// マテリアルに対応するポリゴン頂点の数だけ繰り返す
			for(int polyVtxCnt = 0; polyVtxCnt < this -> material[matCnt].numPolygonVertex; polyVtxCnt++)
			{
				// 頂点座標の設定
				vtx[polyVtxCnt].pos = this -> material[matCnt].vertexAry[polyVtxCnt].pos;
				
				// 法線の設定
				vtx[polyVtxCnt].nor = this -> material[matCnt].vertexAry[polyVtxCnt].normal;
			
				// 頂点カラーの設定
				if(this->vtxColor != NULL)
				{
					vtx[polyVtxCnt].col = this -> material[matCnt].vertexAry[polyVtxCnt].color;
				}
				else
				{
					vtx[polyVtxCnt].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				}

				// テクスチャ座標の設定
				if(this->uvSet != NULL)
				{
					vtx[polyVtxCnt].uv = this -> material[matCnt].vertexAry[polyVtxCnt].uv;
				}
				else
				{
					vtx[polyVtxCnt].uv = D3DXVECTOR2(0.0f, 0.0f);
				}	
			}

			// 頂点データをアンロックする
			this->vtxBuff[matCnt]->Unlock();
		}
	}

	return S_OK;
}
//********************************************************************************************************************
// funcinfo：終了
//********************************************************************************************************************
void Object::Uninit(void)
{
	for( int cnt = 0; cnt < this -> numMaterial; cnt++ )
	{
		if(this->vtxBuff[cnt] != NULL)
		{// 頂点バッファの開放
			this->vtxBuff[cnt]->Release();
			this->vtxBuff[cnt] = NULL;
		}	
	}
	delete[] this->vtxBuff;
	this->vtxBuff = NULL;

	if(this->idxBuff != NULL)
	{// インデックスバッファの開放
		this->idxBuff->Release();
		this->idxBuff = NULL;
			
	}
}
//********************************************************************************************************************
// funcinfo：更新
//********************************************************************************************************************
void Object::Update(void)
{
	PrintDebugProc("\n");
	for(int materialCnt = 0; materialCnt < this -> numMaterial; materialCnt++)
	{
		PrintDebugProc("[ %d番目のマテリアル ]\n", materialCnt + 1);
		PrintDebugProc("ポリゴン頂点数：%d\n", this -> material[materialCnt].numPolygonVertex);
		PrintDebugProc("ポリゴン数：%d\n", this -> material[materialCnt].numPolygon);
		PrintDebugProc("ambient：[r:%f g:%f b:%f]\n", this -> material[materialCnt].ambient.r, this -> material[materialCnt].ambient.g ,this -> material[materialCnt].ambient.b);
		PrintDebugProc("diffuse：[r:%f g:%f b:%f]\n", this -> material[materialCnt].diffuse.r, this -> material[materialCnt].diffuse.g ,this -> material[materialCnt].diffuse.b);
		PrintDebugProc("\n");
	}

	PrintDebugProc("頂点数：%d\n", this -> numVertex);
	PrintDebugProc("ポリゴン数：%d\n", this -> numPolygon);
	PrintDebugProc("インデックス数：%d\n", this -> numIndex);
	PrintDebugProc("\n");
}
//********************************************************************************************************************
// funcinfo：描画
//********************************************************************************************************************
void Object::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LPD3DXEFFECT pEffect = GetEffect();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate, mWVP;
	CAMERA *pCarema = GetCamera();

	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // カリングなし

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&this->mtxWorld);
	// スケールを反映
	D3DXMatrixScaling(&mtxScl,this->scl.x,this->scl.y,this->scl.z);
	D3DXMatrixMultiply(&this->mtxWorld,&this->mtxWorld,&mtxScl);
	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, this->rot.y, this->rot.x, this->rot.z);
	D3DXMatrixMultiply(&this->mtxWorld, &this->mtxWorld, &mtxRot);
	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, this->pos.x, this->pos.y, this->pos.z);
	D3DXMatrixMultiply(&this->mtxWorld, &this->mtxWorld, &mtxTranslate);

#if 0
	// ワールドマトリックスの設定
	pDevice -> SetTransform(D3DTS_WORLD, &this->mtxWorld);
#endif

	// エフェクト内のグローバル変数をセット
	D3DXMatrixIdentity(&mWVP);
	mWVP = this->mtxWorld * pCarema->mtxView * pCarema->mtxProjection;
	pEffect->SetMatrix( "mWVP", &mWVP );
	pEffect->SetMatrix( "mWorld", &this->mtxWorld );
	pEffect->SetVector( "lightDir0", &D3DXVECTOR4( GetLight(0)->Direction, 0.0f ) );
	pEffect->SetVector( "lightCol0", &D3DXVECTOR4( GetLight(0)->Diffuse.r, GetLight(0)->Diffuse.g, GetLight(0)->Diffuse.b, GetLight(0)->Diffuse.a ) );
	pEffect->SetVector( "lightDir1", &D3DXVECTOR4( GetLight(1)->Direction, 0.0f ) );
	pEffect->SetVector( "lightCol1", &D3DXVECTOR4( GetLight(1)->Diffuse.r, GetLight(1)->Diffuse.g, GetLight(1)->Diffuse.b, GetLight(1)->Diffuse.a ) );
	pEffect->SetVector( "lightDir2", &D3DXVECTOR4( GetLight(2)->Direction, 0.0f ) );
	pEffect->SetVector( "lightCol2", &D3DXVECTOR4( GetLight(2)->Diffuse.r, GetLight(2)->Diffuse.g, GetLight(2)->Diffuse.b, GetLight(2)->Diffuse.a ) );
	
	// テクニックをセット
	pEffect->SetTechnique( "MainShader" );
	// エフェクト開始
	UINT passes;
    pEffect->Begin( &passes, 0 );
    
	// マテリアル数分繰り返す
	for( int matCnt = 0; matCnt < this->numMaterial; matCnt++)
	{
		// エフェクト内のグローバル変数をセット
		pEffect->SetVector( "matDiffuse", &D3DXVECTOR4( this -> material[matCnt].diffuse.r,
														this -> material[matCnt].diffuse.g,
														this -> material[matCnt].diffuse.b,
														0.0f));
		pEffect->SetVector( "matAmbient", &D3DXVECTOR4( this -> material[matCnt].ambient.r,
														this -> material[matCnt].ambient.g,
														this -> material[matCnt].ambient.b,
														0.0f));

		// テクニックのパスの数だけ繰り返す
		for(UINT i=0; i<passes; i++)
		{
			// パスをセット
			pEffect->BeginPass(i);
        
			// 頂点バッファをデバイスのデータストリームにバインド
			pDevice -> SetStreamSource(0, this->vtxBuff[matCnt], 0, sizeof(VERTEX_3D));
			//// インデックスバッファをレンダリングパイプラインに設定
			//pDevice -> SetIndices(idxBuff);
			// 頂点フォーマットの設定
			pDevice -> SetFVF(FVF_VERTEX_3D);
			// テクスチャの設定
			pDevice -> SetTexture(0,this -> tex);
			// ポリゴンの描画
			pDevice -> DrawPrimitive(	D3DPT_TRIANGLELIST,					// プリミティブの描き方 //D3DPT_TRIANGLELIST //D3DPT_TRIANGLESTRIP //D3DPT_TRIANGLEFAN //D3DPT_LINELIST
										0,									// 頂点バッファの描き始めの頂点番号
										this->material[matCnt].numPolygon	// 描画するポリゴン数
										);
		}
		
		// パスの終わり
        pEffect->EndPass();
    }
	// エフェクト終了
    pEffect->End();

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	// 裏面をカリング

}
//********************************************************************************************************************
// funcinfo：総数取得
//********************************************************************************************************************
int Object::GetNumAll(void)
{
	return numAll;
}
//********************************************************************************************************************
// funcinfo：総数を初期化
//********************************************************************************************************************
void Object::ZeroNum(void)
{
	numAll = 0;	
}
//********************************************************************************************************************
// funcinfo：インスタンスを作成
//********************************************************************************************************************
void Object::CreateObject(void)
{
	int index;
	
	for(int cntFbx = 0; cntFbx < MyFbxFile::MAX_FBX; cntFbx++)
	{
		index = numAll;			// object[0]を無視してnewしないための変数

		if(object[index] != NULL)
		{
			object[index] -> Uninit();
			delete object[index];
			object[index] = NULL;
		}

		// インスタンスの生成
		object[index] = new Object;

		// 初期化処理
		object[index] -> Init(cntFbx);
	}
}
//********************************************************************************************************************
// funcinfo：動的に確保したメモリをすべて開放する関数
//********************************************************************************************************************
void Object::ReleaseAll(void)
{
	for( int cnt = 0; cnt < numAll; cnt++)
	{
		if(object[cnt] != NULL)
		{
			object[cnt] -> Uninit();
			delete object[cnt];
			object[cnt] = NULL;
		}
	}
}
//********************************************************************************************************************
// funcinfo：動的に確保したメモリをすべて開放する関数
//********************************************************************************************************************
void Object::UpdateAll(void)
{
	for( int cnt = 0; cnt < numAll; cnt++)
	{
		if(object[cnt] != NULL)
		{
			object[cnt] -> Update();
			
		}
	}
}
//********************************************************************************************************************
// funcinfo：動的に確保したメモリをすべて開放する関数
//********************************************************************************************************************
void Object::DrawAll(void)
{
	for( int cnt = 0; cnt < numAll; cnt++)
	{
		if(object[cnt] != NULL)
		{
			object[cnt] -> Draw();
			
		}
	}
}