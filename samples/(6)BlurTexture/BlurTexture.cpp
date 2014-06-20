// BlurTexture.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "BlurTexture.h"
#include <crtdbg.h>

#include <easy3d.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LOADSTRING 100

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

int scid_org, texid_org;
int scid_small, texid_small;
int scid_work, texid_work;
int scid_blur, texid_blur;
int scid_final, texid_final;

int spid1, spid2, spid3;


// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static int SetMediaDir();
static int OneTimeSceneInit();
static int Render3DEnvironment();
static int DrawText();

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
	LoadString(hInstance, IDC_BLURTEXTURE, szWindowClass, MAX_LOADSTRING);
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


	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BLURTEXTURE));

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


    return (int)msg.wParam;
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BLURTEXTURE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_BLURTEXTURE);
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

//   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 550, 600, NULL, NULL, hInstance, NULL);


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
	E3DCOLOR4UC clearcol = { 0, 0, 0, 0 };
	ret = E3DSetClearCol( scid, clearcol );
	_ASSERT( !ret );

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
	sprintf_s( signame, MAX_PATH, "%s\\poo\\poo.sig", szMediaDir );
	ret = E3DSigLoad( signame, 0, 1.0f, &hsid1 ); 
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	char motname[MAX_PATH];
	sprintf_s( motname, MAX_PATH, "%s\\poo\\idling.qua", szMediaDir );
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
	E3DCOLOR4UC col= {255, 200, 200, 200 };
	ret = E3DSetDirectionalLight( lid1, ldir, col );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	int okflag;
	SIZE largesize = { 256, 256 };
	SIZE smallsize = { 64, 64 };
	ret = E3DCreateRenderTargetTexture( largesize, D3DFMT_X8R8G8B8, &scid_org, &texid_org, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}
	E3DCOLOR4UC clearcol = { 0, 0, 0, 0 };
	ret = E3DSetClearCol( scid_org, clearcol );
	_ASSERT( !ret );

	ret = E3DCreateRenderTargetTexture( smallsize, D3DFMT_A8R8G8B8, &scid_small, &texid_small, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DCreateRenderTargetTexture( smallsize, D3DFMT_A8R8G8B8, &scid_work, &texid_work, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DCreateRenderTargetTexture( smallsize, D3DFMT_A8R8G8B8, &scid_blur, &texid_blur, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DCreateRenderTargetTexture( largesize, D3DFMT_A8R8G8B8, &scid_final, &texid_final, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}

	char spname[MAX_PATH];
	sprintf_s( spname, MAX_PATH, "%s\\dummy256.png", szMediaDir );

	ret = E3DCreateSprite( spname, 0, 0, &spid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetTextureToDispObj( -2, spid1, texid_org );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DCreateSprite( spname, 0, 0, &spid2 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetTextureToDispObj( -2, spid2, texid_blur );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DCreateSprite( spname, 0, 0, &spid3 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetTextureToDispObj( -2, spid3, texid_final );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


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
	int curframe;
	ret = E3DSetNewPose( hsid1, &curframe );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//�e�N�X�`���ɃV�[���������_�����O
	ret = E3DBeginScene( scid_org, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	int status;
	ret = E3DChkInView( scid_org, hsid1, &status );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DRender( scid_org, hsid1, 0, 1, 0, 0 );
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


	//�T�C�Y�̏������e�N�X�`���Ƀ����_�����O
	E3DCOLOR4F col1 = { 1.0f, 1.0f, 1.0f, 1.0f };
	ret = E3DBlendRTTexture( scid_small, texid_org, col1, D3DTEXF_LINEAR, 
		-1, col1, D3DTEXF_LINEAR, 
		D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ONE, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	//�ڂ�������
//	ret = E3DBlurTexture16Box( scid_blur, texid_small );
//	ret = E3DBlurTextureGauss5x5( scid_blur, texid_small, 10.0 );
	ret = E3DBlurTextureGauss( scid_blur, scid_work, texid_work, texid_small, 20.0f, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	//�ڂ����ƌ��摜���A�b�h���[�h�Ńu�����h
	E3DCOLOR4F col2 = { 1.0f, 1.0f, 1.0f, 1.0f };
	E3DCOLOR4F col3 = { 1.0f, 1.0f, 1.0f, 1.0f };
	ret = E3DBlendRTTexture( scid_final, texid_org, col2, D3DTEXF_POINT,
		texid_blur, col3, D3DTEXF_LINEAR,
		D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ONE, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



	//���C����ʂɕ\��
	ret = E3DBeginScene( scid, 0 );
	_ASSERT( !ret );
	ret = E3DBeginSprite();
	_ASSERT( !ret );
	
	//���摜��\��
	float scalex = 1.0f;
	float scaley = 1.0f;
	D3DXVECTOR3 tra = D3DXVECTOR3( 0.0f, 0.0f, 0.1f );
	ret = E3DRenderSprite( spid1, scalex, scaley, tra );
	_ASSERT( !ret );

	//�ڂ����摜��\��
	tra.x = 256.0f;
	tra.y = 0.0f;
	ret = E3DRenderSprite( spid2, scalex, scaley, tra );
	_ASSERT( !ret );

	//�u�����h�摜��\��
	tra.x = 0.0f;
	tra.y = 256.0f;
	ret = E3DRenderSprite( spid3, scalex, scaley, tra );
	_ASSERT( !ret );

	ret = E3DEndSprite();
	_ASSERT( !ret );
	ret = E3DEndScene();
	_ASSERT( !ret );

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

	return 0;
}

