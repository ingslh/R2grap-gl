#include "R2grapDx.h"
#include "codec/JsonReader.h"
#include "d3dApp.h"

namesoace R2grap{

R2grapDx::R2grapDx(const std::string& filename){
  reader_ = std::make_shared<JsonReader>("../assets/" + filename);
  auto window_width = AniInfoManager::GetIns().GetWidth();
  auto window_height = AniInfoManager::GetIns().GetHeight();

  HINSTANCE hInstance = GetModuleHandle(NULL);
  dx_render_ = std::make_shared<D3DRender>(hInstance, L"R2GrapDx", window_width, window_height);
}

R2grapDx::run(){
  if(!dx_render_->Init()) return 0;

  return dx_render_->Run();
}

}