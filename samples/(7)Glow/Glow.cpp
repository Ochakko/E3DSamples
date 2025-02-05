// Glow.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Glow.h"
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
int retfps;
int lid1;
int keybuf[256];

int scid_org, texid_org;
int scid_glow, texid_glow;
int scid_small, texid_small;
int scid_work, texid_work;
int scid_blur, texid_blur;
int scid_final, texid_final;

int spid1, spid2, spid3;
int materialno;
int savemode;
E3DCOLOR4F savecol;
int changedir;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static int SetMediaDir();
static int OneTimeSceneInit();
static int Render3DEnvironment();
static int DrawText();
static int ChangeGlowParams();

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
	LoadString(hInstance, IDC_GLOW, szWindowClass, MAX_LOADSTRING);
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


	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GLOW));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GLOW));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GLOW);
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
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

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
	E3DSetClearCol( scid, clearcol );

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
	sprintf_s( signame, MAX_PATH, "%s\\glow.sig", szMediaDir );
	ret = E3DSigLoad( signame, 0, 1.0f, &hsid1 ); 
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
	//オリジナルシーン用のテクスチャは、グロー対応のためA8ではなくX8で作成することに注意
	ret = E3DCreateRenderTargetTexture( largesize, D3DFMT_X8R8G8B8, &scid_org, &texid_org, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}
	E3DCOLOR4UC clearcol = { 0, 30, 65, 90 };
	ret = E3DSetClearCol( scid_org, clearcol );
	_ASSERT( !ret );

	ret = E3DCreateRenderTargetTexture( largesize, D3DFMT_A8R8G8B8, &scid_glow, &texid_glow, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}
	clearcol.r = 0; clearcol.g = 0; clearcol.b = 0;
	ret = E3DSetClearCol( scid_glow, clearcol );
	_ASSERT( !ret );

	ret = E3DCreateRenderTargetTexture( smallsize, D3DFMT_A8R8G8B8, &scid_small, &texid_small, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetClearCol( scid_small, clearcol );
	_ASSERT( !ret );

	ret = E3DCreateRenderTargetTexture( smallsize, D3DFMT_A8R8G8B8, &scid_work, &texid_work, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetClearCol( scid_work, clearcol );
	_ASSERT( !ret );

	ret = E3DCreateRenderTargetTexture( smallsize, D3DFMT_A8R8G8B8, &scid_blur, &texid_blur, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetClearCol( scid_blur, clearcol );
	_ASSERT( !ret );

	ret = E3DCreateRenderTargetTexture( largesize, D3DFMT_A8R8G8B8, &scid_final, &texid_final, &okflag );
	if( ret || !okflag ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetClearCol( scid_final, clearcol );
	_ASSERT( !ret );



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
	ret = E3DSetTextureToDispObj( -2, spid2, texid_glow );
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

	ret = E3DGetMaterialNoByName( hsid1, "gamen", &materialno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetMaterialGlowParams( hsid1, materialno, &savemode, &savecol );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	changedir = 0;


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
	int ret;

	ret = E3DGetKeyboardCnt( keybuf );
	_ASSERT( !ret );

	ret = ChangeGlowParams();
	_ASSERT( !ret );

	//テクスチャにシーンをレンダリング
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
	ret = E3DRender( scid_org, hsid1, 0, 1, 0, 0 );//不透明
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DRender( scid_org, hsid1, 1, 1, 0, 0 );//半透明
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


	//グローを表示。Zバッファはオリジナルシーンのものを使う。
	ret = E3DBeginSceneZbuf( scid_glow, 4, scid_org );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DRenderGlow( &hsid1, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DEndScene();


	//グローをサイズの小さいテクスチャにコピー
	E3DCOLOR4F col1 = { 1.0f, 1.0f, 1.0f, 1.0f };
	ret = E3DBlendRTTexture( scid_small, texid_glow, col1, D3DTEXF_LINEAR, 
		-1, col1, D3DTEXF_LINEAR, 
		D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ONE );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	//ぼかし処理
	ret = E3DBlurTextureGauss( scid_blur, scid_work, texid_work, texid_small, 20.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	//ぼかしと元画像をアッドモードでブレンド
	E3DCOLOR4F col2 = { 1.0f, 1.0f, 1.0f, 1.0f };
	E3DCOLOR4F col3 = { 1.0f, 1.0f, 1.0f, 1.0f };
	ret = E3DBlendRTTexture( scid_final, texid_org, col2, D3DTEXF_POINT,
		texid_blur, col3, D3DTEXF_LINEAR,
		D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ONE );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



	//メイン画面に表示
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

	//グロー画像を表示
	tra.x = 256.0f;
	tra.y = 0.0f;
	ret = E3DRenderSprite( spid2, scalex, scaley, tra );
	_ASSERT( !ret );

	//ブレンド画像を表示
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
	texpos.x  = 10; texpos.y = 0;
	E3DCOLOR4UC texcol;
	texcol.a = 255, texcol.r = 255; texcol.g = 255; texcol.b = 255;
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.x  = 10; texpos.y = 20;
	strcpy_s( mes, 1024, "push 1 key !!!" );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int ChangeGlowParams()
{

	int ret;
	//1キーを押したら初期状態の場合は変化を開始し、変化しているときは初期状態に戻す。
	if( keybuf['1'] == 1 ){
		if( changedir != 0 ){
			changedir = 0;
			ret = E3DSetMaterialGlowParams( hsid1, materialno, savemode, 1, savecol );
			_ASSERT( !ret );
		}else{
			changedir = 1;
			ret = E3DSetMaterialGlowParams( hsid1, materialno, savemode, 1, savecol );
			_ASSERT( !ret );
		}
	}

	//グロー色の赤係数が５より小さい間は0.1ずつ増やし、５より大きくなったら0.1ずつ減らす。
	int tempmode;
	E3DCOLOR4F tempcol;
	ret = E3DGetMaterialGlowParams( hsid1, materialno, &tempmode, &tempcol );
	_ASSERT( !ret );
	if( changedir == 1 ){
		if( tempcol.r < 5.0 ){
			tempcol.r += 0.1f;
		}else{
			changedir = 2;
			tempcol.r = 5.0f;
		}
	}else{
		if( changedir == 2 ){
			if( tempcol.r > 1.0f ){
				tempcol.r -= 0.1f;
			}else{
				changedir = 1;
				tempcol.r = 1.0f;
			}
		}
	}

	ret = E3DSetMaterialGlowParams( hsid1, materialno, tempmode, 1, tempcol );
	_ASSERT( !ret );


	return 0;
}