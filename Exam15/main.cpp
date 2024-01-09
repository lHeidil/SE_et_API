#include "Wind.h"
#include <windowsx.h>
#include <string>
//#include "Resource.h"

/****************************** classe ComplexWind *******************************/ 

class CodeWind: public CWind{
	LPCTSTR code;
	static const COLORREF bkgndColors[3];
	static const RECT rpos;
	CWind *pb11, *pb12, *pb13,*pb21, *pb22, *pb23,*pb31, *pb32, *pb33,*pb41, *pb42, *pb43, *ps;
	int time,timeToClose;
	LRESULT WmTimer(HWND hWnd, WPARAM wP, LPARAM lP){
		::SetFocus(m_hWnd);
		::SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)(LONG_PTR)CreateSolidBrush(bkgndColors[time]));
		timeToClose--;
		time++;
		if (time==3) time=0;
		if (timeToClose==0) ::SendMessage(hWnd, WM_CLOSE,0,0);
		::InvalidateRect(m_hWnd,NULL,TRUE);	
		return 0;
	}
  virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP)
  {
	  return ::SendMessage(hWnd,WM_NCLBUTTONDOWN,HTCAPTION,0);
  }

  virtual LRESULT WmKeyDown(HWND hWnd, WPARAM wP, LPARAM lP){
	  switch(wP){
		case VK_ESCAPE: //escape key
			
			return ::SendMessage(hWnd,WM_CLOSE,0,0);  
		default: return -1;
	  }
  }
  LRESULT WmPaint(HWND hWnd, WPARAM wP, LPARAM lP)
	{ 
    RECT r; ::GetClientRect(hWnd, &r);  //GetClientRect instead of GetWindowRect for client area dimensions
    PAINTSTRUCT ps;//pour tout les app qui dessine
    HDC dc=::BeginPaint(hWnd,&ps);

	//dessiner le text
    //tout passe entre ses 2 lignes
    HBRUSH hb= ::CreateSolidBrush(0x0000FF);
    HGDIOBJ hb_old=::SelectObject(dc,hb);
    //hnbd2 nrsm hna

	TCHAR buf[14];

	_stprintf_s(buf,_T("Taper le code"));

	#ifdef USE_CONSOLE
	printf("%d %c %c \n",(int)_tcslen(buf),buf[2],*code); //test:printing length, a char from the buffer, a char from LPCSTR
	#endif //USE_CONSOLE

	::SetBkMode(dc,TRANSPARENT);
	::SetTextColor(dc,RGB(0, 0, 0));
	::SetTextAlign(dc,TA_LEFT);
	::TextOut(dc,r.left+10,r.top+12,buf,(int)_tcslen(buf));

	//lzm da fla5er
    ::SelectObject(dc,hb_old); //lzm arg3ha zy mkant
    ::DeleteObject(hb); //destroy el obj

    //da el ligne lli mbeno
    ::EndPaint(hWnd,&ps);

  return 0;
  }
public:
	CodeWind(LPCTSTR _code=_T("2015")):
		CWind(0x00FF00,_T(""),&rpos,WS_VISIBLE | WS_POPUPWINDOW ,0, CClsWind::DEF_STYLE),code(_code),time(0),timeToClose(6)
		{
			::SetTimer(m_hWnd,1,2000,::TIMERPROC()); //timerID=1, interruptTime=2000us
			RECT r={0,0,80,40};
			r.top+=40;
			pb11=new CWind(this, 1, _T("1"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
			r.left+=80;
			pb12=new CWind(this, 2, _T("2"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
			r.left+=80;
			pb13=new CWind(this, 3, _T("3"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
			
			r.top+=40;
			r.left-=160;
			pb21=new CWind(this, 4, _T("4"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
			r.left+=80;
			pb22=new CWind(this, 5, _T("5"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
			r.left+=80;
			pb23=new CWind(this, 6, _T("6"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));

			r.top+=40;
			r.left-=160;
			pb31=new CWind(this, 7, _T("7"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
			r.left+=80;
			pb32=new CWind(this, 8, _T("8"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
			r.left+=80;
			pb33=new CWind(this, 9, _T("9"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));

			r.top+=40;
			r.left-=160;
			pb41=new CWind(this, 10, _T("*"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
			r.left+=80;
			pb42=new CWind(this, 11, _T("0"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
			r.left+=80;
			pb43=new CWind(this, 12, _T("#"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
		}

	~CodeWind()
	{
		if (m_hWnd != 0){

			::KillTimer(m_hWnd, 1);
			::DestroyWindow(m_hWnd);
	
			if(pb11) delete pb11;
			if(pb12) delete pb12;
			if(pb13) delete pb13;
			if(pb21) delete pb21;
			if(pb22) delete pb22;
			if(pb23) delete pb23;
			if(pb31) delete pb31;
			if(pb32) delete pb32;
			if(pb33) delete pb33;
			if(pb41) delete pb41;
			if(pb42) delete pb42;
			if(pb43) delete pb43;
		}
	}
};
_declspec(selectany) const RECT CodeWind::rpos={0,0,240,200};
_declspec(selectany) const COLORREF CodeWind::bkgndColors[3]={0x00FFFF,0x0000FF,0x00FF00};

/******************************** tests unitaires ***************************/

int test1() // points A-E
{
  CodeWind w1;
  return CWind::MsgLoop();
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	test1();
	return 0;
}
