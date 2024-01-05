#ifndef _WIND_H
#define _WIND_H
#include <windows.h>
#include <tchar.h>

#define USE_CONSOLE
#ifdef USE_CONSOLE
#include <stdio.h>
// this class allow you to use printf in any Windows GUI application
class CConsole
{ 
public: 
#pragma warning( push )
#pragma warning( disable : 4996)
  // attach to the parent console if any (as command line launching) or create a new one
  CConsole() { if(!::AttachConsole(ATTACH_PARENT_PROCESS)) ::AllocConsole(); freopen("CONOUT$","wb",stdout); } 
  ~CConsole() { fclose(stdout); ::FreeConsole(); }
#pragma warning( pop )
};
// this global object can be erased by linker optimizations in Release version so better use this class
// as an acestor of any unique object of your application like CWinApp object in a MFC application
__declspec(selectany) CConsole globConsole;
#endif //USE_CONSOLE

class CClsWind
{
  CClsWind(const CClsWind&) {}
  CClsWind& operator=(const CClsWind&) {return *this;}
public:
  static const UINT DEF_STYLE=CS_HREDRAW | CS_VREDRAW;
protected:
  ATOM ClsAtom;
public:
  CClsWind(): ClsAtom(0) {}
  CClsWind(LPCTSTR name, WNDPROC WProc=::DefWindowProc, UINT ClsStyle=DEF_STYLE,
    COLORREF ClrBack=0xFFFFFF, LPCTSTR CurName=IDC_ARROW): ClsAtom(0)
  { 
    WNDCLASS wcls={ClsStyle,WProc,0,0,::GetModuleHandle(0),0,::LoadCursor(0,CurName),
      ::CreateSolidBrush(ClrBack),0,name};
    ClsAtom=::RegisterClass(&wcls);
  }
  CClsWind(WNDPROC WProc, UINT ClsStyle=DEF_STYLE,
    COLORREF ClrBack=0xFFFFFF, LPCTSTR CurName=IDC_ARROW): ClsAtom(0)
  { 
    TCHAR name[32];
    LARGE_INTEGER t; ::QueryPerformanceCounter(&t);
    wsprintf(name,_T("Cls#%08lX"),t.LowPart);
    WNDCLASS wcls={ClsStyle,WProc,0,0,::GetModuleHandle(0),0,::LoadCursor(0,CurName),
      ::CreateSolidBrush(ClrBack),0,name};
    ClsAtom=::RegisterClass(&wcls);
  }
  ~CClsWind() { if(ClsAtom) ::UnregisterClass((LPCTSTR)ClsAtom,::GetModuleHandle(0)); }
  bool IsValid() const { return ClsAtom!=0; }
  operator LPCTSTR() const {return (LPCTSTR)ClsAtom; }
};

class CWind
{
  CWind(const CWind&) {}
  CWind& operator=(const CWind&) {return *this;}
public:
  static const UINT DEF_STYLE = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
protected:
  static RECT DefRect;
  static CClsWind DefClsWind;
  static CWind* copy_this;

  HWND m_hWnd;
  CClsWind LocCls;
  WNDPROC defproc; // procédure par défaut
  bool is_main; // suis-je une fenêtre principale ?

  LRESULT WndMeth(HWND hWnd, UINT msgID, WPARAM wP, LPARAM lP)
  {
#ifdef USE_CONSOLE
    printf("WndMeth: msg=%04lX\n",msgID);
#endif
    LRESULT res=-1;
    switch(msgID)
    {
    case WM_PAINT:       res=WmPaint(hWnd,wP,lP); break;
    case WM_TIMER:       res=WmTimer(hWnd,wP,lP); break;
    case WM_CLOSE:       res=WmClose(hWnd,wP,lP); break;
    case WM_COMMAND:     res=WmCommand(hWnd,wP,lP); break;
    case WM_CREATE:      res=WmCreate(hWnd,wP,lP); break;
    case WM_KEYUP:       res=WmKeyUp(hWnd,wP,lP); break;
    case WM_KEYDOWN:     res=WmKeyDown(hWnd,wP,lP); break;
    case WM_LBUTTONDOWN: res=WmLButtonDown(hWnd,wP,lP); break;
    case WM_MBUTTONDOWN: res=WmMButtonDown(hWnd,wP,lP); break;
    case WM_RBUTTONDOWN: res=WmRButtonDown(hWnd,wP,lP); break;
    case WM_LBUTTONUP:   res=WmLButtonUp(hWnd,wP,lP); break;
    case WM_MBUTTONUP:   res=WmMButtonUp(hWnd,wP,lP); break;
    case WM_RBUTTONUP:   res=WmRButtonUp(hWnd,wP,lP); break;
    case WM_MOUSEMOVE:   res=WmMouseMove(hWnd,wP,lP); break;
    case WM_DROPFILES:   res=WmDropFiles(hWnd,wP,lP); break;
    case WM_DESTROY: if(is_main) {m_hWnd=0; ::PostQuitMessage(0);} break;
    }
    return res==0 ? res : defproc(hWnd,msgID,wP,lP);
  }
  virtual LRESULT WmPaint(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmTimer(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmClose(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmCommand(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmCreate(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmKeyUp(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmKeyDown(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmMButtonDown(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmRButtonDown(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmLButtonUp(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmMButtonUp(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmRButtonUp(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmMouseMove(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
  virtual LRESULT WmDropFiles(HWND hWnd, WPARAM wP, LPARAM lP) { return -1;}
public:
  // getters/setters
  HWND GetHandle() const { return m_hWnd; }
  operator HWND() const { return m_hWnd; }
  LONG GetStyle() const { return (LONG)::GetWindowLongPtr(m_hWnd,GWL_STYLE); }
  LONG GetExStyle() const { return (LONG)::GetWindowLongPtr(m_hWnd,GWL_EXSTYLE); }
  void SetStyle(LONG s)
  {
    ::SetWindowLongPtr(m_hWnd,GWL_STYLE,(LONG)(LONG_PTR)s);
    ::SetWindowPos(m_hWnd,0,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
  }
  void SetExStyle(LONG s)
  {
    ::SetWindowLongPtr(m_hWnd,GWL_EXSTYLE,(LONG)(LONG_PTR)s);
    ::SetWindowPos(m_hWnd,0,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
  }
  void SetMain(bool _is_main) {is_main = _is_main;}
  bool IsMain() const { return is_main; }
  // pour des fenêtres principales "normales"
  CWind(LPCTSTR name=_T(""),const RECT* pR=0, UINT WndStyle=DEF_STYLE, UINT WndExStyle=0,
    LPCTSTR ClsName=0, WORD MenuID=0):m_hWnd(0), defproc(::DefWindowProc), is_main(true)
  {
    copy_this=this;
    if(!ClsName) ClsName=DefClsWind;
    if(!pR) pR=&DefRect;
    m_hWnd=::CreateWindowEx(WndExStyle,ClsName,name,WndStyle,pR->left,pR->top,pR->right,pR->bottom,0,
      (MenuID ? ::LoadMenu(::GetModuleHandle(0),MAKEINTRESOURCE(MenuID)):0),::GetModuleHandle(0), 0);
    if(ClsName!=DefClsWind) // classe utilisateur arrivée en paramètre
    {
      defproc=(WNDPROC)(LONG_PTR)::GetWindowLongPtr(m_hWnd,GWLP_WNDPROC);
      ::SetWindowLongPtr(m_hWnd,GWLP_WNDPROC,(LONG)(LONG_PTR)WndProc);
      ::SetWindowLongPtr(m_hWnd,GWLP_USERDATA,(LONG)(LONG_PTR)this);
    }
  }
  // pour des fenêtres enfant (ChildID est l'ID de l'enfant, Parent est la fenêtre mere)
  CWind(CWind* Parent, WORD ChildID, LPCTSTR name=_T(""),const RECT* pR=0,
    UINT WndStyle=DEF_STYLE, UINT WndExStyle=0, LPCTSTR ClsName=0):m_hWnd(0), defproc(::DefWindowProc), is_main(false)
  {
    copy_this=this;
    if(!ClsName) ClsName=DefClsWind;
    if(!pR) pR=&DefRect;
    m_hWnd=::CreateWindowEx(WndExStyle,ClsName,name,WndStyle,pR->left,pR->top,pR->right,
      pR->bottom,(Parent ? Parent->m_hWnd : 0), (HMENU)ChildID, ::GetModuleHandle(0), 0);
    if(ClsName!=DefClsWind) // classe utilisateur arrivée en paramètre
    {
      defproc=(WNDPROC)(LONG_PTR)::GetWindowLongPtr(m_hWnd,GWLP_WNDPROC);
      ::SetWindowLongPtr(m_hWnd,GWLP_WNDPROC,(LONG)(LONG_PTR)WndProc);
      ::SetWindowLongPtr(m_hWnd,GWLP_USERDATA,(LONG)(LONG_PTR)this);
    }
  }
  // pour des fenêtres principales "spéciales", on donne la couleur et le style de la classe
  CWind(COLORREF Color, LPCTSTR name=_T(""), const RECT* pR=0, UINT WndStyle=DEF_STYLE,
    UINT WndExStyle=0, UINT ClsStyle=CClsWind::DEF_STYLE) : m_hWnd(0), LocCls(WndProc,ClsStyle,Color),
    defproc(::DefWindowProc), is_main(true)
  {
    copy_this=this;
    if(!pR) pR=&DefRect;
    m_hWnd=::CreateWindowEx(WndExStyle,LocCls,name,WndStyle,pR->left,pR->top,pR->right,
      pR->bottom,0, 0, ::GetModuleHandle(0), 0);
  }
  virtual ~CWind() { if(m_hWnd && ::IsWindow(m_hWnd)) ::DestroyWindow(m_hWnd); }
  // en public pour pouvoir y acceder de l'exterieur
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wP, LPARAM lP)
  {
#ifdef USE_CONSOLE
    printf("WndProc: msg=%04lX\n",msgID);
#endif
    CWind* my_this=(CWind*)(LONG_PTR)::GetWindowLongPtr(hWnd,GWLP_USERDATA);
    if(my_this==0 && copy_this!=0)
      ::SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG)(LONG_PTR)(my_this=copy_this));
    copy_this=0;
    return my_this?my_this->WndMeth(hWnd,msgID,wP,lP): ::DefWindowProc(hWnd,msgID,wP,lP);
  }
  static int MsgLoop() // boucle de messages pour toutes les fenêtres
  {
    MSG msg;
    while(::GetMessage(&msg, 0, 0, 0))
    {
#ifdef USE_CONSOLE
      printf("MsgLoop: msg=%04lX\n",msg.message);
#endif
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
    }
    return (int) msg.wParam;
  }
};

__declspec(selectany) RECT CWind::DefRect={0,0,CW_USEDEFAULT,CW_USEDEFAULT};
__declspec(selectany) CClsWind CWind::DefClsWind(_T("DefClsWind"),CWind::WndProc);
__declspec(selectany) CWind* CWind::copy_this=0;

#endif // _WIND_H
