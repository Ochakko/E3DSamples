// Effect_yamucha1.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Effect_yamucha1.h"
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
double retfps;
int lid1;

int screenw = 256;
int screenh = 256;

int texid_sphereMap;
int scid_org, scid_final, scid_normal, scid_depth;
int texid_org, texid_final, texid_normal, texid_depth;

int spid1, spw1, sph1;
int spid2, spw2, sph2;
int spid3, spw3, sph3;
int spid4, spw4, sph4;

int effectType = 0;
int effectNum = 4;

int keybuf[256];

D3DXVECTOR3 campos;
float camdegxz, camdegy;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static int SetMediaDir();
static int OneTimeSceneInit();
static int Render3DEnvironment();
static int DrawText();
static int Controll();

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
	LoadString(hInstance, IDC_EFFECT_YAMUCHA1, szWindowClass, MAX_LOADSTRING);
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


	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EFFECT_YAMUCHA1));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EFFECT_YAMUCHA1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EFFECT_YAMUCHA1);
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
      CW_USEDEFAULT, 0, screenw * 2 + 50, screenh * 2 + 50, NULL, NULL, hInstance, NULL);

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

	ZeroMemory( keybuf, sizeof( int ) * 256 );

	campos = D3DXVECTOR3( 0.0f, 0.0f, -1300.0 );
	ret = E3DSetCameraPos( campos );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	camdegxz = 180.0f;
	camdegy = 0.0f;
	ret = E3DSetCameraDeg( camdegxz, camdegy );
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
	sprintf_s( signame, MAX_PATH, "%s\\my05.sig", szMediaDir );
	ret = E3DSigLoad( signame, 0, 1.0f, &hsid1 ); 
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	char motname[MAX_PATH];
	sprintf_s( motname, MAX_PATH, "%s\\アイドル正面.qua", szMediaDir );
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
//////////////////
	//スフィアマップ読み込み
	char sphname[MAX_PATH];
	sprintf_s( sphname, MAX_PATH, "%s\\spheremap.bmp", szMediaDir );
	ret = E3DCreateTexture( sphname, D3DPOOL_DEFAULT, 0, &texid_sphereMap );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//レンダリングテクスチャを作成
	int okflag;
	SIZE scsize;
	scsize.cx = screenw;
	scsize.cy = screenh;
	ret = E3DCreateRenderTargetTexture( scsize, D3DFMT_A8R8G8B8, &scid_org, &texid_org, &okflag );
	_ASSERT( !ret );
	ret = E3DCreateRenderTargetTexture( scsize, D3DFMT_A8R8G8B8, &scid_final, &texid_final, &okflag );
	_ASSERT( !ret );
	ret = E3DCreateRenderTargetTexture( scsize, D3DFMT_A8R8G8B8, &scid_normal, &texid_normal, &okflag );
	_ASSERT( !ret );
	ret = E3DCreateRenderTargetTexture( scsize, D3DFMT_A8R8G8B8, &scid_depth, &texid_depth, &okflag );
	_ASSERT( !ret );

///////
	char spname[MAX_PATH];
	sprintf_s( spname, MAX_PATH, "%s\\dummy256.png", szMediaDir );
	ret = E3DCreateSprite( spname, 0, 0, &spid1 );
	_ASSERT( !ret );
	ret = E3DSetTextureToDispObj( -2, spid1, texid_org );
	_ASSERT( !ret );
	ret = E3DGetSpriteSize( spid1, &spw1, &sph1 );
	_ASSERT( !ret );

	ret = E3DCreateSprite( spname, 0, 0, &spid2 );
	_ASSERT( !ret );
	ret = E3DSetTextureToDispObj( -2, spid2, texid_final );
	_ASSERT( !ret );
	ret = E3DGetSpriteSize( spid2, &spw2, &sph2 );
	_ASSERT( !ret );

	ret = E3DCreateSprite( spname, 0, 0, &spid3 );
	_ASSERT( !ret );
	ret = E3DSetTextureToDispObj( -2, spid3, texid_normal );
	_ASSERT( !ret );
	ret = E3DGetSpriteSize( spid3, &spw3, &sph3 );
	_ASSERT( !ret );


	ret = E3DCreateSprite( spname, 0, 0, &spid4 );
	_ASSERT( !ret );
	ret = E3DSetTextureToDispObj( -2, spid4, texid_depth );
	_ASSERT( !ret );
	ret = E3DGetSpriteSize( spid4, &spw4, &sph4 );
	_ASSERT( !ret );


	//背景
	char bgname[MAX_PATH];
	sprintf_s( bgname, MAX_PATH, "%s\\check.bmp", szMediaDir );
	ret = E3DCreateBG( scid_final, bgname, "dummy", 0.0f, 0.0f, 0, 30000.0f );
	_ASSERT( !ret );

	effectType = 0;
	effectNum = 4;

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
	E3DGetKeyboardCnt( keybuf );

	ret = Controll();
	_ASSERT( !ret );

	int curframe;
	ret = E3DSetNewPose( hsid1, &curframe );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	//背景を描画
	int state1;
	E3DBeginScene( scid_final, 0 );
		E3DChkInView( scid_final, hsid1, &state1 );
	E3DEndScene();
	
	//エフェクト前のオリジナルとしてストック
	E3DCOLOR4F col1 = { 1.0f, 1.0f, 1.0f, 1.0f };
	E3DBlendRTTexture( scid_org,  texid_final, col1, D3DTEXF_POINT, -1, col1, D3DTEXF_POINT, D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ONE );

	//深度マップ・法線マップを描画
	ret = E3DRenderZandN( scid_depth, scid_normal, scid_final, hsid1, 4, 1+2+4 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
//	E3DRenderZandN scid_depth,scid_normal,scid_final,hsid1,4+8,2

	D3DXVECTOR4 fl4_0;

	switch( effectType ){
	case 0:
	case 2:
		//環境マップエフェクト
		E3DShaderConstUserTex( 0, texid_org );
		E3DShaderConstUserTex( 1, texid_normal );
		E3DShaderConstUserTex( 2, texid_sphereMap );
		ret = E3DCallUserShader( 0, 0, scid_final );
		_ASSERT( !ret );
		break;
	case 1:
	case 3:
		//環境マップ付き屈折エフェクト
		fl4_0.x = 0.105f;			//屈折色収差における赤成分の移動量
		fl4_0.y = 0.1f;			//屈折色収差における緑成分の移動量
		fl4_0.z = 0.095f;			//屈折色収差における青成分の移動量
		fl4_0.w =  0.1f;			//垂直入射時のフレネル反射係数(0.0～1.0)
		E3DShaderConstUserFL4( 0, fl4_0 );
		E3DShaderConstUserTex( 0, texid_org );
		E3DShaderConstUserTex( 1, texid_normal );
		E3DShaderConstUserTex( 2, texid_sphereMap );
		ret = E3DCallUserShader( 1, 0, scid_final );
		_ASSERT( !ret );
		break;
	default:
		_ASSERT( 0 );
		break;
	}

	//モデルを描画
	if( effectType >= 2 ){
		E3DBeginScene( scid_final, 1, scid_final );
			E3DRender( scid_final, hsid1, 0, 1, 0, 0 );
			E3DRender( scid_final, hsid1, 1, 1, 1, 0 );
		E3DEndScene();
	}

	//4画面を表示
	D3DXVECTOR3 tra;
	E3DBeginScene( scid, 0 );
		E3DBeginSprite();
			tra = D3DXVECTOR3( 0.0f, 0.0f, 0.5f );
        	E3DRenderSprite( spid1, 1.0f, 1.0f, tra );

			tra = D3DXVECTOR3( (float)spw1, 0.0f, 0.5f );
        	E3DRenderSprite( spid2, 1.0, 1.0, tra );

			tra = D3DXVECTOR3( 0.0f, (float)sph1, 0.5f );
        	E3DRenderSprite( spid3, 1.0, 1.0, tra );

			tra = D3DXVECTOR3( (float)spw1, (float)sph1, 0.5f );
        	E3DRenderSprite( spid4, 1.0, 1.0, tra );
        E3DEndSprite();
	
		DrawText();

	E3DEndScene();
	E3DPresent( scid );


	ret = E3DPCWait( 30, &retfps );
	_ASSERT( !ret );

	return 0;
}

int DrawText()
{

	char mes[1024];
	sprintf_s( mes, 1024, "fps : %f", retfps );

	D3DXVECTOR2 texpos;
	texpos.x  = 10; texpos.y = 10;
	E3DCOLOR4UC texcol;
	texcol.a = 255, texcol.r = 255; texcol.g = 0; texcol.b = 0;
	E3DDrawText( texpos, 1.0f, texcol, mes );


	texpos.y += 20;
	strcpy_s( mes, 1024, "[SPACE] changeFX" );
	E3DDrawText( texpos, 1.0f, texcol, mes );

	texpos.y += 20;
	strcpy_s( mes, 1024, "FXtype : " );
	switch( effectType ){
	case 0: 
		strcat_s( mes, 1024, "SphereMap" );
		break;
	case 1: 
		strcat_s( mes, 1024, "Refract+SphereMap" );
		break;
	case 2: 
		strcat_s( mes, 1024, "Texture+SphereMap" );
		break;
	case 3: 
		strcat_s( mes, "Texture+Refract+SphereMap" );
		break;
	default:
		break;
	}
	E3DDrawText( texpos, 1.0f, texcol, mes );

	return 0;
}

int Controll()
{
	int ret;
	//カメラの操作
	if( keybuf[VK_LEFT] ){
		camdegxz += 5.0f;
	}
	if( keybuf[VK_RIGHT] ){
		camdegxz -= 5.0f;
	}

	float dis= 1300.0f;
	campos.y = 700.0f;
	campos.x = dis * sin( -camdegxz / 180.0f * 3.14f );
	campos.z = dis * cos( -camdegxz / 180.0f * 3.14f );
	
	ret = E3DSetCameraPos( campos );
	_ASSERT( !ret );
	ret = E3DSetCameraDeg( camdegxz, camdegy );
	_ASSERT( !ret );

	//エフェクトの種類を変更
	if( keybuf[VK_SPACE] == 1 ){
		effectType = ( effectType + 1 ) % effectNum;
	}
	
	return 0;
}