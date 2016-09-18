#include "Game.h"

bool CGame::Update() {
	// �^�C�}�[
	nowTime = timeGetTime();
	progressTime = nowTime - prevTime;
	idealTime = (DWORD)(frame * (100.0F / fps));
	if(idealTime > progressTime)
		Sleep(idealTime - progressTime);
	if(progressTime >= 1000) {
		prevFrame = frame;
		frame = 0;
		prevTime = nowTime;
	}
	frame++;

	// �C���v�b�g�̍X�V
	GetInput();

	// �Q�[������
	switch(phase) {
		case 0:		// ���[�f�B���O
			Start(9, 9, 0, 1);
			menu_index = 0;
			choicetime = nowTime;
			phase++;
			break;
		case 1:		// ���C�����j���[
			if(nowTime-choicetime > 200) {
				if(input.y > 500) {
					menu_index++;
					if(menu_index > 2) {
						menu_index = 0;
					}
					choicetime = nowTime;
				} else if(input.y < -500) {
					menu_index--;
					if(menu_index < 0) {
						menu_index = 2;
					}
					choicetime = nowTime;
				}
			}
			if(input.push_A) {
				pWavDecide->Stop();
				pWavDecide->SetCurrentPosition(0);
				pWavDecide->Play(0,0,0);
				switch(menu_index) {
					case 0:
						Start(9,9,10,1);
						break;
					case 1:
						Start(17,17,40,2);
						break;
					case 2:
						Start(29,29,99,3);
						break;
				}
				phase++;
			}
			break;
		case 2:		// �Q�[��
			if(rest <= 0) {
				phase++;
				break;
			}
			today = timeGetTime();
			// �����o�ߔ���
			if(today-yesterday > 1000) {
				yesterday = today;
				gold -= 100 * (days/100+1);
				days++;
			}
			// �����s������
			if(gold < 0 && !salvage_f) {
				MessageBox(NULL, "���������s���Ă��܂��܂����E�E�E", "�Q�[���I�[�o�[", MB_OK);
				phase+=2;
				break;
			}
			if(sonar_f) {
				if(today-sonarday > 1000) {
					Sonar();
					sonar_f = false;
				}
				break;
			}
			if(salvage_f) {
				if(today-salvageday > salvagetime) {
					Salvage();
					salvage_f = false;
				}
				break;
			}
			if(input.push_A) {
				pWavSonar->Stop();
				pWavSonar->SetCurrentPosition(0);
				pWavSonar->Play(0, 0, NULL);
				sonarday = today;
				sonar_f = true;
				break;
			}
			if(input.push_B) {
				pWavSalvage->Stop();
				pWavSalvage->SetCurrentPosition(0);
				pWavSalvage->Play(0, 0, NULL);
				salvageday = today;
				salvagetime = 3000;
				salvage_f = true;
				gold -= 10000;
				break;
			}
			if(input.x > 500) {
				Turn(west);
			} else if(input.x < -500) {
				Turn(east);
			} else if(input.y > 500) {
				Turn(south);
			} else if(input.y < -500) {
				Turn(north);
			}
			// �ړ�
			Move();
			// �}�b�v�O����
			if(player.x < 0 || player.x >= map.width || player.z < 0 || player.z >= map.height) {
				MessageBox(NULL, "�}�b�v�͈̔͊O�ɏo�Ă��܂��܂����E�E�E", "�Q�[���I�[�o�[", MB_OK);
				phase+=2;
				break;
			}
			break;
		case 3:		// �Q�[���N���A
			MessageBox(NULL, "���߂łƂ��������܂��I", "�Q�[���N���A", MB_OK);
			phase = 0;
			break;
		case 4:		// �Q�[���I�[�o�[
			phase = 0;
			break;
	}

	// �V�[���̍X�V
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);
	if(SUCCEEDED(pDevice->BeginScene())) {
		
		// �`�揈��
		D3DXMATRIXA16 matView;
		D3DXMatrixLookAtLH(&matView, &camera.vecPos, &camera.vecLookAt, &camera.vecEyeVec);
		D3DXMATRIXA16 matProj;
		D3DXMatrixPerspectiveFovLH(&matProj, camera.fov, camera.aspect, camera.zn, camera.zf);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// ��̕`��
		if(SUCCEEDED(sky.pEffect->SetTechnique("render"))) {
			sky.pEffect->Begin(NULL, 0);
			pDevice->SetStreamSource(0, sky.pBuffer, 0, sizeof(VERTEX2));
			pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
			sky.pEffect->SetTexture("DecaleMap", sky.pTexture);
			if(SUCCEEDED(sky.pEffect->BeginPass(0))) {
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
				sky.pEffect->EndPass();
			}
			sky.pEffect->End();
		}

		// �C�̕`��
		ocean.vecWave.x+=0.002f;
		ocean.height+=ocean.delta;
		if(ocean.height<=0.8) {
			ocean.delta=0.004;
		}
		else if(ocean.height>1.2) {
			ocean.delta=-0.004;
		}
		if(SUCCEEDED(ocean.pEffect->SetTechnique("render"))) {
			if(SUCCEEDED(ocean.pEffect->Begin(NULL, 0))) {
				pDevice->SetStreamSource(0, ocean.pBuffer, 0, sizeof(VERTEX));
				pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
				ocean.pEffect->SetTexture("NormalMap", ocean.pNormalMap);
				ocean.pEffect->SetTexture("ReflectMap", ocean.pRefMap);
				ocean.pEffect->SetValue("wave", &ocean.vecWave, sizeof(D3DXVECTOR2));
				ocean.pEffect->SetValue("height", &ocean.height, sizeof(FLOAT));
				ocean.pEffect->SetValue("vecEyePos", &camera.vecPos, sizeof(D3DXVECTOR3));
				ocean.pEffect->SetValue("vecLightDir", &D3DXVECTOR3(1.0f, -1.0f, 1.0f), sizeof(D3DXVECTOR3));
				ocean.pEffect->SetValue("tangent", &D3DXVECTOR3(1.0f/sqrt(2.0f), 0.0f, 1.0f/sqrt(2.0f)), sizeof(D3DXVECTOR3));
				for(int i=0; i<map.width; i++) {
					for(int j=0; j<map.height; j++) {
						ocean.pEffect->BeginPass(0);
						D3DXMatrixTranslation(&ocean.matTrans, -(map.width/2)*1.0f+i*1.0f, 0.0f, (map.height/2)*1.0f-j*1.0f);
						ocean.pEffect->SetMatrix("matW", &(ocean.matTrans));
						ocean.pEffect->SetMatrix("matWVP", &(ocean.matTrans*matView*matProj));
						switch(map.space[i][j].information) {
							case 0:
								ocean.pEffect->SetVector("color", &D3DXVECTOR4(0.1f,0.1f,0.5f,1.0f));
								break;
							case 1:
								ocean.pEffect->SetVector("color", &D3DXVECTOR4(0.1f,0.4f,0.5f,1.0f));
								break;
							case 2:
								ocean.pEffect->SetVector("color", &D3DXVECTOR4(0.5f,0.5f,0.1f,1.0f));
								break;
							default:
								ocean.pEffect->SetVector("color", &D3DXVECTOR4(0.4f,0.1f,0.5f,1.0f));
								break;
						}
						if(player.x == i && player.z == j) {
							ocean.pEffect->SetVector("color", &D3DXVECTOR4(0.4f, 0.4f, 0.4f, 1.0f));
						}
						pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
						ocean.pEffect->EndPass();
					}
				}
				ocean.pEffect->End();
			}
		}

		// �D�̕`��
		if(SUCCEEDED(ship.pEffect->SetTechnique("render"))) {
			if(SUCCEEDED(ship.pEffect->Begin(NULL, 0))) {
				if(SUCCEEDED(ship.pEffect->BeginPass(0))) {
					D3DXMatrixTranslation(&ship.matTrans, player.rx, 0.0001f, player.rz);
					D3DXMatrixRotationYawPitchRoll(&ship.matRot, 0, D3DX_PI/2, player.d*(D3DX_PI/2));
					ship.pEffect->SetMatrix("matWVP", &(ship.matRot*ship.matTrans*matView*matProj));
					if(sonar_f) {
						ship.pEffect->SetVector("color", &D3DXVECTOR4(0.8f,0.8f,0.0f,1.0f));
					} else if(salvage_f) {
						ship.pEffect->SetVector("color", &D3DXVECTOR4(0.8f,0.0f,0.0f,1.0f));
					} else {
						ship.pEffect->SetVector("color", &D3DXVECTOR4(0.0f,0.8f,0.0f,1.0f));
					}
					pDevice->SetStreamSource(0, ship.pBuffer, 0, sizeof(D3DXVECTOR3));
					pDevice->SetFVF(D3DFVF_XYZ);
					pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
					ship.pEffect->EndPass();
				}
				ship.pEffect->End();
			}
		}

		// ���j���[�̕`��
		if(phase == 1) {
			pSprite->Begin(D3DXSPRITE_ALPHABLEND);

			RECT rect = {0, 0, 512, 512};
			pSprite->Draw(menu.pTexTop, &rect, NULL, &D3DXVECTOR3(100,100,0), D3DCOLOR_ARGB(255,255,255,255));
			RECT r = {0,0,512,128};
			for(int i=0; i<3; i++) {
				pSprite->Draw(menu.pTexMenu[i], &r, NULL, &D3DXVECTOR3(700-(i==menu_index?50:0), 125+i*150, 0),
					D3DCOLOR_ARGB(255,255,255,255));
			}

			pSprite->End();
		}

		// �����̕`��
		pSprite->Begin(D3DXSPRITE_ALPHABLEND);
		TCHAR str[128];
		sprintf(str, "fps=%d", fps);
		RECT rect1 = {10, 680, 0, 0};
		pFont->DrawText(pSprite, str, -1, &rect1, DT_CALCRECT, NULL);
		pFont->DrawText(pSprite, str, -1, &rect1, DT_LEFT | DT_BOTTOM, 0xff000000);
		sprintf(str, "money %8d���~ / point �c��%2d�ӏ� / %4dday", gold, rest, days);
		RECT rect2 = {100, 20, 0, 0};
		pFont->DrawText(pSprite, str, -1, &rect2, DT_CALCRECT, NULL);
		pFont->DrawText(pSprite, str, -1, &rect2, DT_LEFT | DT_BOTTOM, 0xff000000);
		pSprite->End();

		pDevice->EndScene();
	}
	if(pDevice->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST) {
		ResetD3D9();
	}
	return true;
}

void CGame::Start(DWORD width, DWORD height, DWORD num_treasure, DWORD difficult) {
	// �}�b�v�̍쐬
	ZeroMemory(&map, sizeof(map));
	map.width = width;
	map.height = height;
	map.treasure = num_treasure;
	// �g���W���[�̔z�u
	srand(timeGetTime());
	int x, y;
	for(DWORD i=0; i<num_treasure; i++) {
		x = rand()%width;
		y = rand()%height;
		if(map.space[x][y].treasure == 0) {
			map.space[x][y].treasure = 12000;
		} else {
			i--;
			continue;
		}
	}

	// �X�e�[�^�X�̏�����
	today = yesterday = timeGetTime();
	days = 0;
	gold = 10000*difficult;
	rest = num_treasure;
	sonar_f = false;
	salvage_f = false;

	// �v���C���[�̏�����
	player.d = south;
	player.x = width/2;
	player.z = height/2;
	player.rx = 0.0f;
	player.rz = 0.0f;
	
	// �J�����̈ړ�
	switch(width) {
		case 9:
			camera.fov = D3DX_PI/15;
			break;
		case 17:
			camera.fov = D3DX_PI/8;
			break;
		case 29:
			camera.fov = D3DX_PI/4;
			break;
	}
}

void CGame::Move() {
	switch(player.d) {
		case north:
			player.rz += 0.02f;
			break;
		case south:
			player.rz -= 0.02f;
			break;
		case east:
			player.rx -= 0.02f;
			break;
		case west:
			player.rx += 0.02f;
			break;
	}
	player.x = (int)(player.rx+(player.rx > 0 ? 0.5f : -0.5f))+map.width/2;
	player.z = -(int)(player.rz+(player.rz > 0 ? 0.5f : -0.5f))+map.height/2;
}

void CGame::Salvage() {
	if(map.space[player.x][player.z].treasure > 0) {
		pWavDecide->Play(0, 0, 0);
		gold += map.space[player.x][player.z].treasure;
		map.space[player.x][player.z].treasure = 0;
		rest--;
	} else {
		pWavCancel->Play(0, 0, 0);
	}
}

void CGame::Sonar() {
	// �O��̃\�i�[�L�^��j��
	for(int i=0; i<map.width; i++) {
		for(int j=0; j<map.height; j++) {
			map.space[i][j].information = 0;
			map.space[i][j].searched = false;
		}
	}
	// �����̎l���Ƀ\�i�[��������
	if(player.x > 0) SonarStep(player.x-1, player.z);
	if(player.x < map.width-1) SonarStep(player.x+1, player.z);
	if(player.z > 0) SonarStep(player.x, player.z-1);
	if(player.z < map.height-1) SonarStep(player.x, player.z+1);
	pWavCancel->Play(0, 0, 0);
}

void CGame::SonarStep(DWORD x, DWORD z) {
	if(map.space[x][z].searched) return;
	int count=0;
	for(int i=-1; i<2; i++) {
		for(int j=-1; j<2; j++) {
			if(x==0 && i==-1) continue;
			if(x==map.width-1 && i==1) continue;
			if(z==0 && j==-1) continue;
			if(z==map.height-1 && j==1) continue;
			if(i==0 && j==0) continue;
			if(map.space[x+i][z+j].treasure > 0) count++;
		}
	}
	map.space[x][z].searched = true;
	if(count==0) {
		if(x > 0) SonarStep(x-1, z);
		if(x < map.width-1) SonarStep(x+1, z);
		if(z > 0) SonarStep(x, z-1);
		if(z < map.height-1) SonarStep(x, z+1);
	} else {
		map.space[x][z].information = count;
	}
}

void CGame::Turn(DIR d) {
	player.d = d;
}