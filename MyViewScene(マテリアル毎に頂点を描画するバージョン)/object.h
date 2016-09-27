//********************************************************************************************************************
// �^�C�g��:		�I�u�W�F�N�g����
// �v���O������:	object.h
//********************************************************************************************************************
#ifndef _OBJECT_H_
#define _OBJECT_H_

//********************************************************************************************************************
// �C���N���[�h�t�@�C��
//********************************************************************************************************************
#include "main.h"
#include "myFbxImporter.h"

#define MAX_OBJECT (256)

//********************************************************************************************************************
// �N���X
//********************************************************************************************************************
// �I�u�W�F�N�g�N���X
class Object
{
public:
	Object();								// �R���X�g���N�^
	~Object();								// �f�X�g���N�^
	void Init(void);						// �������֐�
	HRESULT Init(int importedFbx);			// �C���|�[�g���ꂽFBX�����L�̃����o�ϐ��ɑ�����鏉�����֐�
	void Uninit(void);						// �I���֐�
	void Update(void);						// �X�V�֐�
	void Draw(void);						// �`��֐�
	static	int GetNumAll(void);			// �I�u�W�F�N�g�̑������擾
	static	void ZeroNum(void);				// �I�u�W�F�N�g�̑�����������
	static	void CreateObject(void);		// �I�u�W�F�N�g���쐬����N���X
	static	void ReleaseAll(void);			// ���I�Ɋm�ۂ��������������ׂĊJ������֐�
	static	void UpdateAll(void);			// �I�u�W�F�N�g���ׂĂ̍X�V�֐�
	static	void DrawAll(void);				// �I�u�W�F�N�g���ׂĂ̕`��֐�
protected:
	LPDIRECT3DVERTEXBUFFER9 *vtxBuff;		// ���_�o�b�t�@�ւ̃_�u���|�C���^
	LPDIRECT3DINDEXBUFFER9	idxBuff;		// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^
	LPDIRECT3DTEXTURE9		tex;			// �e�N�X�`���ւ̃|�C���^
	
	D3DXMATRIX				mtxWorld;		// ���[���h�}�g���b�N�X
	D3DXVECTOR3				pos;			// �|���S���\���ʒu�̒��S���W
	D3DXVECTOR3				rot;			// �|���S���̉�]�p
	D3DXVECTOR3				scl;			// �g��k����

	int			numMaterial;		// �}�e���A���̐�
	int			numVertex;			// ���_��
	int			numIndex;			// �C���f�b�N�X��
	int			numPolygon;			// �|���S����
	int			numUvSet;			// UV�Z�b�g�̐�
	
	MyMaterial	*material;			// �}�e���A��
	D3DXVECTOR3	*vtxAry;			// ���_�o�b�t�@
	int			*idxAry;			// �C���f�b�N�X�o�b�t�@
	int			*polygonSize;		// �P�|���S�����̒��_��
	D3DXVECTOR3 *normalAry;			// �@���o�b�t�@
	D3DXCOLOR	*vtxColor;			// ���_�J���[
	UVSET		*uvSet;				// UV�Z�b�g
	
private:
	static int numAll;
	static Object *object[MAX_OBJECT];
};

#endif