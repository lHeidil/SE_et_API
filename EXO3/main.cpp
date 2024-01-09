#include "Wind.h"
#include "resource.h"
#include <windowsx.h>
#include <string>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
/******************************** tests unitaires ***************************/
class CMenuWind: public CWind
{
	static const RECT rpos;
	LRESULT WmCommand(HWND hWnd, WPARAM wP, LPARAM lP) 
	{
		switch(LOWORD(wP))
		{
		case ID_FICHIER_QUITTER:
			::SendMessage(hWnd, WM_CLOSE,0,0);
			break;
		default: return -1;
		}
		return 0;
	}
public :
	CMenuWind(LPCTSTR name = _T("Menu Window")):
		CWind(name,&rpos, WS_POPUP| WS_CAPTION |WS_VISIBLE, 0,0,IDR_MENU1){}
};
_declspec(selectany) const RECT CMenuWind::rpos={0, 0, 500, 500};

class CDessineWind: public CWind //charge non-persistant
{
  COLORREF pen_color;
  int pen_width;
  POINT lastPoint;
protected:
  virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP)//important to start drawing from these coordinates when you stop and restart not from the last point
  {
    int x = GET_X_LPARAM(lP);
    int y = GET_Y_LPARAM(lP);	
	lastPoint.x = x;
    lastPoint.y = y;
	return 0;
  }
  virtual LRESULT WmMouseMove(HWND hWnd, WPARAM wP, LPARAM lP)
  {
	  if(wP==MK_LBUTTON)
	  {
		int x = GET_X_LPARAM(lP);
		int y = GET_Y_LPARAM(lP);

		HDC dc=::GetDC(hWnd);
		HPEN hp= ::CreatePen(PS_SOLID,pen_width,pen_color);
		HGDIOBJ hp_old=::SelectObject(dc,hp);

		/*MoveToEx is used to set the current position in the specified device context.
		It does not draw anything by itself; it just moves the "pen" to a new starting point for subsequent drawing operations.*/
		::MoveToEx(dc,lastPoint.x,lastPoint.y,(LPPOINT) NULL);
		::LineTo(dc, x, y); //(x, y) are the coordinates of the endpoint of the line and i get the starting point from MoveToEx.

		::SelectObject(dc, hp_old);
		::DeleteObject(hp);
		::ReleaseDC(hWnd,dc); //lzm arg3ha zy mkant!!

		lastPoint.x = x;
		lastPoint.y = y;
	  }
	  return 0;
  }
  virtual LRESULT WmKeyDown(HWND hWnd, WPARAM wP, LPARAM lP)
  {
	  printf("\nkeeeeeeeeeeeeey: %d\n\n",wP);
	  switch(wP){
		case VK_ESCAPE: //escape key
			return ::SendMessage(hWnd,WM_CLOSE,0,0);
		case VK_BACK: //backspace key
			::InvalidateRect(hWnd, NULL, TRUE); //redraw since it's not persistance so it will erase
			return 0;
		case 'C':
			pen_color= RGB(rand() % 256, rand() % 256, rand() % 256);
			return 0;
		case 'W':
			pen_width++;
			return 0;
		case 'E':
			pen_width--;
			return 0;
	  return 0;
	  }
  }
  public:
	  void SetPenColor(COLORREF pcolor){pen_color = pcolor;}
	  void SetPenWidth(int pwidth){pen_width = pwidth;}

  CDessineWind(COLORREF Color, const RECT* pR=0):CWind(Color,_T(""),pR,WS_VISIBLE | WS_POPUP | WS_EX_TOPMOST,0,0),pen_color(0xFFFFFF),pen_width(3){}
};

class CWinNoBorder: public CWind
{
protected:
  virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP) //JAMAIS MODIFIER LA CLASSE MERE!!
  { return ::SendMessage(hWnd,WM_NCLBUTTONDOWN,HTCAPTION,0);  }
  virtual LRESULT WmRButtonDown(HWND hWnd, WPARAM wP, LPARAM lP)
  { return ::SendMessage(hWnd,WM_CLOSE,0,0);    }
public:
  CWinNoBorder(COLORREF Color, const RECT* pR=0):CWind(Color,_T(""),pR,WS_VISIBLE | WS_POPUP){}
};

class CFormWind: public CWinNoBorder
{
	
public:	
	void SetRound(int x,int y,int w,int h)
	{
		HRGN hRgn = ::CreateEllipticRgn(x,y,w,h);
		::SetWindowRgn(m_hWnd,hRgn,true);
	}
	void SetTriangle(CONST POINT *p)
	{
		HRGN hRgn = ::CreatePolygonRgn(p,3,WINDING);
		::SetWindowRgn(m_hWnd,hRgn,true);
	}
	void SetRoundRect(int x,int y,int w,int h,int h_ellipse,int w_ellipse)
	{
		HRGN hRgn = ::CreateRoundRectRgn(x,y,w,h,h_ellipse,w_ellipse);
		::SetWindowRgn(m_hWnd,hRgn,true);
	}

	CFormWind(COLORREF Color,const RECT* pR=0):CWinNoBorder(Color,pR){}
};

class CTranspWind: public CWind 
{
  BYTE tran;
  static const RECT rpos;
protected:
 
  virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP)
  {
    if(tran>20) tran-=20;
    ::SetLayeredWindowAttributes(m_hWnd,0,tran,LWA_ALPHA);
	  return 0;
  }
  virtual LRESULT WmRButtonDown(HWND hWnd, WPARAM wP, LPARAM lP) 
  { 
    if(tran<235) tran+=20;
    ::SetLayeredWindowAttributes(m_hWnd,0,tran,LWA_ALPHA); //lzm b3d kol modification a3mlha Appelle
	  return 0;
  }
  public:
  CTranspWind(LPCTSTR WndName= _T("")):CWind(WndName,&rpos),tran(128)//128 nos el transparence
  {
    SetExStyle(WS_EX_LAYERED);
    ::SetLayeredWindowAttributes(m_hWnd,0,tran,LWA_ALPHA);
  } 
};
const RECT CTranspWind::rpos={0,0,200,200};

class CPaintCerclesV2: public CWind //charge persistant
{
  static const int R=40,PAS=100,OFS=50; //pas = distance between cercles
  static const int MAX_CLICK = 50;
  struct InfoClick
  {
    POINT pos;
    bool isLeft;
  } info[MAX_CLICK];
  int nb_click;


protected:
  virtual LRESULT WmPaint(HWND hWnd, WPARAM wP, LPARAM lP) //JAMAIS MODIFIER LA CLASSE MERE!!
  { 
    RECT r; ::GetWindowRect(hWnd,&r);
    PAINTSTRUCT ps;//pour tout les app aui dessine
    HDC dc=::BeginPaint(hWnd,&ps);
    //tout passe entre ses 2 lignes
    HBRUSH hb= ::CreateSolidBrush(0x0000FF);
    HGDIOBJ hb_old=::SelectObject(dc,hb);
    //hnbd2 nrsm hna
    for (int j=0;j<r.bottom/PAS+1;j++)
      for (int i=0;i<r.right/PAS+1;i++)
        ::Ellipse(dc,OFS+i*PAS-R,OFS+j*PAS-R,OFS+i*PAS+R,OFS+j*PAS+R);
    //lzm da fla5er
    ::SelectObject(dc,hb_old); //lzm arg3ha zy mkant
    ::SetBkMode(dc,TRANSPARENT);
    for (int i=0;i<nb_click;i++)
      {
        TCHAR buf[32];
        wsprintf(buf,_T("(%d,%d)"),info[i].pos.x,info[i].pos.y);
        ::SetTextColor(dc,info[i].isLeft? 0x00FF00:0x00FFFF);
        ::TextOut(dc,info[i].pos.x,info[i].pos.y,buf,(int)_tcslen(buf));
      }
    ::DeleteObject(hb); //destroy el obj
    //da el ligne lli mbeno
    ::EndPaint(hWnd,&ps);
  return 0;
  }
  virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP)
  {
    if(nb_click >= MAX_CLICK) return -1;
    info[nb_click].pos.x = LOWORD(lP);
    info[nb_click].pos.y = HIWORD(lP);
    info[nb_click++].isLeft=true; //interdit d'appler wm_paint directement
    ::InvalidateRect(hWnd,0,false); //zy lma a5rgo bara el shasha 5ales f bydtr yrsmha  f7na bn2olo eno msh valid 3shan 7tena 0

	  return 0;
  }
  virtual LRESULT WmRButtonDown(HWND hWnd, WPARAM wP, LPARAM lP) //non-persistance pas d'actualiter
  { 
  
    if(nb_click >= MAX_CLICK) return -1;
    info[nb_click].pos.x = LOWORD(lP);
    info[nb_click].pos.y = HIWORD(lP);
    info[nb_click++].isLeft=false; //interdit d'appler wm_paint directement
    ::InvalidateRect(hWnd,0,false); //zy lma a5rgo bara el shasha 5ales f bydtr yrsmha  f7na bn2olo eno msh valid 3shan 7tena 0

	  return 0;
  }
  public:
  CPaintCerclesV2(COLORREF Color,LPCTSTR WndName= _T(""), const RECT* pR=0,
    UINT WndStyle=DEF_STYLE):CWind(Color,WndName,pR,WndStyle,0,0),nb_click(0){} //lma 7tena 0 f classStyle mb2ash byrsm kol 7ga b2a byrsm bs el goz2 elgdiid
};

class CPaintCercles: public CWind //charge persistant
{
  static const int R=40,PAS=100,OFS=50; //pas distance between cercles
protected:
  virtual LRESULT WmPaint(HWND hWnd, WPARAM wP, LPARAM lP) //JAMAIS MODIFIER LA CLASSE MERE!!
  { 
    RECT r; ::GetWindowRect(hWnd,&r);
    PAINTSTRUCT ps;//pour tout les app aui dessine
    HDC dc=::BeginPaint(hWnd,&ps);
    //tout passe entre ses 2 lignes
    HBRUSH hb= ::CreateSolidBrush(0x0000FF);
    HGDIOBJ hb_old=::SelectObject(dc,hb);
    //hnbd2 nrsm hna
    for (int j=0;j<r.bottom/PAS+1;j++)
      for (int i=0;i<r.right/PAS+1;i++)
        ::Ellipse(dc,OFS+i*PAS-R,OFS+j*PAS-R,OFS+i*PAS+R,OFS+j*PAS+R);
    //lzm da fla5er
    ::SelectObject(dc,hb_old); //lzm arg3ha zy mkant
    ::DeleteObject(hb); //destroy el obj
    //da el ligne lli mbeno
    ::EndPaint(hWnd,&ps);
  return 0;
  }
  virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP)
  {
    TCHAR buf[32];
    int x = GET_X_LPARAM(lP);
    int y = GET_Y_LPARAM(lP);
    wsprintf(buf,_T("(%d,%d)"),x,y);
    HDC dc=::GetDC(hWnd);
    ::SetTextColor(dc,0x00FF00);
    ::SetBkMode(dc,TRANSPARENT);

    ::TextOut(dc,x,y,buf,(int)_tcslen(buf));

    ::ReleaseDC(hWnd,dc); //lzm arg3ha zy mkant
	  return 0;
  }
  virtual LRESULT WmRButtonDown(HWND hWnd, WPARAM wP, LPARAM lP) //non-persistance pas d'actualiter
  { 
  
    TCHAR buf[32];
    int x = GET_X_LPARAM(lP);
    int y = GET_Y_LPARAM(lP);
    wsprintf(buf,_T("(%d,%d)"),x,y);
    HDC dc=::GetDC(hWnd);

    ::SetTextColor(dc,0x00FFFF);
    ::SetBkMode(dc,TRANSPARENT);
    ::TextOut(dc,x,y,buf,(int)_tcslen(buf));

    ::ReleaseDC(hWnd,dc); //lzm arg3ha zy mkant
	  return 0;}
  public:
  CPaintCercles(COLORREF Color,LPCTSTR WndName= _T(""), const RECT* pR=0,
    UINT WndStyle=DEF_STYLE):CWind(Color,WndName,pR,WndStyle,0,0){} //lma 7tena 0 f classStyle mb2ash byrsm kol 7ga b2a byrsm bs el goz2 elgdiid
};




class CWinImg : public CWind
{
    TCHAR imgFileName[MAX_PATH]; //constant is defined in the Windows headers
    bool fitToWindow;
	ULONG_PTR gdiplusToken;

    LRESULT WmPaint(HWND hWnd, WPARAM wP, LPARAM lP)
    {
        PAINTSTRUCT ps;
        HDC dc = ::BeginPaint(hWnd, &ps);

        Gdiplus::Graphics graphics(dc);
        Gdiplus::Image image(imgFileName);

        int w, h;

        if (fitToWindow){
            RECT cR;
            GetClientRect(m_hWnd, &cR);
            w = cR.right - cR.left;h = cR.bottom - cR.top;
        }
        else{w = image.GetWidth();h = image.GetHeight();}

        graphics.DrawImage(&image, 0, 0, w, h);

        ::EndPaint(hWnd, &ps);
        return 0;
    }

    LRESULT WmDropFiles(HWND hWnd, WPARAM wP, LPARAM lP)
    {
        HDROP hDrop = (HDROP)wP;
        UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

        if (fileCount > 0)
        {
            DragQueryFile(hDrop, 0, imgFileName, MAX_PATH);
            // Load the new image and redraw the window
            ::InvalidateRect(m_hWnd, NULL, TRUE);
        }

        DragFinish(hDrop);

        return 0;
    }

	LRESULT WmCommand(HWND hWnd, WPARAM wP, LPARAM lP) 
	{
		switch(LOWORD(wP))
		{
		case ID_FICHIER_QUITTER:
			::SendMessage(hWnd, WM_CLOSE,0,0);
			break;
		case ID_FICHIER_FIT:
			fitToWindow=true;
			break;
		case ID_FICHIER_DONT:
			fitToWindow=false;
			break;
		default: return -1;
		}
		return 0;
	}
public:
    CWinImg(const RECT* pR = 0, bool fitToWindow = false)
        : CWind(_T("WinImg"), pR, DEF_STYLE,0,0,IDR_MENU1), fitToWindow(fitToWindow)
    {
        DragAcceptFiles(m_hWnd, TRUE);
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    }
	~CWinImg(){ Gdiplus::GdiplusShutdown(gdiplusToken);}
};

class CHoleWind: public CWind
{
    int percentage; //80

    virtual LRESULT WmPaint(HWND hWnd, WPARAM wP, LPARAM lP)
    {
        PAINTSTRUCT ps;
        HDC dc = ::BeginPaint(hWnd, &ps);

        RECT cR;
        GetClientRect(m_hWnd, &cR);

        int diameter = min(cR.right - cR.left, cR.bottom - cR.top) * percentage / 100;
        int x = (cR.left + cR.right) / 2;
        int y = (cR.top + cR.bottom) / 2;

        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
        SelectObject(dc, hBrush);

        Ellipse(dc, x - diameter / 2, y - diameter / 2, x + diameter / 2, y + diameter / 2);

        DeleteObject(hBrush);

        ::EndPaint(hWnd, &ps);
        return 0;
    }
public:
    CHoleWind(LPCTSTR WndName= _T(""), const RECT* pR=0) : CWind(WndName,pR),percentage(80)
    {
		  SetExStyle(WS_EX_LAYERED);
		  ::SetLayeredWindowAttributes(m_hWnd,0,255,LWA_COLORKEY);
    }
};

class CEditCopy: public CWind
{
	static const int MAX_SZ=1024;
	static const RECT rpos;
	CWind *pb1, *pb2, *pe1, *pe2, *ps;
protected:
	virtual LRESULT WmCommand(HWND hWnd, WPARAM wP, LPARAM lP) 
	{
		switch(LOWORD(wP))
		{
		case ID_FICHIER_QUITTER:
			::SendMessage(hWnd, WM_CLOSE,0,0);
			break;
		case 1 :
			{
				//::MessageBox(hWnd,_T("Copie de haut en bas"),_T("Message"),MB_OK);
				TCHAR buf[MAX_SZ];
				::SendMessage(*pe2,WM_GETTEXT,MAX_SZ,(LPARAM)buf);
				::SendMessage(*pe1,WM_SETTEXT,0,(LPARAM)buf);
				::SendMessage(*ps,WM_SETTEXT,0,(LPARAM)_T("J'ai copié du haut en bas"));
			}
			break;
		case 2 :
			{
				//::MessageBox(hWnd,_T("Copie de bas en haut "),_T("Message"),MB_OK);
				TCHAR buf[MAX_SZ];
				::SendMessage(*pe1,WM_GETTEXT,MAX_SZ,(LPARAM)buf);
				::SendMessage(*pe2,WM_SETTEXT,0,(LPARAM)buf);
				::SendMessage(*ps,WM_SETTEXT,0,(LPARAM)_T("J'ai copié de bas en haut"));
			}
			break;
		default: return -1;
		}
		return 0;
	}
public :
	CEditCopy(LPCTSTR name = _T("Test Edit Copy")):
		CWind(0x808080, name, &rpos, WS_POPUPWINDOW| WS_CAPTION |WS_VISIBLE, 0, CClsWind::DEF_STYLE, IDR_MENU1)
	{
		RECT r={10,10,100,20};
		pb1=new CWind(this, 1, _T("Copy down"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
		r.top+=30; 
		pb2=new CWind(this, 2, _T("Copy up"), &r, WS_CHILD|WS_VISIBLE,0,_T("Button"));
		r.left+=180;
		pe1=new CWind(this, 4, _T("Text up "), &r, WS_CHILD|WS_VISIBLE|WS_BORDER,0,_T("Edit"));
		r.top-=30;
		pe2=new CWind(this, 3, _T("Text down "), &r, WS_CHILD|WS_VISIBLE|WS_BORDER,0,_T("Edit"));
		ps = new CWind(this, 5, _T("Status"),0,WS_CHILD|WS_VISIBLE,0,_T("msctls_statusbar32"));
	}

	~CEditCopy()
	{
		if(pb1) delete pb1;
		if(pb2) delete pb2;
		if(pe1) delete pe1;
		if(pe2) delete pe2;
		if(ps) delete ps;
	}
};
_declspec(selectany) const RECT CEditCopy::rpos={0, 0, 500, 500};

#ifdef _GDIPLUS_H
using namespace Gdiplus;
class CGDIPLUS
{
	ULONG_PTR token;
public:
	CGDIPLUS(){GdiplusStartupInput inp; GdiplusStartup(&token, &inp, 0);}
	~CGDIPLUS(){GdiplusShutdown(token);}
}glob_gdi;
#endif

class CWinGDIPlus:public CWind
{
protected: 
	virtual LRESULT WmPaint(HWND hWnd, WPARAM wP, LPARAM lP)
	{
		PAINTSTRUCT ps;
		HDC dc = ::BeginPaint(hWnd,&ps);
		PaintGDIPlus (Graphics(dc)); 
		::EndPaint(hWnd,&ps);
		return 0;
	}
	virtual void PaintGDIPlus(Graphics& graph)
	{
		Pen p(Color(255,0,255),6);
		graph.DrawLine(&p, 0, 0, 50, 50);
		RECT r; ::GetClientRect(m_hWnd,&r);
		graph.DrawEllipse(&p, RectF(5,5,REAL(r.right-10),REAL(r.bottom-10)));
	}
public:
	CWinGDIPlus(COLORREF Color=0xFFFFFF, LPCTSTR name = _T(""), const RECT *pR = 0, UINT style = DEF_STYLE, 
		UINT exStyle = 0, UINT ClsStyle=CClsWind::DEF_STYLE, WORD menuID = 0):
	CWind(Color,name, pR, style,exStyle, CClsWind::DEF_STYLE, menuID){}

};


class CWinJO:public CWinGDIPlus
{
protected: 
	void PaintGDIPlus(Graphics& graph)
	{
		RECT r; ::GetClientRect(m_hWnd,&r);
		Pen red(Color(255,0,0),20);
		Pen g(Color(0,255,0),20);
		Pen b(Color(0,0,250),20);
		Pen yw(Color(255,255,0),20);
		Pen bk(Color(0,0,0),20);

		int x=(r.left+r.right)/2;
		int y=(r.top+r.bottom)/2;
		int ray = (r.left+r.right)/8;

		graph.DrawEllipse(&b, RectF(REAL(x-(3.5*ray)),REAL(y - (1.5*ray)),REAL(2 *ray),REAL(2 *ray)));
		graph.DrawEllipse(&bk, RectF(REAL(x - ray), REAL(y - (1.5*ray)), REAL(2 * ray), REAL(2 * ray)));
		graph.DrawEllipse(&red, RectF(REAL(x+(1.5*ray)),REAL(y - (1.5*ray)),REAL(2 *ray),REAL(2 *ray)));
		graph.DrawEllipse(&yw, RectF(REAL(x-(2.25*ray)),REAL(y-(0.5*ray)),REAL(2 *ray),REAL(2 *ray)));
		graph.DrawEllipse(&g, RectF(REAL(x+(0.25*ray)),REAL(y-(0.5*ray)),REAL(2 *ray),REAL(2 *ray)));
	}
public:
	CWinJO(COLORREF Color=0xFFFFFF, LPCTSTR name = _T(""), const RECT *pR = 0, UINT style = DEF_STYLE, 
		UINT exStyle = 0, UINT ClsStyle=CClsWind::DEF_STYLE, WORD menuID = 0):
	CWinGDIPlus(Color,name, pR, style,exStyle, CClsWind::DEF_STYLE, menuID){}
};
int test3A()
{
  RECT r1={0,0,500,500};
  CPaintCercles w1(0xA04040,_T("cercle"),&r1);
  return CWind::MsgLoop();
}

int test3B()
{
  RECT r1={0,0,500,500},r2={600,0,500,500};
  CPaintCercles w1(0xA04040,_T("cercle persistant et text non persistant"),&r1);
  CPaintCerclesV2 w2(0xA04040,_T("cercle persistant et text persistant"),&r2);
  return CWind::MsgLoop();
}
int test3C()
{
  RECT r1={0,0,200,200}, r2={250,0,200,200}, r3={500,0,200,200};
  CFormWind w1(0xA0000FF,&r1);
  CFormWind w2(0x00FF00,&r2);
  CFormWind w3(0xA00FFFF,&r3);
  w1.SetRound(0,0,200,200);
  CONST POINT p[3]= {{0, 200},{200, 200},{100, 0}};
  w2.SetTriangle(p);
  w3.SetRoundRect(0,0,200,200,50,50);
  return CWind::MsgLoop();
}
int test3D()
{
  RECT r1={0,0,1800,1200};
  CDessineWind w1(0x000000,&r1);
  //w1.SetPenColor(0xFF00FF);
  //w1.SetPenWidth(2);
  return CWind::MsgLoop();
}
int test3E()
{
  CMenuWind w1;
  return CWind::MsgLoop();
}
int test3F()
{
  CTranspWind w1(_T("fenetre transparent"));
  return CWind::MsgLoop();
}

int test3I() 
{
	CEditCopy W1;
	return CWind::MsgLoop();
}

int test3J() 
{
	CWinGDIPlus W1(0x00FF00,_T("Tests en GDI+"));
	return CWind::MsgLoop();
}

int test3K() 
{
	CWinJO W1(0xFFFFFF,_T("Drapeau des JO"));
	return CWind::MsgLoop();
}

int test3G() 
{
	RECT r1={0,0,500,500};
	CHoleWind w1(_T("Window with a hole"),&r1);
	return CWind::MsgLoop();
}
int test3H() 
{
	RECT r1={100,200,200,200};RECT r2={350,200,200,200};RECT r3={600,200,200,200};
	RECT r4={200,500,200,200};RECT r5={550,500,200,200};
	CWind w1(0xFF0000,_T("1ère fenêtre"),&r1);
	CWind w2(0xFFFFFF,_T("2ème fenêtre"),&r2);
	CWind w3(0x0000FF,_T("3ème fenêtre"),&r3);
	CWind w4(_T("4ème fenêtre"),&r4,ES_LEFT | WS_VISIBLE,0,L"EDIT"); //use ES_RIGHT for right allignment
	CWind w5(_T("5ème fenêtre"),&r5,WS_VISIBLE | WS_OVERLAPPEDWINDOW,0,L"BUTTON");
	w2.SetMain(false);w3.SetMain(false);w4.SetMain(false);w5.SetMain(false);
	return CWind::MsgLoop();
}
int test3L() 
{
	RECT r1={0,0,500,500};
	CWinImg w1(&r1,true);
	return CWind::MsgLoop();
}
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//return test3A();
	//return test3B();
	//return test3C();
	//return test3D();
	//return test3E();
	//return test3F();

	return test3I();
	//return test3J();
	//return test3K();
	//return test3G();
	//return test3H(); 
	//return test3L(); 
}
