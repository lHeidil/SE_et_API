#ifndef _WIND_H
#define _WIND_H

#include <windows.h>
#include <tchar.h>

#define USE_CONSOLE //mnst3mlsh l task manager wla q2fl l console bl3fya gher nader gdn
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

class CClsWind{//class de fenetre // modele de fenetre
public:
  static const UINT DEF_STYLE=CS_HREDRAW | CS_VREDRAW;
protected:
  ATOM ClsAtom;
public:

  //par defaux pour initializer ClsAtom ressource) = 0 
  CClsWind(): ClsAtom(0){} //on a deja le 2eme constructeur qui a des parametres par defaux and i get warning of plusieurs constructeur ambigus

  // constructeur avec parametres
  CClsWind(LPCTSTR name,WNDPROC WProc=::DefWindowProc,UINT ClsStyle=DEF_STYLE,
    COLORREF ClrBlack = 0xFFFFFF,LPCTSTR CurName=IDC_ARROW):ClsAtom(0)
  {
    WNDCLASS wcls = {ClsStyle,WProc,0,0,::GetModuleHandle(0),0,LoadCursor(0, CurName)
      ,::CreateSolidBrush(ClrBlack),0,name};
    ClsAtom=::RegisterClass(&wcls);
  } 

  //constructeur qui choisit nom aleatoire
  CClsWind(WNDPROC WProc,UINT ClsStyle=DEF_STYLE,
    COLORREF ClrBlack = 0xFFFFFF,LPCTSTR CurName=IDC_ARROW):ClsAtom(0)
  {
    TCHAR name[32];
    LARGE_INTEGER t; ::QueryPerformanceCounter(&t);
    wsprintf(name,_T("Cls#%08lX"),t.LowPart);

    WNDCLASS wcls = {ClsStyle,WProc,0,0,::GetModuleHandle(0),0,LoadCursor(0, CurName)
      ,::CreateSolidBrush(ClrBlack),0,name};
    ClsAtom=::RegisterClass(&wcls);
  } 
    //destructeur a terminer
    ~CClsWind(){
		 if (ClsAtom != 0) {
			//The UnregisterClass function unregisters a window class, freeing the memory required for the class
			//The MAKEINTATOM macro converts the specified atom into a string, so it can be passed to functions 
			//which accept either atoms or strings. we used the typecast operator instead LPCTSTR()
            UnregisterClass(LPCTSTR(ClsAtom), GetModuleHandle(NULL));
        }
	}

	operator LPCTSTR() const {return (LPCTSTR)ClsAtom;}
	bool IsValid() const {return ClsAtom != 0;}

};

class CWind { //:public CClsWind// pas d'heritage! 3shan hwa msh class wa7d sabet da kol fenetre peut appartenir a n'import quelle classe de fenetres
//CWind& operator=(const CWind&) {return *this;}
public:
    //le style (par défaut le style WS_OVERLAPPEDWINDOW et WS_VISIBLE défini 
    //comme la constante DEF_STYLE) et le style étendu de la fenêtre (par défaut nul)
    static const UINT DEF_STYLE=WS_OVERLAPPEDWINDOW | WS_VISIBLE;

protected:
  HWND m_hWnd;//handle de la fenetre
  //Prévoir un membre statique DefRect de type RECT qui aura une position et une 
  //taille par défaut, à utiliser à la place de l'argument, quand l'adresse passée est nulle
  static RECT DefRect; //Creates a Rect object whose x-coordinate, y-coordinate, width, and height are all zero. This is the default constructor

  // Attribut statique pour la classe de fenêtres par défaut
  static CClsWind DefClsWind;
  CClsWind LocCls;
  static CWind* copy_this;
  WNDPROC defproc;
  bool is_main;

    LRESULT WndMeth(HWND hWnd, UINT msgID, WPARAM wP, LPARAM lP)
  {
  #ifdef USE_CONSOLE
    printf("WndMeth: msg=%04lX\n",msgID);
  #endif
    LRESULT res=-1;
    switch(msgID)
    {
      case WM_PAINT:       res=WmPaint(hWnd,wP,lP); break; //hnwd = n_hWnd 3shan mynf3sh a distribue la message si c'est pas a moi
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
      case WM_DESTROY: if(is_main){m_hWnd=0;::PostQuitMessage(0);} break;
    }
      return res==0? res:defproc(hWnd,msgID,wP,lP);
    
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
  void SetMain(bool _is_main){is_main= _is_main;}
  bool IsMain(){return is_main;}
  
  // Constructeur pour fenêtres principales
    CWind(LPCTSTR name = _T(""),const RECT* pR = 0, UINT WndStyle = DEF_STYLE,
      UINT WndExStyle = 0, LPCTSTR ClsName = 0, WORD MenuID = 0):m_hWnd(0),defproc(::DefWindowProc),is_main(true)
  {
    copy_this=this; //3shan a7fz 3leh w2dr a3ml elhacking da? wda hn3mlo 3shan msh e7na lli 3mlen 3shan mtb2ash boucle infini eni bhack nfsy t7t
    if(!ClsName) ClsName=DefClsWind;
    if(!pR) pR=&DefRect;
    m_hWnd=::CreateWindowEx(WndExStyle,ClsName,name,WndStyle,pR->left,pR->top,pR->right,
    pR->bottom,0,(MenuID? ::LoadMenu(::GetModuleHandle(0),MAKEINTRESOURCE(MenuID)):0),::GetModuleHandle(0),0);
    if(ClsName!=DefClsWind) //hnaa hacking WndProc
    {
      WNDPROC proc=(WNDPROC)(LONG_PTR)::GetWindowLongPtr(m_hWnd,GWLP_WNDPROC);
      if ( proc!=WndProc){defproc =proc;} //WndProc dee bt3ty
      ::SetWindowLongPtr(m_hWnd,GWLP_WNDPROC,(LONG)(LONG_PTR)WndProc);
      ::SetWindowLongPtr(m_hWnd,GWLP_USERDATA,(LONG)(LONG_PTR)this);
    }

  }

      // Constructeur pour fenêtres enfants
    CWind(CWind* Parent,WORD ChildID,LPCTSTR name = _T(""),const RECT* pR = 0, UINT WndStyle = DEF_STYLE,
		UINT WndExStyle = 0, LPCTSTR ClsName = 0):m_hWnd(0),defproc(::DefWindowProc),is_main(false) //m3ndosh Menu 3shan hya remplace bl CHILD ID
  {
	copy_this=this;
    if(!ClsName) ClsName=DefClsWind;
    if(!pR) pR=&DefRect;
    m_hWnd=::CreateWindowEx(WndExStyle,ClsName,name,WndStyle,pR->left,pR->top,pR->right,
    pR->bottom,(Parent ? Parent->m_hWnd : 0),(HMENU)ChildID,::GetModuleHandle(0),0);
    if(ClsName!=DefClsWind)
    {
      WNDPROC proc=(WNDPROC)(LONG_PTR)::GetWindowLongPtr(m_hWnd,GWLP_WNDPROC);
      if ( proc!=WndProc){defproc =proc;} 
      ::SetWindowLongPtr(m_hWnd,GWLP_WNDPROC,(LONG)(LONG_PTR)WndProc);
      ::SetWindowLongPtr(m_hWnd,GWLP_USERDATA,(LONG)(LONG_PTR)this);
    }
  }
// Constructeur pour fenêtres speciale
    CWind(COLORREF Color,LPCTSTR name = _T(""),const RECT* pR = 0, UINT WndStyle = DEF_STYLE,
		UINT WndExStyle = 0,UINT ClsStyle=CClsWind::DEF_STYLE, WORD MenuID = 0):m_hWnd(0),LocCls(WndProc,ClsStyle,Color),defproc(::DefWindowProc),is_main(true)//,isDragging(false)
  {
	copy_this=this; 
    if(!pR) pR=&DefRect;
    m_hWnd=::CreateWindowEx(WndExStyle,LocCls,name,WndStyle,pR->left,pR->top,pR->right,
    pR->bottom,0,(MenuID? ::LoadMenu(::GetModuleHandle(0),MAKEINTRESOURCE(MenuID)):0),::GetModuleHandle(0),0); //ghyrna Cls name 3shan ms7naha
    if(LocCls!=DefClsWind) //msh 3rfa da s7 wla eh
    {
      WNDPROC proc=(WNDPROC)(LONG_PTR)::GetWindowLongPtr(m_hWnd,GWLP_WNDPROC);
      if ( proc!=WndProc){defproc =proc;}
      ::SetWindowLongPtr(m_hWnd,GWLP_WNDPROC,(LONG)(LONG_PTR)WndProc);
      ::SetWindowLongPtr(m_hWnd,GWLP_USERDATA,(LONG)(LONG_PTR)this);
    }
  }

    // Destructeur
    virtual ~CWind() {
	  if (m_hWnd != 0) {::DestroyWindow(m_hWnd);}
	}

  HWND GetHandle() { return m_hWnd; }
  operator HWND() const { return m_hWnd; }
  LONG GetStyle() { return (LONG)::GetWindowLongPtr(m_hWnd,GWL_STYLE); } //The GetWindowLongPtr function retrieves information about the specified window.
  LONG GetExStyle() { return (LONG)::GetWindowLongPtr(m_hWnd,GWL_EXSTYLE); }
  void SetStyle(LONG s)
  {
    ::SetWindowLongPtr(m_hWnd,GWL_STYLE,(LONG)(LONG_PTR)s); //The SetWindowLongPtr function changes an attribute of the specified window.
    ::SetWindowPos(m_hWnd,0,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
  }
  void SetExStyle(LONG s)
  {
    ::SetWindowLongPtr(m_hWnd,GWL_EXSTYLE,(LONG)(LONG_PTR)s);
    ::SetWindowPos(m_hWnd,0,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
  }

  static LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wP, LPARAM lP)
  {
  #ifdef USE_CONSOLE
      printf("WndProc: msg=%04lX\n",msgID);
  #endif

      CWind* my_this=(CWind*)(LONG_PTR)::GetWindowLongPtr(hWnd,GWLP_USERDATA);
      if(my_this==0 && copy_this!=0)
        ::SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG)(LONG_PTR)(my_this=copy_this));
      copy_this=0;
      return my_this?my_this->WndMeth(hWnd,msgID,wP,lP): ::DefWindowProc(hWnd,msgID,wP,lP);//lmfrod en 3omrna mnrou7 llfalse 3shan hwa akid f7ga mnhom
  /* LRESULT res=0;
     switch(msgID)
    {
      case WM_DESTROY: ::PostQuitMessage(0); break;
      default: res=::DefWindowProc(hWnd,msgID,wP,lP);
    }
    return res; */
  }
  static int MsgLoop() // boucle de messages pour toutes les fenêtres
  {
    MSG msg;
    while(::GetMessage(&msg, 0, 0, 0))
    {
       #ifdef USE_CONSOLE
      printf("Msg Loop: msg=%04lX\n",msg.message);
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