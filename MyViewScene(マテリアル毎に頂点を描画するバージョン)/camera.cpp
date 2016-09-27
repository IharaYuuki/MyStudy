/*******************************************************************************
* �^�C�g��:		�J��������
* �v���O������:	camera.cpp
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "camera.h"
#include "input.h"
#include "debugProc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	CAM_POS_V_X				(0.0f)			// �J�����̎��_�����ʒu(X���W)
#define	CAM_POS_V_Y				(20.0f)			// �J�����̎��_�����ʒu(Y���W)
#define	CAM_POS_V_Z				(-20.0f)		// �J�����̎��_�����ʒu(Z���W)
#define	CAM_POS_R_X				(0.0f)			// �J�����̒����_�����ʒu(X���W)
#define	CAM_POS_R_Y				(0.0f)			// �J�����̒����_�����ʒu(Y���W)
#define	CAM_POS_R_Z				(0.0f)			// �J�����̒����_�����ʒu(Z���W)

#define	VIEW_ANGLE				(D3DXToRadian(45.0f))							// �r���[���ʂ̎���p
#define	VIEW_ASPECT				((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��
#define	VIEW_NEAR_Z				(10.0f)											// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z				(10000.0f)										// �r���[���ʂ�FarZ�l
#define	VALUE_MOVE_CAMERA		(0.2f)											// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA		(D3DX_PI * 0.002f)								// �J�����̉�]��
#define	VALUE_ADJUST_DISTANCE	(2.0f)											// ���_�ƒ����_�̋���������
#define HALF_OF_PI				(D3DX_PI / 2)

#define	DISTANCE_CAMERA_R (30)					// ���f���̎����̐�܂ł̋���

#define	CHASE_HEIGHT_V (CAM_POS_V_Y)			// �ǐՎ��̎��_�̍���
#define	CHASE_HEIGHT_R (CAM_POS_R_Y)			// �ǐՎ��̒����_�̍���

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
CAMERA g_camera;		// �J�������

CAMERA_MODE g_cameraMode = RELEASE_CAMERA;
/*******************************************************************************
�֐���:	HRESULT InitCamera(void)
����:	�Ȃ�
�߂�l:	HRESULT�^
����:	�J�����̏�����
*******************************************************************************/
HRESULT InitCamera(void)
{
	// ���_������
	g_camera.vPosV = D3DXVECTOR3(CAM_POS_V_X,CAM_POS_V_Y,CAM_POS_V_Z);
	// �����_������
	g_camera.vPosR = D3DXVECTOR3(CAM_POS_R_X,CAM_POS_R_Y,CAM_POS_R_Z);
	// ������x�N�g��������
	g_camera.vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	// ���_�̖ړI�ʒu
	g_camera.vPosVDest = g_camera.vPosV;
	// �����_�̖ړI�ʒu
	g_camera.vPosRDest = g_camera.vPosR;
	// �J���������i��]�p�j�̏�����
	g_camera.vRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	// ���_�ƒ����_�̋��������߂�
	g_camera.fDistance = sqrt(powf(g_camera.vPosR.x - g_camera.vPosV.x, 2.0f) +  powf(g_camera.vPosR.y - g_camera.vPosV.y, 2.0f) + powf(g_camera.vPosR.z - g_camera.vPosV.z, 2.0f));
	// �n�߂̎��_�ƒ����_�̋���
	g_camera.fInitialDistance = g_camera.fDistance;
	// ���_�̍�����������
	g_camera.fChaseHeightV = CHASE_HEIGHT_V;
	// �����_�̍�����������
	g_camera.fChaseHeightR = CHASE_HEIGHT_R;
	// �n�߂̎��_�ƒ����_����钼�p�O�p�`�̊p�x�̏�����
	g_camera.fInitialSin = fabs((g_camera.vPosR.y - g_camera.vPosV.y) / g_camera.fInitialDistance);
	g_camera.fInitialCos = fabs((g_camera.vPosR.z - g_camera.vPosV.z) / g_camera.fInitialDistance);

	return S_OK;
}

//=============================================================================
// �J�����̏I������
//=============================================================================
void UninitCamera(void)
{

}

//=============================================================================
// �J�����̍X�V����
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
	
		/* �����_���� */
		// ������(���_�𒆐S�ɒ����_�����)
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
		// �E����(���_�𒆐S�ɒ����_�����)
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
		// �����_��ړ�
		if(GetKeyboardPress(DIK_I))
		{
			g_camera.vPosR.y += VALUE_MOVE_CAMERA;
		}
		// �����_���ړ�
		if(GetKeyboardPress(DIK_K))
		{
			g_camera.vPosR.y -= VALUE_MOVE_CAMERA;
		}
	
		// ���_���s�ړ�
		// ��ړ�
		if(GetKeyboardPress(DIK_T))
		{
			g_camera.vPosR += g_camera.vecVR * VALUE_MOVE_CAMERA;
			g_camera.vPosV += g_camera.vecVR * VALUE_MOVE_CAMERA;
		}
		// ���ړ�
		if(GetKeyboardPress(DIK_G))
		{
			g_camera.vPosR -= g_camera.vecVR * VALUE_MOVE_CAMERA;
			g_camera.vPosV -= g_camera.vecVR * VALUE_MOVE_CAMERA;
		}
		// �E�ړ�
		if(GetKeyboardPress(DIK_H))
		{
			g_camera.vPosR -= g_camera.vecVerToVR * VALUE_MOVE_CAMERA;
			g_camera.vPosV -= g_camera.vecVerToVR * VALUE_MOVE_CAMERA;
		}
		// ���ړ�
		if(GetKeyboardPress(DIK_F))
		{
			g_camera.vPosR += g_camera.vecVerToVR * VALUE_MOVE_CAMERA;
			g_camera.vPosV += g_camera.vecVerToVR * VALUE_MOVE_CAMERA;
		}
	}
}

//=============================================================================
// �J�����̐ݒ菈��
//=============================================================================
void SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// �r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_camera.mtxView);
	// �r���[�}�g���b�N�X�̍쐬
	D3DXMatrixLookAtLH(&g_camera.mtxView, &g_camera.vPosV, &g_camera.vPosR, &g_camera.vecU);
	// �r���[�}�g���b�N�X�̐ݒ�
	pDevice -> SetTransform(D3DTS_VIEW, &g_camera.mtxView);
	// �v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_camera.mtxProjection);
	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(&g_camera.mtxProjection, VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);
	// �v���W�F�N�V�����}�g���b�N�X�̐ݒ�(�����ϊ��̐ݒ�)
	pDevice -> SetTransform(D3DTS_PROJECTION, &g_camera.mtxProjection);
}

/*******************************************************************************
�֐���:	CAMERA *GetCamera(void)
����:	�J�����̃|�C���^���擾
*******************************************************************************/
CAMERA *GetCamera(void)
{
	return &g_camera;
}
