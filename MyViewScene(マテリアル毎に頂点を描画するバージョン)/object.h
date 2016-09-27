//********************************************************************************************************************
// タイトル:		オブジェクト処理
// プログラム名:	object.h
//********************************************************************************************************************
#ifndef _OBJECT_H_
#define _OBJECT_H_

//********************************************************************************************************************
// インクルードファイル
//********************************************************************************************************************
#include "main.h"
#include "myFbxImporter.h"

#define MAX_OBJECT (256)

//********************************************************************************************************************
// クラス
//********************************************************************************************************************
// オブジェクトクラス
class Object
{
public:
	Object();								// コンストラクタ
	~Object();								// デストラクタ
	void Init(void);						// 初期化関数
	HRESULT Init(int importedFbx);			// インポートされたFBXを下記のメンバ変数に代入する初期化関数
	void Uninit(void);						// 終了関数
	void Update(void);						// 更新関数
	void Draw(void);						// 描画関数
	static	int GetNumAll(void);			// オブジェクトの総数を取得
	static	void ZeroNum(void);				// オブジェクトの総数を初期化
	static	void CreateObject(void);		// オブジェクトを作成するクラス
	static	void ReleaseAll(void);			// 動的に確保したメモリをすべて開放する関数
	static	void UpdateAll(void);			// オブジェクトすべての更新関数
	static	void DrawAll(void);				// オブジェクトすべての描画関数
protected:
	LPDIRECT3DVERTEXBUFFER9 *vtxBuff;		// 頂点バッファへのダブルポインタ
	LPDIRECT3DINDEXBUFFER9	idxBuff;		// インデックスバッファへのポインタ
	LPDIRECT3DTEXTURE9		tex;			// テクスチャへのポインタ
	
	D3DXMATRIX				mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3				pos;			// ポリゴン表示位置の中心座標
	D3DXVECTOR3				rot;			// ポリゴンの回転角
	D3DXVECTOR3				scl;			// 拡大縮小率

	int			numMaterial;		// マテリアルの数
	int			numVertex;			// 頂点数
	int			numIndex;			// インデックス数
	int			numPolygon;			// ポリゴン数
	int			numUvSet;			// UVセットの数
	
	MyMaterial	*material;			// マテリアル
	D3DXVECTOR3	*vtxAry;			// 頂点バッファ
	int			*idxAry;			// インデックスバッファ
	int			*polygonSize;		// １ポリゴン内の頂点数
	D3DXVECTOR3 *normalAry;			// 法線バッファ
	D3DXCOLOR	*vtxColor;			// 頂点カラー
	UVSET		*uvSet;				// UVセット
	
private:
	static int numAll;
	static Object *object[MAX_OBJECT];
};

#endif