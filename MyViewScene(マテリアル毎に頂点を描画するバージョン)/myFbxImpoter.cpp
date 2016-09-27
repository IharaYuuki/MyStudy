//********************************************************************************************************************
// �^�C�g��:		FBX�C���|�[�g����
// �v���O������:	myFbxImporter.cpp
//********************************************************************************************************************
//********************************************************************************************************************
// �C���N���[�h�t�@�C��
//********************************************************************************************************************
#include "myFbxImporter.h"

//********************************************************************************************************************
// �}�N����`
//********************************************************************************************************************
#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pManager->GetIOSettings()))
#endif

//********************************************************************************************************************
// �C�����C���֐�
//********************************************************************************************************************

//********************************************************************************************************************
// �ÓI�����o�ϐ��̐錾
//********************************************************************************************************************
MyFbxFile	*MyFbxFile::m_myFbxFile[MAX_FBX] = {};
int			MyFbxFile::m_myFbxFileNum = 0;

//**********************************************************************************************************************************************
// 
// MyFbxImporter�N���X�̎���
//
//**********************************************************************************************************************************************
//********************************************************************************************************************
// funcinfo�F�R���X�g���N�^
//********************************************************************************************************************
MyFbxImporter::MyFbxImporter()
{

}
//********************************************************************************************************************
// funcinfo�F�f�X�g���N�^
//********************************************************************************************************************
MyFbxImporter::~MyFbxImporter()
{

}
//********************************************************************************************************************
// funcinfo�FFBX�̏����擾���郁�C���֐�
//********************************************************************************************************************
void MyFbxImporter::GetFbxInfo(void)
{
	FbxManager* lSdkManager = NULL;
    FbxScene* lScene = NULL;
    bool lResult;

	// FbxManager���쐬�@IOSettings���쐬�@FbxSceneObject�i�C���|�[�g����Fbx�t�@�C������ێ�����j���쐬
    InitializeSdkObjects(lSdkManager, lScene);

    // FBX�t�@�C���ւ̃p�X���i�[
	FbxString lFilePath[MyFbxFile::MAX_FBX] = {
		("data/MODEL/first1.fbx"),
		//("data/MODEL/monstercharacter.fbx"),
	};

	// �|���S�����O�p�����邽�߂̃R���o�[�^�[
	FbxGeometryConverter lGeomConverter(lSdkManager);
	
	//�ǂݍ��݂���FBX�̕������J��Ԃ�
	for(int cntFile = 0; cntFile < MyFbxFile::MAX_FBX; cntFile++)
	{
		if( lFilePath[cntFile].IsEmpty() )
		{
			lResult = false;
		}
		else
		{
			// FBX�t�@�C�����V�[���ɃC���|�[�g
			lResult = LoadScene(lSdkManager, lScene, lFilePath[cntFile].Buffer());
			// �|���S�����O�p��
			lGeomConverter.Triangulate( lScene, true, false );	
		}

		if(lResult == true)
		{
			// FBX�t�@�C���̕`��ɕK�v�ȏ����쐬����
			//�i�ǂݍ��݂���FBX�̕������V�[�������ւ��ČJ��Ԃ����悤�ɂ��Ă���j
			MyFbxFile::CreateFbxInfo(lScene);
		}
	}

    // Destroy all objects created by the FBX SDK.
    DestroySdkObjects(lSdkManager, lResult);

    return;
}
//********************************************************************************************************************
// funcinfo�FFbxManager���쐬�@IOSettings���쐬�@FbxSceneObject�i�C���|�[�g����Fbx�t�@�C������ێ�����j���쐬
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
// funcinfo�F
//********************************************************************************************************************
void MyFbxImporter::DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
    //Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
    if( pManager ) pManager->Destroy();
	//if( pExitStatus ) FBXSDK_printf("Program Success!\n");
}
//********************************************************************************************************************
// funcinfo�F
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
// funcinfo�F
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
// MyFbxFile�N���X�̎���
//
//**********************************************************************************************************************************************
//********************************************************************************************************************
// funcinfo�F�R���X�g���N�^
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

	// FBX�t�@�C�������C���N�������g
	m_myFbxFileNum++;
}
//********************************************************************************************************************
// funcinfo�F�f�X�g���N�^
//********************************************************************************************************************
MyFbxFile::~MyFbxFile()
{

}
//********************************************************************************************************************
// funcinfo�F�X�^�[�g�t���[���@�G���h�t���[�����V�[������擾
//********************************************************************************************************************
void MyFbxFile::GetGlobalTimeSettings(FbxGlobalSettings* pGlobalSettings)
{
    FbxTimeSpan lTs;
    pGlobalSettings->GetTimelineDefaultTimeSpan(lTs);
	// �X�^�[�g�t���[��
    m_lStart = lTs.GetStart();
	// �G���h�t���[��
    m_lEnd   = lTs.GetStop();
}
//********************************************************************************************************************
// funcinfo�F�V�[�����烋�[�g�m�[�h�̎q���擾���AGetContent(FbxNode* pNode)�Ɏq�m�[�h��n���֐�
//********************************************************************************************************************
void MyFbxFile::GetContent(FbxScene* pScene)
{
	int i;
	// �V�[�����烋�[�g�m�[�h���擾
    FbxNode* lNode = pScene->GetRootNode();

    if(lNode)
    {
		// ���[�g�m�[�h�̎q�����J��Ԃ�
        for(i = 0; i < lNode->GetChildCount(); i++)
        {
            GetContent(lNode->GetChild(i));
        }
    }
}
//********************************************************************************************************************
// funcinfo�F�n���ꂽ�m�[�h�̑������擾���A�������ɈقȂ�֐��Ƀm�[�h��n�����������ꂽ��A�ċA�I�ɍX�Ȃ�q�m�[�h�������ɓn���֐�
//********************************************************************************************************************
void MyFbxFile::GetContent(FbxNode* pNode)
{
	FbxNodeAttribute::EType lAttributeType;
    int i;

	// �m�[�h�̑��������݂��Ȃ��Ȃ��
    if(pNode->GetNodeAttribute() == NULL)
    {
        FBXSDK_printf("NULL Node Attribute\n\n");
    }
	// �m�[�h�̑��������݂���Ȃ��
    else
    {
		// �m�[�h�̑������擾
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
			// ���b�V�������擾
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
// funcinfo�F�m�[�h���烁�b�V���̏����擾����֐�
//********************************************************************************************************************
void MyFbxFile::GetMesh(FbxNode* pNode)
{
	// �m�[�h�����b�V���ɃL���X�g����
	FbxMesh* lMesh = (FbxMesh*) pNode->GetNodeAttribute ();

	// �}�e���A�������擾
	GetMaterial(lMesh);
	// �}�e���A���ƃ|���S���̑Ή����擾
	GetMaterialConnection(lMesh);
	// ���_�����擾����
    GetControlPoints(lMesh);
	// �@�������擾����
	GetNormal(lMesh);
	// ���_�J���[�����擾
	GetVertexColor(lMesh);
	// UV�����擾
	GetUV(lMesh);
	// �e�N�X�`�������擾
	GetTextureNames(lMesh);
	// �}�e���A�����ɒ��_�o�b�t�@���쐬
	CreateVertex();
}
//********************************************************************************************************************
// funcinfo�F�}�e���A���Ή��|���S�����_�z����쐬�A�}�e���A���Ή����_�����v�Z
//********************************************************************************************************************
void MyFbxFile::CreateVertex(void)
{
	/* �}�e���A���ɑΉ�����|���S���̒��_�����擾 */
	// �}�e���A���Ή����_���������|���S����
	for(int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		this->materialAry[materialCnt].numPolygonVertex = 0;
	}
	// �}�e���A�����������J��Ԃ�
	for(int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		// ���ꂼ��̃}�e���A���ɑΉ�����|���S�������J��Ԃ�
		for(int eachMatPolyCnt = 0; eachMatPolyCnt < this->materialAry[materialCnt].numPolygon; eachMatPolyCnt++)
		{
			// �e�|���S���̒��_���������J��Ԃ��i�O�p�������Ă���O��łR�ƋL�q���Ă���j
			for (int cnt = 0; cnt < 3; cnt++)
			{
				// �}�e���A���ɑΉ�����|���S���̑������J�E���g�A�b�v
				this->materialAry[materialCnt].numPolygonVertex++;
			}
		}
	}

	/* ���ꂼ��̃}�e���A���ɑΉ�����|���S�����_�̑�����vertexAry�𐶐����� */
	for(int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		this->materialAry[materialCnt].vertexAry = new MyVertex[ this->materialAry[materialCnt].numPolygonVertex ];
	}

	/* vertexAry�Ƀ|���S�����_���W�����i�[ */
	// �|���S�����_�̃J�E���g
	int PolyVtxCntInMat = 0;             
	// �}�e���A�����������J��Ԃ�    
	for(int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		// ���ꂼ��̃}�e���A���ɑΉ�����|���S�������J��Ԃ�
		for(int eachMatPolyCnt = 0; eachMatPolyCnt < this->materialAry[materialCnt].numPolygon; eachMatPolyCnt++)
		{
			// �e�|���S���̒��_���������J��Ԃ��i�O�p�������Ă���O��łR�ƋL�q���Ă���j
			for (int cnt = 0; cnt < 3; cnt++)
			{
				// �|���S�����_�ԍ����v�Z�i�O�p�������Ă���O��łR�ƋL�q���Ă���j
				int PolyVtxIndex = cnt + 3 * this->materialAry[materialCnt].polygonIndexAry[eachMatPolyCnt];

				// �|���S�����_���W�ɃR���g���[���_���W���i�[����
				this->materialAry[materialCnt].vertexAry[PolyVtxCntInMat].pos = controlPointsAry[ vertexIndexAry[PolyVtxIndex] ];

				// �}�b�s���O���[�h�ɂ��i�[���@�̕���
				/**************************************/
				// �@�����i�[����
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
				// ���_�J���[���i�[����
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

				// UV���X�g�p�̃C�e���[�^��錾
				std::list<point2>::iterator uvBuff;
				if(uvSet != NULL)
				{
					uvBuff = uvSet[0].uvBuffer.begin(); // �C�e���[�^
				}
				// UV���i�[����
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
		// �}�e���A���Ή��̃|���S�����_�J�E���g��������
		PolyVtxCntInMat = 0;
	}
}
//********************************************************************************************************************
// funcinfo�F�}�e���A�������擾
//********************************************************************************************************************
void MyFbxFile::GetMaterial(FbxMesh* pMesh)
{
	// �}�e���A���̑��݂��`�F�b�N
	FbxNode* node = pMesh->GetNode();
	if( node == 0 )
	{
		return;
	}

	// �}�e���A���̐�
	this->numMaterial = node->GetMaterialCount();
	if( this->numMaterial == 0 )
	{
		return;
	}

	// �}�e���A�������̃}�e���A���z��𐶐�
	this->materialAry = new MyMaterial[ this->numMaterial ] ;

	// �}�e���A���̐������J��Ԃ�
	for (int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		FbxSurfaceMaterial* material = node->GetMaterial( materialCnt );
		if ( material != 0 )
		{
			// Lambert��Phong��
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
				// Phong��Lanmert��e�Ɏ��̂�Phong��n��
				GetLambertInfo( phong, materialCnt );

				// �X�y�L����
				this->materialAry[materialCnt].specular.r = (float)phong->Specular.Get()[ 0 ];
				this->materialAry[materialCnt].specular.g = (float)phong->Specular.Get()[ 1 ];
				this->materialAry[materialCnt].specular.b = (float)phong->Specular.Get()[ 2 ];
				//setTextureInfo( phong, FbxSurfaceMaterial::sSpecular, specularTexName_ );

				// ����
				this->materialAry[materialCnt].shininess = (float)phong->Shininess.Get();

				// ����
				this->materialAry[materialCnt].reflectivity = (float)phong->ReflectionFactor.Get();
			}
		}
	}
}
//********************************************************************************************************************
// funcinfo�F�����o�[�g�}�e���A�������擾
//********************************************************************************************************************
void MyFbxFile::GetLambertInfo(FbxSurfaceLambert* lambert, int materialCnt)
{
	// �A���r�G���g
	this->materialAry[materialCnt].ambient.r = (float)lambert->Ambient.Get()[ 0 ];
	this->materialAry[materialCnt].ambient.g = (float)lambert->Ambient.Get()[ 1 ];
	this->materialAry[materialCnt].ambient.b = (float)lambert->Ambient.Get()[ 2 ];
	//setTextureInfo( lambert, FbxSurfaceMaterial::sAmbient, ambientTexName_ );

	// �f�B�t���[�Y
	this->materialAry[materialCnt].diffuse.r = (float)lambert->Diffuse.Get()[ 0 ];
	this->materialAry[materialCnt].diffuse.g = (float)lambert->Diffuse.Get()[ 1 ];
	this->materialAry[materialCnt].diffuse.b = (float)lambert->Diffuse.Get()[ 2 ];
	//setTextureInfo( lambert, FbxSurfaceMaterial::sDiffuse, diffuseTexName_ );

	// �G�~�b�V�u
	this->materialAry[materialCnt].emissive.r = (float)lambert->Emissive.Get()[ 0 ];
	this->materialAry[materialCnt].emissive.g = (float)lambert->Emissive.Get()[ 1 ];
	this->materialAry[materialCnt].emissive.b = (float)lambert->Emissive.Get()[ 2 ];
	//setTextureInfo( lambert, FbxSurfaceMaterial::sEmissive, emissiveTexName_ );

	// �o���v
	this->materialAry[materialCnt].bump.x		= (float)lambert->Bump.Get()[ 0 ];
	this->materialAry[materialCnt].bump.y		= (float)lambert->Bump.Get()[ 1 ];
	this->materialAry[materialCnt].bump.z		= (float)lambert->Bump.Get()[ 2 ];
	//setTextureInfo( lambert, FbxSurfaceMaterial::sBump, bumpTexName_ );

	// ���ߓx
	this->materialAry[materialCnt].transparency = (float)lambert->TransparencyFactor.Get();
}
//********************************************************************************************************************
// funcinfo�F�}�e���A���Ή��̃|���S�����ƃ|���S���C���f�b�N�X�z����擾����
//********************************************************************************************************************
void MyFbxFile::GetMaterialConnection(FbxMesh* pMesh)
{
	/* �}�e���A���̑��݂��`�F�b�N */
	FbxNode* node = pMesh->GetNode();
	if ( node == 0 )
	{
		return;
	}
	// �}�e���A���̐�
	this->numMaterial = node->GetMaterialCount();
	if( this->numMaterial == 0 )
	{
		return;
	}

	// ���ꂼ��̃}�e���A���ɑΉ�����|���S������z��Ƃ��Đ���
	int *numPolygonInMaterial = new int[this->numMaterial];
	for(int cnt = 0; cnt < this->numMaterial; cnt++)
	{
		numPolygonInMaterial[cnt] = 0;
	}

	// �|���S���̑������擾
	this->numPolygon = pMesh->GetPolygonCount();

	for(int polygonCnt = 0; polygonCnt < this->numPolygon; polygonCnt++)
    {
        for(int materialElementCnt = 0; materialElementCnt < pMesh->GetElementMaterialCount(); materialElementCnt++)
        {
			// �}�e���A���G�������g���擾
            FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial( materialElementCnt );
			// �}�e���A���i���o�[���擾
			int lMatId = -1;
			lMatId = lMaterialElement->GetIndexArray().GetAt( polygonCnt );
			if(lMatId >= 0)
			{
				// ���ꂼ��̃}�e���A���ɑΉ�����|���S�������J�E���g�A�b�v
				numPolygonInMaterial[lMatId]++;
			}
        }
    }

	for(int cnt = 0; cnt < this->numMaterial; cnt++)
	{
		// ���ꂼ��̃}�e���A���ɑΉ�����|���S���C���f�b�N�X�z������ꂼ��̃|���S����������
		this->materialAry[cnt].polygonIndexAry = new int[numPolygonInMaterial[cnt]];
		// ���ꂼ��̃}�e���A���ɑΉ�����|���S������ۑ�
		this->materialAry[cnt].numPolygon = numPolygonInMaterial[cnt];
	}

	// ���ꂼ��̃}�e���A���ɑΉ�����|���S������������
	for(int cnt = 0; cnt < this->numMaterial; cnt++)
	{
		numPolygonInMaterial[cnt] = 0; // �����ȉ����炱�̕ϐ��͑����ł͂Ȃ��J�E���g�Ƃ��Ďg�p
	}

	for(int polygonCnt = 0; polygonCnt < this->numPolygon; polygonCnt++)
    {
        for(int materialElementCnt = 0; materialElementCnt < pMesh->GetElementMaterialCount(); materialElementCnt++)
        {
			// �}�e���A���G�������g���擾
            FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial( materialElementCnt );
			// �}�e���A���i���o�[���擾
			int lMatId = -1;
			lMatId = lMaterialElement->GetIndexArray().GetAt( polygonCnt );
			if(lMatId >= 0)
			{
				// ���ꂼ��̃}�e���A���ɑΉ�����|���S���z��Ƀ|���S���i���o�[�������A�C���f�b�N�X�z����쐬����
				this->materialAry[lMatId].polygonIndexAry[ numPolygonInMaterial[lMatId] ] = polygonCnt;
				numPolygonInMaterial[lMatId]++;
			}
        }
    }
}
//********************************************************************************************************************
// funcinfo�F���_�̈ʒu�E�C���f�b�N�X�E�|���S���T�C�Y�����擾�i�ŏ��Ɏ擾���邱�Ɓj
//********************************************************************************************************************
void MyFbxFile::GetControlPoints(FbxMesh* pMesh)
{
	// ���_�̑������擾
	this->numControlPoint = pMesh->GetControlPointsCount();
	// ���_���W�z��̐擪�A�h���X���擾
    FbxVector4* lControlPoints = pMesh->GetControlPoints();
	// ���_�����̒��_�o�b�t�@�𓮓I�ɍ쐬
	this->controlPointsAry = new D3DXVECTOR3[ numControlPoint ];
	// ���_�o�b�t�@�ɒ��_���R�s�[
    for (int i = 0; i < numControlPoint; i++)
    {
		// ���_���i�[
		this->controlPointsAry[i].x = lControlPoints[i][0];
		this->controlPointsAry[i].y = lControlPoints[i][1];
		this->controlPointsAry[i].z = lControlPoints[i][2];
		//this->controlPointsAry[i].pos.w = lControlPoints[i][3];
	}

	// �C���f�b�N�X�̑������擾
	this->numIndex = pMesh->GetPolygonVertexCount();
	// �C���f�b�N�X�̐擪�A�h���X���擾
	int* index = pMesh->GetPolygonVertices();
	// �C���f�b�N�X�̑������̃C���f�b�N�X�o�b�t�@�𓮓I�ɐ���
	this->vertexIndexAry = new int[ numIndex ];
	// �C���f�b�N�X�o�b�t�@�ɃC���f�b�N�X���R�s�[
    for (int i = 0; i < numIndex; i++)
    {
		// �C���f�b�N�X���i�[
		this->vertexIndexAry[i] = index[i];
	}

	// �|���S���̑������擾
	this->numPolygon = pMesh->GetPolygonCount();
	// �|���S���̑�����polygonSize�𐶐�
	this->polygonSize = new int[ numPolygon ];
	// �|���S�����̃|���S���T�C�Y���R�s�[
	for(int i = 0; i < numPolygon; i++)
	{
		// �|���S���T�C�Y���擾
		this->polygonSize[i] = pMesh->GetPolygonSize(i);
	}
}
//********************************************************************************************************************
// funcinfo�F�@�������擾
//********************************************************************************************************************
void MyFbxFile::GetNormal(FbxMesh* pMesh)
{
	// �@���Z�b�g�̐����擾�iFBX�t�@�C���͕����̖@���Z�b�g�����j
	// �u�����_�[�łȂɂ������G�N�X�|�[�g���Ă���΂P�̂͂�
	int normalElementNum = pMesh->GetElementNormalCount();

	for(int normalElementCnt = 0; normalElementCnt < normalElementNum; normalElementCnt++)
	{
		//********* ���͕���normalElement���T�|�[�g���Ȃ� *********//
		if(normalElementCnt == 0)
		{
			// �@�����擾
			FbxGeometryElementNormal* normal = pMesh->GetElementNormal(normalElementCnt);
			// �@�������擾
			int normalNum = normal->GetDirectArray().GetCount();
			// �@�������̖@���z��𓮓I�ɍ쐬
			this->normalAry = new D3DXVECTOR3[ normalNum ];

			// �}�b�s���O���[�h�̎擾
			FbxGeometryElement::EMappingMode mapping = normal->GetMappingMode();
			this->normalMapMode = mapping;

			// ���t�@�����X���[�h�̎擾
			FbxGeometryElement::EReferenceMode reference = normal->GetReferenceMode();

			// �}�b�s���O���[�h�Ɉ˂镪��
			switch(mapping)
			{
			// �}�b�s���O���[�h���R���g���[���_�̏ꍇ
			case FbxGeometryElement::eByControlPoint:
				// ���t�@�����X���[�h�Ɉ˂镪��
				switch (reference)
				{
				// ���t�@�����X���[�h���R���g���[���_�̕��я��Ŗ@�����i�[����Ă���ꍇ
				case FbxGeometryElement::eDirect:
					// �@�����R�s�[
					for(int normalCnt = 0; normalCnt < normalNum; normalCnt++)
					{
						this->normalAry[normalCnt].x = normal->GetDirectArray().GetAt(normalCnt)[0];
						this->normalAry[normalCnt].y = normal->GetDirectArray().GetAt(normalCnt)[1];
						this->normalAry[normalCnt].z = normal->GetDirectArray().GetAt(normalCnt)[2];
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					// ���݂��Ȃ�
					break;
				default:
					break; // other reference modes not shown here!
				}
				break;
			// �}�b�s���O���[�h���|���S�����_�̏ꍇ
			case FbxGeometryElement::eByPolygonVertex:
				// ���t�@�����X���[�h�Ɉ˂镪��
				switch (reference)
				{
				// ���t�@�����X���[�h���R���g���[���_�̕��я��Ŗ@�����i�[����Ă���ꍇ
				case FbxGeometryElement::eDirect:
					// �@�����R�s�[
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
						// �@���̓Ǝ��z�񂩂�normalCnt�Ԗڂ̃|���S�����_�̖@���h�c���擾
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
// funcinfo�F���_�J���[�����擾
//********************************************************************************************************************
void MyFbxFile::GetVertexColor(FbxMesh* pMesh)
{
	 // ���_�J���[�Z�b�g�����擾
	int colorElementNum = pMesh->GetElementVertexColorCount();

	// ���C���[�������J��Ԃ�
	for (int colorElementCnt = 0; colorElementCnt < colorElementNum; colorElementCnt++)
	{
		//********* ���͕���colorElement���T�|�[�g���Ȃ� *********//
		if(colorElementCnt == 0)
		{
			// ���_�J���[�Z�b�g���擾
			FbxGeometryElementVertexColor* color = pMesh->GetElementVertexColor( colorElementCnt );

			// �}�b�s���O���[�h�̎擾
			FbxGeometryElement::EMappingMode mappingMode = color->GetMappingMode();
			this->vertexColorMapMode = mappingMode;

			// ���t�@�����X���[�h�̎擾
			FbxGeometryElement::EReferenceMode referenceMode = color->GetReferenceMode();

			switch (mappingMode)
			{
			default:
				break;
			case FbxGeometryElement::eByControlPoint:
				switch (referenceMode)
				{
				case FbxGeometryElement::eDirect:
					// ���_�C���f�b�N�X���̒��_�J���[�z��𐶐�
					this->vertexColorAry = new D3DXCOLOR[ numControlPoint ];

					// ���_�̐��������_�J���[���擾
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
						// ���_�J���[�Ǝ��̃C���f�b�N�X�o�b�t�@�̃A�h���X���擾
						FbxLayerElementArrayTemplate<int>* index = &color->GetIndexArray();
						// �C���f�b�N�X���̒��_�J���[�z��𐶐�
						this->vertexColorAry = new D3DXCOLOR[ numControlPoint ];

						// ���_�̐��������_�J���[���擾
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
						// ���_�C���f�b�N�X���̒��_�J���[�z��𐶐�
						this->vertexColorAry = new D3DXCOLOR[ numIndex ];

						// ���_�̐��������_�J���[���擾
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
							// ���_�J���[�Ǝ��̃C���f�b�N�X�o�b�t�@�̃A�h���X���擾
							FbxLayerElementArrayTemplate<int>* index = &color->GetIndexArray();
							int indexNum = index->GetCount();
							// �C���f�b�N�X���̒��_�J���[�z��𐶐�
							this->vertexColorAry = new D3DXCOLOR[ indexNum ];

							 // ���_�̐��������_�J���[���擾
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
// funcinfo�F�t�u�����擾
//********************************************************************************************************************
void MyFbxFile::GetUV(FbxMesh* pMesh)
{
	point2 temp;
	// UV�Z�b�g�����擾
	this->numUvSet = pMesh->GetElementUVCount();
	// UV�Z�b�g����UVSET�𐶐�
	if(this->numUvSet != 0)
	{
		this->uvSet = new UVSET[this->numUvSet];
	}
	
	for (int uvElementCnt = 0; uvElementCnt < this->numUvSet; uvElementCnt++)
	{
		//********* ���͕���uvElement���T�|�[�g���Ȃ� *********//
		if(uvElementCnt == 0)
		{
			// UV�o�b�t�@���擾
			FbxGeometryElementUV* UV = pMesh->GetElementUV( uvElementCnt );

			// �}�b�s���O���[�h�̎擾
			FbxGeometryElement::EMappingMode mapping = UV->GetMappingMode();
			this->uvMapMode = mapping;

			// ���t�@�����X���[�h�̎擾
			FbxGeometryElement::EReferenceMode reference = UV->GetReferenceMode();
		
			// UV�����擾�i���X�g�łt�u����ۑ����Ă��邩��K�v�Ȃ������j
			int uvCount = UV->GetDirectArray().GetCount();

			// <int>���ǂ����͖��m�F
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
					// ------------�������m���͖��m�F--------------
					for(int vtxCnt = 0; vtxCnt < numControlPoint; vtxCnt++)
					{
						// UV���R�s�[ 
						temp.x = (float)UV->GetDirectArray().GetAt(vtxCnt)[0];
						// �u�����_�[�̓e�N�X�`���̍��������_�����炻�̕␳
						temp.y = 1.0f - (float)UV->GetDirectArray().GetAt(vtxCnt)[1];
						// ���X�g�̌���temp���i�[
						this->uvSet[uvElementCnt].uvBuffer.push_back(temp);
					}
					// UVSet�����擾
					this->uvSet[uvElementCnt].uvSetName = UV->GetName();
					break;
				case FbxGeometryElement::eIndexToDirect:
					// ------------�������m���͖��m�F--------------
					int id;
					for(int vtxCnt = 0; vtxCnt < numControlPoint; vtxCnt++)
					{
						id = UV->GetIndexArray().GetAt(vtxCnt);
						// UV���R�s�[ 
						temp.x = (float)UV->GetDirectArray().GetAt(id)[0];
						// �u�����_�[�̓e�N�X�`���̍��������_�����炻�̕␳
						temp.y = 1.0f - (float)UV->GetDirectArray().GetAt(id)[1];
						// ���X�g�̌���temp���i�[
						this->uvSet[uvElementCnt].uvBuffer.push_back(temp);
					}
					// UVSet�����擾
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
					// UV���R�s�[ 
					for(int uvIndexCnt = 0; uvIndexCnt < uvIndexNum; uvIndexCnt++)
					{
						// UV���R�s�[ 
						temp.x = (float)UV->GetDirectArray().GetAt(uvIndex->GetAt(uvIndexCnt))[0];
						// �u�����_�[�̓e�N�X�`���̍��������_�����炻�̕␳
						temp.y = 1.0f - (float)UV->GetDirectArray().GetAt(uvIndex->GetAt(uvIndexCnt))[1];
						// ���X�g�̌���temp���i�[
						this->uvSet[uvElementCnt].uvBuffer.push_back(temp);
					}
					// UVSet�����擾
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
// funcinfo�F�e�N�X�`�������擾����i�t�u���̌�Ɏ擾���邱�Ɓj
//********************************************************************************************************************
void MyFbxFile::GetTextureNames(FbxMesh* pMesh)
{
	// ���b�V������m�[�h���擾
	FbxNode* node = pMesh->GetNode();

	// �}�e���A���̐����擾
	this->numMaterial = node->GetMaterialCount();

	// �}�e���A���̐������J��Ԃ�
	for (int materialCnt = 0; materialCnt < this->numMaterial; materialCnt++)
	{
		// �}�e���A�����擾
		FbxSurfaceMaterial* material = node->GetMaterial( materialCnt );
		// �f�B�t���[�Y�v���p�e�B�݂̂����͒��ׂĂ���
		FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

		// �v���p�e�B�������Ă���e�N�X�`�����C���[�̐����`�F�b�N
		int TextureLayerNum = prop.GetSrcObjectCount<FbxTexture>();

		// �e�N�X�`�����C���[�̐������J��Ԃ�
		for(int TextureLayerCnt = 0; TextureLayerCnt < TextureLayerNum; TextureLayerCnt++)
		{

			// �A�^�b�`���ꂽ�e�N�X�`�������C���[�h�e�N�X�`�����ǂ����m���߂�
			FbxLayeredTexture* layeredTexture = prop.GetSrcObject<FbxLayeredTexture>(TextureLayerCnt);
			// ���C���[�h�e�N�X�`���Ȃ��
			if(layeredTexture)
			{
				// ���C���[�h�e�N�X�`�������擾�i�P�̃}�e���A���ɂ����e�N�X�`�����\���Ă��邩�H�j
				int layeredTextureNum = layeredTexture -> GetSrcObjectCount<FbxFileTexture>();
			
				// ���C���[�h�e�N�X�`���������J��Ԃ�
				for(int layeredTextureCnt = 0;  layeredTextureCnt < layeredTextureNum; layeredTextureCnt++)
				{
					// ���C���[�h�e�N�X�`�����擾
					FbxFileTexture* texture = layeredTexture -> GetSrcObject<FbxFileTexture>( layeredTextureCnt );
    
					if(texture)
					{
						// �e�N�X�`�������擾
						//std::string textureName = texture->GetName();
						std::string textureName = texture->GetRelativeFileName();

						// UVSet�����擾
						std::string UVSetName = texture->UVSet.Get().Buffer();

						// UVSet�����r���Ή����Ă���e�N�X�`���Ȃ�ێ�
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
			// ���C���[�h�e�N�X�`���łȂ��Ȃ��
			else
			{
				// //no layered texture simply get on the property
				FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>( TextureLayerCnt );
				if(texture)
				{
					// �e�N�X�`�������擾
					//std::string textureName = texture->GetName();
					std::string textureName = texture->GetRelativeFileName();

					// UVSet�����擾
					std::string UVSetName = texture->UVSet.Get().Buffer();

					// UVSet�����r���Ή����Ă���e�N�X�`���Ȃ�ێ�
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
// funcinfo�F FBX�t�@�C���̏��i���L�̃����o�ϐ��j���擾����֐�
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
// funcinfo�FFBX�t�@�C�������쐬����
//********************************************************************************************************************
void MyFbxFile::CreateFbxInfo(FbxScene* pScene)
{
	int index;
	index = m_myFbxFileNum;					// m_myFbxFileNum[0]�𖳎�����new���Ȃ����߂̕ϐ�

	if(m_myFbxFile[index] != NULL)
	{
		m_myFbxFile[index] -> Uninit();
		delete m_myFbxFile[index];
		m_myFbxFile[index] = NULL;
	}

	// �C���X�^���X�̐���
	m_myFbxFile[index] = new MyFbxFile;

	// ���_�E�}�e���A���Ȃǂ��擾
	m_myFbxFile[index]->GetContent(pScene);
}
//********************************************************************************************************************
// funcinfo�FMyFbxFile�N���X�̃C���X�^���X�̃|�C���^��Ԃ�
//********************************************************************************************************************
MyFbxFile* MyFbxFile::GetMyFbxFile(int importedFbx)
{
	return m_myFbxFile[importedFbx]; // MyFbxFile�N���X�̃C���X�^���X�̃|�C���^��Ԃ�
}
//********************************************************************************************************************
// funcinfo�FFBX�t�@�C���������ׂĊJ������
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
// funcinfo�F�I������
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
// funcinfo�F
//********************************************************************************************************************
