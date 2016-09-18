#include "Game.h"

CGame *CGame::pSelf = NULL;

CGame::CGame() {
	pSelf = this;
	phase = 0;
	Start(9, 9, 0, 1);
}

CGame::~CGame() {
}

bool CGame::Init(HWND hWnd) {
	if(FAILED(InitD3D9(hWnd))) {
		MessageBox(NULL, "Direct3D9�̏������Ɏ��s", NULL, MB_OK);
		return false;
	}
	if(FAILED(InitDInput(hWnd))) {
		MessageBox(NULL, "DirectInput�̏������Ɏ��s", NULL, MB_OK);
		return false;
	}
	if(FAILED(InitDSound(hWnd))) {
		MessageBox(NULL, "DirectSound�̏������Ɏ��s", NULL, MB_OK);
		return false;
	}

	Create3DObject();

	fps = 60;
	frame = 0;
	prevFrame = 0;
	nowTime = timeGetTime();
	prevTime = nowTime;
	idealTime = (DWORD)(frame * (100.0F / fps));
	progressTime = 0;

	return true;
}

void CGame::Reflesh() {
	ResetInput();
}

void CGame::Release() {
	// ���\�[�X�̉��
	SAFE_RELEASE(ocean.pNormalMap);
	SAFE_RELEASE(ocean.pRefMap);
	SAFE_RELEASE(ocean.pBuffer);
	SAFE_RELEASE(ocean.pEffect);
	SAFE_RELEASE(ship.pBuffer);
	SAFE_RELEASE(ship.pEffect);
	SAFE_RELEASE(sky.pBuffer);
	SAFE_RELEASE(sky.pEffect);
	SAFE_RELEASE(sky.pTexture);

	// �f�o�C�X�̉��
	SAFE_RELEASE(pFont);
	SAFE_RELEASE(pSprite);
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pD3d);

	// �f�o�C�X�̉��
	SAFE_RELEASE(pKey);
	for(int i=0, n=pPads.size(); i<n; i++) {
		SAFE_RELEASE(pPads[i]);
	}
	SAFE_RELEASE(pInput);

	// �f�o�C�X�̉��
	SAFE_RELEASE(pWavDecide);
	SAFE_RELEASE(pWavCancel);
	SAFE_RELEASE(pWavOcean);
	SAFE_RELEASE(pWavSonar);
	SAFE_RELEASE(pWavSalvage);
	SAFE_RELEASE(pSound);
}
