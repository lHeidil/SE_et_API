#include "Wind.h"
#include <windowsx.h>
#include "Resource.h"

/****************************** classe TetrisWind *******************************/ 
class TetrisWind: public CWind
{
  enum EState{ST_Stop=0, ST_NewPiece, ST_Dropping, ST_Dropped};
  int state;
  BOOLEAN paused;
  static const RECT rpos;
  static const int STEP=32;
  static const int DIMX=12;
  static const int DIMY=22;
  int casesJeu[DIMX][DIMY];
  POINT currPos[4];
  size_t currIdx;

  // ** *     * **   ** *** ****
  // ** *** ***  ** **   * 
  // O  J   L   Z   S    T   I
  static const size_t NB_PIECES=7;
  static const DWORD color[NB_PIECES+1];
  static const POINT piece[NB_PIECES][4];
  virtual LRESULT WmKeyDown(HWND hWnd, WPARAM wP, LPARAM lP){
	  switch(wP){
		case VK_LEFT:  //still missing to check out of dimensions
      if(state == ST_Dropping && !paused)
      { 
          Replace(TRUE);
          for (int j=0;j<4;j++)
          {
            currPos[j].x--; //bt5od int w trg3 point bt3t lcarre
          }
          if (Test(currIdx,currPos) == FALSE) 
          {
            for (int j=0;j<4;j++)
            {currPos[j].x++;}
          }
          Replace();
      }
      break;
    case VK_RIGHT:  //still missing to check out of dimensions
      if(state == ST_Dropping && !paused)
      { 
          Replace(TRUE); //true pour effacer
          for (int j=0;j<4;j++)
          {
            currPos[j].x++; //bt5od int w trg3 point bt3t lcarre
          }
           if (Test(currIdx,currPos) == FALSE) 
          {
            for (int j=0;j<4;j++)
            {currPos[j].x--;}
          }
          Replace();
      }
      break;
    case VK_SPACE:
      if(state != ST_Stop){
      paused = !paused ; 
      TCHAR buffer[20];
      if (paused){wsprintf(buffer,_T("Tetris en pause"));}
      else { wsprintf(buffer,_T("Tetris en cours")); }
      SetWindowText(hWnd, buffer);
      }
      break;
    /*case VK_ESCAPE: //just testing
      state=ST_Stop;break;*/
		default: return -1;
		}
		return 0;
	}
  BOOLEAN Test(size_t _currIdx,POINT* _currPos) //currIdx not useful ?
  {
    for (int j=0;j<4;j++){ //il y erreur
      if (_currPos[j].x < -1 || _currPos[j].x > DIMX-1)
      {  return FALSE;  }
    }
    for (int j=0;j<4;j++){
      if (_currPos[j].y < 0 || _currPos[j].y > DIMY-1)
      {  return FALSE;  }
    }

    return TRUE;
  }
  void Replace(BOOLEAN efface = FALSE)
  {
    for (int j=0;j<4;j++) {
    if (efface) casesJeu[currPos[j].x][currPos[j].y] = 0;
    else casesJeu[currPos[j].x][currPos[j].y] =currIdx+1;
    }
    ::InvalidateRect(m_hWnd, NULL, TRUE);
  }
  virtual LRESULT WmTimer(HWND hWnd, WPARAM wP, LPARAM lP){
    if (!paused)
    {
      if(state == ST_NewPiece)
      {
        if (currIdx == NB_PIECES) currIdx=0; else currIdx++;
        for (int j=0;j<4;j++){
          currPos[j].y = piece[currIdx][j].y; 
          currPos[j].x = piece[currIdx][j].x+4; 
        }

        if  (Test(currIdx,currPos) == FALSE) state = ST_Stop;
        else{
        Replace();
        state = ST_Dropping;
        }
      }
       if(state == ST_Dropping)
      {
        Replace(TRUE);
        for (int j=0;j<4;j++){
          currPos[j].y++; //bt5od int w trg3 point bt3t lcarre
        }
        if  (Test(currIdx,currPos) == FALSE){
          for (int j=0;j<4;j++)
          {
            currPos[j].y--; //bt5od int w trg3 point bt3t lcarre
          }
          state = ST_Dropped;
        }
        Replace();
        
      }
       if(state == ST_Dropped)
       {
         state = ST_NewPiece;
       }
       if(state == ST_Stop)
       {
         ::KillTimer(m_hWnd, 1);
       }
      
      
    }
    return 0;
	}
  virtual LRESULT WmClose(HWND hWnd, WPARAM wP, LPARAM lP) { 
    if(state != ST_Stop){
     return ::MessageBox(m_hWnd,_T("Voulez-vous fermer la fenêtre?"),_T("Cloture"),MB_YESNO | MB_ICONQUESTION)==IDYES?-1:0;
    }
  return -1;
  }
  virtual LRESULT WmCommand(HWND hWnd, WPARAM wP, LPARAM lP) 
	{
		switch(LOWORD(wP))
		{
		case ID_TETRIS_Q:
			::SendMessage(hWnd, WM_CLOSE,0,0);break;
		case ID_TETRIS_NOUVEAUJEU:
      state=ST_NewPiece;
      Reset();
      ::SetTimer(m_hWnd,1,600,(TIMERPROC) NULL); //timerID=1, interruptTime=600us
		break;
    default: return -1;
		}
		return 0;
	}
  LRESULT WmPaint(HWND hWnd, WPARAM wP, LPARAM lP)
	{ 
    RECT r; ::GetClientRect(hWnd, &r);  //GetClientRect instead of GetWindowRect for client area dimensions
    PAINTSTRUCT ps;//pour tout les app qui dessine
    HDC dc=::BeginPaint(hWnd,&ps);

	  //dessiner les carreaux
    HBRUSH hb;
    HGDIOBJ hb_old;
    HPEN hpen=::CreatePen(PS_SOLID,1,0x000000);
    HGDIOBJ holdpen=::SelectObject(dc,hpen);
    //casesJeu[0][0]= 1; //just a test
    //casesJeu[1][1]= 2; //just a test
    //Begin
    for(int i=0;i<DIMX;i++){
      for(int j=0;j<DIMY;j++){
        hb= ::CreateSolidBrush(color[casesJeu[i][j]]);
        hb_old=::SelectObject(dc,hb);
        ::Rectangle(dc,i*STEP,j*STEP,(i*STEP)+STEP,(j*STEP)+STEP);
      }
    }
	 
    //Finish
    ::SelectObject(dc,hb_old); //return to old
    ::DeleteObject(hb); //destroy obj

    ::EndPaint(hWnd,&ps);

  return 0;
  }
  void Reset()
  {
    currIdx=0;
    for(int i=0;i<DIMX;i++){
      for(int j=0;j<DIMY;j++){
        casesJeu[i][j] = 0;  }}
  ::InvalidateRect(m_hWnd, NULL, TRUE);
	}

  public:
TetrisWind(LPCTSTR name = _T("Tetris")):
		CWind(name,&rpos,WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION,0,0,IDR_MENU1),state(ST_Stop),paused(FALSE),currIdx(0)
		{
      RECT r; ::GetClientRect(m_hWnd,&r);
      #ifdef USE_CONSOLE
      printf (" %d  %d\n",r.right,r.bottom);
      printf (" %d  %d\n",rpos.right,rpos.bottom);
      printf (" %d  %d\n",rpos.right+(rpos.right-r.right),rpos.bottom+(rpos.bottom-r.bottom));
      #endif
      ::SetWindowPos(m_hWnd,0,0,0,rpos.right+(rpos.right-r.right),rpos.bottom+(rpos.bottom-r.bottom),SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED); //deleted nomove w nosize
      
      Reset();
		}
~TetrisWind(){
		if (m_hWnd != 0){
			::KillTimer(m_hWnd, 1);
			::DestroyWindow(m_hWnd);
		}}
};
_declspec(selectany) const RECT TetrisWind::rpos={0,0,TetrisWind::DIMX*32,TetrisWind::DIMY*32};
__declspec(selectany) const DWORD TetrisWind::color[NB_PIECES+1]={0xA0A0A0,0x00FFFF,0xFF0000,0x00A0FF,0x0000FF,0x00C000,0xA000A0,0xFFFF00};
__declspec(selectany) const POINT TetrisWind::piece[NB_PIECES][4]={ // chaque pièce a 4 cases
  {{0,0},{1,0},{0,1},{1,1}},  // O
  {{0,0},{0,1},{1,1},{2,1}},  // J
  {{2,0},{0,1},{1,1},{2,1}},  // L
  {{0,0},{1,0},{1,1},{2,1}},  // Z
  {{0,1},{1,1},{1,0},{2,0}},  // S
  {{0,0},{1,0},{2,0},{1,1}},  // T
  {{-1,0},{0,0},{1,0},{2,0}}, // I
};

/******************************** tests unitaires ***************************/

int test1()
{
  TetrisWind w1;
  return CWind::MsgLoop();
}

int limited(LONG maxInst=2)
{
  HANDLE hSemApp =::CreateSemaphore(0,maxInst,maxInst,_T("Exam24-Semaphore"));
  if(::WaitForSingleObject(hSemApp,INFINITE)==WAIT_OBJECT_0)
		{
      test1();
      ::ReleaseSemaphore(hSemApp,1,0);
    }
  ::CloseHandle(hSemApp);
  return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
  return limited();
  //return test1();
}
