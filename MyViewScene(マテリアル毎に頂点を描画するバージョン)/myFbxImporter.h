//********************************************************************************************************************
// タイトル:		FBXインポート処理
// プログラム名:	myFbxImporter.h
//********************************************************************************************************************
#ifndef _MYFBXIMPORTER_H_
#define _MYFBXIMPORTER_H_

//********************************************************************************************************************
// インクルードファイル
//********************************************************************************************************************
#include "main.h"
#include <string>
#include <list>

//********************************************************************************************************************
// 前方宣言
//********************************************************************************************************************
class MyFbxFile;

//********************************************************************************************************************
// 構造体・共用体
//********************************************************************************************************************
// MyVector3共用体
union MyVector3
{
	struct { float x, y, z; };
	struct { float r, g, b; };
	float m[ 3 ];
};
// ＵＶ用の２変数構造体
typedef struct Point2DF
{
  float x, y;

  bool operator==(Point2DF& val)
  {
    if(this->x == val.x && this->y == val.y)
	{
      return true;
    }
    return false;
  }
} point2;
// UVSET構造体
typedef struct UVSET
{
	std::string uvSetName;
	std::list<std::string> textures;
	std::list<point2> uvBuffer;
} UvSet;
// MyVertex構造体
typedef struct MYVERTEX
{
	D3DXVECTOR3		pos;
	D3DXVECTOR3		normal;
	D3DXCOLOR		color;
	D3DXVECTOR2		uv;
} MyVertex;
// MyMaterial構造体
typedef struct MYMATERIAL
{
	// マテリアル情報
	MyVector3	ambient;
	MyVector3	diffuse;
	MyVector3	emissive;
	MyVector3	bump;
	float		transparency;
	MyVector3	specular;
	float		shininess;
	float		reflectivity;
	// マテリアルに対応した情報
	int			*polygonIndexAry;
	int			numPolygon;
	MyVertex	*vertexAry;					// ポリゴン頂点配列へのポインタ
	int			numPolygonVertex;			// マテリアルに対応するポリゴン頂点の総数
} MyMaterial;

//********************************************************************************************************************
// クラス
//********************************************************************************************************************
// FBXインポータクラス
class MyFbxImporter
{
public:
	MyFbxImporter();
	~MyFbxImporter();
	/* FBXの情報を取得するメイン関数 */
	static void GetFbxInfo(void);

	/* 共通の初期化処理 */
	// FbxManagerを作成　IOSettingsを作成　FbxSceneObject（インポートするFbxファイル情報を保持する）を作成
	static void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
	static void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
	static void CreateAndFillIOSettings(FbxManager* pManager);
	static bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);
	static bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat=-1, bool pEmbedMedia=false);
};

// FBXファイルクラス
class MyFbxFile
{
public:
	// インポートするFBXファイル
	typedef enum
	{
		TEST_FBX = 0,
		MAX_FBX
	} IMPORTED_FBX;
	// 取得するFBXファイル情報の種類
	/*typedef enum
	{
		VERTEX_NUM = 0,
		INDEX_NUM,
		POLYGON_NUM,
		UVSET_NUM,
		MATERIAL_NUM,
		VERTEX_BUFF,		
		INDEX_BUFF,		
		POLYGON_SIZE,
		NORMAL_BUFF,	
		VERTEX_COLOR,
		UV_SET,	
		MATERIAL,
		MAX_FBX_INFO
	} FBX_INFO;*/
	/* メンバ関数 */
	MyFbxFile();
	~MyFbxFile();
	// グローバルタイムセットを取得する
	void GetGlobalTimeSettings(FbxGlobalSettings* pGlobalSettings);
	// FBXの中身をすべて取得する
	void GetContent(FbxScene* pScene);
	void GetContent(FbxNode* pNode);
	void GetMesh(FbxNode* pNode);
	void GetMaterial(FbxMesh* pMesh);
	void GetLambertInfo(FbxSurfaceLambert* lambert, int materialCnt); // LambertMatを取得する
	void GetMaterialConnection(FbxMesh* pMesh);
	void GetControlPoints(FbxMesh* pMesh);
	void GetNormal(FbxMesh* pMesh);
	void GetVertexColor(FbxMesh* pMesh);
	void GetUV(FbxMesh* pMesh);
	void GetTextureNames(FbxMesh* pMesh);
	void CreateVertexBuffer(void);

	// FBXファイルの情報（下記のメンバ変数）をinfoにセットする関数
	int				GetNumMaterial(void);
	int				GetNumPolygon(void);
	int				GetNumControlPoints(void);
	int				GetNumIndex(void);
	int				GetNumUvSet(void);
	MyMaterial*		GetMaterial(void);
	D3DXVECTOR3*	GetControlPointsAry(void);
	int*			GetIndexAry(void);
	D3DXVECTOR3*	GetNormalAry(void);
	D3DXCOLOR*		GetVertexColorAry(void);
	UVSET*			GetUvSet(void);
	int*			GetPolygonSize(void);
	// FBXファイルの情報を下記のメンバ変数に代入する関数
	static void CreateFbxInfo(FbxScene* pScene);
	// FBXファイルへのポインタを取得する関数
	static MyFbxFile* GetMyFbxFile(int importedFbx);
	// 動的に確保したメモリをすべて開放する関数
	static void ReleaseAll(void);
	void Uninit(void);
	
private:
	FbxTime		m_lStart;	// スタートフレーム
	FbxTime		m_lEnd;		// エンドフレーム
	
	int			numMaterial;			// マテリアルの数
	int			numPolygon;				// ポリゴン総数
	int			numControlPoint;		// コントロール点総数
	int			numIndex;				// インデックス数
	int			numUvSet;				// UVセットの数
	
	MyMaterial	*materialAry;			// マテリアル
	D3DXVECTOR3	*controlPointsAry;		// コントロール点配列
	int			*vertexIndexAry;		// インデックス配列へのポインタ
	D3DXVECTOR3 *normalAry;				// 法線配列
	D3DXCOLOR	*vertexColorAry;		// 頂点カラー
	UVSET		*uvSet;					// UVセット配列
	int			*polygonSize;			// １ポリゴン内の頂点数
	
	FbxGeometryElement::EMappingMode normalMapMode;
	FbxGeometryElement::EMappingMode vertexColorMapMode;
	FbxGeometryElement::EMappingMode uvMapMode;

	static	MyFbxFile	*m_myFbxFile[MAX_FBX];
	static	int			m_myFbxFileNum;
	// Myメッシュクラスのインスタンス（頂点・ポリゴン・マテリアル・ボーン）
};



#endif