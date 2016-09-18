#include "Game.h"

//=============================================================
// 利用可能なジョイスティックを列挙して初期化する
//=============================================================
BOOL CALLBACK CGame::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdid, VOID *p)
{
	LPDIRECTINPUTDEVICE8 pPad=NULL;
	if (!FAILED(pSelf->pInput->CreateDevice(pdid->guidInstance, &pPad, NULL))) {
		// デバイスの登録
		pSelf->pPads.push_back(pPad);
	}
	return DIENUM_CONTINUE;
}

//=============================================================
// コントローラーのプロパティを列挙して設定する
//=============================================================
BOOL CALLBACK CGame::EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdid, VOID* device)
{
	LPDIRECTINPUTDEVICE8 pPad = (LPDIRECTINPUTDEVICE8)device;
	if (pdid->dwType & DIDFT_AXIS) {
		DIPROPRANGE dip;
		dip.diph.dwSize = sizeof(DIPROPRANGE);
		dip.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dip.diph.dwHow = DIPH_BYID;
		dip.diph.dwObj = pdid->dwType;
		dip.lMin= -1000;
		dip.lMax= 1000;
		// パッドの設定
		pPad->SetProperty(DIPROP_RANGE, &dip.diph);
	}
	return DIENUM_CONTINUE;
}

HRESULT CGame::InitDInput(HWND hWnd) {
	// DirectInputの作成
	if(FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pInput, NULL))) {
		return E_FAIL;
	}

	// ジョイスティックデバイスの列挙及び取得
	if(FAILED(pInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY))) {
		return E_FAIL;
	}
	// 各ジョイスティックデバイスを設定する
	for(int i=0, n=pPads.size(); i<n; i++) {
		if(FAILED(pPads[i]->SetDataFormat(&c_dfDIJoystick2))) {
			return E_FAIL;
		}
		if(FAILED(pPads[i]->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND))) {
			return E_FAIL;
		}
		if(FAILED(pPads[i]->EnumObjects(EnumObjectsCallback, pPads[i], DIDFT_ALL))) {
			return E_FAIL;
		}
	}

	// キーボードデバイスの取得及び設定
	if(FAILED(pInput->CreateDevice(GUID_SysKeyboard, &pKey, NULL))) {
		return E_FAIL;
	}
	if(FAILED(pKey->SetDataFormat(&c_dfDIKeyboard))) {
		return E_FAIL;
	}
	if(FAILED(pKey->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND))) {
		return E_FAIL;
	}

	// デバイスの更新
	pKey->Acquire();
	for(int i=0, n=pPads.size(); i<n; i++)
	{
		pPads[i]->Acquire();
	}

	return S_OK;
}

HRESULT CGame::ResetInput() {
	if(FAILED(pKey->Acquire())) {
		return E_FAIL;
	}
	for(int i=0, n=pPads.size(); i<n; i++)
	{
		pPads[i]->Acquire();
	}
	return S_OK;
}

void CGame::GetInput() {
	bool pressA_f = false;
	bool pressB_f = false;
	// インプット情報の初期化
	input.x = 0;
	input.y = 0;
	input.push_A = false;
	input.push_B = false;

	// キーボードの入力情報取得
	BYTE diks[256];
	pKey->GetDeviceState(sizeof(diks), &diks);
	// ジョイスティックの入力情報取得
	DIJOYSTATE2 *js = new DIJOYSTATE2[pPads.size()];
	for(int i=0, n=pPads.size(); i<n; i++) {
		pPads[i]->GetDeviceState(sizeof(DIJOYSTATE2), &js[i]);
	}

	// 入力情報に基づいてコントローラーにデータを設定
	if(diks[DIK_LEFT] & 0x80) input.x -= 1000;
	if(diks[DIK_RIGHT] & 0x80) input.x += 1000;
	if(diks[DIK_UP] & 0x80) input.y -= 1000;
	if(diks[DIK_DOWN] & 0x80) input.y += 1000;
	if(diks[DIK_Z] & 0x80) {
		if(!input.prevA) {
			input.push_A = true;
		}
		pressA_f = true;
	}
	if(diks[DIK_X] & 0x80) {
		if(!input.prevB) {
			input.push_B = true;
		}
		pressB_f = true;
	}

	for(int i=0, n=pPads.size(); i<n; i++) {
		input.x += js[i].lX;
		input.y += js[i].lY;
		if(js[i].rgbButtons[0] & 0x80) {
			if(!input.prevA) {
				input.push_A = true;
			}
			pressA_f = true;
		}
		if(js[i].rgbButtons[1] & 0x80) {
			if(!input.prevB) {
				input.push_B = true;
			}
			pressB_f = true;
		}
	}
	SAFE_DELETE_ARRAY(js);

	if(pressA_f) {
		input.prevA = true;
	} else {
		input.prevA = false;
	}
	if(pressB_f) {
		input.prevB = true;
	} else {
		input.prevB = false;
	}
}