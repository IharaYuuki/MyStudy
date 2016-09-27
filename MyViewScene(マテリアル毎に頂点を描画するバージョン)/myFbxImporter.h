//********************************************************************************************************************
// �^�C�g��:		FBX�C���|�[�g����
// �v���O������:	myFbxImporter.h
//********************************************************************************************************************
#ifndef _MYFBXIMPORTER_H_
#define _MYFBXIMPORTER_H_

//********************************************************************************************************************
// �C���N���[�h�t�@�C��
//********************************************************************************************************************
#include "main.h"
#include <string>
#include <list>

//********************************************************************************************************************
// �O���錾
//********************************************************************************************************************
class MyFbxFile;

//********************************************************************************************************************
// �\���́E���p��
//********************************************************************************************************************
// MyVector3���p��
union MyVector3
{
	struct { float x, y, z; };
	struct { float r, g, b; };
	float m[ 3 ];
};
// �t�u�p�̂Q�ϐ��\����
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
// UVSET�\����
typedef struct UVSET
{
	std::string uvSetName;
	std::list<std::string> textures;
	std::list<point2> uvBuffer;
} UvSet;
// MyVertex�\����
typedef struct MYVERTEX
{
	D3DXVECTOR3		pos;
	D3DXVECTOR3		normal;
	D3DXCOLOR		color;
	D3DXVECTOR2		uv;
} MyVertex;
// MyMaterial�\����
typedef struct MYMATERIAL
{
	// �}�e���A�����
	MyVector3	ambient;
	MyVector3	diffuse;
	MyVector3	emissive;
	MyVector3	bump;
	float		transparency;
	MyVector3	specular;
	float		shininess;
	float		reflectivity;
	// �}�e���A���ɑΉ��������
	int			*polygonIndexAry;
	int			numPolygon;
	MyVertex	*vertexAry;					// �|���S�����_�z��ւ̃|�C���^
	int			numPolygonVertex;			// �}�e���A���ɑΉ�����|���S�����_�̑���
} MyMaterial;

//********************************************************************************************************************
// �N���X
//********************************************************************************************************************
// FBX�C���|�[�^�N���X
class MyFbxImporter
{
public:
	MyFbxImporter();
	~MyFbxImporter();
	/* FBX�̏����擾���郁�C���֐� */
	static void GetFbxInfo(void);

	/* ���ʂ̏��������� */
	// FbxManager���쐬�@IOSettings���쐬�@FbxSceneObject�i�C���|�[�g����Fbx�t�@�C������ێ�����j���쐬
	static void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
	static void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
	static void CreateAndFillIOSettings(FbxManager* pManager);
	static bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);
	static bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat=-1, bool pEmbedMedia=false);
};

// FBX�t�@�C���N���X
class MyFbxFile
{
public:
	// �C���|�[�g����FBX�t�@�C��
	typedef enum
	{
		TEST_FBX = 0,
		MAX_FBX
	} IMPORTED_FBX;
	// �擾����FBX�t�@�C�����̎��
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
	/* �����o�֐� */
	MyFbxFile();
	~MyFbxFile();
	// �O���[�o���^�C���Z�b�g���擾����
	void GetGlobalTimeSettings(FbxGlobalSettings* pGlobalSettings);
	// FBX�̒��g�����ׂĎ擾����
	void GetContent(FbxScene* pScene);
	void GetContent(FbxNode* pNode);
	void GetMesh(FbxNode* pNode);
	void GetMaterial(FbxMesh* pMesh);
	void GetLambertInfo(FbxSurfaceLambert* lambert, int materialCnt); // LambertMat���擾����
	void GetMaterialConnection(FbxMesh* pMesh);
	void GetControlPoints(FbxMesh* pMesh);
	void GetNormal(FbxMesh* pMesh);
	void GetVertexColor(FbxMesh* pMesh);
	void GetUV(FbxMesh* pMesh);
	void GetTextureNames(FbxMesh* pMesh);
	void CreateVertexBuffer(void);

	// FBX�t�@�C���̏��i���L�̃����o�ϐ��j��info�ɃZ�b�g����֐�
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
	// FBX�t�@�C���̏������L�̃����o�ϐ��ɑ������֐�
	static void CreateFbxInfo(FbxScene* pScene);
	// FBX�t�@�C���ւ̃|�C���^���擾����֐�
	static MyFbxFile* GetMyFbxFile(int importedFbx);
	// ���I�Ɋm�ۂ��������������ׂĊJ������֐�
	static void ReleaseAll(void);
	void Uninit(void);
	
private:
	FbxTime		m_lStart;	// �X�^�[�g�t���[��
	FbxTime		m_lEnd;		// �G���h�t���[��
	
	int			numMaterial;			// �}�e���A���̐�
	int			numPolygon;				// �|���S������
	int			numControlPoint;		// �R���g���[���_����
	int			numIndex;				// �C���f�b�N�X��
	int			numUvSet;				// UV�Z�b�g�̐�
	
	MyMaterial	*materialAry;			// �}�e���A��
	D3DXVECTOR3	*controlPointsAry;		// �R���g���[���_�z��
	int			*vertexIndexAry;		// �C���f�b�N�X�z��ւ̃|�C���^
	D3DXVECTOR3 *normalAry;				// �@���z��
	D3DXCOLOR	*vertexColorAry;		// ���_�J���[
	UVSET		*uvSet;					// UV�Z�b�g�z��
	int			*polygonSize;			// �P�|���S�����̒��_��
	
	FbxGeometryElement::EMappingMode normalMapMode;
	FbxGeometryElement::EMappingMode vertexColorMapMode;
	FbxGeometryElement::EMappingMode uvMapMode;

	static	MyFbxFile	*m_myFbxFile[MAX_FBX];
	static	int			m_myFbxFileNum;
	// My���b�V���N���X�̃C���X�^���X�i���_�E�|���S���E�}�e���A���E�{�[���j
};



#endif