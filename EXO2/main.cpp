#include "Wind.h"
//#include "resource.h"
#include <windowsx.h>

/******************************** tests unitaires ***************************/
class CTimerWind: public CWind  
{
	int time;
	TCHAR buffer[20];
	LRESULT WmTimer(HWND hWnd, WPARAM wP, LPARAM lP)
	{
		time++;
        _stprintf_s(buffer, _T("%d"), time); //converting int to LPCTSTR
		//printf("timeeeerrr: %d\n",time);
		SetWindowText(hWnd, buffer);
		return 0;
	}
	
  public:  
  CTimerWind(COLORREF color):CWind(color),time(0)	{::SetTimer(m_hWnd,1,1000,::TIMERPROC());} //timerID=1, interruptTime=1000us
  ~CTimerWind()	{if (m_hWnd != 0){::KillTimer(m_hWnd, 1);::DestroyWindow(m_hWnd);}}
};
class CWinNoBorder: public CWind
{
protected:
  virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP) //JAMAIS MODIFIER LA CLASSE MERE!!
  { return ::SendMessage(hWnd,WM_NCLBUTTONDOWN,HTCAPTION,0);  }
  virtual LRESULT WmRButtonDown(HWND hWnd, WPARAM wP, LPARAM lP)
  { return ::SendMessage(hWnd,WM_CLOSE,0,0);    }
public:
  CWinNoBorder(COLORREF Color,LPCTSTR WndName= _T(""), const RECT* pR=0,bool _ismain=false):CWind(Color,_T(""),pR,WS_VISIBLE | WS_POPUP){}
};

class CCloseWind: public CWind
{
protected:
   virtual LRESULT WmClose(HWND hWnd, WPARAM wP, LPARAM lP) 
   { 
     return ::MessageBox(m_hWnd,_T("Vouler vous fermer la fenetre?"),_T("Fermiture"),MB_YESNO | MB_ICONQUESTION)==IDYES?-1:0;
   }
public:
  CCloseWind(COLORREF Color,LPCTSTR WndName= _T(""), const RECT* pR=0):CWind(Color,WndName,pR){}
};

class CGlueWind: public CWind
{
protected:
   virtual LRESULT WmMouseMove(HWND hWnd, WPARAM wP, LPARAM lP) 
   {
     if (wP!=0) return -1; //envoie au proc par def
     RECT r; ::GetWindowRect(hWnd,&r);
     POINT p={GET_X_LPARAM(lP),GET_Y_LPARAM(lP)};

     ::ClientToScreen(hWnd,&p);
     ::MoveWindow(hWnd,p.x-(r.right-r.left)/2,p.y-(r.bottom-r.top)/2,r.right-r.left,r.bottom-r.top,false); //understand p and r...
     return 0;
   }
public:
  CGlueWind(COLORREF Color,LPCTSTR WndName= _T(""), const RECT* pR=0):CWind(Color,WndName,pR){}
};


int test2A()
{
  CClsWind cls1(CWind::WndProc,CClsWind::DEF_STYLE,0xFF0000);
  CClsWind cls2(CWind::WndProc,CClsWind::DEF_STYLE,0xFFFFFF);
  CClsWind cls3(CWind::WndProc,CClsWind::DEF_STYLE,0x0000FF);
  RECT r1={100,200,200,200};RECT r2={350,200,200,200};RECT r3={600,200,200,200};
  CWind w1(_T("1ere fenetre"),&r1,WS_VISIBLE | WS_OVERLAPPEDWINDOW,0,cls1);
  CWind w2(_T("2eme fenetre"),&r2,WS_VISIBLE | WS_OVERLAPPEDWINDOW,0,cls2);
  CWind w3(_T("3eme fenetre"),&r3,WS_VISIBLE | WS_OVERLAPPEDWINDOW,0,cls3);
  w1.SetMain(true);w2.SetMain(false);w3.SetMain(false);
  return CWind::MsgLoop();
}
int test2B()
{
  RECT r1={100,200,200,200};RECT r2={350,200,200,200};RECT r3={600,200,200,200};RECT r4={100,500,200,200};RECT r5={600,500,200,200};
  CWinNoBorder w1(0xFF0000,_T("1ere fenetre"),&r1);
  CWinNoBorder w2(0x0000FF,_T("2eme fenetre"),&r2);
  CWinNoBorder w3(0xFFFF00,_T("3eme fenetre"),&r3);
 
  
  return CWind::MsgLoop();
}
int test2C()
{
  RECT r1={100,200,200,200};RECT r2={350,200,200,200};RECT r3={600,200,200,200};RECT r4={100,500,200,200};RECT r5={600,500,200,200};
  CCloseWind w1(0xFF0000,_T("1ere fenetre"),&r1);
  CCloseWind w2(0x0000FF,_T("2eme fenetre"),&r2);
  CCloseWind w3(0xFFFF00,_T("3eme fenetre"),&r3);
  w1.SetMain(true);w2.SetMain(false);w3.SetMain(false);
  return CWind::MsgLoop();
}

int test2D()
{
  CTimerWind w1(0x000000);
  return CWind::MsgLoop();
}
int test2E()
{
  RECT r1={0,0,200,200};
  CGlueWind w1(0xFFFF00,_T("la fenetre qui colle"),&r1);
  return CWind::MsgLoop();
}




int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
  //return test2A();
  //return test2B();
  //return test2C();
  //return test2D();
  //return test2E();
}

