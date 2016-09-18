#include "Game.h"

HRESULT CGame::InitDSound(HWND hWnd) {
	// DirectSound�̏�����
	if(FAILED(DirectSoundCreate8(NULL, &pSound, NULL)))
	{
		return E_FAIL;
	}
	pSound->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE);

	// ���\�[�X�̍쐬
	CreateSoundObject();
	pWavOcean->Play(0, 0, DSBPLAY_LOOPING);

	return S_OK;
}

HRESULT CGame::CreateSoundBuffer(LPSTR filename, LPDIRECTSOUNDBUFFER *ppWavBuffer) {
	// wav�t�@�C�����J���Ă���
	HMMIO hMmio = NULL;
	DWORD dwWavSize = 0;
	WAVEFORMATEX* pwfex;
	MMCKINFO ckInfo;
	MMCKINFO riffckInfo;
	PCMWAVEFORMAT pcmWaveFormat;
	hMmio = mmioOpen(filename, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if(MMSYSERR_NOERROR !=mmioDescend(hMmio, &riffckInfo, NULL, NULL))
	{
		MessageBox(NULL, "�`�����N�̓ǂݍ��݂Ɏ��s", filename, MB_OK);
		return E_FAIL;
	}
	if( (riffckInfo.ckid != mmioFOURCC('R', 'I', 'F', 'F')) || (riffckInfo.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
	{
		MessageBox(NULL, "�����WAV�t�@�C���ł͂���܂���", filename, MB_OK);
		return E_FAIL;
	}
	ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if(MMSYSERR_NOERROR != mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK))
	{
		MessageBox(NULL, "�`�����N�̓ǂݍ��݂Ɏ��s", filename, MB_OK);
		return E_FAIL;
	}
	mmioRead(hMmio, (HPSTR) &pcmWaveFormat, sizeof(pcmWaveFormat));
	//���j�APCM�ŁA���A�}���`�`�����l��WAV�͑z��O
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        pwfex = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
        if( NULL == pwfex )
		{
			MessageBox(NULL, "���s", NULL, MB_OK);
			return E_FAIL;
		}
        memcpy( pwfex, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        pwfex->cbSize = 0;
    }
    else
	{
		MessageBox(0,"�{�T���v���͕W���̃��j�APCM�t�H�[�}�b�g��z�肵�Ă��܂�","",MB_OK);
		return E_FAIL;
	}
	if(MMSYSERR_NOERROR !=mmioAscend(hMmio, &ckInfo, NULL))
	{
		delete pwfex;
		MessageBox(NULL, "���s", NULL, MB_OK);
		return E_FAIL;
	}
	// wave�t�@�C�����̉��f�[�^��ǂݍ���
	ckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if(MMSYSERR_NOERROR != mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK))
	{
		delete pwfex;
		MessageBox(NULL, "mmio�ǂݍ��ݎ��s", NULL, MB_OK);
		return E_FAIL;
	}
	dwWavSize = ckInfo.cksize;
	// �T�E���h�o�b�t�@�[�̍쐬
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = dwWavSize;
	dsbd.guid3DAlgorithm = DS3DALG_DEFAULT;
	dsbd.lpwfxFormat = pwfex;
	if(FAILED(pSound->CreateSoundBuffer(&dsbd, ppWavBuffer, NULL)))
	{
		delete pwfex;
		MessageBox(NULL, "�T�E���h�o�b�t�@�[�̍쐬�Ɏ��s", filename, MB_OK);
		return E_FAIL;
	}
	delete pwfex;
	// �g�`�f�[�^���������ނ��߂Ƀo�b�t�@�����b�N����
	VOID* pBuffer = NULL;
	DWORD dwBufferSize = 0;
	(*ppWavBuffer)->Lock(0, dwWavSize, &pBuffer, &dwBufferSize, NULL, NULL, 0);
	// �o�b�t�@�ɔg�`�f�[�^����������
	FILE* fp=fopen(filename, "rb");
	if(fp == NULL)
	{
		MessageBox(NULL, "�t�@�C�������݂��܂���", filename, MB_OK);
	}
	fseek(fp, riffckInfo.dwDataOffset+sizeof(FOURCC), SEEK_SET);
	BYTE* pWavData = new BYTE[dwBufferSize];
	fread(pWavData, 1, dwBufferSize, fp);
	for(DWORD i = 0; i<dwBufferSize; i++)
	{
		*((BYTE*)pBuffer+i) = *((BYTE*)pWavData+i);
	}
	fclose(fp);
	delete pWavData;
	// �������݂��I�����ăo�b�t�@���A�����b�N����
	(*ppWavBuffer)->Unlock(pBuffer, dwBufferSize, NULL, 0);
	return S_OK;
}

HRESULT CGame::CreateSoundObject() {
	CreateSoundBuffer("decide.wav", &pWavDecide);
	CreateSoundBuffer("cancel.wav", &pWavCancel);
	CreateSoundBuffer("ocean.wav", &pWavOcean);
	CreateSoundBuffer("sonar.wav", &pWavSonar);
	CreateSoundBuffer("salvage.wav", &pWavSalvage);
	return S_OK;
}
