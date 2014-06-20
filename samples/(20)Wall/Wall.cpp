// Wall.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Wall.h"
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
int hsid0, hsid1, hsid2;
int motid1;
int maxframe1;
double retfps;
int lid1;

D3DXVECTOR3 pos1, befpos1;

int result1, result2;
D3DXVECTOR3 adj1, adj2;
D3DXVECTOR3 n1, n2;


#define MAPSIZE 120000.0f
#define MAPDIV 50
#define MAPHEIGHT 5500.0f

#define	PAI2DEG		57.29577951308
#define	DEG2PAI		0.01745329251994

int keybuf[256];

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static int SetMediaDir();
static int OneTimeSceneInit();
static int Render3DEnvironment();
static int MoveChara();
static int ChkConf();
static int MoveCamera();
static int DrawText();


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
	LoadString(hInstance, IDC_WALL, szWindowClass, MAX_LOADSTRING);
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

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WALL));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WALL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WALL);
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


   return TRUE;
}

int OneTimeSceneInit()
{
	int ret;

	result1 = 0;
	result2 = 0;
	adj1 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	adj2 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	n1 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	n2 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );


	D3DXVECTOR3 campos = D3DXVECTOR3( 0.0f, 2500.0f, 0.0f );
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
	ret = E3DSetProjection( 1000.0f, 50000.0f, 60.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	

	char gname1[MAX_PATH];
	char gname2[MAX_PATH];
	char gname3[MAX_PATH];
	char gname4[MAX_PATH];
	sprintf_s( gname1, MAX_PATH, "%s\\g_6_20_1.bmp", szMediaDir );
	sprintf_s( gname2, MAX_PATH, "%s\\8.bmp", szMediaDir );
	sprintf_s( gname3, MAX_PATH, "%s\\g_river_6_20_1.bmp", szMediaDir );
	sprintf_s( gname4, MAX_PATH, "%s\\BG43.bmp", szMediaDir );
	ret = E3DLoadGroundBMP( gname1, gname2, gname3, gname4,
		MAPSIZE, MAPSIZE, MAPDIV, MAPDIV, MAPHEIGHT, &hsid0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	char signame[MAX_PATH];
	sprintf_s( signame, MAX_PATH, "%s\\morob_6_26_r3.sig", szMediaDir );
	ret = E3DSigLoad( signame, 0, 1.0f, &hsid1 ); 
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	char motname[MAX_PATH];
	sprintf_s( motname, MAX_PATH, "%s\\morob_6_26_r3.qua", szMediaDir );
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
	pos1.x = 53898.0f;
	pos1.y = 3000.0f;//初期位置は必ず地面よりも上でなければならない。
	pos1.z = 44982.0;
	befpos1 = pos1;
	D3DXVECTOR3 deg1( 0.0f, 180.0f, 0.0f );
	ret = E3DSetPos( hsid1, pos1 );
	_ASSERT( !ret );
	ret = E3DSetDir( hsid1, deg1 );
	_ASSERT( !ret );

	ret = E3DSetBeforePos( hsid1 );//位置を初期化した直後に必ず呼ぶこと！
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = E3DSetMovableArea( gname2, MAPSIZE, MAPSIZE, MAPDIV, MAPDIV, MAPHEIGHT + 1000.0f, &hsid2 );
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

	E3DCOLOR4UC fogcol;
	fogcol.a = 255;
	fogcol.r = 0;
	fogcol.g = 0;
	fogcol.b = 255;
	ret = E3DSetLinearFogParams( 1, fogcol, 1000.0f, 50000.0f, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	char bgname1[MAX_PATH];
	char bgname2[MAX_PATH];
	sprintf_s( bgname1, MAX_PATH, "%s\\lake.bmp", szMediaDir );
	sprintf_s( bgname2, MAX_PATH, "%s\\cloud3.bmp", szMediaDir );
	ret = E3DCreateBG( scid, bgname1, bgname2, 0.001f, 0.0f, 0, 50000.0f );
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
	ret = E3DGetKeyboardState( keybuf );
	_ASSERT( !ret );
	if( keybuf[VK_ESCAPE] == 1 ){
		return 1;
	}

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

	int curframe;
	ret = E3DSetNewPose( hsid1, &curframe );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	int status0, status1, status2;
	ret = E3DChkInView( scid, hsid0, &status0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DChkInView( scid, hsid1, &status1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DChkInView( scid, hsid2, &status2 );//壁もChkInViewが必要
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = ChkConf();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = E3DBeginScene( scid, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
			//不透明
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
			//半透明
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

	befpos1 = pos1;

	ret = E3DGetPos( hsid1, &pos1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetBeforePos( hsid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DPCWait( 60, &retfps );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int MoveChara()
{
	int ret;
	float posstep = 100.0f;
	float degstep = 5.0f;
	//keybuf は、GetKeyboardState 命令で、セットされます。

	if( keybuf[VK_UP] == 1 ){
		ret = E3DPosForward( hsid1, posstep );//矢印上
		_ASSERT( !ret );
	}
	if( keybuf[VK_DOWN] == 1 ){
		ret = E3DPosForward( hsid1, -posstep );//矢印下
		_ASSERT( !ret );
	}
	if( keybuf[VK_LEFT] == 1 ){
		ret = E3DRotateY( hsid1, -degstep );//矢印左
		_ASSERT( !ret );
	}
	if( keybuf[VK_RIGHT] == 1 ){
		ret = E3DRotateY( hsid1, degstep );//矢印右
		_ASSERT( !ret );
	}



	return 0;
}

int MoveCamera()
{
	int ret;
	D3DXVECTOR3 savepos, backpos;

	ret = E3DGetPos( hsid1, &savepos );
	_ASSERT( !ret );
	ret = E3DPosForward( hsid1, -2500.0f );
	_ASSERT( !ret );
	ret = E3DGetPos( hsid1, &backpos );
	_ASSERT( !ret );
	ret = E3DSetPos( hsid1, savepos );
	_ASSERT( !ret );

	D3DXVECTOR3 campos, camtarget, camup;
	campos = D3DXVECTOR3( backpos.x, backpos.y + 2000.0f, backpos.z );
	camtarget = D3DXVECTOR3( savepos.x, savepos.y + 800.0f, savepos.z );
	camup = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	ret = E3DSetCameraPos( campos );
	_ASSERT( !ret );
	ret = E3DSetCameraTarget( camtarget, camup );
	_ASSERT( !ret );

	return 0;
}

int ChkConf()
{
	int ret;


//壁(hsid2)とキャラクタ(hsid1)のあたり判定
	ret = E3DChkConfWall( hsid1, hsid2, 10.0f, &result1, &adj1, &n1 );
	_ASSERT( !ret );
	if( result1 ){

		//向きを、修正する。

			//面の法線と９０度、ー９０度の２つのベクトルを求める。
		D3DXVECTOR3 kabe1, kabe2;
		D3DXVECTOR3 nkabe1, nkabe2;

		ret = E3DVec3RotateY( n1, 90.0f, &kabe1 );
		_ASSERT( !ret );
		ret = E3DVec3Normalize( kabe1, &nkabe1 );
		_ASSERT( !ret );

		ret = E3DVec3RotateY( n1, -90.0f, &kabe2 );
		_ASSERT( !ret );
		ret = E3DVec3Normalize( kabe2, &nkabe2 );
		_ASSERT( !ret );

			//キャラクターの向いている向きを求める。
		ret = E3DGetPos( hsid1, &pos1 );
		_ASSERT( !ret );
		D3DXVECTOR3 vec, nvec;
		vec = D3DXVECTOR3( pos1.x - befpos1.x, 0.0f, pos1.z - befpos1.z );
		ret = E3DVec3Normalize( vec, &nvec );
		_ASSERT( !ret );

			// ９０度、ー９０度の２つのベクトルのベクトルと、キャラクターの向きの内積を取り、
			// 内積の大きな方を選んで、そちらに、向きを変える。
		float dot, dot1, dot2;
		dot1 = D3DXVec3Dot( &nvec, &nkabe1 );
		dot2 = D3DXVec3Dot( &nvec, &nkabe2 );

		D3DXVECTOR2 nvecxz, nkabexz;
		int ccw;

		if( dot1 >= dot2 ){
			nvecxz = D3DXVECTOR2( nvec.x, nvec.z );
			nkabexz = D3DXVECTOR2( nkabe1.x, nkabe1.z );
			dot = dot1;
			ret = E3DVec2CCW( nvecxz, nkabexz, &ccw );
			_ASSERT( !ret );
		}else{
			nvecxz = D3DXVECTOR2( nvec.x, nvec.z );
			nkabexz = D3DXVECTOR2( nkabe2.x, nkabe2.z );
			dot = dot2;
			ret = E3DVec2CCW( nvecxz, nkabexz, &ccw );
			_ASSERT( !ret );
		}

		dot = min( 1.0f, dot );
		dot = max( -1.0f, dot );

		float newdeg;
		newdeg = (float)acos( dot ) * (float)PAI2DEG;
		
		if( ccw > 0 ){
			newdeg *= -1.0f;
		}
		ret = E3DRotateY( hsid1, newdeg / 20.0f );
		_ASSERT( !ret );

		// 位置を修正する
		pos1 = adj1;
		ret = E3DSetPos( hsid1, pos1 );
		_ASSERT( !ret );
	}


	// 地面と、キャラクタのあたり判定	
	ret = E3DChkConfGround( hsid1, hsid0, 1, 200.0f, -1000.0f, &result2, &adj2, &n2 );
	if( result2 != 0 ){
		ret = E3DSetPos( hsid1, adj2 );
		_ASSERT( !ret );
	}

	return 0;
}


int DrawText()
{
	int ret;
	char mes[1024];
	sprintf_s( mes, 1024, "fps : %f", retfps );

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
	sprintf_s( mes, 1024, "result1 %d, adj1 %f, %f, %f", result1, adj1.x, adj1.y, adj1.z );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 20;
	sprintf_s( mes, 1024, "result2 %d, adj2 %f, %f, %f", result2, adj2.x, adj2.y, adj2.z );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

