/*******************************************************************************
* �^�C�g��:		���C������
* �v���O������:	main.h
*******************************************************************************/
#ifndef _MAIN_H_
#define _MAIN_H_

#define USE_SOUND		// �T�E���h�g�p�̗L��
//#undef USE_SOUND
//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include <windows.h>
#include <fbxsdk.h>
#include "d3dx9.h"
#define DIRECTINPUT_VERSION (0x0800)
#include "dinput.h"
#include "input.h"

#ifdef USE_SOUND
#include "xaudio2.h"		// XAudio2�֘A�̃w�b�_�[
#endif

//*****************************************************************************
// ���C�u�����̃����N
//*****************************************************************************
#if 1	// [������"0"�ɂ����ꍇ�A"�\���v���p�e�B" -> "�����J" -> "����" -> "�ǉ��̈ˑ��t�@�C��"�ɑΏۃ��C�u������ݒ肷��]
#pragma comment (lib, "d3d9.lib")		// �`�揈���ɕK�v
#pragma comment (lib, "d3dx9.lib")		// [d3d9.lib]�̊g�����C�u����
#pragma comment (lib, "dxguid.lib")		// DirectX�R���|�[�l���g�g�p�ɕK�v
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "winmm.lib")
#endif

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#if 1

#define SCREEN_WIDTH	(1760)//(1280)	// �E�C���h�E�̕�
#define SCREEN_HEIGHT	(990)//(720)	// �E�C���h�E�̍���

#else

#define SCREEN_WIDTH	(1280)//(1760)	// �E�C���h�E�̕�
#define SCREEN_HEIGHT	(720)//(990)	// �E�C���h�E�̍���

#endif

#define NUM_VERTEX (4)
#define NUM_POLYGON (2)

#define SAFE_RELEASE(ptr)			{ if(ptr) { (ptr)->Release(); (ptr) = NULL; } }

/*******************************************************************************
* �\���̒�`
*******************************************************************************/
// ��L�Q�c�|���S�����_�t�H�[�}�b�g�ɍ��킹���\���̂��`
typedef struct
{
	D3DXVECTOR3 pos;	// ���_���W
	float		rhw;	// �e�N�X�`���̃p�[�X�y�N�e�B�u�R���N�g�p
	D3DCOLOR	col;	// ���ˌ�(���_�J���[)
	D3DXVECTOR2 uv;	// �e�N�X�`�����W
} VERTEX_2D;

// ��L�R�c�|���S�����_�t�H�[�}�b�g�ɍ��킹���\���̂��`
typedef struct
{
	D3DXVECTOR3 pos;	// ���_���W
	D3DXVECTOR3 nor;	// �@���x�N�g��
	D3DCOLOR	col;	// ���ˌ�
	D3DXVECTOR2 uv;	// �e�N�X�`�����W
} VERTEX_3D;

#if 0
class CMyWindow
{
private:
	float fWidth;
	float fHeight;
	float fRatioNewWidthToOldWidth;		// �i���݂̃N���C�A���g�����j / �i�N���C�A���g�����̏����l�j
	float fRatioNewHeightToOldHeight;	// �i���݂̃N���C�A���g�c���j / �i�N���C�A���g�c���̏����l�j
public:
	void UpdateSize(void);
};
#else
typedef struct
{
	float fWidth;
	float fHeight;
	float fRatioNewWidthToOldWidth;		// �i���݂̃N���C�A���g�����j / �i�N���C�A���g�����̏����l�j
	float fRatioNewHeightToOldHeight;	// �i���݂̃N���C�A���g�c���j / �i�N���C�A���g�c���̏����l�j
} SCREEN;
#endif

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LPDIRECT3DDEVICE9 GetDevice(void);

HWND GetHandleWindow(void);
HINSTANCE GetHandleInstance(void);

void InitWindowSize(void);
void UpdateWindowSize(void);
SCREEN GetScreenInfo(void);

#endif