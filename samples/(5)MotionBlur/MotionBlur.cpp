// MotionBlur.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "MotionBlur.h"

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

float proj_far = 50000.0f;
int scid = 0;
int hsid0 = 0;//�n��
int hsid1 = 0;//�L�����N�^�[
D3DXVECTOR3 curpos;//�L�����N�^�[�̌��݂̈ʒu
D3DXVECTOR3 befpos;//�L�����N�^�[�̈��O�̈ʒu
int motid1 = 0;//���[�V����ID

#define BBNUM	50//�r���{�[�h��
int bbid[ BBNUM ];//�r���{�[�hID

int retfps;
int lid1;
int keybuf[256];

//���[�V�����u���[�p�̕ϐ�
int blurdisp[3];
int rendercnt = 0;


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
static int ChkConf();

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
	LoadString(hInstance, IDC_MOTIONBLUR, szWindowClass, MAX_LOADSTRING);
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


	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOTIONBLUR));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOTIONBLUR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MOTIONBLUR);
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
      CW_USEDEFAULT, 0, 640, 520, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

	int ret;
	ret = E3DEnableDbgFile();
	if( ret ){
		_ASSERT( 0 );
		return FALSE;
	}
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
	D3DXVECTOR3 campos = D3DXVECTOR3( 53898.0f, 3000.0f, 44982.0f - 2500.0f );
	ret = E3DSetCameraPos( campos );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	D3DXVECTOR3 camtar = D3DXVECTOR3( 53898.0f, 2500.0f, 44982.0f );
	D3DXVECTOR3 camup = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	ret = E3DSetCameraTarget( camtar, camup );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetProjection( 1000.0f, proj_far, 60.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//�n�ʃf�[�^�̃��[�h
	char name1[MAX_PATH];
	sprintf_s( name1, MAX_PATH, "%s\\g_6_20_1.bmp", szMediaDir );
	char name2[MAX_PATH];
	sprintf_s( name2, MAX_PATH, "%s\\8.bmp", szMediaDir );
	char name3[MAX_PATH];
	sprintf_s( name3, MAX_PATH, "%s\\g_river_6_20_1.bmp", szMediaDir );
	char name4[MAX_PATH];
	sprintf_s( name4, MAX_PATH, "%s\\BG43.bmp", szMediaDir );
	float mapsize = 120000.0f;
	int mapdiv = 50;
	float mapheight = 5500.0f;
	ret = E3DLoadGroundBMP( name1, name2, name3, name4, mapsize, mapsize, mapdiv, mapdiv, mapheight, &hsid0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	//�������l�̃��[�h
	char signame[MAX_PATH];
	sprintf_s( signame, MAX_PATH, "%s\\morob_6_26_r3.sig", szMediaDir );
	ret = E3DSigLoad( signame, 0, 1.0f, &hsid1 ); 
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	char quaname[MAX_PATH];
	sprintf_s( quaname, MAX_PATH, "%s\\morob_6_26_r3.qua", szMediaDir );
	int maxframe;
	ret = E3DAddMotion( hsid1, quaname, 1.0f, &motid1, &maxframe );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetMotionKind( hsid1, motid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	curpos = D3DXVECTOR3( 53898.0f, 3000.0f, 44982.0f );
	befpos = curpos;
	ret = E3DSetPos( hsid1, curpos );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	D3DXVECTOR3 deg = D3DXVECTOR3( 0.0f, 120.0f, 0.0f );
	ret = E3DSetDir( hsid1, deg );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetBeforePos( hsid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//�؁i�r���{�[�h�j�̓ǂݍ��݂Ɛݒu
	
		// PosOnGround ���߂��g�����߂ɁAChkInView���A���炩���߁A�Ă�ł����K�v������܂��B
	int state;
	ret = E3DChkInView( scid, hsid0, &state );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	srand( 321 );
	int bbcnt;
	for( bbcnt = 0; bbcnt < BBNUM; bbcnt++ ){
		int treekind;
		char treename[MAX_PATH];

		treekind = rand() % 3;
		if( treekind == 0 ){
			sprintf_s( treename, MAX_PATH, "%s\\tree01S.tga", szMediaDir );
		}else if( treekind == 1 ){
			sprintf_s( treename, MAX_PATH, "%s\\tree02S.tga", szMediaDir );
		}else{
			sprintf_s( treename, MAX_PATH, "%s\\tree35S.tga", szMediaDir );
		}
		ret = E3DCreateBillboard( treename, 2000.0f, 3000.0f, 0, 0, 0, bbid + bbcnt );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		int divsize;
		divsize = (int)mapsize / 4;
		int randx, randz;
		randx = ( rand() % divsize ) * 4;
		randz = ( rand() % divsize ) * 4;

		ret = E3DSetBillboardOnGround( bbid[bbcnt], hsid0, mapheight, -100.0f, (float)randx, (float)randz );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	
	//���C�g
	ret = E3DCreateLight( &lid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	D3DXVECTOR3 ldir( 0.0f, -1.0f, 2.0f );
	E3DCOLOR4UC col= {255, 255, 255, 255 };
	ret = E3DSetDirectionalLight( lid1, ldir, col );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
   
	//�t�H�O
	E3DCOLOR4UC fogcol = { 255, 0, 0, 255 };
	ret = E3DSetLinearFogParams( 1, fogcol, 1000.0f, proj_far, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	//���[�V�����u���[�̏���
		//�R�Ԗڂ̈����ŉ��t���[���O�̃f�[�^�Ńu���[����邩���w��
	ret = E3DSetMotionBlur( hsid0, BLUR_CAMERA, 15 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetMotionBlur( hsid1, BLUR_CAMERA, 5 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetMotionBlur( -1, BLUR_CAMERA, 3 );//-1�̓r���{�[�h
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	blurdisp[0] = hsid0;
	blurdisp[1] = hsid1;
	blurdisp[2] = -1;

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
	E3DGetKeyboardCnt( keybuf );

	ret = MoveChara();
	_ASSERT( !ret );
	ret = MoveCamera();
	_ASSERT( !ret );

	int frameno;
	ret = E3DSetNewPose( hsid1, &frameno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DBeginScene( scid, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
		
		//������`�F�b�N
		int status;
		ret = E3DChkInView( scid, hsid0, &status );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = E3DChkInView( scid, hsid1, &status );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		//�����蔻��( ChkInView����ŌĂ� )
		ret = ChkConf();
		_ASSERT( !ret );


		//�s���������������_�[
		ret = E3DRender( scid, hsid0, 0, 1, 0, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = E3DRender( scid, hsid1, 0, 1, 0, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		//�����������������_�[
		ret = E3DRender( scid, hsid0, 1, 1, 1, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = E3DRender( scid, hsid1, 1, 1, 1, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = E3DRenderBillboard( scid, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		//���[�V�����u���[�̕`��
		if( rendercnt == 0 ){
			//E3DSetMotinBlur�����Ă��珉��̃����_�[���ɂ͕K���K�v�B
			//Set�ł͂Ȃ���Init�ł���Ƃ���ɒ��ӁB
			ret = E3DInitBeforeBlur( hsid0 );
			_ASSERT( !ret );
			ret = E3DInitBeforeBlur( hsid1 );
			_ASSERT( !ret );
			ret = E3DInitBeforeBlur( -1 );
			_ASSERT( !ret );
		}
		ret = E3DRenderMotionBlur( scid, blurdisp, 3 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		rendercnt++;

		//���t���[���Ăяo��
		E3DSetBeforeBlur( hsid0 );
		E3DSetBeforeBlur( hsid1 );
		E3DSetBeforeBlur( -1 );

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

	//�ʒu�̋L�^
	ret = E3DGetPos( hsid1, &curpos );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetBeforePos( hsid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	befpos = curpos;

	return 0;
}





int DrawText()
{
	int ret;
	char mes[1024];
	sprintf_s( mes, 1024, "fps : %d", retfps );

	D3DXVECTOR2 texpos;
	texpos.x  = 10; texpos.y = 0;
	E3DCOLOR4UC texcol;
	texcol.a = 255, texcol.r = 255; texcol.g = 255; texcol.b = 255;
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 20;	
	strcpy_s( mes, 1024, "Up, Down, Left, Right key --> Chara Move" );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 20;	
	strcpy_s( mes, 1024, "Shift + Up, Down, Left, Right key --> Camera Move" );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int MoveChara()
{
	float movestep = 100.0f;
	float degstep = 5.0f;

	if( keybuf[VK_SHIFT] == 0 ){
		//�V�t�g�L�[�������Ă��Ȃ��Ƃ��L�����N�^�[�𓮂���
		//�V�t�g�L�[�������Ă���Ƃ��̓J�����𓮂���(MoveCamera)
		if( keybuf[VK_UP] != 0 ){
			E3DPosForward( hsid1, movestep );
		}
		if( keybuf[VK_DOWN] != 0 ){
			E3DPosForward( hsid1, -movestep );
		}
		if( keybuf[VK_LEFT] != 0 ){
			E3DRotateY( hsid1, -degstep );
		}
		if( keybuf[VK_RIGHT] != 0 ){
			E3DRotateY( hsid1, degstep );
		}
	}

	return 0;
}

int MoveCamera()
{
	float cstep = 100.0f;

	if( keybuf[ VK_SHIFT ] != 0 ){
		//�V�t�g�L�[�������Ă���Ƃ��ɃJ�����𓮂����B
		//�V�t�g�L�[�������Ă��Ȃ��Ƃ��̓L�����N�^�[�𓮂����iMoveChara�j
		if( keybuf[ VK_UP ] != 0 ){
			E3DCameraShiftUp( cstep );
		}
		if( keybuf[ VK_DOWN ] != 0 ){
			E3DCameraShiftDown( cstep );
		}
		if( keybuf[ VK_LEFT ] != 0 ){
			E3DCameraShiftLeft( cstep );
		}
		if( keybuf[ VK_RIGHT ] != 0 ){
			E3DCameraShiftRight( cstep );
		}
	}
	

	return 0;
}

int ChkConf()
{
	int ret;
	int result0 = 0;

	// �n�ʂƃL�����N�^�̂����蔻��	
	D3DXVECTOR3 adjust, n;
	ret = E3DChkConfGround( hsid1, hsid0, 1, 200.0f, -100.0f, &result0, &adjust, &n );
	_ASSERT( !ret );
	if( result0 != 0 ){
		curpos = adjust;
		ret = E3DSetPos( hsid1, curpos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}