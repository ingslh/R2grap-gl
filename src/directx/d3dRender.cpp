#include "d3dRender.h"
#include "d3dUtil.h"
#include "DXTrace.h"

const D3D11_INPUT_ELEMENT_DESC D3DRender::VertexPosColor::inputLayout[2] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3DRender::D3DRender(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight)
  :D3DApp(hInstance, windowName, initWidth, initHeight), m_CBuffer(){}

D3DRender::~D3DRender(){}

bool D3DRender::Init(const std::vector<R2grap::RePathObj>& objs){
  if(!D3DApp::Init()) return false;
  
  if(!InitEffect()) return false;

  if(!InitResource()) return false;

  objs_ = objs;
  return true;
}

void D3DRender::OnResize(){
  D3DRender::OnResize();
}

void D3DRender::UpdateScene(){

}

void D3DRender::DrawScene(){

}

bool D3DRender::InitEffect(){ //blend shader
  ComPtr<ID3DBlob> blob;

  //vertices shader
  HR(CreateShaderFromFile(L"../src/shader/hlsl/r2grap_d3d_VS.cso", L"../src/shader/hlsl/r2grap_d3d_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
  HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
  //vertices layout
  HR(m_pd3dDevice->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout),
    blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

  //frame shader
  HR(CreateShaderFromFile(L"../src/shader/hlsl/r2grap_d3d_PS.cso", L"../src/shader/hlsl/r2grap_d3d_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
  HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

  return true;
}

bool D3DRender::InitResource(){
  if (objs_.empty()) return false;

  for (auto ind = 0; ind < objs_.size(); ind++) {
    auto fill = objs_[ind].fill;
    auto stroke = objs_[ind].stroke;
    glm::vec4 tmp_color = fill ? fill->color : stroke->color;
    DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(tmp_color.r, tmp_color.g, tmp_color.b, 1);

    auto path_data = objs_[ind].path;
    if (!path_data->has_keyframe) {
      auto vert_array = path_data->verts;
      auto vertices = new VertexPosColor[vert_array.size() / 3];
      for (auto i = 0; i < vert_array.size() / 3; i = i + 3) {
        vertices[i].pos = DirectX::XMFLOAT3(vert_array[i], vert_array[i + 1], vert_array[i + 2]);
        vertices[i].color = color;
      }
      D3D11_BUFFER_DESC vbd;
      ZeroMemory(&vbd, sizeof(vbd));
      vbd.Usage = D3D11_USAGE_IMMUTABLE;
      vbd.ByteWidth = sizeof vertices;
      vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      vbd.CPUAccessFlags = 0;

      D3D11_SUBRESOURCE_DATA InitData;
      ZeroMemory(&InitData, sizeof(InitData));
      InitData.pSysMem = vertices;
      HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));
      delete vertices;

      if (path_data->closed) {
        auto tri_array = path_data->tri_ind;
        auto indices = new DWORD[tri_array.size()];
        for (auto j = 0; j < tri_array.size(); j++)
          indices[j] = static_cast<DWORD>(tri_array[j]);
        D3D11_BUFFER_DESC ibd;
        ZeroMemory(&ibd, sizeof(ibd));
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = sizeof indices;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        InitData.pSysMem = indices;
        HR(m_pd3dDevice->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
        m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
      }
    }else {

    }
    
  }
}

