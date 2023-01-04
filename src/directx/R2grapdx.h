#pragma once
#include "d3dApp.h"

namespace R2grap {

class R2grapDx : public D3DApp {
public:
  struct VertexPosColor
  {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
  };
public:
  R2grapDx(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);
  ~R2grapDx();

  bool Init();
  void OnResize();
  void UpdateScene();

private:
  bool InitEffect();
  bool InitResource();

private:
  ComPtr<ID3D11InputLayout> m_pVertexLayout;	
  ComPtr<ID3D11Buffer> m_pVertexBuffer;		
  ComPtr<ID3D11VertexShader> m_pVertexShader;	
  ComPtr<ID3D11PixelShader> m_pPixelShader;	
};

}