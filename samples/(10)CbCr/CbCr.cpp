// CbCr.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "CbCr.h"
#include <crtdbg.h>

#include <easy3d.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名
char szMediaDir[MAX_PATH];
int scid;
int hsid1;
int motid1;
int maxframe1;
int retfps;
int lid1;

int scid_org, texid_org;
int scid_cbcr, texid_cbcr;
int spid1, spw1, sph1;
int spid2, spw2, sph2;

float cb, cr;
int keybuf[256];

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static int SetMediaDir();
static int OneTimeSceneInit();
static int Render3DEnvironment();
static int DrawText();
static int ChangeCbCr();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ここにコードを挿入してください。
	MSG msg;
	HACCEL hAccelTable;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CBCR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
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

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CBCR));

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
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CBCR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CBCR);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

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
	E3DCOLOR4UC clearcol = { 0, 180, 180, 180 };
	ret = E3DSetClearCol( scid, clearcol );


   return TRUE;
}

int OneTimeSceneInit()
{
	int ret;

	cb = -0.2f;
	cr = 0.1f;

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
	E3DCOLOR4UC col= {255, 255, 255, 255 };
	ret = E3DSetDirectionalLight( lid1, ldir, col );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	int okflag;
	SIZE texsize;
	texsize.cx = 640 / 2;
	texsize.cy = 480;
	ret = E3DCreateRenderTargetTexture( texsize, D3DFMT_A8R8G8B8, &scid_org, &texid_org, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DCreateRenderTargetTexture( texsize, D3DFMT_A8R8G8B8, &scid_cbcr, &texid_cbcr, &okflag );
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
	ret = E3DGetSpriteSize( spid1, &spw1, &sph1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DCreateSprite( spname, 0, 0, &spid2 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetTextureToDispObj( -2, spid2, texid_cbcr );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetSpriteSize( spid2, &spw2, &sph2 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;

}


//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
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
		// 選択されたメニューの解析:
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
		// TODO: 描画コードをここに追加してください...
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

// バージョン情報ボックスのメッセージ ハンドラです。
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
	E3DGetKeyboardCnt( keybuf );

	ChangeCbCr();

	int ret;
	int curframe;
	ret = E3DSetNewPose( hsid1, &curframe );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

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
	ret = E3DEndScene();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DCbCrRTTexture( scid_cbcr, texid_org, cb, cr );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = E3DBeginScene( scid, 0 );
	_ASSERT( !ret );
	ret = E3DBeginSprite();
	_ASSERT( !ret );
	
	//元画像を表示
	float scalex = 1.0f;
	float scaley = 1.0f;
	D3DXVECTOR3 tra = D3DXVECTOR3( 0.0f, 0.0f, 0.1f );
	ret = E3DRenderSprite( spid1, scalex, scaley, tra );
	_ASSERT( !ret );

	//Monoイメージを表示
	tra.x = (float)spw1;
	tra.y = 0.0f;
	ret = E3DRenderSprite( spid2, scalex, scaley, tra );
	_ASSERT( !ret );

	ret = E3DEndSprite();
	_ASSERT( !ret );
	ret = DrawText();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
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
	texpos.x  = 10; texpos.y = 20;
	E3DCOLOR4UC texcol;
	texcol.a = 255, texcol.r = 255; texcol.g = 255; texcol.b = 255;
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 20;
	sprintf_s( mes, 1024, "1,2key --> Change Cb %f", cb );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 20;
	sprintf_s( mes, 1024, "3,4key --> Change Cr %f", cr );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int ChangeCbCr()
{
	if( keybuf['1'] != 0 ){
		cb += 0.005f;
	}
	if( keybuf['2'] != 0 ){
		cb -= 0.005f;
	}

	if( keybuf['3'] != 0 ){
		cr += 0.005f;
	}
	if( keybuf['4'] != 0 ){
		cr -= 0.005f;
	}

	return 0;
}