#include "Wind.h"
#include <windowsx.h>
#include "resource.h"

class WinGraph: public CWind
{
  
  size_t nb_links;
  size_t nb_pts;
  size_t* tab_links;
  POINT* tab_points;
  bool* tab_inter;
  BOOL done,lose;
  size_t current_point;

  int time;

	LRESULT WmTimer(HWND hWnd, WPARAM wP, LPARAM lP){
		if (!done){
			if (time == 0) lose = true;
			else if (time>0) time--;
			::InvalidateRect(hWnd, NULL, TRUE);
		}
		//if (time == 25) done=true; just test
		return 0;}

	void UpdateInter()
	{
		bool currentdone=true;
		for (size_t i = 0; i < nb_links; i++) {
        tab_inter[i] = false;
		}
		
		for (size_t i=0; i < 2*nb_links; i+=2)
		{
			for (size_t j=0; j < 2*nb_links; j+=2)
			{
				if(j!=i && tab_links[i] != tab_links[j] && tab_links[i+1] != tab_links[j] 
				&& tab_links[i] != tab_links[j+1]  && tab_links[i+1] != tab_links[j+1] )
				{
					if(Intersect(tab_points[tab_links[i]],tab_points[tab_links[i+1]],tab_points[tab_links[j]],tab_points[tab_links[j+1]]))
					{	
						tab_inter[i/2] = true; 
						currentdone = false;
					}
				}
				
			}
		}
		done = currentdone;
		::InvalidateRect(m_hWnd, NULL, TRUE);
		//return done;
	}
  // retourne true s'il y a une intersection entre les segments [p0,p1] et [p2,p3]
  static bool Intersect(const POINT& p0, const POINT& p1, const POINT& p2, const POINT& p3)
  {
    int s1_x= p1.x - p0.x, s1_y= p1.y - p0.y, s2_x= p3.x - p2.x, s2_y= p3.y - p2.y;
    int delta=(-s2_x * s1_y + s1_x * s2_y);
    if(delta==0) return false;
    float s = (-s1_y * (p0.x - p2.x) + s1_x * (p0.y - p2.y)) / (float)delta;
    float t = ( s2_x * (p0.y - p2.y) - s2_y * (p0.x - p2.x)) / (float)delta;
    return s >= 0 && s <= 1 && t >= 0 && t <= 1;
  }
  LRESULT WmCommand(HWND hWnd, WPARAM wP, LPARAM lP) 
	{
		switch(LOWORD(wP))
		{
		case ID_JEU_QUITTER:
			::SendMessage(hWnd, WM_CLOSE,0,0);break;
		case ID_JEU_NOUVEAU:
			init();break;
		default: return -1;
		}
		return 0;
	}
  LRESULT WmClose(HWND hWnd, WPARAM wP, LPARAM lP) { 
     return ::MessageBox(m_hWnd,_T("Voulez-vous fermer la fenêtre?"),_T("Cloture"),MB_YESNO | MB_ICONQUESTION)==IDYES?-1:0;
  }
  virtual LRESULT WmLButtonDown(HWND hWnd, WPARAM wP, LPARAM lP)
  {
	  if(!done && !lose)
	  {
	  current_point = nb_pts; //reassuring
	  ::SetCapture(hWnd);
	  int x = GET_X_LPARAM(lP);
	  int y = GET_Y_LPARAM(lP);

	  for (size_t i = 0; i < nb_pts; i++)
	  {
		  if ((x > tab_points[i].x-5 && x < tab_points[i].x+5) &&
			  (y > tab_points[i].y-5 && y < tab_points[i].y+5) )
		  {
			current_point=i;
		  }
	  }
	  }
	return 0;
  }
   virtual LRESULT WmLButtonUp(HWND hWnd, WPARAM wP, LPARAM lP)
   {
	   current_point = nb_pts; //reassuring
	   ::ReleaseCapture();
	   return 0;
   }
   virtual LRESULT WmMouseMove(HWND hWnd, WPARAM wP, LPARAM lP)
  {
	  if(wP==MK_LBUTTON && !done && !lose)
	  {
		tab_points[current_point].x = GET_X_LPARAM(lP);
		tab_points[current_point].y = GET_Y_LPARAM(lP);
		UpdateInter();
	  }
	  return 0;
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
    
	::SetBkMode(dc,TRANSPARENT);
	TCHAR buf[16];
    
	if(done){
		::SetTextColor(dc,RGB(0, 255, 0));
		_stprintf_s(buf,_T("Gagné: %d sec !"),time);
	}
	else if (lose){
		::SetTextColor(dc,RGB(255, 0, 0));
		_stprintf_s(buf,_T("Perdu !"));
	}
	else 
	{
		_stprintf_s(buf,_T("Restent %d sec"),time); //converting int to LPCTSTR
		::SetTextColor(dc,RGB(255, 127, 40));
	}
	::SetTextAlign(dc,TA_LEFT);
	::TextOut(dc,r.left,r.top,buf,(int)_tcslen(buf));

	//lzm da fla5er
    ::SelectObject(dc,hb_old); //lzm arg3ha zy mkant
    ::DeleteObject(hb); //destroy el obj

	//dessiner les segments
	HPEN hp= 0;
	HGDIOBJ hp_old=0;

	/*MoveToEx is used to set the current position in the specified device context.
	It does not draw anything by itself; it just moves the "pen" to a new starting point for subsequent drawing operations.*/
	for (size_t i = 0; i < 2*nb_links; i+=2)
	{
		
		if(!tab_inter[i/2]){
			hp= ::CreatePen(PS_SOLID,4,0x00FF00);
			hp_old=::SelectObject(dc,hp);}
		else {
			hp= ::CreatePen(PS_SOLID,4,0x0000FF);
			hp_old=::SelectObject(dc,hp);}
		#ifdef USE_CONSOLE
		//printf("point %d @ (%d,%d) to point %d @ (%d,%d) \n",tab_links[i],tab_points[tab_links[i]].x,tab_points[tab_links[i]].y,tab_links[i+1],tab_points[tab_links[i+1]].x,tab_points[tab_links[i+1]].y);
		printf("%s \n", tab_inter[i/2] ? "true" : "false");
		#endif
		::MoveToEx(dc,tab_points[tab_links[i]].x, tab_points[tab_links[i]].y,(LPPOINT) NULL);
		::LineTo(dc,tab_points[tab_links[i+1]].x, tab_points[tab_links[i+1]].y); //(x, y) are the coordinates of the endpoint of the line and i get the starting point from MoveToEx.
		
		::SelectObject(dc, hp_old);
		::DeleteObject(hp);
	}
	//dessiner les sommets
	hb= ::CreateSolidBrush(0x00FF00);
    hb_old=::SelectObject(dc,hb);
    //hnbd2 nrsm hna
	for (size_t i = 0; i < nb_pts; i++){
		if (current_point == i) 
		{	hb= ::CreateSolidBrush(0x00FFFF);
			
			hb_old=::SelectObject(dc,hb);
			::Ellipse(dc,tab_points[current_point].x-5,tab_points[current_point].y-5,tab_points[current_point].x+5,tab_points[current_point].y+5);
			
			::SelectObject(dc,hb_old);
		}
		else
		{
		::Ellipse(dc,tab_points[i].x-5,tab_points[i].y-5,tab_points[i].x+5,tab_points[i].y+5);
		}
	}
        
    //lzm da fla5er
    ::SelectObject(dc,hb_old); //lzm arg3ha zy mkant
    ::DeleteObject(hb); //destroy el obj

    //da el ligne lli mbeno
    ::EndPaint(hWnd,&ps);

  return 0;
  }
  void init()
  {
		current_point= nb_pts;//impossible so making sure to select nothing
		done =false;
		lose =false;
		time = 30;
	    ::SetTimer(m_hWnd,1,1000,::TIMERPROC()); //timerID=1, interruptTime=1000us

		tab_points = new POINT[nb_pts]; 
		RECT r; ::GetClientRect(m_hWnd, &r);
		
		for (size_t i = 0; i < nb_pts; ++i)
		{
			int rndmX = r.right+10;
			while (rndmX>r.right || rndmX < r.left) {rndmX=Randnum();}
			(*(tab_points+i)).x = rndmX; 
			int rndmY = r.bottom+10;
			while (rndmY>r.bottom || rndmY < r.top) {rndmY=Randnum();}
			(tab_points+i)->y = rndmY; 
			#ifdef USE_CONSOLE
				printf("%d %d",(*(tab_points+i)).x,(tab_points+i)->y);
			#endif
		}

		tab_inter = new bool[nb_links];
		UpdateInter();
  }
  public:
	WinGraph(size_t _nb_pts,size_t _nb_links,size_t* _tab, LPCTSTR name = _T("Graph1"), const RECT* pR=0):
		CWind(name,pR,WS_VISIBLE | WS_OVERLAPPEDWINDOW,0,0,IDR_MENU1),nb_pts(_nb_pts),nb_links(_nb_links)
		{
			tab_links = new size_t[2*nb_links]; 
			for (size_t i = 0; i < 2*nb_links; ++i){
				*(tab_links+i) = _tab[i]; }
			init();
		}

	int Randnum()
  {
    static int last_num=0;
    LARGE_INTEGER t; ::QueryPerformanceCounter(&t);
    return last_num=(~last_num^(t.LowPart*t.LowPart*t.LowPart))&0xFFFFFF;
  }
	~WinGraph(){
		if (m_hWnd != 0){
			delete[] tab_links;
			delete[] tab_points;
			delete[] tab_inter;
			::KillTimer(m_hWnd, 1);
			::DestroyWindow(m_hWnd);
		}}
};

int test1()
{
//  0------------1
//  |\          /|
//  | \        / |
//  |  4------5  |
//  |  |      |  |
//  |  6------7  |
//  | /        \ |
//  |/          \|
//  2------------3
  size_t links[]={0,1, 1,3, 2,3, 0,2, 0,4, 1,5, 7,3, 6,2, 4,5, 5,7, 7,6, 6,4};
  WinGraph w1(8,sizeof(links)/sizeof(links[0])/2,links,_T("Graph1"),&CWind::Rect(0,0,600,600));
  return CWind::MsgLoop();
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
   // test point F à insérer ici
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
