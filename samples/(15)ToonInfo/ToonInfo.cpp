// ToonInfo.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ToonInfo.h"
#include <crtdbg.h>

#include <easy3d.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define	DEG2PAI		0.01745329251994


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
int fontid1;

int toon1num = 0;
E3DTOON1MATERIAL* toon1ptr = 0;


// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
static int SetMediaDir();
static int OneTimeSceneInit();
static int Render3DEnvironment();
static int DrawText();
static int SetToon1Info();
static int GetToon1Info();
static void DestroyObjs();


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ここにコードを挿入してください。
	MSG msg;
	HACCEL hAccelTable;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TOONINFO, szWindowClass, MAX_LOADSTRING);
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

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TOONINFO));

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

	DestroyObjs();
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TOONINFO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TOONINFO);
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
      0, 0, 700, 680, NULL, NULL, hInstance, NULL);

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

	ret = E3DCreateLight( &lid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	D3DXVECTOR3 ldir( 1.0f, 0.0f, 1.0f );
	E3DCOLOR4UC col= {255, 255, 255, 255 };
	ret = E3DSetDirectionalLight( lid1, ldir, col );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DCreateFont( 20, 0, 400, 0, 0, 0, "ＭＳ ゴシック", &fontid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//ret = SetToon1Info();
	//if( ret ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	ret = GetToon1Info();
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
	int ret;

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
		ret = E3DRender( scid, hsid1, 1, 1, 0, 0 );
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

	char mes[1024];

	POINT texpos;
	texpos.x = 0;
	texpos.y = 0;
	E3DCOLOR4UC texcol;
	texcol.a = 255, texcol.r = 255; texcol.g = 255; texcol.b = 255;

	int matno;
	for( matno = 0; matno < toon1num; matno++ ){
		E3DTOON1MATERIAL* curtoon1;
		curtoon1 = toon1ptr + matno;

		texpos.y += 20;
		sprintf_s( mes, 1024, "materialname : %s", curtoon1->name );
		E3DDrawTextByFontID( scid, fontid1, texpos, mes, texcol );
		
		texpos.y += 20;
		sprintf_s( mes, 1024, "diffuse %f %f %f", curtoon1->diffuse.r, curtoon1->diffuse.g, curtoon1->diffuse.b );
		E3DDrawTextByFontID( scid, fontid1, texpos, mes, texcol );

		texpos.y += 20;
		sprintf_s( mes, 1024, "specular %f %f %f", curtoon1->specular.r, curtoon1->specular.g, curtoon1->specular.b );
		E3DDrawTextByFontID( scid, fontid1, texpos, mes, texcol );

		texpos.y += 20;
		sprintf_s( mes, 1024, "ambient %f %f %f", curtoon1->ambient.r, curtoon1->ambient.g, curtoon1->ambient.b );
		E3DDrawTextByFontID( scid, fontid1, texpos, mes, texcol );

		texpos.y += 20;
		sprintf_s( mes, 1024, "darknl %f, brightnl %f", curtoon1->darknl, curtoon1->brightnl );
		E3DDrawTextByFontID( scid, fontid1, texpos, mes, texcol );

		texpos.y += 20;
		sprintf_s( mes, 1024, "edgecol0 %f %f %f, valid %d, inv %d, width %f", 
			curtoon1->edgecol0.r, curtoon1->edgecol0.g, curtoon1->edgecol0.b,
			curtoon1->edgevalid0, curtoon1->edgeinv0, curtoon1->edgewidth0 );
		E3DDrawTextByFontID( scid, fontid1, texpos, mes, texcol );
	
	}
	
	return 0;
}

void DestroyObjs()
{
	if( toon1ptr ){
		free( toon1ptr );
		toon1ptr = 0;
	}
	toon1num = 0;

	return;
}

int GetToon1Info()
{
	DestroyObjs();

	int ret;
	int partno = -1;
	ret = E3DGetPartNoByName( hsid1, "body", &partno );
	if( ret || (partno < 0) ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DGetToon1MaterialNum( hsid1, partno, &toon1num );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( toon1num <= 0 ){
		return 0;
	}

	toon1ptr = (E3DTOON1MATERIAL*)malloc( sizeof( E3DTOON1MATERIAL ) * toon1num );
	if( !toon1ptr ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( toon1ptr, sizeof( E3DTOON1MATERIAL ) * toon1num );

	ret = E3DGetToon1Material( hsid1, partno, toon1ptr, toon1num );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int SetToon1Info()
{
	int ret;
	int bodyno = -1;
	ret = E3DGetPartNoByName( hsid1, "body", &bodyno );
	_ASSERT( !ret );

	ret = E3DSetToon1Name( hsid1, bodyno, "Material2", "hada" );
	_ASSERT( !ret );

	RDBColor3f redcol;
	redcol.r = 1.0f; redcol.g = 0.0f; redcol.b = 0.0f;
	ret = E3DSetToon1Diffuse( hsid1, bodyno, "hada", redcol );
	_ASSERT( !ret );

	RDBColor3f gcol;
	gcol.r = 0.0f; gcol.g = 1.0f; gcol.b = 0.0f;
	ret = E3DSetToon1Ambient( hsid1, bodyno, "hada", gcol );
	_ASSERT( !ret );

	RDBColor3f bcol;
	bcol.r = 0.0f; bcol.g = 0.0f; bcol.b = 1.0f;
	ret = E3DSetToon1Specular( hsid1, bodyno, "hada", bcol );
	_ASSERT( !ret );

	float dnl = (float)cos( 20.0f * DEG2PAI );
	float bnl = (float)cos( 80.0f * DEG2PAI );
	ret = E3DSetToon1NL( hsid1, bodyno, "hada", dnl, bnl );
	_ASSERT( !ret );

	RDBColor3f wcol;
	wcol.r = 1.0f; wcol.g = 1.0f; wcol.b = 1.0f;
	ret = E3DSetToon1Edge0( hsid1, bodyno, "hada", wcol, 1, 0, 30.0f );
	_ASSERT( !ret );

	return 0;
}