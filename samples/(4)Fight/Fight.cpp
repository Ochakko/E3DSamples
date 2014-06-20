// Fight.cpp : アプリケーションのエントリ ポイントを定義します。
//
// キャラ１は矢印キーで移動、B, N, Mで技、BNMをタイミングよく連打すると投げ技
// キャラ２はWSADで移動、Z,X,Cで技、ZXCをタイミングよく連打すると投げ技




#include "stdafx.h"
#include "Fight.h"

#include <crtdbg.h>

#include <easy3d.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define MAX_LOADSTRING 100


#define GROUND_SIZE	60000.0
#define GROUND_DIVX	30
#define GROUND_DIVZ	30
#define GROUND_HEIGHT	0.0
#define GRAVITY	-3.0
#define NAGEY	70.0
#define HPMAX	20000

typedef struct tag_charainfo
{
	int hsid;
	int qid;
	int motid;
	int frameno;
	int nage;
	int nagerare;
	int hp;
	D3DXVECTOR3 pos;
	float nagevy;
	D3DXVECTOR3 befpos;
} CHARAINFO;

enum {
	MOT_IDLING,
	MOT_WALK,
	MOT_TSUKI,
	MOT_KICK,
	MOT_DEFENCE,
	MOT_MOCHIAGE,
	MOT_JITABATA,
	MOT_NAGERARE,
	MOT_TSUNAGI,
	MOT_MAX
};

enum {
	CHARA_RED,
	CHARA_BLUE,
	CHARA_MAX
};

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名
char szMediaDir[MAX_PATH];

int scid;
CHARAINFO ci[CHARA_MAX];
float cameraheight;
float cameradist;
D3DXVECTOR3 campos;
int motnum;
int* motionidptr = 0;
int confflag = 0;
int gameflag = 0;
int groundhsid;
int lid1;
int keybuf[256];
int retfps;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static int SetMediaDir();
static int OneTimeSceneInit();
static int Render3DEnvironment();
static int DrawText();
static void DestroyObjs();
static int FrameMove();
static int Render();
static int MoveChara( int chara1, int chara2, char key1, char key2, char key3, char key4, char key5, char key6, char key7 );
static int MoveCamera();
static int ChkConfChara();
static int ChangeHP( int chara1, int chara2 );
static int ChkGameOver();


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
	LoadString(hInstance, IDC_FIGHT, szWindowClass, MAX_LOADSTRING);
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

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FIGHT));

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


    return (int)msg.wParam;
}

void DestroyObjs()
{
	if( motionidptr ){
		free( motionidptr );
		motionidptr = 0;
	}
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FIGHT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_FIGHT);
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

   //hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

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

	cameradist = 3000.0f;
	cameraheight = 800.0f;
	campos.x = (float)GROUND_SIZE * 0.5f;
	campos.y = (float)GROUND_HEIGHT + cameraheight;
	campos.z = ( (float)GROUND_SIZE * 0.5f ) - cameradist;

	ret = E3DSetProjection( 500.0f, 50000.0f, 60.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ZeroMemory( ci, sizeof( CHARAINFO ) * CHARA_MAX );


//赤キャラ初期化
	char signame[MAX_PATH];
	sprintf_s( signame, MAX_PATH, "%s\\poo\\poo.sig", szMediaDir );
	ret = E3DSigLoad( signame, 0, 1.0f, &(ci[CHARA_RED].hsid) ); 
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ci[CHARA_RED].pos = D3DXVECTOR3( 1000.0f, GROUND_HEIGHT, 0.0f );
	ret = E3DSetPos( ci[CHARA_RED].hsid, ci[CHARA_RED].pos );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	E3DCOLOR4UC redcol = { 255, 255, 0, 0 };
	ret = E3DSetToonEdge0Color( ci[CHARA_RED].hsid, -1, "dummy", redcol );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetToonEdge0Width( ci[CHARA_RED].hsid, -1, "dummy", 10.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DRotateY( ci[CHARA_RED].hsid, 90.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	char motname[MAX_PATH];
	sprintf_s( motname, MAX_PATH, "%s\\poo\\poo.moa", szMediaDir );
	ret = E3DLoadMOAFile( ci[CHARA_RED].hsid, motname, 10, 1.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DCreateQ( &(ci[CHARA_RED].qid) );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetBeforePos( ci[CHARA_RED].hsid );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ci[CHARA_RED].hp = HPMAX;

//青きゃらの初期化
	sprintf_s( signame, MAX_PATH, "%s\\poo\\poo.sig", szMediaDir );
	ret = E3DSigLoad( signame, 0, 1.0f, &(ci[CHARA_BLUE].hsid) ); 
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ci[CHARA_BLUE].pos = D3DXVECTOR3( -1000.0f, GROUND_HEIGHT, 0.0f );
	ret = E3DSetPos( ci[CHARA_BLUE].hsid, ci[CHARA_BLUE].pos );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	E3DCOLOR4UC bcol = { 255, 0, 0, 255 };
	ret = E3DSetToonEdge0Color( ci[CHARA_BLUE].hsid, -1, "dummy", bcol );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetToonEdge0Width( ci[CHARA_BLUE].hsid, -1, "dummy", 10.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DRotateY( ci[CHARA_BLUE].hsid, -90.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	sprintf_s( motname, MAX_PATH, "%s\\poo\\poo.moa", szMediaDir );
	ret = E3DLoadMOAFile( ci[CHARA_BLUE].hsid, motname, 10, 1.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DCreateQ( &(ci[CHARA_BLUE].qid) );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DSetBeforePos( ci[CHARA_BLUE].hsid );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ci[CHARA_BLUE].hp = HPMAX;
///////////////////

	E3DGetMotionNum( ci[CHARA_RED].hsid, &motnum );
	motionidptr = (int*)malloc( sizeof( int ) * motnum );
	if( !motionidptr ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetMotionIDByName( ci[CHARA_RED].hsid, "idling", motionidptr + MOT_IDLING );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetMotionIDByName( ci[CHARA_RED].hsid, "walk", motionidptr + MOT_WALK );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetMotionIDByName( ci[CHARA_RED].hsid, "tsuki", motionidptr + MOT_TSUKI );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetMotionIDByName( ci[CHARA_RED].hsid, "kick", motionidptr + MOT_KICK );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetMotionIDByName( ci[CHARA_RED].hsid, "defence", motionidptr + MOT_DEFENCE );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetMotionIDByName( ci[CHARA_RED].hsid, "mochiage", motionidptr + MOT_MOCHIAGE );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetMotionIDByName( ci[CHARA_RED].hsid, "jitabata", motionidptr + MOT_JITABATA );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetMotionIDByName( ci[CHARA_RED].hsid, "nagerare", motionidptr + MOT_NAGERARE );
		if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DGetMotionIDByName( ci[CHARA_RED].hsid, "tsunagi", motionidptr + MOT_TSUNAGI );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
////////////////////

	char bgname[MAX_PATH];	
	sprintf_s( bgname, MAX_PATH, "%s\\bmpfile\\a007bcg.bmp", szMediaDir );
	ret = E3DCreateBG( scid, bgname, "dummy", 0.0f, 0.0f, 0, 50000.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	char gname[MAX_PATH];
	sprintf_s( gname, MAX_PATH, "%s\\bmpfile\\gplane.mqo", szMediaDir );
	ret = E3DLoadMQOFileAsGround( gname, 100.0, 0, BONETYPE_RDB2, &groundhsid );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



	ret = E3DCreateLight( &lid1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	D3DXVECTOR3 ldir( 0.0f, -1.0f, 1.0f );
	E3DCOLOR4UC col= {255, 255, 255, 255 };
	ret = E3DSetDirectionalLight( lid1, ldir, col );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	confflag = 0;
	gameflag = 0;


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
	ret = FrameMove();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Render();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int FrameMove()
{
	int ret;
	E3DGetKeyboardCnt( keybuf );

	if( gameflag == 0 ){
		ret = MoveChara( CHARA_RED, CHARA_BLUE, VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, 'B', 'N', 'M' );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = MoveChara( CHARA_BLUE, CHARA_RED, 'W', 'S', 'A', 'D', 'Z', 'X', 'C' );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = MoveCamera();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	int stat;
	ret = E3DChkInView( scid, groundhsid, &stat );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DChkInView( scid, ci[CHARA_RED].hsid, &stat );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = E3DChkInView( scid, ci[CHARA_BLUE].hsid, &stat );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( gameflag == 0 ){
		ret = ChkConfChara();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = ChangeHP( CHARA_RED, CHARA_BLUE );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = ChangeHP( CHARA_BLUE, CHARA_RED );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = ChkGameOver();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}



	return 0;
}


int MoveChara( int chara1, int chara2, char key1, char key2, char key3, char key4, 
	char key5, char key6, char key7 )
{
	int ret;

	int eventno;

	if( ci[chara1].nagerare == 0 ){
		eventno = 0;
		if( keybuf[key5] == 1 )
			eventno = 2;
		if( keybuf[key6] == 1 )
			eventno = 4;
		if( keybuf[key7] == 1 )
			eventno = 8;
		if( (keybuf[key5] != 0) || (keybuf[key6] != 0) ){
			ret = E3DPosForward( ci[chara1].hsid, 5.0f );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			ret = E3DGetPos( ci[chara1].hsid, &(ci[chara1].pos) );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
		if( eventno == 0 ){
			if( keybuf[key1] != 0 ){
				ret = E3DPosForward( ci[chara1].hsid, 16.0f );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				ret = E3DGetPos( ci[chara1].hsid, &(ci[chara1].pos) );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				eventno = 1;
			}
			if( keybuf[key2] != 0 ){
				ret = E3DPosForward( ci[chara1].hsid, -16.0f );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				ret = E3DGetPos( ci[chara1].hsid, &(ci[chara1].pos) );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				eventno = 1;
			}
			if( keybuf[key3] != 0 ){
				ret = E3DRotateY( ci[chara1].hsid, 2.0f );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				eventno = 1;
			}
			if( keybuf[key4] != 0 ){
				ret = E3DRotateY( ci[chara1].hsid, -2.0f );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				eventno = 1;
			}
		}
		if( (ci[chara1].motid == *(motionidptr + MOT_MOCHIAGE)) && (ci[chara1].nage == 0) && 
			(ci[chara2].nagerare == 0) ){
			ret = E3DDirToTheOther( ci[chara1].hsid, ci[chara2].hsid );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}

			D3DXVECTOR3 diff;
			diff = ci[chara1].pos - ci[chara2].pos;
			float mag;
			mag = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
			if( mag < 900000.0f ){
				ci[chara1].nage = 1;
				ci[chara2].nagerare = 1;
				ci[chara2].nagevy = NAGEY;
				D3DXVECTOR3 bonepos;
				ret = E3DGetBonePos( ci[chara1].hsid, 18, 1, ci[chara1].motid, ci[chara1].frameno, &bonepos );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				ret = E3DGetDirQ2( ci[chara1].hsid, ci[chara1].qid );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
				D3DXVECTOR3 befvec( 0.0f, 0.0f, -1.0f );
				D3DXVECTOR3 dir;
				ret = E3DMultQVec( ci[chara1].qid, befvec, &dir );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				ci[chara2].pos.x = bonepos.x + dir.x * 300.0f;
				ci[chara2].pos.y = bonepos.y - 300.0f;
				ci[chara2].pos.z = bonepos.z + dir.z * 300.0f;
				ret = E3DSetPos( ci[chara2].hsid, ci[chara2].pos );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				ret = E3DGetBoneQ( ci[chara1].hsid, 18, ci[chara1].motid, ci[chara1].frameno, 2, ci[chara2].qid );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
				ret = E3DSetDirQ2( ci[chara2].hsid, ci[chara2].qid );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
				ret = E3DSetNewPoseByMOA( ci[chara2].hsid, 16 );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

			}
		}
		ret = E3DSetNewPoseByMOA( ci[chara1].hsid, eventno );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}else{

		ci[chara1].nagerare += 1;

		if( ci[chara1].nagerare < 50 ){	
			ret = E3DSetNewPoseByMOA( ci[chara1].hsid, 16 );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}

		}else{
			if( ci[chara1].nagerare < 120 ){
				ret = E3DGetDirQ2( ci[chara2].hsid, ci[chara2].qid );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				D3DXVECTOR3 befvec2( 0.0f, 0.0f, -1.0f );
				D3DXVECTOR3 dir2;
				ret = E3DMultQVec( ci[chara2].qid, befvec2, &dir2 );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				ci[chara1].pos.x += dir2.x * 20.0f;
				ci[chara1].pos.z += dir2.z * 20.0f;
				ci[chara1].nagevy += GRAVITY;
				ci[chara1].pos.y += ci[chara1].nagevy;

				if( ci[chara1].pos.y < GROUND_HEIGHT )
					ci[chara1].pos.y = GROUND_HEIGHT;
				ret = E3DSetPos( ci[chara1].hsid, ci[chara1].pos );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				ret = E3DSetNewPoseByMOA( ci[chara1].hsid, 32 );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

			}else{
				ci[chara1].nagerare = 0;
				ci[chara2].nage = 0;
				ci[chara1].pos.y = GROUND_HEIGHT;
				ret = E3DCopyQ( ci[chara1].qid, ci[chara2].qid );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				ret = E3DSetDirQ2( ci[chara1].hsid, ci[chara1].qid );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				ret = E3DSetPos( ci[chara1].hsid, ci[chara1].pos );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				ret = E3DSetNewPoseByMOA( ci[chara1].hsid, 0 );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

 			}	
		}
	}

	int tempframeno;
	ret = E3DGetMotionFrameNo( ci[chara1].hsid, &(ci[chara1].motid), &(ci[chara1].frameno) );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( ci[chara1].motid == 0 ){
		ret = E3DGetNextMotionFrameNo( ci[chara1].hsid, 0, &(ci[chara1].motid), &tempframeno );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

 	}







	return 0;
}

int MoveCamera()
{
	int ret;
	D3DXVECTOR3 center, diff;
	center = ( ci[CHARA_RED].pos + ci[CHARA_BLUE].pos ) * 0.5f;
	diff = ci[CHARA_BLUE].pos - ci[CHARA_RED].pos;

	if( (diff.x != 0.0f) || (diff.y != 0.0f) || (diff.z != 0.0f) ){
		float charadist;
		charadist = D3DXVec3Length( &diff );

		cameradist = 2000.0f + ( charadist * 0.33f );
		D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );
		D3DXVECTOR3 cross;
		D3DXVec3Cross( &cross, &diff, &up );
		D3DXVec3Normalize( &cross, &cross );
		campos.x = center.x + cross.x * cameradist;
		campos.y = center.y + cameraheight;
		campos.z = center.z + cross.z * cameradist;
		D3DXVECTOR3 camdir;
		camdir.x = center.x - campos.x;
		camdir.y = 0.0f;
		camdir.z = center.z - campos.z;
		int cameradiv = 3;

		ret = E3DSetCameraPos( campos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = E3DCameraLookAt( camdir, 0, cameradiv );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}
int ChkConfChara()
{
	int ret;
	int inviewflag;
	if( (ci[CHARA_RED].nagerare == 0) && (ci[CHARA_BLUE].nagerare == 0) ){ 
		ret = E3DChkConflictOBB( ci[CHARA_RED].hsid, -1, ci[CHARA_BLUE].hsid, -1, &confflag, &inviewflag ); 
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		confflag = 0;
	}
	if( confflag != 0 ){
		ret = E3DSeparateFrom( ci[CHARA_RED].hsid, ci[CHARA_BLUE].hsid, 6.0f );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = E3DSeparateFrom( ci[CHARA_BLUE].hsid, ci[CHARA_RED].hsid, 6.0f );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}
	ret = E3DGetPos( ci[CHARA_RED].hsid, &(ci[CHARA_RED].pos) );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = E3DGetPos( ci[CHARA_BLUE].hsid, &(ci[CHARA_BLUE].pos) );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}
int ChangeHP( int chara1, int chara2 )
{

	if( confflag != 0 ){
		if( (ci[chara1].motid == *( motionidptr + MOT_TSUKI )) && 
			(ci[chara2].motid != *( motionidptr + MOT_DEFENCE )) ){
			ci[chara2].hp -= 100;
		}
		if( (ci[chara1].motid == *( motionidptr + MOT_KICK )) && 
			(ci[chara2].motid != *( motionidptr + MOT_DEFENCE )) ){
			ci[chara2].hp -= 200;
		}
	}else{
		if( ci[chara1].nagerare != 0 ){
			ci[chara1].hp -= 40;
		}
	}


	return 0;
}
int ChkGameOver()
{
	int ret;

	if( ci[CHARA_RED].hp <= 0 ){
		gameflag = 1;
		char bgname[MAX_PATH];
		sprintf_s( bgname, MAX_PATH, "%s\\bmpfile\\bluewin.png", szMediaDir );
		ret = E3DCreateBG( scid, bgname, "dummy", 0.0f, 0.0f, 0, 50000.0f );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}
	if( ci[CHARA_BLUE].hp <= 0 ){
		gameflag = 1;
		char bgname[MAX_PATH];
		sprintf_s( bgname, MAX_PATH, "%s\\bmpfile\\redwin.png", szMediaDir );
		ret = E3DCreateBG( scid, bgname, "dummy", 0.0f, 0.0f, 0, 50000.0f ); 
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}


int Render()
{
	int ret;

	ret = E3DBeginScene( scid, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

		ret = E3DRender( scid, groundhsid, 0, 1, 0, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = E3DRender( scid, ci[CHARA_RED].hsid, 0, 1, 0, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = E3DRender( scid, ci[CHARA_BLUE].hsid, 0, 1, 0, 0 );
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

	E3DSetBeforePos( ci[CHARA_RED].hsid );
	E3DSetBeforePos( ci[CHARA_BLUE].hsid );

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
	texcol.a = 255, texcol.r = 0; texcol.g = 0; texcol.b = 255;
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 20;
	sprintf_s( mes, 1024, "blue HP %d,    red HP %d", ci[CHARA_BLUE].hp, ci[CHARA_RED].hp );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	texpos.y += 20;
	sprintf_s( mes, 1024, "confflag %d", confflag );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 20;
	sprintf_s( mes, 1024, "red nage %d, nagerare %d, pos(%f, %f, %f)", ci[CHARA_RED].nage, ci[CHARA_RED].nagerare,
		ci[CHARA_RED].pos.x, ci[CHARA_RED].pos.y, ci[CHARA_RED].pos.z );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	texpos.y += 20;
	sprintf_s( mes, 1024, "blue nage %d, nagerare %d, pos(%f, %f, %f)", ci[CHARA_BLUE].nage, ci[CHARA_BLUE].nagerare,
		ci[CHARA_BLUE].pos.x, ci[CHARA_BLUE].pos.y, ci[CHARA_BLUE].pos.z );
	ret = E3DDrawText( texpos, 1.0, texcol, mes );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}





