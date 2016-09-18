#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "UsingDirectX.h"

enum DIR {
	north,
	east,
	south,
	west
};

struct SPACE {
	int treasure;
	BYTE information;
	bool searched;
};

struct MAP {
	SPACE space[30][30];
	int width;
	int height;
	int treasure;
};

struct PLAYER {
	float rx, rz;
	int x, z;
	DIR d;
};

struct CAMERA {
	D3DXVECTOR3 vecPos;
	D3DXVECTOR3 vecLookAt;
	D3DXVECTOR3 vecEyeVec;
	FLOAT fov;
	FLOAT aspect;
	FLOAT zn;
	FLOAT zf;
};

struct USER_INPUT {
	// A,Bボタンを押した瞬間か
	bool push_A, push_B;
	// 前のフレームの入力情報
	bool prevA, prevB;
	// x,y入力
	int x,y;
};

struct VERTEX {
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex0;
};

struct OCEAN {
	LPD3DXEFFECT pEffect;
	LPDIRECT3DVERTEXBUFFER9 pBuffer;
	LPDIRECT3DTEXTURE9 pNormalMap;
	LPDIRECT3DTEXTURE9 pRefMap;
	D3DXMATRIXA16 matTrans;
	D3DXVECTOR2 vecWave;
	FLOAT height;
	FLOAT delta;
};

struct SHIP {
	LPD3DXEFFECT pEffect;
	LPDIRECT3DVERTEXBUFFER9 pBuffer;
	D3DXMATRIXA16 matTrans;
	D3DXMATRIXA16 matRot;
};

struct VERTEX2 {
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex0;
};

struct SKY {
	LPDIRECT3DVERTEXBUFFER9 pBuffer;
	LPDIRECT3DTEXTURE9 pTexture;
	LPD3DXEFFECT pEffect;
};

struct MENU {
	LPDIRECT3DTEXTURE9 pTexTop;
	LPDIRECT3DTEXTURE9 pTexMenu[3];
};

#endif