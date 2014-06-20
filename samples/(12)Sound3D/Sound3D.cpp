// Sound3D.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "Sound3D.h"
#include <crtdbg.h>

#include <easy3d.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define MAX_LOADSTRING 100

#define	DEG2PAI	0.01745329251994f


// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��
char szMediaDir[MAX_PATH];
int scid;
int retfps;
int soundid0, soundid1, soundid2;
float volume = 1.0f;
int counter;
int fontid1;
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
static int KeyIn();
static int Move3DSound();


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
	LoadString(hInstance, IDC_SOUND3D, szWindowClass, MAX_LOADSTRING);
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


	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SOUND3D));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SOUND3D));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SOUND3D);
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

	D3DXVECTOR3 campos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	ret = E3DSetCameraPos( campos );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	D3DXVECTOR3 camtar( 0.0f, 0.0f, 100.0f );
	D3DXVECTOR3 camup( 0.0f, 1.0f, 0.0f );
	ret = E3DSetCameraTarget( camtar, camup );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DSetProjection( 1000.0f, 30000.0f, 60.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	//���{��t�H���g
	ret = E3DCreateFont( 20, 0, 400, 0, 0, 0, "�l�r �S�V�b�N", &fontid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	char filename[MAX_PATH];
	sprintf_s( filename, MAX_PATH, "%s\\test.wav", szMediaDir );
	ret = E3DLoadSound( filename, 0, 1, 5, &soundid0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	sprintf_s( filename, MAX_PATH, "%s\\sbounce.wav", szMediaDir );
	ret = E3DLoadSound( filename, 1, 1, 5, &soundid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
//ver5.3.0.3�Œǉ��B���̓͂��������w��I�I�I�I�I�I�I�I�I�I�I�I
	ret = E3DSet3DSoundEmiDist( soundid1, 15.0f );
	_ASSERT( !ret );


	sprintf_s( filename, MAX_PATH, "%s\\score.wav", szMediaDir );
	ret = E3DLoadSound( filename, 1, 1, 5, &soundid2 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
//ver5.3.0.3�Œǉ��B���̓͂��������w��I�I�I�I�I�I�I�I�I�I�I�I
	ret = E3DSet3DSoundEmiDist( soundid2, 15.0f );
	_ASSERT( !ret );

//XAUDIO2_LOOP_INFINITE
	ret = E3DPlaySound( soundid0, 0, 0, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
//ver5.3.0.4�Œǉ��B�t�F�[�h���ʁBplay������ŌĂԁB
	ret = E3DFadeSound( soundid0, 10000, 1.0, 0.0 );
	_ASSERT( !ret );


	ret = E3DPlaySound( soundid1, 0, 0, 255 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DPlaySound( soundid2, 0, 0, 255 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	volume = 0.0f;
	counter = 0;

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

	ret = KeyIn();
	_ASSERT( !ret );

	ret = Move3DSound();
	_ASSERT( !ret );

	ret = E3DUpdateSound();
	_ASSERT( !ret );

	ret = E3DBeginScene( scid, 0 );
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

int Move3DSound()
{
	//�Q�̂RD�T�E���h���݂��ɂP�W�O�x�̈ʑ����������Ȃ���~������邮����B
	int ret;
	counter++;

	D3DXVECTOR3 pos, v;

	float rad = (float)counter * 0.5f * DEG2PAI;
	pos.x = (float)sin( rad ) * 5.0f;
	pos.y = 0.0f;
	pos.z = (float)cos( rad ) * 5.0f;

	float vrad = ((float)counter * 0.5f + 3.0f) * DEG2PAI;
	v.x = (float)sin( vrad ) * 5.0f;
	v.y = 0.0f;
	v.z = (float)cos( vrad ) * 5.0f;

	ret = E3DSet3DSoundMovement( soundid1, pos, v );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

;///////////
	D3DXVECTOR3 pos2, v2;

	float rad2 = ((float)counter * 0.5f + 180.0f ) * DEG2PAI;
	pos2.x = (float)sin( rad2 ) * 5.0f;
	pos2.y = 0.0f;
	pos2.z = (float)cos( rad2 ) * 5.0f;

	float vrad2 = ((float)counter * 0.5f + 180.0f + 3.0f) * DEG2PAI;
	v2.x = (float)sin( vrad2 ) * 5.0f;
	v2.y = 0.0f;
	v2.z = (float)cos( vrad2 ) * 5.0f;

	ret = E3DSet3DSoundMovement( soundid2, pos2, v2 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

;///////////
	ret = E3DSet3DSoundListenerMovement( -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	

	return 0;
}

int KeyIn()
{
	int ret;
	// ���ʂ̕ύX
	int volumeflag = 0;
	if( keybuf['O'] != 0 ) {
		volume += 0.1f;
		if( volume > 100.0f ){
			volume = 100.0f;//�����͏o���Ȃ��B
		}
		volumeflag = 1;		
	}
	if( keybuf['P'] != 0 ) {
		volume -= 0.1f;
		if( volume < 0.0f ){
			volume = 0.0f;
		}
		volumeflag = 1;
	}
	if( volumeflag == 1 ){
		ret = E3DSetSoundVolume( volume, -1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}
	
	return 0;
}

int DrawText()
{
	int ret;
	char mes[1024];
	sprintf_s( mes, 1024, "fps : %d", retfps );

	D3DXVECTOR2 texpos;
	texpos.x  = 10; texpos.y = 10;
	E3DCOLOR4UC texcol;
	texcol.a = 255, texcol.r = 255; texcol.g = 255; texcol.b = 255;
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	POINT tpos = { 10, 40 };
	strcpy_s( mes, 1024, "3D�T�E���h��̊�����ɂ́A�Q�ȏ�̃X�s�[�J�[���K�v�ł��B\n�w�b�h�t�H���ł��n�j�ł��B" );
	ret = E3DDrawTextByFontID( scid, fontid1, tpos, mes, texcol );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 100;
	sprintf_s( mes, 1024, "volume %f", volume );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 20;
	strcpy_s( mes, 1024, " volume UP <--- Push O key    volume DOWN <--- Push P key" );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

