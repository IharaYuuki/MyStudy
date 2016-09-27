/*******************************************************************************
* �^�C�g��:		�J��������
* �v���O������:	camera.h
*******************************************************************************/
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Main.h"

//*****************************************************************************
// �񋓑�
//*****************************************************************************
typedef enum
{
	RELEASE_CAMERA = 0,
	DEBUG_CAMERA,
} CAMERA_MODE;

//*****************************************************************************
// �J�����̍\����
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 vPosV;			// ���_
	D3DXVECTOR3 vPosR;			// �����_
	D3DXVECTOR3 vecU;			// ������x�N�g��
	D3DXVECTOR3	vPosVDest;		// ���_�̖ړI�ʒu
	D3DXVECTOR3	vPosRDest;		// �����_�̖ړI�ʒu
	D3DXVECTOR3 vecVR;			// ���_���璍���_�����ւ̒P�ʃx�N�g��
	D3DXVECTOR3 vecVerToVR;// �x�N�g��VR�ɐ����ȒP�ʃx�N�g��
	D3DXVECTOR3 vRot;			// �����i��]�p�j
	float fDistance;			// ���_�ƒ����_�̋���
	float fDistanceXZ;
	float fInitialDistance;		// �n�߂̎��_�ƒ����_�̋���
	float fChaseHeightV;		// ���_�̍���
	float fChaseHeightR;		// �����_�̍���
	D3DXMATRIX mtxProjection;	// �v���W�F�N�V�����}�g���b�N�X
	D3DXMATRIX mtxView;			// �r���[�}�g���b�N�X
	float fInitialSin;
	float fInitialCos;
} CAMERA;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);
CAMERA *GetCamera(void);

#endif
