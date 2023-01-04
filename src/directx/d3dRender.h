#pragma once
#include "d3dApp.h"
#include "glm/glm.hpp"


class D3DRender : public D3DApp {
public:
  struct VertexPosColor
  {
    glm::vec3 pos;
    glm::vec4 color;
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
  };

  struct ConstantBuffer
  {
      glm::mat4 world;
      glm::mat4 view;
      glm::mat4 proj;
  };

public:
  D3DRender(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);
  ~D3DRender();

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
  ConstantBuffer m_CBuffer;  	
};

