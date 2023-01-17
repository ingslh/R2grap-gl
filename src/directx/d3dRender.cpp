#include "d3dRender.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include "d3dApp.h"
#include "PathRenderData.h"
#include "camera.h"

const D3D11_INPUT_ELEMENT_DESC D3DRender::VertexPosColor::inputLayout[2] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3DRender::D3DRender(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight, unsigned frame_count)
  :D3DApp(hInstance, windowName, initWidth, initHeight, frame_count), m_CBuffer(), frame_count_(frame_count){}

D3DRender::~D3DRender(){}

bool D3DRender::Init(const std::vector<R2grap::RePathObj>& objs){
  objs_ = objs;
  if(!D3DApp::Init()) return false;
  
  if(!InitEffect()) return false;

  if(!InitResource()) return false;
  return true;
}

void D3DRender::OnResize(){
  D3DApp::OnResize();
}


void D3DRender::UpdateScene(float dt){
}

void D3DRender::DrawScene(){
  assert(m_pd3dImmediateContext);
  assert(m_pSwapChain);

  static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&black));
  m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  glm::mat4 old_trans;
  
  for (auto i = 0; i < objs_.size(); i++) {
    if (objs_[i].in_pos > static_cast<float>(played_) || objs_[i].out_pos < static_cast<float>(played_)) continue;
    glm::mat4 trans;
    if (!objs_[i].keep_trans) {
      trans = objs_[i].trans[played_];
      old_trans = trans;
    }
    else {
      trans = old_trans;
    }
    m_CBuffer.transform = XMMatrixTranspose(DirectX::XMMATRIX(trans[0].x, trans[0].y, trans[0].z, trans[0].w,
                                                              trans[1].x, trans[1].y, trans[1].z, trans[1].w,
                                                              trans[2].x, trans[2].y, trans[2].z, trans[2].w,
                                                              trans[3].x, trans[3].y, trans[3].z, trans[3].w));

    //update m_pConstantBuffer
    D3D11_MAPPED_SUBRESOURCE mappedData;
    HR(m_pd3dImmediateContext->Map(m_pConstantBufferList[i].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
    memcpy_s(mappedData.pData, sizeof(m_CBuffer), &m_CBuffer, sizeof(m_CBuffer));
    m_pd3dImmediateContext->Unmap(m_pConstantBufferList[i].Get(), 0);
    m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBufferList[i].GetAddressOf());

    UINT stride = sizeof(VertexPosColor);
    UINT offset = 0;
    m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBufferList[i].GetAddressOf(), &stride, &offset);
    if (objs_[i].path->closed) {
      m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBufferList[i].Get(), DXGI_FORMAT_R32_UINT, 0);
      m_pd3dImmediateContext->DrawIndexed(objs_[i].path->tri_ind.size(), 0, 0);

      /*m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
      m_pd3dImmediateContext->Draw(objs_[i].path->verts.size() / 3, 0);*/
    }
    else {
      m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
      m_pd3dImmediateContext->Draw(objs_[i].path->verts.size() / 3, 0);
    }
  }
  played_ = played_ >= frame_count_ ? 0 : ++played_;
  HR(m_pSwapChain->Present(0, 0));
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
      for (auto i = 0; i < vert_array.size() / 3; i++) {
        vertices[i].pos =  DirectX::XMFLOAT3(vert_array[3 * i], vert_array[3 * i + 1], vert_array[3 * i + 2]);
        vertices[i].color = color;
      }

      D3D11_BUFFER_DESC vbd;
      ZeroMemory(&vbd, sizeof(vbd));
      vbd.Usage = D3D11_USAGE_IMMUTABLE;
      vbd.ByteWidth = sizeof(VertexPosColor) * (vert_array.size() / 3);
      vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      vbd.CPUAccessFlags = 0;
      D3D11_SUBRESOURCE_DATA InitData;
      ZeroMemory(&InitData, sizeof(InitData));
      InitData.pSysMem = vertices;
      ComPtr<ID3D11Buffer> vertexBuffer;
      HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, vertexBuffer.GetAddressOf()));
      m_pVertexBufferList.push_back(vertexBuffer);
      //m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBufferList.back().GetAddressOf(), &stride, &offset);
      delete []vertices;

      if (path_data->closed) {
        auto tri_array = path_data->tri_ind;
        DWORD* indices = new DWORD[tri_array.size()];
        for (auto j = 0; j < tri_array.size(); j = j + 3) {
          std::vector<unsigned> tmp_array(tri_array.begin() + j, tri_array.begin() + j + 3);
          std::sort(tmp_array.begin(), tmp_array.end());
          memcpy(&indices[j], &tmp_array.front(), 3 * sizeof(unsigned));
        }
        D3D11_BUFFER_DESC ibd;
        ZeroMemory(&ibd, sizeof(ibd));
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = sizeof(DWORD) * tri_array.size();
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        InitData.pSysMem = indices;
        ComPtr<ID3D11Buffer> indexBuffer;
        HR(m_pd3dDevice->CreateBuffer(&ibd, &InitData, indexBuffer.GetAddressOf()));
        m_pIndexBufferList[m_pVertexBufferList.size() - 1] = indexBuffer;
       // m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBufferList[m_pVertexBufferList.size() - 1].Get(), DXGI_FORMAT_R32_UINT, 0);
        delete []indices;
      }
    }else {

    }
    //constant buffer, not use initial values
    D3D11_BUFFER_DESC cbd;
    ZeroMemory(&cbd, sizeof(cbd));
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ComPtr<ID3D11Buffer> constantBuffer;
    HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, constantBuffer.GetAddressOf()));
    m_pConstantBufferList.push_back(constantBuffer);
  }

  m_CBuffer.world = DirectX::XMMatrixIdentity();
  m_CBuffer.view = XMMatrixTranspose(DirectX::XMMatrixLookAtLH(
   DirectX::XMVectorSet(0.0f, 0.0f, -.5f, 0.0f),
   DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
   DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
  ));
  m_CBuffer.transform = DirectX::XMMatrixIdentity();
  m_CBuffer.proj = XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, AspectRatio(), 0.1f, 100.0f));

  //set input layout
  m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
  //Bind shaders to render pipelines
  m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);

  m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

  D3D11_RASTERIZER_DESC rasterizerDesc;
  ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
  rasterizerDesc.FillMode = D3D11_FILL_SOLID;
  rasterizerDesc.CullMode = D3D11_CULL_NONE;
  rasterizerDesc.FrontCounterClockwise = false;
  rasterizerDesc.DepthClipEnable = true;
  HR(m_pd3dDevice->CreateRasterizerState(&rasterizerDesc, m_pRSWireframe.GetAddressOf()));
  m_pd3dImmediateContext->RSSetState(m_pRSWireframe.Get());

  //set debug info
  D3D11SetDebugObjectName(m_pVertexLayout.Get(), "VertexPosColorLayout");
  D3D11SetDebugObjectName(m_pVertexShader.Get(), "Shader_VS");
  D3D11SetDebugObjectName(m_pPixelShader.Get(), "Shader_PS");
  for (auto ind = 0; ind < objs_.size(); ind++) {
    D3D11SetDebugObjectName(m_pVertexBufferList[ind].Get(), "VertexBuffer-" + std::to_string(ind));
    D3D11SetDebugObjectName(m_pConstantBufferList[ind].Get(), "ConstantBuffer-" + std::to_string(ind));
  }
  for (auto& pair : m_pIndexBufferList) {
    D3D11SetDebugObjectName(pair.second.Get(), "IndexBuffer-" + std::to_string(pair.first));
  }
  return true;
}

