#ifndef GAME_H
#define GAME_H

#include "UsingDirectX.h"
#include "GameData.h"

class CGame {
public:
	CGame();
	~CGame();
	// 初期化
	bool Init(HWND hWnd);
	// 更新
	bool Update();
	// リフレッシュ
	void Reflesh();
	// 解放
	void Release();
private:
	static CGame *pSelf;

	//============================================
	//============================================
	// fps_timer
	DWORD nowTime, prevTime, idealTime, progressTime;
	int frame, prevFrame;
	int fps;

	//============================================
	//============================================
	// Direct3D9 - Function
	HRESULT InitD3D9(HWND hWnd);
	HRESULT Create3DObject();
	HRESULT ResetD3D9();
	// Direct3D9 - Device
	D3DPRESENT_PARAMETERS d3dpp;
	LPDIRECT3D9 pD3d;
	LPDIRECT3DDEVICE9 pDevice;
	// Direct3D9 - Sprite
	LPD3DXSPRITE pSprite;
	// Direct3D9 - Font
	LPD3DXFONT pFont;
	// Direct3D9 - Object
	OCEAN ocean;
	SHIP ship;
	SKY sky;
	MENU menu;

	// Camera
	CAMERA camera;

	//============================================
	//============================================
	// DirectInput8 - Function
	static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdid, VOID *p);
	static BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdid, VOID* device);
	HRESULT InitDInput(HWND hWnd);
	HRESULT ResetInput();
	void GetInput();
	// DirectInput8
	LPDIRECTINPUT8 pInput;
	// DirectInput8 - Devices
	LPDIRECTINPUTDEVICE8 pKey;
	vector<LPDIRECTINPUTDEVICE8> pPads;
	// DirectInput8 - Buffer
	USER_INPUT input;

	//============================================
	//============================================
	// DirectSound - Function
	HRESULT InitDSound(HWND hWnd);
	HRESULT CreateSoundBuffer(LPSTR filename, LPDIRECTSOUNDBUFFER *ppWavBuffer);
	HRESULT CreateSoundObject();
	// DirectSound
	LPDIRECTSOUND8 pSound;
	// DirectSound - WaveBuffer
	LPDIRECTSOUNDBUFFER pWavDecide;
	LPDIRECTSOUNDBUFFER pWavCancel;
	LPDIRECTSOUNDBUFFER pWavOcean;
	LPDIRECTSOUNDBUFFER pWavSonar;
	LPDIRECTSOUNDBUFFER pWavSalvage;

	//============================================
	//============================================
	// Game - Function
	void Start(DWORD width, DWORD height, DWORD num_treasure, DWORD difficult);
	void Move();
	void Turn(DIR d);
	void Sonar();
	void SonarStep(DWORD x, DWORD z);
	void Salvage();
	// 日数
	DWORD days;
	DWORD today, yesterday;
	// ソナーとサルベージ
	DWORD sonarday;
	bool sonar_f;
	DWORD salvageday;
	DWORD salvagetime;
	bool salvage_f;
	// 資金
	int gold;
	// 残りポイント
	DWORD rest;
	// マップ
	MAP map;
	// プレイヤーの位置
	PLAYER player;
	// フェーズ
	DWORD phase;

	// メインメニュー項目番号
	int menu_index;
	DWORD choicetime;
};

#endif
