#include "main.h"

LPD3DXEFFECT g_pEffect;

void InitEffectFile(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	LPD3DXBUFFER pCompilationErrors = NULL;

	D3DXCreateEffectFromFile(
		pDevice,								// LPDIRECT3DDEVICE9 pDevice
		"data/EFFECT/BaseEffect.fx",			// LPCSTR            pSrcFile
		NULL,									// CONST D3DXMACRO*  pDefines
		NULL,									// LPD3DXINCLUDE     pInclude
		D3DXSHADER_DEBUG,						// DWORD             Flags
		NULL,									// LPD3DXEFFECTPOOL  pPool
		&g_pEffect,								// LPD3DXEFFECT*     ppEffect
		&pCompilationErrors);					// LPD3DXBUFFER      *ppCompilationErrors

	if(pCompilationErrors)
	{
		MessageBox(NULL,(char*)pCompilationErrors->GetBufferPointer(),
			"D3DXCreateEffectFromFile", 0);
		pCompilationErrors->Release();
	}
}

LPD3DXEFFECT GetEffect(void)
{
	return g_pEffect;
}