#include "Game.h"

HRESULT CGame::InitD3D9(HWND hWnd) {
	if( NULL == (pD3d = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		return E_FAIL;
	}
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferWidth = 1280;
	d3dpp.BackBufferHeight = 720;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.Windowed = TRUE;
	// デバイスの生成
	if(FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &pDevice)))
	{
		if(FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &pDevice)))
		{
			if(FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &pDevice)))
			{
				if(FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &pDevice)))
				{
					return E_FAIL;
				}
			}
		}
	}
	// スプライトの作成
	if(FAILED(D3DXCreateSprite(pDevice, &pSprite))) {
		return E_FAIL;
	}
	// フォントの作成
	if(FAILED(D3DXCreateFont(pDevice, 24, 18, FW_REGULAR, NULL, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, PROOF_QUALITY,
		FIXED_PITCH | FF_MODERN, NULL, &pFont))) {
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CGame::ResetD3D9() {
	if(pDevice->TestCooperativeLevel() == D3DERR_DEVICELOST) {
		return E_FAIL;

		// リソースの解放
		SAFE_RELEASE(ocean.pNormalMap);
		SAFE_RELEASE(ocean.pRefMap);
		SAFE_RELEASE(ocean.pBuffer);
		SAFE_RELEASE(ocean.pEffect);
		SAFE_RELEASE(ship.pBuffer);
		SAFE_RELEASE(ship.pEffect);
		SAFE_RELEASE(sky.pBuffer);
		SAFE_RELEASE(sky.pEffect);
		SAFE_RELEASE(sky.pTexture);

		// デバイスのリセット
		if(pDevice->Reset(&d3dpp) != D3D_OK) {
			MessageBox(NULL, "Direct3DDevice9のリセットに失敗しました", NULL, MB_OK);
		}

		// リソースの再生成
		Create3DObject();
	}
}

HRESULT CGame::Create3DObject() {

	// Ocean
	if(FAILED(pDevice->CreateVertexBuffer(
		4*sizeof(VERTEX),
		D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
		D3DPOOL_MANAGED,
		&ocean.pBuffer,
		NULL))) {
			return E_FAIL;
	}
	VERTEX *v;
	ocean.pBuffer->Lock(0, 0, (void**)&v, 0);
	
	v[0].pos = D3DXVECTOR3(-0.5f, 0.0f, 0.5f);
	v[0].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	v[0].tex0 = D3DXVECTOR2(0.0f, 0.0f);

	v[1].pos = D3DXVECTOR3(0.5f, 0.0f, 0.5f);
	v[1].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	v[1].tex0 = D3DXVECTOR2(1.0f, 0.0f);
	
	v[2].pos = D3DXVECTOR3(-0.5f, 0.0f, -0.5f);
	v[2].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	v[2].tex0 = D3DXVECTOR2(0.0f, 1.0f);
	
	v[3].pos = D3DXVECTOR3(0.5f, 0.0f, -0.5f);
	v[3].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	v[3].tex0 = D3DXVECTOR2(1.0f, 1.0f);
	
	ocean.pBuffer->Unlock();

	if(FAILED(D3DXCreateEffectFromFile(pDevice, "ocean.fx", NULL, NULL, 0, NULL, &ocean.pEffect, NULL))) {
		MessageBox(NULL, "OceanEffectの作成に失敗", NULL, MB_OK);
		return E_FAIL;
	}
	LPDIRECT3DTEXTURE9 tex;
	D3DSURFACE_DESC desc;
	if(FAILED(D3DXCreateTextureFromFile(pDevice, "ocean.png", &tex))) {
		MessageBox(NULL, "OceanHeightMapTextureの作成に失敗", NULL, MB_OK);
		return E_FAIL;
	}
	tex->GetLevelDesc(0,&desc);
	if(FAILED(D3DXCreateTexture(pDevice, desc.Width, desc.Height, 0, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &ocean.pNormalMap)))
	{
		MessageBox(NULL, "テクスチャの読み込みに失敗しました", NULL, MB_OK);
	}
	if(FAILED(D3DXComputeNormalMap(ocean.pNormalMap, tex, NULL, 0, D3DX_CHANNEL_LUMINANCE, 3.0f))) {
		MessageBox(NULL, "OceanNormalMapの作成に失敗", NULL, MB_OK);
		return E_FAIL;
	}
	tex->Release();
	if(FAILED(D3DXCreateTextureFromFile(pDevice, "ref.jpg", &ocean.pRefMap))) {
		MessageBox(NULL, "ReflectTextureの作成に失敗", NULL, MB_OK);
		return E_FAIL;
	}

	ocean.delta = 0.002f;
	ocean.height = 1.0f;
	ocean.vecWave = D3DXVECTOR2(0.0f, 0.0f);

	// Ship
	if(FAILED(pDevice->CreateVertexBuffer(
		3*sizeof(D3DXVECTOR3),
		D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ,
		D3DPOOL_MANAGED,
		&ship.pBuffer,
		NULL))) {
			return E_FAIL;
	}
	D3DXVECTOR3 *v3;
	ship.pBuffer->Lock(0, 0, (void**)&v3, 0);
	v3[0] = D3DXVECTOR3(0.0f, 0.23f, 0.0f);
	v3[1] = D3DXVECTOR3(0.12f, -0.23f, 0.0f);
	v3[2] = D3DXVECTOR3(-0.12f, -0.23f, 0.0f);
	ship.pBuffer->Unlock();

	if(FAILED(D3DXCreateEffectFromFile(pDevice, "ship.fx", NULL, NULL, 0, NULL, &ship.pEffect, NULL))) {
		MessageBox(NULL, "ShipEffectの作成に失敗", NULL, MB_OK);
		return E_FAIL;
	}

	// Sky
	if(FAILED(pDevice->CreateVertexBuffer(
		4*sizeof(VERTEX2),
		D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ | D3DFVF_TEX1,
		D3DPOOL_MANAGED,
		&sky.pBuffer,
		NULL))) {
			return E_FAIL;
	}
	VERTEX2 *v2;
	if(FAILED(sky.pBuffer->Lock(0, 0, (void**)&v2, 0))) {
	}
	
	v2[0].pos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	v2[0].tex0 = D3DXVECTOR2(0.0f, 0.0f);

	v2[1].pos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	v2[1].tex0 = D3DXVECTOR2(1.0f, 0.0f);
	
	v2[2].pos = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	v2[2].tex0 = D3DXVECTOR2(0.0f, 1.0f);
	
	v2[3].pos = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	v2[3].tex0 = D3DXVECTOR2(1.0f, 1.0f);
	
	sky.pBuffer->Unlock();
	if(FAILED(D3DXCreateTextureFromFile(pDevice, "sky.jpg", &sky.pTexture))) {
		MessageBox(NULL, "SkyTextureの作成に失敗", NULL, MB_OK);
		return E_FAIL;
	}
	if(FAILED(D3DXCreateEffectFromFile(pDevice, "sky.fx", NULL, NULL, 0, NULL, &sky.pEffect, NULL))) {
		MessageBox(NULL, "SkyEffectの作成に失敗", NULL, MB_OK);
		return E_FAIL;
	}

	// Menu
	if(FAILED(D3DXCreateTextureFromFile(pDevice, "top.jpg", &menu.pTexTop))) {
		MessageBox(NULL, "Textureの作成に失敗", NULL, MB_OK);
		return E_FAIL;
	}
	if(FAILED(D3DXCreateTextureFromFile(pDevice, "menu0.jpg", &menu.pTexMenu[0]))) {
		MessageBox(NULL, "Textureの作成に失敗", NULL, MB_OK);
		return E_FAIL;
	}
	if(FAILED(D3DXCreateTextureFromFile(pDevice, "menu1.jpg", &menu.pTexMenu[1]))) {
		MessageBox(NULL, "Textureの作成に失敗", NULL, MB_OK);
		return E_FAIL;
	}
	if(FAILED(D3DXCreateTextureFromFile(pDevice, "menu2.jpg", &menu.pTexMenu[2]))) {
		MessageBox(NULL, "Textureの作成に失敗", NULL, MB_OK);
		return E_FAIL;
	}

	// Camera
	camera.vecPos = D3DXVECTOR3(0.0f, 30.0f, -25.0f);
	camera.vecLookAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	camera.vecEyeVec = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	camera.fov = D3DX_PI/15;		// 9*9
	camera.zn = 0.0f;
	camera.zf = 1000.0f;
	D3DVIEWPORT9 vp;
	pDevice->GetViewport(&vp);
	camera.aspect = (float)vp.Width / (float)vp.Height;

	return S_OK;
}