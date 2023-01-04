#include "d3dRender.h"
#include "d3dUtil.h"
#include "DXTrace.h"

D3DRender::D3DRender(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight)
  :D3DApp(hInstance, windowName, initWidth, initHeight), m_CBuffer(){}

  D3DRender::~D3DRender{}

  bool D3DRender::Init(){
    if(!D3DApp::Init()) return false;
    
    if(!InitEffect()) return false;

    if(!InitResource()) return false;

    return true;
  }
