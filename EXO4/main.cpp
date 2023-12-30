#include "Wind.h"
#include "resource.h"
#include <windowsx.h>


class CWorkWind: public CWind
{
	static const RECT rpos;
  static const LONG SEM_Places=3;
  static  DWORD WINAPI WorkAsyncTh(LPVOID arg) //methode satic n'a pas de this
  {
    ((CWorkWind*)arg)->SlowWork(0); //type cast au classe car il etait void
    return 0;
  }

  static  DWORD WINAPI WorkSyncMutexTh(LPVOID arg) //la deuxieme attend n'est pas un time_out mais le 3eme car il a attendu 10 seconds donc il l'a abondonner
  {((CWorkWind*)arg)->WorkSyncMutex(); return 0;}
  void WorkSyncMutex()
  {
    if(::WaitForSingleObject(hmtx,10000)==WAIT_OBJECT_0){ //10 secs
      SlowWork(1); ::ReleaseMutex(hmtx);
    } //si WAIT_TIMEOUT on fait rien // les 2 seuls reponse qu'on il n'y a pas de problem du code
  }

  static  DWORD WINAPI WorkSyncSCTh(LPVOID arg)  
  {((CWorkWind*)arg)->WorkSyncSC(); return 0;}
  void WorkSyncSC()
  {
#if 1 //comme le mutex mais attend infinie pas xx seconds
    //version 1 entre obligatoir et attend possible(inifinie)
    EnterCriticalSection(&sect);
    // dans la section critique
    SlowWork(2);
    LeaveCriticalSection(&sect);
#else // une seul
    //version 2: test préalable à l'entrée, sans attente
    BOOL res=TryEnterCriticalSection(&sect);
    if(res)
    {
      SlowWork(2);
      LeaveCriticalSection(&sect);
    }
#endif
  }

  static  DWORD WINAPI WorkSyncSemaTh(LPVOID arg) 
  {
    ((CWorkWind*)arg)->WorkSyncSema(); 
    return 0;
  }
  void WorkSyncSema()
  {
    if(::WaitForSingleObject(hsem,INFINITE/*10000*/)==WAIT_OBJECT_0){ 
      SlowWork(3); ::ReleaseSemaphore(hsem,1,0);
    }
  }

  static  DWORD WINAPI WorkSyncEventTh(LPVOID arg) 
  {
    ((CWorkWind*)arg)->WorkSyncEvent();    
    return 0;
  }
    
  void WorkSyncEvent()
  {
    LONG idx=::InterlockedIncrement(&incTh) -1;
    if(::WaitForSingleObject(hevent,INFINITE)==WAIT_OBJECT_0){ 
      SlowWork(idx);
    } 
  }

	LRESULT WmCommand(HWND hWnd, WPARAM wP, LPARAM lP) 
	{
		switch(LOWORD(wP))
		{
		case ID_FICHIER_QUITTER:
			::SendMessage(hWnd, WM_CLOSE,0,0);
			break;
    case ID_FICHIER_WORKSYNC:
      SlowWork(0); //bloquant dans le thread principale n'est pas bon
      break;
    case ID_FICHIER_WORKASYNC: 
      ::CloseHandle(::CreateThread(0,0,WorkAsyncTh,this,0,0)); //pas besoin de garder le handle et le thread n'est pas mort
      break;
    case ID_FICHIER_WORKSYNCMUTEX:
      ::CloseHandle(::CreateThread(0,0,WorkSyncMutexTh,this,0,0));
      break;
    case ID_FICHIER_WORKSYNCSECTIONCRITIQUE:
      ::CloseHandle(::CreateThread(0,0,WorkSyncSCTh,this,0,0));
      break;
    case ID_FICHIER_WORKSYNCSEMAPHORE:
      ::CloseHandle(::CreateThread(0,0,WorkSyncSemaTh,this,0,0));
      break;
    case ID_FICHIER_WORKSYNCEVENT:
      incTh=0;
      for(int i=0;i<4;i++)
        ::CloseHandle(::CreateThread(0,0,WorkSyncEventTh,this,0,0));    
      break;
    case ID_FICHIER_WORKSYNCSTART: 
      ::PulseEvent(hevent);
      break;

		default: return -1;
		}
		return 0;
	}
  HANDLE hmtx,hsem,hevent; //mtx comme mutex,semaphore
  CRITICAL_SECTION sect;
  LONG incTh;
public :
  COLORREF RandColor()
  {
    static COLORREF last_clr=0x0000FF;
    LARGE_INTEGER t; ::QueryPerformanceCounter(&t);
    return last_clr=(~last_clr^(t.LowPart*t.LowPart*t.LowPart))&0xFFFFFF;
  }
  void SlowWork(int idx)
  {
    RECT r; ::GetClientRect(m_hWnd,&r);
    HDC dc=::GetDC(m_hWnd);
    HGDIOBJ hpen=::CreatePen(PS_SOLID,1,RandColor());
    HGDIOBJ holdpen=::SelectObject(dc,hpen);
    for(int y=0; y<r.bottom; y++) {MoveToEx(dc,idx*r.right/4,y,0);LineTo(dc,(idx+1)*r.right/4,y); Sleep(30);}
    ::SelectObject(dc,holdpen);
    ::DeletePen(hpen);
    ::ReleaseDC(m_hWnd,dc);
  }
	CWorkWind(LPCTSTR name = _T("Working Window")):
		CWind(name,&rpos, WS_POPUP| WS_CAPTION |WS_VISIBLE, 0,0,IDR_MENU1)
	{
    hmtx=::CreateMutex(0,false,0);//posseder par le thread on est ici thread principale donc on ne le donne a person 0
      //anonyme car on ne parle pas entre 2 processus
    ::InitializeCriticalSection(&sect);
    hsem=::CreateSemaphore(0,SEM_Places,SEM_Places,0);//nombre max des places qu'il peut les modeliser , il decremente
	  hevent=::CreateEvent(0,true,false,0);
  }

	~CWorkWind()
	{
    ::CloseHandle(hevent);
    ::CloseHandle(hmtx);
    ::CloseHandle(hsem); //release libere la place mais il reste a fonctionner!
    ::DeleteCriticalSection(&sect);
	}
};
_declspec(selectany) const RECT CWorkWind::rpos={0, 0, 500, 500};


int test4A()
{
  CWorkWind w1;
  return CWind::MsgLoop();
}

int unique()
{
  HANDLE hMtxApp =::CreateMutex(0,true,_T("Exo4-Mutex-Unique")); //avant la ceation d'objet , true pour prendre qu debut
  if(hMtxApp==0) //chance 0
  {
    //::MessageBox(0,_T("Erreur handle NULL"), _T("Exo4"),MB_OK);
    return 0;
  }
  if(::GetLastError()==ERROR_ALREADY_EXISTS)
  {
    //::MessageBox(0,_T("Je ne suis pas seul"), _T("Exo4"),MB_OK);
  }
  else //quand c'est  la 2eme processus ou plus apres la premiere
  {
    //::MessageBox(0,_T("Je suis seul actuellement"), _T("Exo4"),MB_OK);
    test4A();
    ::ReleaseMutex(hMtxApp);
  }

  ::CloseHandle(hMtxApp);
  return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
  return unique();
  
}