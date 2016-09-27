//********************************************************************************************************************
// タイトル:		FBXインポート処理
// プログラム名:	myFbxImporter.cpp
//********************************************************************************************************************
//********************************************************************************************************************
// インクルードファイル
//********************************************************************************************************************
#include "myFbxImporter.h"

//********************************************************************************************************************
// マクロ定義
//********************************************************************************************************************
#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pManager->GetIOSettings()))
#endif

//********************************************************************************************************************
// インライン関数
//********************************************************************************************************************

//********************************************************************************************************************
// 静的メンバ変数の宣言
//********************************************************************************************************************
MyFbxFile	*MyFbxFile::m_myFbxFile[MAX_FBX] = {};
int			MyFbxFile::m_myFbxFileNum = 0;

//**********************************************************************************************************************************************
// 
// MyFbxImporterクラスの実体
//
//**********************************************************************************************************************************************
//********************************************************************************************************************
// funcinfo：コンストラクタ
//********************************************************************************************************************
MyFbxImporter::MyFbxImporter()
{

}
//********************************************************************************************************************
// funcinfo：デストラクタ
//********************************************************************************************************************
MyFbxImporter::~MyFbxImporter()
{

}
//********************************************************************************************************************
// funcinfo：FBXの情報を取得するメイン関数
//********************************************************************************************************************
void MyFbxImporter::GetFbxInfo(void)
{
	FbxManager* lSdkManager = NULL;
    FbxScene* lScene = NULL;
    bool lResult;

	// FbxManagerを作成　IOSettingsを作成　FbxSceneObject（インポートするFbxファイル情報を保持する）を作成
    InitializeSdkObjects(lSdkManager, lScene);

    // FBXファイルへのパスを格納
	FbxString lFilePath[MyFbxFile::MAX_FBX] = {
		("data/MODEL/first1.fbx"),
		//("data/MODEL/monstercharacter.fbx"),
	};

	// ポリゴンを三角化するためのコンバーター
	FbxGeometryConverter lGeomConverter(lSdkManager);
	
	//読み込みたいFBXの分だけ繰り返す
	for(int cntFile = 0; cntFile < MyFbxFile::MAX_FBX; cntFile++)
	{
		if( lFilePath[cntFile].IsEmpty() )
		{
			lResult = false;
		}
		else
		{
			// FBXファイルをシーンにインポート
			lResult = LoadScene(lSdkManager, lScene, lFilePath[cntFile].Buffer());
			// ポリゴンを三角化
			lGeomConverter.Triangulate( lScene, true, false );	
		}

		if(lResult == true)
		{
			// FBXファイルの描画に必要な情報を作成する
			//（読み込みたいFBXの分だけシーンを取り替えて繰り返されるようにしてある）
			MyFbxFile::CreateFbxInfo(lScene);
		}
	}

    // Destroy all objects created by the FBX SDK.
    DestroySdkObjects(lSdkManager, lResult);

    return;
}
//********************************************************************************************************************
// funcinfo：FbxManagerを作成　IOSettingsを作成　FbxSceneObject（インポートするFbxファイル情報を保持する）を作成
//********************************************************************************************************************
void MyFbxImporter::InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
    //The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
    pManager = FbxManager::Create();
    if( !pManager )
    {
        //FBXSDK_printf("Error: Unable to create FBX Manager!\n");
        exit(1);
    }
	else
	{
		//FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());
	}

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

    //Create an FBX scene. This object holds most objects imported/exported from/to files.
    pScene = FbxScene::Create(pManager, "My Scene");
	if( !pScene )
    {
        //FBXSDK_printf("Error: Unable to create FBX scene!\n");
        exit(1);
    }
}
//********************************************************************************************************************
// funcinfo：
//********************************************************************************************************************
void MyFbxImporter::DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
    //Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
    if( pManager ) pManager->Destroy();
	//if( pExitStatus ) FBXSDK_printf("Program Success!\n");
}
//********************************************************************************************************************
// funcinfo：
//********************************************************************************************************************
bool MyFbxImporter::LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    //int lFileFormat = -1;
    //int i, lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(pManager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
        FbxString error = lImporter->GetStatus().GetErrorString();
        //FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
       // FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

        if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            //FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            //FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    //FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        //FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        //// From this point, it is possible to access animation stack information without
        //// the expense of loading the entire file.

        //FBXSDK_printf("Animation Stack Information\n");

        //lAnimStackCount = lImporter->GetAnimStackCount();

        //FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
        //FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        //FBXSDK_printf("\n");

        //for(i = 0; i < lAnimStackCount; i++)
        //{
        //    FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

        //    FBXSDK_printf("    Animation Stack %d\n", i);
        //    FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
        //    FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

        //    // Change the value of the import name if the animation stack should be imported 
        //    // under a different name.
        //    FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

        //    // Set the value of the import state to false if the animation stack should be not
        //    // be imported. 
        //    FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
        //    FBXSDK_printf("\n");
        //}

        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
        IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
        IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
        IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
        IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
        IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
        IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);

    //if(lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
    //{
    //    //FBXSDK_printf("Please enter password: ");

    //    lPassword[0] = '\0';

    //    FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
    //    scanf("%s", lPassword);
    //    FBXSDK_CRT_SECURE_NO_WARNING_END

    //    FbxString lString(lPassword);

    //    IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
    //    IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

    //    lStatus = lImporter->Import(pScene);

    //    if(lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
    //    {
    //        FBXSDK_printf("\nPassword is wrong, import aborted.\n");
    //    }
    //}

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}
//********************************************************************************************************************
// funcinfo：
//********************************************************************************************************************
bool MyFbxImporter::SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
{
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    FbxExporter* lExporter = FbxExporter::Create(pManager, "");

    if( pFileFormat < 0 || pFileFormat >= pManager->GetIOPluginRegistry()->GetWriterFormatCount() )
    {
        // Write in fall back format in less no ASCII format found
        pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        //Try to export in ASCII if possible
        int lFormatIndex, lFormatCount = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

        for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
        {
            if (pManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
            {
                FbxString lDesc =pManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                const char *lASCII = "ascii";
                if (lDesc.Find(lASCII)>=0)
                {
                    pFileFormat = lFormatIndex;
                    break;
                }
            }
        } 
    }

    // Set the export states. By default, the export states are always set to 
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
    // shows how to change these states.
    IOS_REF.SetBoolProp(EXP_FBX_MATERIAL,        true);
    IOS_REF.SetBoolProp(EXP_FBX_TEXTURE,         true);
    IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED,        pEmbedMedia);
    IOS_REF.SetBoolProp(EXP_FBX_SHAPE,           true);
    IOS_REF.SetBoolProp(EXP_FBX_GOBO,            true);
    IOS_REF.SetBoolProp(EXP_FBX_ANIMATION,       true);
    IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Initialize the exporter by providing a filename.
    if(lExporter->Initialize(pFilename, pFileFormat, pManager->GetIOSettings()) == false)
    {
        FBXSDK_printf("Call to FbxExporter::Initialize() failed.\n");
        FBXSDK_printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
        return false;
    }

    FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
    FBXSDK_printf("FBX file format version %d.%d.%d\n\n", lMajor, lMinor, lRevision);

    // Export the scene.
    lStatus = lExporter->Export(pScene); 

    // Destroy the exporter.
    lExporter->Destroy();
    return lStatus;
}
//**********************************************************************************************************************************************
// 
// MyFbxFileクラスの実体
//
//**********************************************************************************************************************************************
//********************************************************************************************************************
// funcinfo：コンストラクタ
//********************************************************************************************************************
MyFbxFile::MyFbxFile()
{
	numControlPoint = 0;		
	numIndex = 0;			
	numPolygon = 0;		
	this->numUvSet = 0;			
	this->numMaterial = 0;

	controlPointsAry = NULL;		
	vertexIndexAry = NULL;		
	polygonSize = NULL;	
	this->normalAry = NULL;	
	this->vertexColorAry = NULL;
	this->uvSet = NULL;		
	materialAry = NULL;

	// FBXファイル数をインクリメント
	m_myFbxFileNum++;
}
//********************************************************************************************************************
// funcinfo：デストラクタ
//********************************************************************************************************************
MyFbxFile::~MyFbxFile()
{

}
//********************************************************************************************************************
// funcinfo：スタートフレーム　エンドフレームをシーンから取得
//********************************************************************************************************************
void MyFbxFile::GetGlobalTimeSettings(FbxGlobalSettings* pGlobalSettings)
{
    FbxTimeSpan lTs;
    pGlobalSettings->GetTimelineDefaultTimeSpan(lTs);
	// スタートフレーム
    m_lStart = lTs.GetStart();
	// エンドフレーム
    m_lEnd   = lTs.GetStop();
}
//********************************************************************************************************************
// funcinfo：シーンからルートノードの子を取得し、GetContent(FbxNode* pNode)に子ノードを渡す関数
//********************************************************************************************************************
void MyFbxFile::GetContent(FbxScene* pScene)
{
	int i;
	// シーンからルートノードを取得
    FbxNode* lNode = pScene->GetRootNode();

    if(lNode)
    {
		// ルートノードの子供分繰り返す
        for(i = 0; i < lNode->GetChildCount(); i++)
        {
            GetContent(lNode->GetChild(i));
        }
    }
}
//********************************************************************************************************************
// funcinfo：渡されたノードの属性を取得し、属性毎に異なる関数にノードを渡し処理がされた後、再帰的に更なる子ノードを自分に渡す関数
//********************************************************************************************************************
void MyFbxFile::GetContent(FbxNode* pNode)
{
	FbxNodeAttribute::EType lAttributeType;
    int i;

	// ノードの属性が存在しないならば
    if(pNode->GetNodeAttribute() == NULL)
    {
        FBXSDK_printf("NULL Node Attribute\n\n");
    }
	// ノードの属性が存在するならば
    else
    {
		// ノードの属性を取得
        lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

        switch (lAttributeType)
        {
	    default:
	        break;
        case FbxNodeAttribute::eMarker:  
            //DisplayMarker(pNode);
            break;

        case FbxNodeAttribute::eSkeleton:  
            //DisplaySkeleton(pNode);
            break;

        case FbxNodeAttribute::eMesh:
			// メッシュ情報を取得
            GetMesh(pNode);
            break;

        case FbxNodeAttribute::eNurbs:      
            //DisplayNurb(pNode);
            break;

        case FbxNodeAttribute::ePatch:     
            //DisplayPatch(pNode);
            break;

        case FbxNodeAttribute::eCamera:    
            //DisplayCamera(pNode);
            break;

        case FbxNodeAttribute::eLight:     
            //DisplayLight(pNode);
            break;

        case FbxNodeAttribute::eLODGroup:
            //DisplayLodGroup(pNode);
            break;
        }   
    }

    /*DisplayUserProperties(pNode);
    DisplayTarget(pNode);
    DisplayPivotsAndLimits(pNode);
    DisplayTransformPropagation(pNode);
    DisplayGeometricTransform(pNode);*/

    for(i = 0; i < pNode->GetChildCount(); i++)
    {
        GetContent(pNode->GetChild(i));
    }
}
//********************************************************************************************************************
// funcinfo：ノードからメッシュの情報を取得する関数
//********************************************************************************************************************
void MyFbxFile::GetMesh(FbxNode* pNode)
{
	// ノードをメッシュにキャストする
	FbxMesh* lMesh = (FbxMesh*) pNode->GetNodeAttribute ();

	// マテリアル情報を取得
	GetMaterial(lMesh);
	// マテリアルとポリゴンの対応を取得
	GetMaterialConnection(lMesh);
	// 頂点情報を取得する
    GetControlPoints(lMesh);
	// 法線情報を取得する
	GetNormal(lMesh);
	// 頂点カラー情報を取得
	GetVertexColor(lMesh);
	// UV情報を取得
	GetUV(lMesh);
	// テクスチャ情報を取得
	GetTextureNames(lMesh);
	// マテリアル毎に頂点バッファを作成
	CreateVertex();
}
//********************************************************************************************************************
// funcinfo：マテリアル対応ポリゴン頂点配列を作成、マテリアル対応頂点数を計算
//********************************************************************************************************************
void MyFbxFile::CreateVertex(void)
{
	/* マテリアルに対応するポリゴンの頂点数を取得 */
	// マテリアル対応頂点数を初期ポリゴン化
	for(int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		this->materialAry[materialCnt].numPolygonVertex = 0;
	}
	// マテリアル数分だけ繰り返す
	for(int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		// それぞれのマテリアルに対応するポリゴン数分繰り返す
		for(int eachMatPolyCnt = 0; eachMatPolyCnt < this->materialAry[materialCnt].numPolygon; eachMatPolyCnt++)
		{
			// 各ポリゴンの頂点数分だけ繰り返す（三角化をしている前提で３と記述してある）
			for (int cnt = 0; cnt < 3; cnt++)
			{
				// マテリアルに対応するポリゴンの総数をカウントアップ
				this->materialAry[materialCnt].numPolygonVertex++;
			}
		}
	}

	/* それぞれのマテリアルに対応するポリゴン頂点の総数分vertexAryを生成する */
	for(int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		this->materialAry[materialCnt].vertexAry = new MyVertex[ this->materialAry[materialCnt].numPolygonVertex ];
	}

	/* vertexAryにポリゴン頂点座標情報を格納 */
	// ポリゴン頂点のカウント
	int PolyVtxCntInMat = 0;             
	// マテリアル数分だけ繰り返す    
	for(int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		// それぞれのマテリアルに対応するポリゴン数分繰り返す
		for(int eachMatPolyCnt = 0; eachMatPolyCnt < this->materialAry[materialCnt].numPolygon; eachMatPolyCnt++)
		{
			// 各ポリゴンの頂点数分だけ繰り返す（三角化をしている前提で３と記述してある）
			for (int cnt = 0; cnt < 3; cnt++)
			{
				// ポリゴン頂点番号を計算（三角化をしている前提で３と記述してある）
				int PolyVtxIndex = cnt + 3 * this->materialAry[materialCnt].polygonIndexAry[eachMatPolyCnt];

				// ポリゴン頂点座標にコントロール点座標を格納する
				this->materialAry[materialCnt].vertexAry[PolyVtxCntInMat].pos = controlPointsAry[ vertexIndexAry[PolyVtxIndex] ];

				// マッピングモードによる格納方法の分岐
				/**************************************/
				// 法線を格納する
				switch(normalMapMode)
				{
				case FbxGeometryElement::eByControlPoint:
					this->materialAry[materialCnt].vertexAry[PolyVtxCntInMat].normal = normalAry[ vertexIndexAry[PolyVtxIndex] ];
					break;
				case FbxGeometryElement::eByPolygonVertex:
					this->materialAry[materialCnt].vertexAry[PolyVtxCntInMat].normal = normalAry[ PolyVtxIndex ];
					break;
				default:
					break;
				}
				// 頂点カラーを格納する
				switch(vertexColorMapMode)
				{
				case FbxGeometryElement::eByControlPoint:
					this->materialAry[materialCnt].vertexAry[PolyVtxCntInMat].color = vertexColorAry[ vertexIndexAry[PolyVtxIndex] ];
					break;
				case FbxGeometryElement::eByPolygonVertex:
					this->materialAry[materialCnt].vertexAry[PolyVtxCntInMat].color = vertexColorAry[ PolyVtxIndex ];
					break;
				default:
					break;
				}

				// UVリスト用のイテレータを宣言
				std::list<point2>::iterator uvBuff;
				if(uvSet != NULL)
				{
					uvBuff = uvSet[0].uvBuffer.begin(); // イテレータ
				}
				// UVを格納する
				switch(uvMapMode)
				{
				case FbxGeometryElement::eByControlPoint:
					for(int cnt = 0; cnt < vertexIndexAry[PolyVtxIndex]; cnt++)
					{
						uvBuff++;
					}
					this->materialAry[materialCnt].vertexAry[PolyVtxCntInMat].uv.x = uvBuff->x;
					this->materialAry[materialCnt].vertexAry[PolyVtxCntInMat].uv.y = uvBuff->y;
					break;
				case FbxGeometryElement::eByPolygonVertex:
					for(int cnt = 0; cnt < PolyVtxIndex; cnt++)
					{
						uvBuff++;
					}
					this->materialAry[materialCnt].vertexAry[PolyVtxCntInMat].uv.x = uvBuff->x;
					this->materialAry[materialCnt].vertexAry[PolyVtxCntInMat].uv.y = uvBuff->y;
					break;
				default:
					break;
				}
				PolyVtxCntInMat++;
			}
		}// for eachMatPolyCnt
		// マテリアル対応のポリゴン頂点カウントを初期化
		PolyVtxCntInMat = 0;
	}
}
//********************************************************************************************************************
// funcinfo：マテリアル情報を取得
//********************************************************************************************************************
void MyFbxFile::GetMaterial(FbxMesh* pMesh)
{
	// マテリアルの存在をチェック
	FbxNode* node = pMesh->GetNode();
	if( node == 0 )
	{
		return;
	}

	// マテリアルの数
	this->numMaterial = node->GetMaterialCount();
	if( this->numMaterial == 0 )
	{
		return;
	}

	// マテリアル数分のマテリアル配列を生成
	this->materialAry = new MyMaterial[ this->numMaterial ] ;

	// マテリアルの数だけ繰り返す
	for (int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		FbxSurfaceMaterial* material = node->GetMaterial( materialCnt );
		if ( material != 0 )
		{
			// LambertかPhongか
			if ( material->GetClassId().Is( FbxSurfaceLambert::ClassId ) )
			{
				// Lambert
				FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)material;
				GetLambertInfo( lambert, materialCnt );
				
			}
			else if ( material->GetClassId().Is( FbxSurfacePhong::ClassId ) )
			{
				// Phong
				FbxSurfacePhong* phong = (FbxSurfacePhong*)material;
				// PhongはLanmertを親に持つのでPhongを渡す
				GetLambertInfo( phong, materialCnt );

				// スペキュラ
				this->materialAry[materialCnt].specular.r = (float)phong->Specular.Get()[ 0 ];
				this->materialAry[materialCnt].specular.g = (float)phong->Specular.Get()[ 1 ];
				this->materialAry[materialCnt].specular.b = (float)phong->Specular.Get()[ 2 ];
				//setTextureInfo( phong, FbxSurfaceMaterial::sSpecular, specularTexName_ );

				// 光沢
				this->materialAry[materialCnt].shininess = (float)phong->Shininess.Get();

				// 反射
				this->materialAry[materialCnt].reflectivity = (float)phong->ReflectionFactor.Get();
			}
		}
	}
}
//********************************************************************************************************************
// funcinfo：ランバートマテリアル情報を取得
//********************************************************************************************************************
void MyFbxFile::GetLambertInfo(FbxSurfaceLambert* lambert, int materialCnt)
{
	// アンビエント
	this->materialAry[materialCnt].ambient.r = (float)lambert->Ambient.Get()[ 0 ];
	this->materialAry[materialCnt].ambient.g = (float)lambert->Ambient.Get()[ 1 ];
	this->materialAry[materialCnt].ambient.b = (float)lambert->Ambient.Get()[ 2 ];
	//setTextureInfo( lambert, FbxSurfaceMaterial::sAmbient, ambientTexName_ );

	// ディフューズ
	this->materialAry[materialCnt].diffuse.r = (float)lambert->Diffuse.Get()[ 0 ];
	this->materialAry[materialCnt].diffuse.g = (float)lambert->Diffuse.Get()[ 1 ];
	this->materialAry[materialCnt].diffuse.b = (float)lambert->Diffuse.Get()[ 2 ];
	//setTextureInfo( lambert, FbxSurfaceMaterial::sDiffuse, diffuseTexName_ );

	// エミッシブ
	this->materialAry[materialCnt].emissive.r = (float)lambert->Emissive.Get()[ 0 ];
	this->materialAry[materialCnt].emissive.g = (float)lambert->Emissive.Get()[ 1 ];
	this->materialAry[materialCnt].emissive.b = (float)lambert->Emissive.Get()[ 2 ];
	//setTextureInfo( lambert, FbxSurfaceMaterial::sEmissive, emissiveTexName_ );

	// バンプ
	this->materialAry[materialCnt].bump.x		= (float)lambert->Bump.Get()[ 0 ];
	this->materialAry[materialCnt].bump.y		= (float)lambert->Bump.Get()[ 1 ];
	this->materialAry[materialCnt].bump.z		= (float)lambert->Bump.Get()[ 2 ];
	//setTextureInfo( lambert, FbxSurfaceMaterial::sBump, bumpTexName_ );

	// 透過度
	this->materialAry[materialCnt].transparency = (float)lambert->TransparencyFactor.Get();
}
//********************************************************************************************************************
// funcinfo：マテリアル対応のポリゴン数とポリゴンインデックス配列を取得する
//********************************************************************************************************************
void MyFbxFile::GetMaterialConnection(FbxMesh* pMesh)
{
	/* マテリアルの存在をチェック */
	FbxNode* node = pMesh->GetNode();
	if ( node == 0 )
	{
		return;
	}
	// マテリアルの数
	this->numMaterial = node->GetMaterialCount();
	if( this->numMaterial == 0 )
	{
		return;
	}

	// それぞれのマテリアルに対応するポリゴン数を配列として生成
	int *numPolygonInMaterial = new int[this->numMaterial];
	for(int cnt = 0; cnt < this->numMaterial; cnt++)
	{
		numPolygonInMaterial[cnt] = 0;
	}

	// ポリゴンの総数を取得
	this->numPolygon = pMesh->GetPolygonCount();

	for(int polygonCnt = 0; polygonCnt < this->numPolygon; polygonCnt++)
    {
        for(int materialElementCnt = 0; materialElementCnt < pMesh->GetElementMaterialCount(); materialElementCnt++)
        {
			// マテリアルエレメントを取得
            FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial( materialElementCnt );
			// マテリアルナンバーを取得
			int lMatId = -1;
			lMatId = lMaterialElement->GetIndexArray().GetAt( polygonCnt );
			if(lMatId >= 0)
			{
				// それぞれのマテリアルに対応するポリゴン数をカウントアップ
				numPolygonInMaterial[lMatId]++;
			}
        }
    }

	for(int cnt = 0; cnt < this->numMaterial; cnt++)
	{
		// それぞれのマテリアルに対応するポリゴンインデックス配列をそれぞれのポリゴン数分生成
		this->materialAry[cnt].polygonIndexAry = new int[numPolygonInMaterial[cnt]];
		// それぞれのマテリアルに対応するポリゴン数を保存
		this->materialAry[cnt].numPolygon = numPolygonInMaterial[cnt];
	}

	// それぞれのマテリアルに対応するポリゴン数を初期化
	for(int cnt = 0; cnt < this->numMaterial; cnt++)
	{
		numPolygonInMaterial[cnt] = 0; // ここ以下からこの変数は総数ではなくカウントとして使用
	}

	for(int polygonCnt = 0; polygonCnt < this->numPolygon; polygonCnt++)
    {
        for(int materialElementCnt = 0; materialElementCnt < pMesh->GetElementMaterialCount(); materialElementCnt++)
        {
			// マテリアルエレメントを取得
            FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial( materialElementCnt );
			// マテリアルナンバーを取得
			int lMatId = -1;
			lMatId = lMaterialElement->GetIndexArray().GetAt( polygonCnt );
			if(lMatId >= 0)
			{
				// それぞれのマテリアルに対応するポリゴン配列にポリゴンナンバーを代入し、インデックス配列を作成する
				this->materialAry[lMatId].polygonIndexAry[ numPolygonInMaterial[lMatId] ] = polygonCnt;
				numPolygonInMaterial[lMatId]++;
			}
        }
    }
}
//********************************************************************************************************************
// funcinfo：頂点の位置・インデックス・ポリゴンサイズ情報を取得（最初に取得すること）
//********************************************************************************************************************
void MyFbxFile::GetControlPoints(FbxMesh* pMesh)
{
	// 頂点の総数を取得
	this->numControlPoint = pMesh->GetControlPointsCount();
	// 頂点座標配列の先頭アドレスを取得
    FbxVector4* lControlPoints = pMesh->GetControlPoints();
	// 頂点数分の頂点バッファを動的に作成
	this->controlPointsAry = new D3DXVECTOR3[ numControlPoint ];
	// 頂点バッファに頂点をコピー
    for (int i = 0; i < numControlPoint; i++)
    {
		// 頂点を格納
		this->controlPointsAry[i].x = lControlPoints[i][0];
		this->controlPointsAry[i].y = lControlPoints[i][1];
		this->controlPointsAry[i].z = lControlPoints[i][2];
		//this->controlPointsAry[i].pos.w = lControlPoints[i][3];
	}

	// インデックスの総数を取得
	this->numIndex = pMesh->GetPolygonVertexCount();
	// インデックスの先頭アドレスを取得
	int* index = pMesh->GetPolygonVertices();
	// インデックスの総数分のインデックスバッファを動的に生成
	this->vertexIndexAry = new int[ numIndex ];
	// インデックスバッファにインデックスをコピー
    for (int i = 0; i < numIndex; i++)
    {
		// インデックスを格納
		this->vertexIndexAry[i] = index[i];
	}

	// ポリゴンの総数を取得
	this->numPolygon = pMesh->GetPolygonCount();
	// ポリゴンの総数分polygonSizeを生成
	this->polygonSize = new int[ numPolygon ];
	// ポリゴン毎のポリゴンサイズをコピー
	for(int i = 0; i < numPolygon; i++)
	{
		// ポリゴンサイズを取得
		this->polygonSize[i] = pMesh->GetPolygonSize(i);
	}
}
//********************************************************************************************************************
// funcinfo：法線情報を取得
//********************************************************************************************************************
void MyFbxFile::GetNormal(FbxMesh* pMesh)
{
	// 法線セットの数を取得（FBXファイルは複数の法線セットをもつ）
	// ブレンダーでなにもせずエクスポートしていれば１つのはず
	int normalElementNum = pMesh->GetElementNormalCount();

	for(int normalElementCnt = 0; normalElementCnt < normalElementNum; normalElementCnt++)
	{
		//********* 今は複数normalElementをサポートしない *********//
		if(normalElementCnt == 0)
		{
			// 法線を取得
			FbxGeometryElementNormal* normal = pMesh->GetElementNormal(normalElementCnt);
			// 法線数を取得
			int normalNum = normal->GetDirectArray().GetCount();
			// 法線数分の法線配列を動的に作成
			this->normalAry = new D3DXVECTOR3[ normalNum ];

			// マッピングモードの取得
			FbxGeometryElement::EMappingMode mapping = normal->GetMappingMode();
			this->normalMapMode = mapping;

			// リファレンスモードの取得
			FbxGeometryElement::EReferenceMode reference = normal->GetReferenceMode();

			// マッピングモードに依る分岐
			switch(mapping)
			{
			// マッピングモードがコントロール点の場合
			case FbxGeometryElement::eByControlPoint:
				// リファレンスモードに依る分岐
				switch (reference)
				{
				// リファレンスモードがコントロール点の並び順で法線が格納されている場合
				case FbxGeometryElement::eDirect:
					// 法線をコピー
					for(int normalCnt = 0; normalCnt < normalNum; normalCnt++)
					{
						this->normalAry[normalCnt].x = normal->GetDirectArray().GetAt(normalCnt)[0];
						this->normalAry[normalCnt].y = normal->GetDirectArray().GetAt(normalCnt)[1];
						this->normalAry[normalCnt].z = normal->GetDirectArray().GetAt(normalCnt)[2];
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					// 存在しない
					break;
				default:
					break; // other reference modes not shown here!
				}
				break;
			// マッピングモードがポリゴン頂点の場合
			case FbxGeometryElement::eByPolygonVertex:
				// リファレンスモードに依る分岐
				switch (reference)
				{
				// リファレンスモードがコントロール点の並び順で法線が格納されている場合
				case FbxGeometryElement::eDirect:
					// 法線をコピー
					for(int normalCnt = 0; normalCnt < normalNum; normalCnt++)
					{
						this->normalAry[normalCnt].x = normal->GetDirectArray().GetAt(normalCnt)[0];
						this->normalAry[normalCnt].y = normal->GetDirectArray().GetAt(normalCnt)[1];
						this->normalAry[normalCnt].z = normal->GetDirectArray().GetAt(normalCnt)[2];
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					int id;
					for(int normalCnt = 0; normalCnt < normalNum; normalCnt++)
					{
						// 法線の独自配列からnormalCnt番目のポリゴン頂点の法線ＩＤを取得
						id = normal->GetIndexArray().GetAt(normalCnt);

						this->normalAry[normalCnt].x = normal->GetDirectArray().GetAt(id)[0];
						this->normalAry[normalCnt].y = normal->GetDirectArray().GetAt(id)[1];
						this->normalAry[normalCnt].z = normal->GetDirectArray().GetAt(id)[2];
					}
					break;
				default:
					break; // other reference modes not shown here!
				}
				break;
			default:
				break;
			}
		}
	}
}
//********************************************************************************************************************
// funcinfo：頂点カラー情報を取得
//********************************************************************************************************************
void MyFbxFile::GetVertexColor(FbxMesh* pMesh)
{
	 // 頂点カラーセット数を取得
	int colorElementNum = pMesh->GetElementVertexColorCount();

	// レイヤー数だけ繰り返す
	for (int colorElementCnt = 0; colorElementCnt < colorElementNum; colorElementCnt++)
	{
		//********* 今は複数colorElementをサポートしない *********//
		if(colorElementCnt == 0)
		{
			// 頂点カラーセットを取得
			FbxGeometryElementVertexColor* color = pMesh->GetElementVertexColor( colorElementCnt );

			// マッピングモードの取得
			FbxGeometryElement::EMappingMode mappingMode = color->GetMappingMode();
			this->vertexColorMapMode = mappingMode;

			// リファレンスモードの取得
			FbxGeometryElement::EReferenceMode referenceMode = color->GetReferenceMode();

			switch (mappingMode)
			{
			default:
				break;
			case FbxGeometryElement::eByControlPoint:
				switch (referenceMode)
				{
				case FbxGeometryElement::eDirect:
					// 頂点インデックス分の頂点カラー配列を生成
					this->vertexColorAry = new D3DXCOLOR[ numControlPoint ];

					// 頂点の数だけ頂点カラーを取得
					for(int vtxCnt = 0; vtxCnt < numControlPoint; vtxCnt++)
					{
						this->vertexColorAry[vtxCnt].r = (float)color->GetDirectArray().GetAt(vtxCnt)[0];
						this->vertexColorAry[vtxCnt].g = (float)color->GetDirectArray().GetAt(vtxCnt)[1];
						this->vertexColorAry[vtxCnt].b = (float)color->GetDirectArray().GetAt(vtxCnt)[2];
						this->vertexColorAry[vtxCnt].a = (float)color->GetDirectArray().GetAt(vtxCnt)[3];
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						// 頂点カラー独自のインデックスバッファのアドレスを取得
						FbxLayerElementArrayTemplate<int>* index = &color->GetIndexArray();
						// インデックス分の頂点カラー配列を生成
						this->vertexColorAry = new D3DXCOLOR[ numControlPoint ];

						// 頂点の数だけ頂点カラーを取得
						for(int vtxCnt = 0; vtxCnt < numControlPoint; vtxCnt++)
						{
							this->vertexColorAry[vtxCnt].r = (float)color->GetDirectArray().GetAt(index->GetAt( vtxCnt ))[0];
							this->vertexColorAry[vtxCnt].g = (float)color->GetDirectArray().GetAt(index->GetAt( vtxCnt ))[1];
							this->vertexColorAry[vtxCnt].b = (float)color->GetDirectArray().GetAt(index->GetAt( vtxCnt ))[2];
							this->vertexColorAry[vtxCnt].a = (float)color->GetDirectArray().GetAt(index->GetAt( vtxCnt ))[3];
						}
					}
					break;
				default:
					break; // other reference modes not shown here!
				}
				break;
			case FbxGeometryElement::eByPolygonVertex:
				{
					switch (referenceMode)
					{
					case FbxGeometryElement::eDirect:
						// 頂点インデックス分の頂点カラー配列を生成
						this->vertexColorAry = new D3DXCOLOR[ numIndex ];

						// 頂点の数だけ頂点カラーを取得
						for(int idxCnt = 0; idxCnt < numIndex; idxCnt++)
						{
							this->vertexColorAry[idxCnt].r = (float)color->GetDirectArray().GetAt(idxCnt)[0];
							this->vertexColorAry[idxCnt].g = (float)color->GetDirectArray().GetAt(idxCnt)[1];
							this->vertexColorAry[idxCnt].b = (float)color->GetDirectArray().GetAt(idxCnt)[2];
							this->vertexColorAry[idxCnt].a = (float)color->GetDirectArray().GetAt(idxCnt)[3];
						}
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							// 頂点カラー独自のインデックスバッファのアドレスを取得
							FbxLayerElementArrayTemplate<int>* index = &color->GetIndexArray();
							int indexNum = index->GetCount();
							// インデックス分の頂点カラー配列を生成
							this->vertexColorAry = new D3DXCOLOR[ indexNum ];

							 // 頂点の数だけ頂点カラーを取得
							for(int idxCnt = 0; idxCnt < indexNum; idxCnt++)
							{
								this->vertexColorAry[idxCnt].r = (float)color->GetDirectArray().GetAt(index->GetAt( idxCnt ))[0];
								this->vertexColorAry[idxCnt].g = (float)color->GetDirectArray().GetAt(index->GetAt( idxCnt ))[1];
								this->vertexColorAry[idxCnt].b = (float)color->GetDirectArray().GetAt(index->GetAt( idxCnt ))[2];
								this->vertexColorAry[idxCnt].a = (float)color->GetDirectArray().GetAt(index->GetAt( idxCnt ))[3];
							}
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

			case FbxGeometryElement::eByPolygon:	// doesn't make much sense for UVs
			case FbxGeometryElement::eAllSame:		// doesn't make much sense for UVs
			case FbxGeometryElement::eNone:			// doesn't make much sense for UVs
				break;
			}
		}// for if(colorElementCnt == 0)
	}
}
//********************************************************************************************************************
// funcinfo：ＵＶ情報を取得
//********************************************************************************************************************
void MyFbxFile::GetUV(FbxMesh* pMesh)
{
	point2 temp;
	// UVセット数を取得
	this->numUvSet = pMesh->GetElementUVCount();
	// UVセット数分UVSETを生成
	if(this->numUvSet != 0)
	{
		this->uvSet = new UVSET[this->numUvSet];
	}
	
	for (int uvElementCnt = 0; uvElementCnt < this->numUvSet; uvElementCnt++)
	{
		//********* 今は複数uvElementをサポートしない *********//
		if(uvElementCnt == 0)
		{
			// UVバッファを取得
			FbxGeometryElementUV* UV = pMesh->GetElementUV( uvElementCnt );

			// マッピングモードの取得
			FbxGeometryElement::EMappingMode mapping = UV->GetMappingMode();
			this->uvMapMode = mapping;

			// リファレンスモードの取得
			FbxGeometryElement::EReferenceMode reference = UV->GetReferenceMode();
		
			// UV数を取得（リストでＵＶ情報を保存しているから必要ないかも）
			int uvCount = UV->GetDirectArray().GetCount();

			// <int>かどうかは未確認
			FbxLayerElementArrayTemplate<int>* uvIndex = &UV->GetIndexArray();
			int uvIndexNum = uvIndex->GetCount();

			switch (mapping)
			{
			default:
				break;
			case FbxGeometryElement::eByControlPoint:
				switch (reference)
				{
				case FbxGeometryElement::eDirect:
					// ------------↓↓正確かは未確認--------------
					for(int vtxCnt = 0; vtxCnt < numControlPoint; vtxCnt++)
					{
						// UVをコピー 
						temp.x = (float)UV->GetDirectArray().GetAt(vtxCnt)[0];
						// ブレンダーはテクスチャの左下が原点だからその補正
						temp.y = 1.0f - (float)UV->GetDirectArray().GetAt(vtxCnt)[1];
						// リストの後ろにtempを格納
						this->uvSet[uvElementCnt].uvBuffer.push_back(temp);
					}
					// UVSet名を取得
					this->uvSet[uvElementCnt].uvSetName = UV->GetName();
					break;
				case FbxGeometryElement::eIndexToDirect:
					// ------------↓↓正確かは未確認--------------
					int id;
					for(int vtxCnt = 0; vtxCnt < numControlPoint; vtxCnt++)
					{
						id = UV->GetIndexArray().GetAt(vtxCnt);
						// UVをコピー 
						temp.x = (float)UV->GetDirectArray().GetAt(id)[0];
						// ブレンダーはテクスチャの左下が原点だからその補正
						temp.y = 1.0f - (float)UV->GetDirectArray().GetAt(id)[1];
						// リストの後ろにtempを格納
						this->uvSet[uvElementCnt].uvBuffer.push_back(temp);
					}
					// UVSet名を取得
					this->uvSet[uvElementCnt].uvSetName = UV->GetName();
					break;
				default:
					break; // other reference modes not shown here!
				}
				break;

			case FbxGeometryElement::eByPolygonVertex:
				switch (reference)
				{
				case FbxGeometryElement::eDirect:
				case FbxGeometryElement::eIndexToDirect:
					// UVをコピー 
					for(int uvIndexCnt = 0; uvIndexCnt < uvIndexNum; uvIndexCnt++)
					{
						// UVをコピー 
						temp.x = (float)UV->GetDirectArray().GetAt(uvIndex->GetAt(uvIndexCnt))[0];
						// ブレンダーはテクスチャの左下が原点だからその補正
						temp.y = 1.0f - (float)UV->GetDirectArray().GetAt(uvIndex->GetAt(uvIndexCnt))[1];
						// リストの後ろにtempを格納
						this->uvSet[uvElementCnt].uvBuffer.push_back(temp);
					}
					// UVSet名を取得
					this->uvSet[uvElementCnt].uvSetName = UV->GetName();
					break;
				default:
					break; // other reference modes not shown here!
				}
				break;

			case FbxGeometryElement::eByPolygon:	// doesn't make much sense for UVs
			case FbxGeometryElement::eAllSame:		// doesn't make much sense for UVs
			case FbxGeometryElement::eNone:			// doesn't make much sense for UVs
				break;
			}
		}
	}
}
//********************************************************************************************************************
// funcinfo：テクスチャ情報を取得する（ＵＶ情報の後に取得すること）
//********************************************************************************************************************
void MyFbxFile::GetTextureNames(FbxMesh* pMesh)
{
	// メッシュからノードを取得
	FbxNode* node = pMesh->GetNode();

	// マテリアルの数を取得
	this->numMaterial = node->GetMaterialCount();

	// マテリアルの数だけ繰り返す
	for (int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		// マテリアルを取得
		FbxSurfaceMaterial* material = node->GetMaterial( materialCnt );
		// ディフューズプロパティのみを今は調べている
		FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

		// プロパティが持っているテクスチャレイヤーの数をチェック
		int TextureLayerNum = prop.GetSrcObjectCount<FbxTexture>();

		// テクスチャレイヤーの数だけ繰り返す
		for(int TextureLayerCnt = 0; TextureLayerCnt < TextureLayerNum; TextureLayerCnt++)
		{

			// アタッチされたテクスチャがレイヤードテクスチャかどうか確かめる
			FbxLayeredTexture* layeredTexture = prop.GetSrcObject<FbxLayeredTexture>(TextureLayerCnt);
			// レイヤードテクスチャならば
			if(layeredTexture)
			{
				// レイヤードテクスチャ数を取得（１つのマテリアルにいくつテクスチャが貼られているか？）
				int layeredTextureNum = layeredTexture -> GetSrcObjectCount<FbxFileTexture>();
			
				// レイヤードテクスチャ数だけ繰り返す
				for(int layeredTextureCnt = 0;  layeredTextureCnt < layeredTextureNum; layeredTextureCnt++)
				{
					// レイヤードテクスチャを取得
					FbxFileTexture* texture = layeredTexture -> GetSrcObject<FbxFileTexture>( layeredTextureCnt );
    
					if(texture)
					{
						// テクスチャ名を取得
						//std::string textureName = texture->GetName();
						std::string textureName = texture->GetRelativeFileName();

						// UVSet名を取得
						std::string UVSetName = texture->UVSet.Get().Buffer();

						// UVSet名を比較し対応しているテクスチャなら保持
						for (int uvElementCnt = 0; uvElementCnt < this->numUvSet; uvElementCnt++)
						{
							if(this->uvSet[uvElementCnt].uvSetName == UVSetName)
							{
								this->uvSet[uvElementCnt].textures.push_back(textureName);
							}
						}
					}
				}
			}
			// レイヤードテクスチャでないならば
			else
			{
				// //no layered texture simply get on the property
				FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>( TextureLayerCnt );
				if(texture)
				{
					// テクスチャ名を取得
					//std::string textureName = texture->GetName();
					std::string textureName = texture->GetRelativeFileName();

					// UVSet名を取得
					std::string UVSetName = texture->UVSet.Get().Buffer();

					// UVSet名を比較し対応しているテクスチャなら保持
					for (int uvElementCnt = 0; uvElementCnt < this->numUvSet; uvElementCnt++)
					{
						if(this->uvSet[uvElementCnt].uvSetName == UVSetName)
						{
							this->uvSet[uvElementCnt].textures.push_back(textureName);
						}
					}
				}
			}
		}
	}
}

//********************************************************************************************************************
// funcinfo： FBXファイルの情報（下記のメンバ変数）を取得する関数
//********************************************************************************************************************
int MyFbxFile::GetNumMaterial(void)
{
	return this->numMaterial;
}
int MyFbxFile::GetNumPolygon(void)
{
	return numPolygon;
}
int MyFbxFile::GetNumControlPoints(void)
{
	return numControlPoint;
}
int MyFbxFile::GetNumIndex(void)
{
	return numIndex;
}
int MyFbxFile::GetNumUvSet(void)
{
	return this->numUvSet;
}
MyMaterial* MyFbxFile::GetMaterial(void)
{
	return materialAry;
}
D3DXVECTOR3* MyFbxFile::GetControlPointsAry(void)
{
	return controlPointsAry;
}
int* MyFbxFile::GetIndexAry(void)
{
	return vertexIndexAry;
}
D3DXVECTOR3* MyFbxFile::GetNormalAry(void)
{
	return this->normalAry;
}
D3DXCOLOR* MyFbxFile::GetVertexColorAry(void)
{
	return this->vertexColorAry;
}
UVSET* MyFbxFile::GetUvSet(void)
{
	return this->uvSet;
}
int* MyFbxFile::GetPolygonSize(void)
{
	return polygonSize;
}
//********************************************************************************************************************
// funcinfo：FBXファイル情報を作成する
//********************************************************************************************************************
void MyFbxFile::CreateFbxInfo(FbxScene* pScene)
{
	int index;
	index = m_myFbxFileNum;					// m_myFbxFileNum[0]を無視してnewしないための変数

	if(m_myFbxFile[index] != NULL)
	{
		m_myFbxFile[index] -> Uninit();
		delete m_myFbxFile[index];
		m_myFbxFile[index] = NULL;
	}

	// インスタンスの生成
	m_myFbxFile[index] = new MyFbxFile;

	// 頂点・マテリアルなどを取得
	m_myFbxFile[index]->GetContent(pScene);
}
//********************************************************************************************************************
// funcinfo：MyFbxFileクラスのインスタンスのポインタを返す
//********************************************************************************************************************
MyFbxFile* MyFbxFile::GetMyFbxFile(int importedFbx)
{
	return m_myFbxFile[importedFbx]; // MyFbxFileクラスのインスタンスのポインタを返す
}
//********************************************************************************************************************
// funcinfo：FBXファイル情報をすべて開放する
//********************************************************************************************************************
void MyFbxFile::ReleaseAll(void)
{
	for( int cnt = 0; cnt < m_myFbxFileNum; cnt++)
	{
		if(m_myFbxFile[cnt] != NULL)
		{
			m_myFbxFile[cnt] -> Uninit();
			delete m_myFbxFile[cnt];
			m_myFbxFile[cnt] = NULL;
		}
	}
}
//********************************************************************************************************************
// funcinfo：終了処理
//********************************************************************************************************************
void MyFbxFile::Uninit(void)
{
	// 
	if(controlPointsAry != NULL)
	{
		delete[] controlPointsAry;
		controlPointsAry = NULL;
	}
	// 
	if(vertexIndexAry != NULL)
	{
		delete[] vertexIndexAry;
		vertexIndexAry = NULL;
	}
	// 
	if(polygonSize != NULL)
	{
		delete[] polygonSize;
		polygonSize = NULL;
	}
	// 
	if(this->normalAry != NULL)
	{
		delete[] this->normalAry;
		this->normalAry = NULL;
	}
	// 
	if(this->vertexColorAry != NULL)
	{
		delete[] this->vertexColorAry;
		this->vertexColorAry = NULL;
	}
	// 
	if(this->uvSet != NULL)
	{
		delete[] this->uvSet;
		this->uvSet = NULL;
	}
	// 
	if(materialAry != NULL)
	{
		for(int cnt = 0; cnt < this->numMaterial; cnt++)
		{
			delete this->materialAry[cnt].polygonIndexAry;
			delete this->materialAry[cnt].vertexAry;
			this->materialAry[cnt].polygonIndexAry = NULL;
			this->materialAry[cnt].vertexAry = NULL;
		}

		delete[] materialAry;
		materialAry = NULL;
	}
}
//********************************************************************************************************************
// funcinfo：
//********************************************************************************************************************
