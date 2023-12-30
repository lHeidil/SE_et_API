// test.cpp : définit le point d'entrée pour l'application.
//

#include "stdafx.h"
#include "test.h"

#define MAX_LOADSTRING 100

// Variables globales :
HINSTANCE hInst;								// instance actuelle
TCHAR szTitle[MAX_LOADSTRING];					// Le texte de la barre de titre
TCHAR szWindowClass[MAX_LOADSTRING];			// le nom de la classe de fenêtre principale

// Pré-déclarations des fonctions incluses dans ce module de code :
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

/*Entry point for the application.Parameters: the instance,
previous instance, command line, and initial show state*/
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//UNREFERENCED_PARAMETER is used to avoid compiler warnings about unused parameters
	UNREFERENCED_PARAMETER(hPrevInstance); 
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO : placez ici le code.
	//Variables for handling Windows messages and accelerators.
	MSG msg;
	HACCEL hAccelTable;

	// Initialise les chaînes globales
	//Load strings for the title and window class from the resource file.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Effectue l'initialisation de l'application :
	if (!InitInstance (hInstance, nCmdShow))
	{ 
		return FALSE; //Exit if unsuccessful
	}
	//Load accelerators 
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST));

	// Boucle de messages principale :
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FONCTION : MyRegisterClass()
//
//  BUT : inscrit la classe de fenêtre.
//
//  COMMENTAIRES :
//
//    Cette fonction et son utilisation sont nécessaires uniquement si vous souhaitez que ce code
//    soit compatible avec les systèmes Win32 avant la fonction 'RegisterClassEx'
//    qui a été ajoutée à Windows 95. Il est important d'appeler cette fonction
//    afin que l'application dispose des petites icônes correctes qui lui sont
//    associées.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	//this combination means to redraw the window when there is a vertical or horizontal changes
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	//no dwExStyle means no extended window styles are being set
	wcex.lpfnWndProc	= WndProc; // Window procedure callback
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	
	//(HBRUSH) is a handle to a brush. This type is declared in WinDef.h
	//COLOR_WINDOWis a constant representing a system-defined color. It is part of the Windows API
	//and is used to specify the default background color of a window.
	//wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);

	//A COLORREF structure that specifies the color of the brush.
	//The color specifies an RGB value and can be constructed with the RGB macro in WINDOWS.H. 
	wcex.hbrBackground = CreateSolidBrush(RGB(128, 0, 128)); // Violet
	
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FONCTION : InitInstance(HINSTANCE, int)
//
//   BUT : enregistre le handle de l'instance et crée une fenêtre principale
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        créons et affichons la fenêtre principale du programme.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Stocke le handle d'instance dans la variable globale

   /*The CreateWindow function creates an overlapped, pop-up, or child window.
   It specifies the window class, window title, window style, and (optionally)
   the initial position and size of the window. The function also specifies the
   window's parent or owner, if any, and the window's menu.*/
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FONCTION : WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  BUT :  traite les messages pour la fenêtre principale.
//
//  WM_COMMAND	- traite le menu de l'application
//  WM_PAINT	- dessine la fenêtre principale
//  WM_DESTROY	- génère un message d'arrêt et retourne
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND: //Certain commands generated by menu items, toolbar buttons, or accelerator keys
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Analyse les sélections de menu :
		switch (wmId)
		{
		case IDM_ABOUT:
			//Display the About dialog
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			//Close the main window
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT: //The WM_PAINT message is sent when the UpdateWindow or RedrawWindow member function is called.
		// Handle painting the main window
		hdc = BeginPaint(hWnd, &ps);
		// TODO : ajoutez ici le code de dessin...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY: //OnDestroy is called after the CWnd object is removed from the screen. 
		// Post a quit message to exit the application
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		// Initialization of the About dialog
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			// Close the About dialog on OK or Cancel button click
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
