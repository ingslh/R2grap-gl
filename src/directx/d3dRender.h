#pragma once
#include "d3dApp.h"
#include "glm/glm.hpp"
#include "PathRenderData.h"

class D3DRender : public D3DApp {
public:
  struct VertexPosColor
  {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
  };

  struct ConstantBuffer
  {
    DirectX::XMMATRIX transform;
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
  };

public:
  D3DRender(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight, unsigned frame_count);
  ~D3DRender();

  bool Init(const std::vector<R2grap::RePathObj>& objs);
  void OnResize();
  void UpdateScene(float dt);
  void DrawScene();

private:
  bool InitEffect();
  bool InitResource();

private:
  ComPtr<ID3D11InputLayout> m_pVertexLayout;	
  //std::vector<ComPtr<ID3D11Buffer>> m_pVertexBufferList;		
  //std::vector<ComPtr<ID3D11Buffer>> m_pIndexBufferList;    
  ComPtr<ID3D11Buffer> m_pVertexBuffer;
  ComPtr<ID3D11Buffer> m_pIndexBuffer;
  ComPtr<ID3D11Buffer> m_pConstantBuffer;         

  ComPtr<ID3D11VertexShader> m_pVertexShader;	
  ComPtr<ID3D11PixelShader> m_pPixelShader;
  ConstantBuffer m_CBuffer;  	

  std::vector<R2grap::RePathObj> objs_;
  unsigned frame_count_ = 0;
  unsigned played_ = 0;
};

