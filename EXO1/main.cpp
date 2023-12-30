#include "Wind.h"
#include "resource.h"
#include <windowsx.h>

/******************************** tests unitaires ***************************/

int test1BC_v0()
{
  CWind w1;
  return CWind::MsgLoop();
}
int test1BC_v1()
{
  CWind w1(_T("1ere fenetre"));
  return CWind::MsgLoop();
}
int test1BC_v2()
{
  RECT r={0,0,400,400};
  CWind w1(_T("1ere fenetre"),&r);
  return CWind::MsgLoop();
}
int test1BC_v3()
{
  RECT r={0,0,400,400};
  CWind w1(_T("1ere fenetre"),&r,WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_VSCROLL);
  return CWind::MsgLoop();
}
int test1BC_v4()
{
  RECT r={0,0,400,400};
  CWind w1(_T("1ere fenetre"),&r,WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_VSCROLL,WS_EX_TOPMOST);
  return CWind::MsgLoop();
}


int test1BC_v5()
{
  RECT r={0,0,400,400};
  CClsWind cls(CWind::WndProc,CClsWind::DEF_STYLE,0xFF00FF);
  CWind w1(_T("1ere fenetre"),&r,WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_VSCROLL,WS_EX_TOPMOST,cls);
  return CWind::MsgLoop();
}
//on va commencer a gerer des ressources
int test1BC_v6()
{
  RECT r={0,0,400,400};
  CClsWind cls(CWind::WndProc,CClsWind::DEF_STYLE,0xFF00FF);
  CWind w1(_T("1ere fenetre"),&r,WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_VSCROLL,WS_EX_TOPMOST,cls,IDR_MENU1);
  return CWind::MsgLoop();
}
int test1D()
{
  RECT r={0,0,400,400};
  CWind w1(_T("Mon boutton"),&r,WS_POPUP |WS_VISIBLE ,0,_T("Button"));
  return CWind::MsgLoop(); // mfesh 7ga bttb3 gher l msgLOOP 3shan a5dna  el WindProc msh fl class bt3na l7d m5lnah byb3tlna lawl hacking
}
int test1F_v1()
{
  RECT r1={0,0,400,400};RECT r2={400,0,400,400};RECT r3={800,0,400,400};
  CWind w1(_T("1ere fenetre"),&r1),w2(_T("2eme  fenetre"),&r2),w3(_T("3eme  fenetre"),&r3);
  return CWind::MsgLoop();
}
int test1F_v2()
{
  RECT r1={0,0,400,400};RECT r2={400,0,400,400};RECT r3={800,0,400,400};
  CWind w1(_T("1ere fenetre"),&r1),w2(_T("2eme  fenetre"),&r2),w3(_T("3eme  fenetre"),&r3);
  w1.SetMain(false), w2.SetMain(false);
  return CWind::MsgLoop();
}


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
  //return test1BC_v0();
  //return test1BC_v1();
  //return test1BC_v2();
  //return test1BC_v3();
  //return test1BC_v4();
  //return test1BC_v5();
  //return test1BC_v6();
  //return test1D();
  //return test1F_v1();
  //return test1F_v2();
}
