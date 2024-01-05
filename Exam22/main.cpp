#include "Wind.h"
#include <windowsx.h>
#include "Resource.h"

/****************************** classe ComplexWind *******************************/ 

class ComplexWind: public CWind{
	HMENU menu_hnd;
	HBRUSH hb;
	POINT lastPoint;
	virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP)//important to start drawing from these coordinates when you stop and restart not from the last point
  {
    int x = GET_X_LPARAM(lP);
    int y = GET_Y_LPARAM(lP);	
	lastPoint.x = x;
    lastPoint.y = y;
	return 0;
  }
  virtual LRESULT WmLButtonMove(HWND hWnd, WPARAM wP, LPARAM lP)
  {
	  if(wP==MK_LBUTTON)
	  {
		int x = GET_X_LPARAM(lP);
		int y = GET_Y_LPARAM(lP);

		HDC dc=::GetDC(hWnd);
		HBRUSH hb= hb= ::CreateSolidBrush(0xFFFF00);
		HGDIOBJ hb_old=::SelectObject(dc,hb);

		::Ellipse(dc,lastPoint.x,lastPoint.y,x,y);

		::SelectObject(dc, hb_old);
		::DeleteObject(hb);
		::ReleaseDC(hWnd,dc); //lzm arg3ha zy mkant!!

		lastPoint.x = x;
		lastPoint.y = y;
		
	  }
	  return 0;
  }
	void SetBack(COLORREF color)
	{
		//solution tanya ashl btlwn el bkground
		//SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)(LONG_PTR)CreateSolidBrush(color)); 
		 
		//solution eli 3yzha
		HDC dc= ::GetDC(m_hWnd);
		 hb= ::CreateSolidBrush(color);
		HGDIOBJ hb_old=::SelectObject(dc,hb);
		
		InvalidateRect(m_hWnd, NULL, TRUE);
		
        ::SelectObject(dc,hb_old); //lzm arg3ha zy mkant
	}
	LRESULT WmCommand(HWND hWnd, WPARAM wP, LPARAM lP) 
	{
		switch(LOWORD(wP))
		{
		case ID_APPLICATION_QUITTER:
			//::SendMessage(hWnd, WM_CLOSE,0,0);break;
			::DestroyWindow(m_hWnd);break;
		case ID_APPLICATION_QUITTERDANS3S:
			::SetTimer(m_hWnd,1,3000,(TIMERPROC) NULL);break; //timerID=1, interruptTime=3000us
		case ID_APPLICATION_QUITTERDANS10S:
			::SetTimer(m_hWnd,2,10000,(TIMERPROC) NULL);break; //timerID=2, interruptTime=10000us
		case ID_FEN40001:
			SetBack(0xFFFFFF);break;
		case ID_FEN40002:
			SetBack(0x0000FF);break;
		case ID_FEN40003:
			SetBack(0x00FF00);break;
		case ID_FEN40004:
			SetBack(0xFF0000);break;
		case ID_FEN40005:
			menu_hnd= ::GetMenu(m_hWnd);
			::SetMenu(m_hWnd,0);
			::SetWindowLongPtr(m_hWnd,GWL_STYLE,WS_VISIBLE | WS_POPUP | WS_EX_TOPMOST); //tc mn topmost w hya style msh exStyle
			::SetWindowPos(m_hWnd,0,0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN),SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED); //shlt nomove w nosize
			break;
		default: return -1;
		}
		return 0;
	}
	 virtual LRESULT WmKeyDown(HWND hWnd, WPARAM wP, LPARAM lP){
	  switch(wP){
		case VK_ESCAPE: //escape key
			//CWind::SetStyle(CWind::DEF_STYLE); //msh mfrod a3ml kda
			::SetMenu(m_hWnd,menu_hnd);break;
		case VK_DELETE :
			::InvalidateRect(hWnd, NULL, TRUE);break; //redraw since it's not persistance so it will erase
		case 'W':
			SetBack(0xFFFFFF);break;
		case 'R':
			SetBack(0x0000FF);break;
		case 'V':
			SetBack(0x00FF00);break;
		case 'B':
			SetBack(0xFF0000);break;
		default: return -1;
		}
		return 0;
	}
	LRESULT WmClose(HWND hWnd, WPARAM wP, LPARAM lP) { 
     return ::MessageBox(m_hWnd,_T("Voulez-vous fermer la fen�tre?"),_T("Cloture"),MB_YESNO | MB_ICONQUESTION)==IDYES?-1:0;}

	LRESULT WmTimer(HWND hWnd, WPARAM wP, LPARAM lP){::DestroyWindow(m_hWnd);return 0;}
	
	LRESULT WmPaint(HWND hWnd, WPARAM wP, LPARAM lP)
	{ 
    RECT r; ::GetClientRect(hWnd, &r);  //GetClientRect instead of GetWindowRect for client area dimensions
	PAINTSTRUCT ps;//pour tout les app qui dessine
    HDC dc=::BeginPaint(hWnd,&ps);

	//background filling
	::FillRect(dc,&r,hb);
	::DeleteObject(hb); //destroy el obj lma b3mlha fo2 mbtshtghlsh
	//::SetBkColor(dc,::GetDCBrushColor(dc)); di msh btshtghl lma b7otha t7t btlwn l text lma tb2a opaque

    //tout passe entre ses 2 lignes
    HBRUSH hb= ::CreateSolidBrush(0x0000FF);
    HGDIOBJ hb_old=::SelectObject(dc,hb);
    //hnbd2 nrsm hna
    //::SetBkColor(dc,color);

	::SetBkMode(dc,TRANSPARENT);
	TCHAR buf[10];
    wsprintf(buf,_T("Heidi GAD"));
    ::SetTextColor(dc,0x000000);
	::SetTextAlign(dc,TA_RIGHT);
	::TextOut(dc,r.right,r.bottom-20,buf,(int)_tcslen(buf));

    //lzm da fla5er
    ::SelectObject(dc,hb_old); //lzm arg3ha zy mkant
    ::DeleteObject(hb); //destroy el obj
    //da el ligne lli mbeno
    ::EndPaint(hWnd,&ps);

  return 0;
  }
public:
	ComplexWind(LPCTSTR name = _T("Fen�tre complexe"), const RECT* pR=0):
		CWind(name,pR,WS_VISIBLE | WS_OVERLAPPEDWINDOW,0,0,IDR_MENU1),menu_hnd(0){}
	~ComplexWind(){
		if (m_hWnd != 0){
			::KillTimer(m_hWnd, 1);
			::DestroyWindow(m_hWnd);
		}}
};

/******************************** tests unitaires ***************************/

int test1() // points A-E
{
  RECT r={0,0,500,500};
  ComplexWind w1(_T("Fen�tre complexe"),&r);
  return CWind::MsgLoop();
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
  // test point F � ins�rer ici
  HANDLE hMtxApp =::CreateMutex(0,true,_T("Exam22-Mutex-Unique")); //avant la ceation d'objet , true pour prendre qu debut
  if(hMtxApp==0) //chance 0
  {
    //::MessageBox(0,_T("Erreur handle NULL"), _T("Avertissement"),MB_OK);
    return 0;
  }
  if(::GetLastError()==ERROR_ALREADY_EXISTS) //quand c'est  la 2eme processus ou plus apres la premiere
  {
    ::MessageBox(0,_T("Deuxieme ouverture! Impossible!"), _T("Avertissement"),MB_OK);
  }
  else 
  {
    //::MessageBox(0,_T("Je suis seul actuellement"), _T("Avertissement"),MB_OK);
	test1();
    ::ReleaseMutex(hMtxApp);
  }

  ::CloseHandle(hMtxApp);
  return 0;
}