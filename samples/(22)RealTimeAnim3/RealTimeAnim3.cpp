// RealTimeAnim3.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "RealTimeAnim3.h"

#include <crtdbg.h>

#include <easy3d.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define MAX_LOADSTRING 100

char szMediaDir[MAX_PATH];
int scid;
int hsid0;
int hsid1;
int hsids;
int materialno;
int fps;
double retfps;
int lid1;

int camanimno, camanimframenum;
int gpanimno, gpanimframenum;
int moeanimno, moeanimframenum;
int sndanimno, sndanimframenum;
int bgmid;
int bgmstartcnt, bgmendcnt;
int bgmloopmode;
int playflag = 0;
int stopflag = 0;
int sndframeno = 0;

int scid2;
int texid2;

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

static int SetMediaDir();
static int OneTimeSceneInit();
static int Render3DEnvironment();


// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

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
	LoadString(hInstance, IDC_REALTIMEANIM3, szWindowClass, MAX_LOADSTRING);
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


	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REALTIMEANIM3));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REALTIMEANIM3));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_REALTIMEANIM3);
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
      0, 0, 640, 480, NULL, NULL, hInstance, NULL);

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
	
	ret = E3DSetProjection( 1000.0f, 40000.0f, 60.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

//地面読み込み
	char gname[MAX_PATH];
	sprintf_s( gname, MAX_PATH, "%s\\stage\\stage2.mqo", szMediaDir );
	ret = E3DLoadMQOFileAsGround( gname, 1000.0f, 0, BONETYPE_RDB2, &hsid0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	char gname2[MAX_PATH];
	sprintf_s( gname2, MAX_PATH, "%s\\stage\\screen.mqo", szMediaDir );
	ret = E3DLoadMQOFileAsGround( gname2, 1000.0f, 0, BONETYPE_RDB2, &hsids );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//レンダーターゲットテクスチャの作成
	int okflag = 0;
	SIZE screensize;
	screensize.cx = 256;
	screensize.cy = 256;
	ret = E3DCreateRenderTargetTexture( screensize, D3DFMT_A8R8G8B8, &scid2, &texid2, &okflag );
	if( okflag == 0 ){
		_ASSERT( 0 );
		return 1;
	}
	//レンダーターゲットテクスチャをscreenに貼り付ける。
	ret = E3DGetMaterialNoByName( hsids, "matscreen", &materialno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetTextureToMaterial( hsids, materialno, texid2 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



//キャラクター読み込み
	char signame[MAX_PATH];
	sprintf_s( signame, MAX_PATH, "%s\\bucho\\buch.sig", szMediaDir );
	ret = E3DSigLoad( signame, 0, 1.0f, &hsid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

//moa読み込み
	char moaname[MAX_PATH];
	sprintf_s( moaname, MAX_PATH, "%s\\bucho\\mot_acc.moa", szMediaDir );
	ret = E3DLoadMOAFile( hsid1, moaname, -1, 1.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

//GPA読み込み
	char gpaname[MAX_PATH];
	sprintf_s( gpaname, MAX_PATH, "%s\\global_pos_anime.gpa", szMediaDir );
	ret = E3DLoadGPFile( hsid1, gpaname, &gpanimno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetGPFrameNum( hsid1, gpanimno, &gpanimframenum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetGPGroundHSID( hsid1, gpanimno, hsid0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetGPAnimNo( hsid1, gpanimno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
//cam読み込み
	char camname[MAX_PATH];
	sprintf_s( camname, MAX_PATH, "%s\\cam_anime.cam", szMediaDir );
	ret = E3DLoadCameraFile( camname, &camanimno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetCameraFrameNum( camanimno, &camanimframenum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetCameraAnimNo( camanimno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

//MOE読み込み
	//moa読み込みよりも後で！！
	char moename[MAX_PATH];
	sprintf_s( moename, MAX_PATH, "%s\\mot_event_anime.moe", szMediaDir );
	ret = E3DLoadMOEFile( hsid1, moename, &moeanimno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetMOEFrameNum( hsid1, moeanimno, &moeanimframenum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetMOEAnimNo( hsid1, moeanimno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

//SSF読み込み
	char ssfname[MAX_PATH];
	sprintf_s( ssfname, MAX_PATH, "%s\\sound_set.ssf", szMediaDir );
	ret = E3DLoadSSF( ssfname );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


//SAF読み込み
	char safname[MAX_PATH];
	sprintf_s( safname, MAX_PATH, "%s\\sound_anime.saf", szMediaDir );
	ret = E3DLoadSAF( safname, &sndanimno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetSndAnimFrameNum( sndanimno, &sndanimframenum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetSndAnimBGM( sndanimno, &bgmid );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetSndAnimFPS( sndanimno, &fps );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetSndAnimMode( sndanimno, &bgmloopmode );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


//ライト設定
	ret = E3DCreateLight( &lid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	D3DXVECTOR3 ldir( 1.0f, -1.0f, 1.0f );
	E3DCOLOR4UC col= {255, 255, 255, 255 };
	ret = E3DSetDirectionalLight( lid1, ldir, col );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


//音楽再生開始
	ret = E3DGetSoundStartCnt( bgmid, &bgmstartcnt );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	sndframeno = 0;//!!!!!!!
	ret = E3DStartSndAnim( sndanimno, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetSoundEndCnt( bgmid, &bgmendcnt );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	playflag = 1;//!!!!!!!!!
	stopflag = 0;

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

	int tmpstartcnt, tmpendcnt;
	ret = E3DGetSoundStartCnt( bgmid, &tmpstartcnt );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetSoundEndCnt( bgmid, &tmpendcnt );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//終了判定
	if( (tmpstartcnt > bgmstartcnt) && (sndframeno >= (sndanimframenum - 1)) ){
		playflag = 0;
	}
	if( tmpendcnt > bgmendcnt ){
		playflag = 0;
	}

	if( playflag == 1 ){

		if( tmpstartcnt <= bgmstartcnt ){
			//まだ再生が始まっていない
			sndframeno = 0;
		}else{
			double cursamples;
			ret = E3DGetSoundSamplesPlayed( bgmid, &cursamples );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			ret = E3DConvSoundSampleToFrame( bgmid, fps, cursamples, &sndframeno );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
		int camframeno;
		camframeno = min( sndframeno, (camanimframenum - 1) );
		camframeno = max( sndframeno, 0 );
		ret = E3DSetCameraFrameNo( camanimno, -1, camframeno, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		int gpframeno;
		gpframeno = min( sndframeno, (gpanimframenum - 1) );
		gpframeno = max( sndframeno, 0 );
		ret = E3DSetGPFrameNo( hsid1, gpanimno, gpframeno, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		int moeframeno;
		moeframeno = min( sndframeno, (moeanimframenum - 1) );
		moeframeno = max( sndframeno, 0 );
		ret = E3DSetMOEFrameNo( hsid1, moeanimno, moeframeno, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = E3DSetSndAnimFrameNo( sndanimno, sndframeno );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}else{
		if( stopflag == 0 ){
			ret = E3DStopSndAnim( sndanimno );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			stopflag = 1;
		}
	}

//レンダーターゲットに描画
	int state0;
	ret = E3DChkInView( scid2, hsid0, &state0 );
	ret = E3DChkInView( scid2, hsid1, &state0 );
	ret = E3DChkInView( scid2, hsids, &state0 );
	ret = E3DBeginScene( scid2, 0 );
		//不透明部分の描画をする
		ret = E3DRender( scid2, hsid0, 0, 1, 0, 0 );
		_ASSERT( !ret );
		ret = E3DRender( scid2, hsid1, 0, 1, 0, 0 );
		_ASSERT( !ret );
		//半透明部分の描画をする
		ret = E3DRender( scid2, hsid0, 1, 1, 1, 0 );
		_ASSERT( !ret );
		ret = E3DRender( scid2, hsid1, 1, 1, 1, 0 );
		_ASSERT( !ret );
	E3DEndScene();



//カメラをメイン画面用に再設定してメイン画面を描画
	D3DXVECTOR3 cpos = D3DXVECTOR3( 0.0f, 5000.0f, 10000.0f );
	ret = E3DSetCameraPos( cpos );
	D3DXVECTOR3 ctpos = D3DXVECTOR3( 0.0f, 5000.0f, 20000.0f );
	D3DXVECTOR3 cupos = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	ret = E3DSetCameraTarget( ctpos, cupos );

	int status;
	ret = E3DChkInView( scid, hsid0, &status );
	ret = E3DChkInView( scid, hsid1, &status );
	ret = E3DChkInView( scid, hsids, &status );

	ret = E3DBeginScene( scid, 0 );
		//不透明
		ret = E3DRender( scid, hsid0, 0, 1, 0, 0 );
		ret = E3DRender( scid, hsid1, 0, 1, 0, 0 );
		ret = E3DRender( scid, hsids, 0, 1, 0, 0 );

		//半透明
		ret = E3DRender( scid, hsid0, 1, 1, 0, 0 );
		ret = E3DRender( scid, hsid1, 1, 1, 0, 0 );
		ret = E3DRender( scid, hsids, 1, 1, 0, 0 );
		
	ret = E3DEndScene();
	ret = E3DPresent( scid );
	ret = E3DPCWait( fps, &retfps );//E3DWaitByFPSよりも高精度

	return 0;
}
