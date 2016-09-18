#include "Game.h"

HRESULT CGame::InitDSound(HWND hWnd) {
	// DirectSoundの初期化
	if(FAILED(DirectSoundCreate8(NULL, &pSound, NULL)))
	{
		return E_FAIL;
	}
	pSound->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE);

	// リソースの作成
	CreateSoundObject();
	pWavOcean->Play(0, 0, DSBPLAY_LOOPING);

	return S_OK;
}

HRESULT CGame::CreateSoundBuffer(LPSTR filename, LPDIRECTSOUNDBUFFER *ppWavBuffer) {
	// wavファイルを開いていく
	HMMIO hMmio = NULL;
	DWORD dwWavSize = 0;
	WAVEFORMATEX* pwfex;
	MMCKINFO ckInfo;
	MMCKINFO riffckInfo;
	PCMWAVEFORMAT pcmWaveFormat;
	hMmio = mmioOpen(filename, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if(MMSYSERR_NOERROR !=mmioDescend(hMmio, &riffckInfo, NULL, NULL))
	{
		MessageBox(NULL, "チャンクの読み込みに失敗", filename, MB_OK);
		return E_FAIL;
	}
	if( (riffckInfo.ckid != mmioFOURCC('R', 'I', 'F', 'F')) || (riffckInfo.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
	{
		MessageBox(NULL, "これはWAVファイルではありません", filename, MB_OK);
		return E_FAIL;
	}
	ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if(MMSYSERR_NOERROR != mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK))
	{
		MessageBox(NULL, "チャンクの読み込みに失敗", filename, MB_OK);
		return E_FAIL;
	}
	mmioRead(hMmio, (HPSTR) &pcmWaveFormat, sizeof(pcmWaveFormat));
	//リニアPCMで、かつ、マルチチャンネルWAVは想定外
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        pwfex = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
        if( NULL == pwfex )
		{
			MessageBox(NULL, "失敗", NULL, MB_OK);
			return E_FAIL;
		}
        memcpy( pwfex, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        pwfex->cbSize = 0;
    }
    else
	{
		MessageBox(0,"本サンプルは標準のリニアPCMフォーマットを想定しています","",MB_OK);
		return E_FAIL;
	}
	if(MMSYSERR_NOERROR !=mmioAscend(hMmio, &ckInfo, NULL))
	{
		delete pwfex;
		MessageBox(NULL, "失敗", NULL, MB_OK);
		return E_FAIL;
	}
	// waveファイル内の音データを読み込む
	ckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if(MMSYSERR_NOERROR != mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK))
	{
		delete pwfex;
		MessageBox(NULL, "mmio読み込み失敗", NULL, MB_OK);
		return E_FAIL;
	}
	dwWavSize = ckInfo.cksize;
	// サウンドバッファーの作成
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
		MessageBox(NULL, "サウンドバッファーの作成に失敗", filename, MB_OK);
		return E_FAIL;
	}
	delete pwfex;
	// 波形データを書き込むためにバッファをロックする
	VOID* pBuffer = NULL;
	DWORD dwBufferSize = 0;
	(*ppWavBuffer)->Lock(0, dwWavSize, &pBuffer, &dwBufferSize, NULL, NULL, 0);
	// バッファに波形データを書き込む
	FILE* fp=fopen(filename, "rb");
	if(fp == NULL)
	{
		MessageBox(NULL, "ファイルが存在しません", filename, MB_OK);
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
	// 書き込みを終了してバッファをアンロックする
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
