// CameraOffset.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

// �J�����A�j�����L�����N�^�[�̈ʒu�A��������J�n���邽�߂̃T���v��
// �����ɂ��Ă̓}�j���A����e3d_CameraOffset.htm���������������B

//���L�[�ŃL�����N�^�[�̈ړ��Ɖ�]
//Z�{���E�L�[�ŃJ������]
//X+���E�L�[�ŃJ�������s�ړ�

//�P�L�[�ŃJ�������L�����N�^�[�̈ʒu�Ɉړ����A�A�j�����Đ������B


#include "stdafx.h"
#include "CameraOffset.h"
#include <crtdbg.h>

#include <easy3d.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LOADSTRING 100

enum {
	CAMERA_NORMAL,
	CAMERA_SETUP,
	CAMERA_ANIM,
	CAMERA_MAX
};

char strcamerastate[CAMERA_MAX][20] = {
	"CAMERA_NORMAL",
	"CAMERA_SETUP",
	"CAMERA_ANIM"
};

#define SETUPTIME	100

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��
char szMediaDir[MAX_PATH];
int scid;
int hsid1;
int motid1;
int maxframe1;
int retfps;
int lid1;

int camanim0, camanim1;
int camerastate;
int cameraframe;

D3DXVECTOR3 camoffbef;
D3DXVECTOR3 camoffaft;
int charaqid, invcharaqid, camqid;
D3DXVECTOR3 aftdiff;

D3DXVECTOR3 pos1;

int keybuf[256];

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
static int SetMediaDir();
static int OneTimeSceneInit();
static int Render3DEnvironment();
static int DrawText();
static int MoveChara();
static int MoveCamera();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �����ɃR�[�h��}�����Ă��������B
	MSG msg;
	HACCEL hAccelTable;

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CAMERAOFFSET, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	int ret;
	ret = OneTimeSceneInit();
	if( ret ){
		_ASSERT( 0 );
		return FALSE;
	}


	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CAMERAOFFSET));

	// Now we're ready to recieve and process Windows messages.
    BOOL bGotMsg;
    //MSG  msg;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

    while( WM_QUIT != msg.message  )
    {
        // Use PeekMessage() if the app is active, so we can use idle time to
        // render the scene. Else, use GetMessage() to avoid eating CPU time.
       // if( m_bActive )
        bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
        //else
          //  bGotMsg = GetMessage( &msg, NULL, 0U, 0U );

        if( bGotMsg )
        {
            // Translate and dispatch the message
            if( 0 == TranslateAccelerator(msg.hwnd, hAccelTable, &msg) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }else{
			if( Render3DEnvironment() != 0 ){
                SendMessage( msg.hwnd, WM_CLOSE, 0, 0 );
            }
        }
    }

	E3DBye();

	return (int) msg.wParam;
}



//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
//  �R�����g:
//
//    ���̊֐�����юg�����́A'RegisterClassEx' �֐����ǉ����ꂽ
//    Windows 95 ���O�� Win32 �V�X�e���ƌ݊�������ꍇ�ɂ̂ݕK�v�ł��B
//    �A�v���P�[�V�������A�֘A�t����ꂽ
//    �������`���̏������A�C�R�����擾�ł���悤�ɂ���ɂ́A
//    ���̊֐����Ăяo���Ă��������B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CAMERAOFFSET));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CAMERAOFFSET);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 640, 480, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

	int ret;
	E3DEnableDbgFile();
	ret = SetMediaDir();
	if( ret ){
		_ASSERT( 0 );
		return FALSE;
	}
	ret = E3DInit( hInst, hWnd, 0, 16, 0, 1, 1, 1, 0, &scid );
	if( ret ){
		_ASSERT( 0 );
		return FALSE;
	}

   return TRUE;
}

int OneTimeSceneInit()
{
	int ret;

	D3DXVECTOR3 campos = D3DXVECTOR3( 0.0f, 800.0f, -2000.0 );
	ret = E3DSetCameraPos( campos );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetCameraDeg( 180.0f, 0.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetProjection( 1000.0f, 30000.0f, 60.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	char signame[MAX_PATH];
	sprintf_s( signame, MAX_PATH, "%s\\my.sig", szMediaDir );
	ret = E3DSigLoad( signame, 0, 1.0f, &hsid1 ); 
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	pos1 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	ret = E3DSetPos( hsid1, pos1 );
	_ASSERT( !ret );


	char motname[MAX_PATH];
	sprintf_s( motname, MAX_PATH, "%s\\�A�C�h������.qua", szMediaDir );
	ret = E3DAddMotion( hsid1, motname, 1.0f, &motid1, &maxframe1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetMotionKind( hsid1, motid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = E3DCreateLight( &lid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	D3DXVECTOR3 ldir( 0.0f, 0.0f, 1.0f );
	E3DCOLOR4UC col= {255, 255, 255, 255 };
	ret = E3DSetDirectionalLight( lid1, ldir, col );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = E3DCreateQ( &charaqid );
	_ASSERT( !ret );
	ret = E3DCreateQ( &invcharaqid );
	_ASSERT( !ret );
	ret = E3DCreateQ( &camqid );
	_ASSERT( !ret );
	ret = E3DInitQ( charaqid );
	_ASSERT( !ret );
	ret = E3DInitQ( invcharaqid );
	_ASSERT( !ret );
	ret = E3DInitQ( camqid );
	_ASSERT( !ret );


	camoffbef = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	camoffaft = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	aftdiff = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	ret = E3DSetCameraOffset( camoffbef, camqid, camoffaft );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	char camname0[MAX_PATH];
	sprintf_s( camname0, MAX_PATH, "%s\\dirZ2.cam", szMediaDir );
	ret = E3DLoadCameraFile( camname0, &camanim0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	char camname1[MAX_PATH];
	sprintf_s( camname1, MAX_PATH, "%s\\movetest2.cam", szMediaDir );
	ret = E3DLoadCameraFile( camname1, &camanim1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DSetCameraAnimNo( camanim0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	camerastate = CAMERA_NORMAL;
	cameraframe = 0;

	return 0;

}


//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �`��R�[�h�������ɒǉ����Ă�������...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���ł��B
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

int SetMediaDir()
{
	char filename[1024];
	char* endptr = 0;

	ZeroMemory( filename, 1024 );
	int ret;

	ret = GetModuleFileName( NULL, filename, 1024 );
	if( ret == 0 ){
		_ASSERT( 0 );
		E3DDbgOut( "SetMediaDir : GetModuleFileName error !!!\n" );
		return 1;
	}

	int ch = '\\';
	char* lasten = 0;
	lasten = strrchr( filename, ch );
	if( !lasten ){
		_ASSERT( 0 );
		E3DDbgOut( "SetMediaDir : strrchr error !!!\n" );
		return 1;
	}

	*lasten = 0;

	char* last2en = 0;
	char* last3en = 0;
	last2en = strrchr( filename, ch );
	if( last2en ){
		*last2en = 0;
		last3en = strrchr( filename, ch );
		if( last3en ){
			if( (strcmp( last2en + 1, "debug" ) == 0) ||
				(strcmp( last2en + 1, "Debug" ) == 0) ||
				(strcmp( last2en + 1, "DEBUG" ) == 0) ||
				(strcmp( last2en + 1, "release" ) == 0) ||
				(strcmp( last2en + 1, "Release" ) == 0) ||
				(strcmp( last2en + 1, "RELEASE" ) == 0)
				){

				endptr = last2en;
			}else{
				endptr = lasten;
			}
		}else{
			endptr = lasten;
		}
	}else{
		endptr = lasten;
	}

	*lasten = '\\';
	if( last2en )
		*last2en = '\\';
	if( last3en )
		*last3en = '\\';

	int leng;
	ZeroMemory( szMediaDir, MAX_PATH );
	leng = (int)( endptr - filename + 1 );
	strncpy_s( szMediaDir, MAX_PATH, filename, leng );
	strcat_s( szMediaDir, MAX_PATH, "Media\\" );

	E3DDbgOut( "SetMediaDir : %s\n", szMediaDir );

	return 0;
}

int Render3DEnvironment()
{
	int ret;
	ret = E3DGetKeyboardCnt( keybuf );
	_ASSERT( !ret );

	ret = MoveChara();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = MoveCamera();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DBeginScene( scid, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

		int status;
		ret = E3DChkInView( scid, hsid1, &status );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = E3DRender( scid, hsid1, 0, 1, 0, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = E3DRender( scid, hsid1, 1, 1, 1, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = DrawText();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		
	ret = E3DEndScene();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DPresent( scid );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DWaitbyFPS( 60, &retfps );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int DrawText()
{
	int ret;
	char mes[1024];
	sprintf_s( mes, 1024, "fps : %d", retfps );

	D3DXVECTOR2 texpos;
	texpos.x  = 10; texpos.y = 100;
	E3DCOLOR4UC texcol;
	texcol.a = 255, texcol.r = 255; texcol.g = 255; texcol.b = 255;
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 20;
	sprintf_s( mes, 1024, "camerastate : %s", strcamerastate[camerastate] );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 20;
	sprintf_s( mes, 1024, "pos1 : %f, %f, %f", pos1.x, pos1.y, pos1.z );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int MoveChara()
{
	int ret;
	if( (keybuf['X'] == 0) && (keybuf['Z'] == 0) ){
		if( keybuf[VK_LEFT] != 0 ){
			ret = E3DRotateQY( charaqid, 2.0f );
			_ASSERT( !ret );
			ret = E3DSetDirQ2( hsid1, charaqid );
			_ASSERT( !ret );
		}
		if( keybuf[VK_RIGHT] != 0 ){
			ret = E3DRotateQY( charaqid, -2.0f );
			_ASSERT( !ret );
			ret = E3DSetDirQ2( hsid1, charaqid );
			_ASSERT( !ret );
		}
		if( keybuf[VK_UP] != 0 ){
			ret = E3DPosForward( hsid1, 50.0f );
			_ASSERT( !ret );
		}
		if( keybuf[VK_DOWN] != 0 ){
			ret = E3DPosForward( hsid1, -50.0f );
			_ASSERT( !ret );
		}
	}
	int frameno1;
	ret = E3DSetNewPose( hsid1, &frameno1 );
	_ASSERT( !ret );

	ret = E3DGetPos( hsid1, &pos1 );
	_ASSERT( !ret );

	return 0;
}

int MoveCamera()
{
	int ret;
	if( camerastate == CAMERA_NORMAL ){
		if( (keybuf['Z'] != 0) && (keybuf[VK_LEFT] != 0) ){
			ret = E3DRotateQY( camqid, 2.0f );
			_ASSERT( !ret );
		}
		if( (keybuf['Z'] != 0) && (keybuf[VK_RIGHT] != 0) ){
			ret = E3DRotateQY( camqid, -2.0f );
			_ASSERT( !ret );
		}
		if( (keybuf['X'] != 0) && (keybuf[VK_LEFT] != 0) ){
			camoffaft.x += 10.0f;
		}
		if( (keybuf['X'] != 0) && (keybuf[VK_RIGHT] != 0) ){
			camoffaft.x -= 10.0;
		}
		if( keybuf['1'] == 1 ){
			camerastate = CAMERA_SETUP;
			cameraframe = 0;
		}
	}



	if( camerastate == CAMERA_SETUP ){
		if( cameraframe == 0 ){
			ret = E3DInvQ( charaqid, invcharaqid );
			_ASSERT( !ret );

			aftdiff = ((pos1 - pos1) - camoffaft) / (float)SETUPTIME;
		}
		camoffbef = -pos1;
		camoffaft += aftdiff;

		float t;
		t = 1.0f / (float)SETUPTIME * (float)cameraframe;
		ret = E3DSlerpQ( camqid, invcharaqid, t, camqid );
		_ASSERT( !ret );
		cameraframe++;

		if( cameraframe >= SETUPTIME ){
			camerastate = CAMERA_ANIM;
			cameraframe = 0;
			ret = E3DSetCameraAnimNo( camanim1 );
			_ASSERT( !ret );
		}
		
	}
	if( camerastate == CAMERA_ANIM ){

		cameraframe++;

		if( cameraframe >= 240 ){
			camerastate = CAMERA_NORMAL;
			cameraframe = 0;
			ret = E3DSetCameraAnimNo( camanim0 );
			_ASSERT( !ret );
		}
	}

	ret = E3DSetCameraOffset( camoffbef, camqid, camoffaft );
	_ASSERT( !ret );
	ret = E3DSetNewCamera( -1 );
	_ASSERT( !ret );

	return 0;
}


