//********************************************************************************************************************
// �^�C�g��:		�I�u�W�F�N�g����
// �v���O������:	object.cpp
//********************************************************************************************************************
//********************************************************************************************************************
// �C���N���[�h�t�@�C��
//********************************************************************************************************************
#include "object.h"
#include <string>
#include <list>
#include "debugProc.h"

//********************************************************************************************************************
// �}�N����`
//********************************************************************************************************************
#define RANGE_OF_PHI		(D3DX_PI) // ���̂x������̊p�x�ӂ͈̔�
#define	MAX_MESH			(10)

//********************************************************************************************************************
// �C�����C���֐�
//********************************************************************************************************************
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

//********************************************************************************************************************
// �ÓI�����o�ϐ��̐錾
//********************************************************************************************************************
int Object::numAll = 0;						// �I�u�W�F�N�g�̑���
Object *Object::object[MAX_OBJECT] = {};	// �I�u�W�F�N�g�N���X�̃C���X�^���X�ւ̃|�C���^

//********************************************************************************************************************
// �v���g�^�C�v�錾
//********************************************************************************************************************															

//********************************************************************************************************************
// �O���[�o���ϐ�
//********************************************************************************************************************

//**********************************************************************************************************************************************
// 
// Object�N���X�̎���
//
//**********************************************************************************************************************************************
//********************************************************************************************************************
// funcinfo�F�R���X�g���N�^
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

	numVertex = 0;			// ���_��
	numIndex = 0;			// �C���f�b�N�X��
	numPolygon = 0;			// �|���S����
	numUvSet = 0;			// UV�Z�b�g�̐�
	numMaterial = 0;		// �}�e���A���̐�
	
	vtxAry = NULL;			// ���_�o�b�t�@
	idxAry = NULL;			// �C���f�b�N�X�o�b�t�@
	polygonSize = NULL;		// �P�|���S�����̒��_��
	normalAry = NULL;		// �@���o�b�t�@
	vtxColor = NULL;		// ���_�J���[
	uvSet = NULL;			// UV�Z�b�g
	material = NULL;		// �}�e���A��

	// �������C���N�������g
	numAll++;
}
//********************************************************************************************************************
// funcinfo�F�f�X�g���N�^
//********************************************************************************************************************
Object::~Object()
{

}
//********************************************************************************************************************
// funcinfo�F������
//********************************************************************************************************************
HRESULT Object::Init(int importedFbx)
{
	// �_�C���N�g�f�o�C�X���擾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �C���|�[�g����FBX�t�@�C���ւ̃|�C���^���擾
	MyFbxFile* myFbxFile = MyFbxFile::GetMyFbxFile(importedFbx);
	
	// �}�e���A�������擾
	this -> numMaterial = myFbxFile -> GetNumMaterial();
	this -> material = myFbxFile -> GetMaterial();
	this -> vtxBuff = new LPDIRECT3DVERTEXBUFFER9[ this -> numMaterial ];

	this -> numPolygon = myFbxFile -> GetNumPolygon();
	this -> numVertex = myFbxFile -> GetNumControlPoints();		
	this -> numIndex = myFbxFile -> GetNumIndex();
	
	/*this -> numUvSet = myFbxFile -> GetNumUvSet();		
	this -> vtxAry = myFbxFile -> GetControlPointsAry();	
	this -> idxAry = myFbxFile -> GetIndexAry();
	this -> normalAry = myFbxFile -> GetNormalAry();
	this -> vtxColor = myFbxFile -> GetVertexColorAry();
	this -> uvSet = myFbxFile -> GetUvSet();
	this -> polygonSize = myFbxFile -> GetPolygonSize();*/

	// �}�e���A�������J��Ԃ�
	for( int matCnt = 0; matCnt < this -> numMaterial; matCnt++)
	{
		// �I�u�W�F�N�g�̒��_�o�b�t�@���쐬
		if(FAILED(pDevice -> CreateVertexBuffer(sizeof(VERTEX_3D) * this -> material[matCnt].numPolygonVertex,
												D3DUSAGE_WRITEONLY,
												FVF_VERTEX_3D,
												D3DPOOL_MANAGED,
												&this -> vtxBuff[matCnt],
												NULL)))
		{
			return E_FAIL;
		}
		//// �I�u�W�F�N�g�̃C���f�b�N�X�o�b�t�@�𐶐�
		//if(FAILED(pDevice -> CreateIndexBuffer(sizeof(WORD) * this -> numIndex,
		//										D3DUSAGE_WRITEONLY,
		//										D3DFMT_INDEX16,
		//										D3DPOOL_MANAGED,
		//										&this -> idxBuff,
		//										NULL)))
		//{
		//	return E_FAIL;
		//}

		{//���_�o�b�t�@�̒��g�𖄂߂�
			VERTEX_3D *vtx;

			// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
			this->vtxBuff[matCnt]->Lock(0,0,(void**)&vtx,0);

			for(int polyVtxCnt = 0; polyVtxCnt < this -> material[matCnt].numPolygonVertex; polyVtxCnt++)
			{
				// ���_���W�̐ݒ�
				vtx[polyVtxCnt].pos = this -> material[matCnt].vertexAry[polyVtxCnt].pos;
				
				// �@���̐ݒ�
				vtx[polyVtxCnt].nor = this -> material[matCnt].vertexAry[polyVtxCnt].normal;
			
				// ���_�J���[�̐ݒ�
				if(vtxColor != NULL)
				{
					vtx[polyVtxCnt].col = this -> material[matCnt].vertexAry[polyVtxCnt].color;
				}
				else
				{
					if(polyVtxCnt % 2 == 0)
					{
						vtx[polyVtxCnt].col = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
					}
					else
					{
						vtx[polyVtxCnt].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
					}
				}

				// �e�N�X�`�����W�̐ݒ�
				if(uvSet != NULL)
				{
					vtx[polyVtxCnt].uv = this -> material[matCnt].vertexAry[polyVtxCnt].uv;
				}
				else
				{
					vtx[polyVtxCnt].uv = D3DXVECTOR2(0.0f, 0.0f);
				}	
			}

			// ���_�f�[�^���A�����b�N����
			this->vtxBuff[matCnt]->Unlock();
		}

		//{//�C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�
		//	WORD *idx;

		//	// �C���f�b�N�X�f�[�^�͈̔͂����b�N���A�C���f�b�N�X�o�b�t�@�ւ̃|�C���^���擾
		//	this->idxBuff->Lock(0,0,(void**)&idx,0);

		//	// �C���f�b�N�X����
		//	for(int cnt = 0; cnt < numIndex; cnt++)
		//	{
		//		idx[cnt] = idxAry[cnt];
		//	}

		//	// �C���f�b�N�X�f�[�^���A�����b�N����
		//	this->idxBuff->Unlock();
		//}
	}

	return S_OK;
}
//********************************************************************************************************************
// funcinfo�F�I��
//********************************************************************************************************************
void Object::Uninit(void)
{
	for( int cnt = 0; cnt < this -> numMaterial; cnt++ )
	{
		if(this->vtxBuff[cnt] != NULL)
		{// ���_�o�b�t�@�̊J��
			this->vtxBuff[cnt]->Release();
			this->vtxBuff[cnt] = NULL;
		}	
	}
	delete[] this->vtxBuff;
	this->vtxBuff = NULL;

	if(this->idxBuff != NULL)
	{// �C���f�b�N�X�o�b�t�@�̊J��
		this->idxBuff->Release();
		this->idxBuff = NULL;
			
	}
}
//********************************************************************************************************************
// funcinfo�F�X�V
//********************************************************************************************************************
void Object::Update(void)
{
	PrintDebugProc("\n");
	for(int materialCnt = 0; materialCnt < this -> numMaterial; materialCnt++)
	{
		PrintDebugProc("[ %d�Ԗڂ̃}�e���A�� ]\n", materialCnt + 1);
		PrintDebugProc("�|���S�����_���F%d\n", this -> material[materialCnt].numPolygonVertex);
		PrintDebugProc("�|���S�����F%d\n", this -> material[materialCnt].numPolygon);
		PrintDebugProc("\n");
	}

	PrintDebugProc("���_���F%d\n", this -> numVertex);
	PrintDebugProc("�|���S�����F%d\n", this -> numPolygon);
	PrintDebugProc("�C���f�b�N�X���F%d\n", this -> numIndex);
	PrintDebugProc("\n");
}
//********************************************************************************************************************
// funcinfo�F�`��
//********************************************************************************************************************
void Object::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // �J�����O�Ȃ�

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&this->mtxWorld);
	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl,this->scl.x,this->scl.y,this->scl.z);
	D3DXMatrixMultiply(&this->mtxWorld,&this->mtxWorld,&mtxScl);
	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, this->rot.y, this->rot.x, this->rot.z);
	D3DXMatrixMultiply(&this->mtxWorld, &this->mtxWorld, &mtxRot);
	// �ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, this->pos.x, this->pos.y, this->pos.z);
	D3DXMatrixMultiply(&this->mtxWorld, &this->mtxWorld, &mtxTranslate);
	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice -> SetTransform(D3DTS_WORLD, &this->mtxWorld);
	
	// �}�e���A�������J��Ԃ�
	for( int matCnt = 0; matCnt < this->numMaterial; matCnt++)
	{
		// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
		pDevice -> SetStreamSource(0, this->vtxBuff[matCnt], 0, sizeof(VERTEX_3D));
		//// �C���f�b�N�X�o�b�t�@�������_�����O�p�C�v���C���ɐݒ�
		//pDevice -> SetIndices(idxBuff);
		// ���_�t�H�[�}�b�g�̐ݒ�
		pDevice -> SetFVF(FVF_VERTEX_3D);
		// �e�N�X�`���̐ݒ�
		pDevice -> SetTexture(0,this -> tex);
		// �|���S���̕`��
		pDevice -> DrawPrimitive(	D3DPT_TRIANGLELIST,					// �v���~�e�B�u�̕`���� //D3DPT_TRIANGLELIST //D3DPT_TRIANGLESTRIP //D3DPT_TRIANGLEFAN //D3DPT_LINELIST
									0,									// ���_�o�b�t�@�̕`���n�߂̒��_�ԍ�
									this->material[matCnt].numPolygon	// �`�悷��|���S����
									);


		//// �|���S���̕`��
		//pDevice -> DrawIndexedPrimitive(D3DPT_TRIANGLELIST,		// �v���~�e�B�u�̕`���� //D3DPT_TRIANGLELIST //D3DPT_TRIANGLESTRIP //D3DPT_TRIANGLEFAN //D3DPT_LINELIST
		//									0,						// �ŏ��̒��_�C���f�b�N�X�܂ł̃I�t�Z�b�g
		//									0,						// �ŏ��̒��_�C���f�b�N�X
		//									this->numVertex,		// ���_��
		//									0,						// �ǂݎ����J�n����ʒu
		//									this->numPolygon		// �|���S����
		//									);
	}

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	// ���ʂ��J�����O

}
//********************************************************************************************************************
// funcinfo�F�����擾
//********************************************************************************************************************
int Object::GetNumAll(void)
{
	return numAll;
}
//********************************************************************************************************************
// funcinfo�F������������
//********************************************************************************************************************
void Object::ZeroNum(void)
{
	numAll = 0;	
}
//********************************************************************************************************************
// funcinfo�F�C���X�^���X���쐬
//********************************************************************************************************************
void Object::CreateObject(void)
{
	int index;
	
	for(int cntFbx = 0; cntFbx < MyFbxFile::MAX_FBX; cntFbx++)
	{
		index = numAll;			// object[0]�𖳎�����new���Ȃ����߂̕ϐ�

		if(object[index] != NULL)
		{
			object[index] -> Uninit();
			delete object[index];
			object[index] = NULL;
		}

		// �C���X�^���X�̐���
		object[index] = new Object;

		// ����������
		object[index] -> Init(cntFbx);
	}
}
//********************************************************************************************************************
// funcinfo�F���I�Ɋm�ۂ��������������ׂĊJ������֐�
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
// funcinfo�F���I�Ɋm�ۂ��������������ׂĊJ������֐�
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
// funcinfo�F���I�Ɋm�ۂ��������������ׂĊJ������֐�
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