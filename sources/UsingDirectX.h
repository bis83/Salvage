#ifndef USINGDIREXTX_H
#define USINGDIRECTX_H

#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <dinput.h>
#include <dsound.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dsound.lib")

#pragma comment(lib, "winmm.lib")

#include <vector>
using namespace std;

#define SAFE_RELEASE(p) { if(p!=NULL) { p->Release(); p=NULL; } }
#define SAFE_DELETE(p) { if(p!=NULL) { delete p; p=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p!=NULL) { delete[] p; p=NULL; } }

#endif
