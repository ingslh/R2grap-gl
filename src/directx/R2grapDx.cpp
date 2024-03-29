#include "R2grapDx.h"
#include "codec/JsonReader.h"
#include "d3dApp.h"
#include "RenderContent.h"

namespace R2grap{

R2grapDx::R2grapDx(const std::string& filename){
  reader_ = std::make_shared<JsonReader>("../assets/" + filename);
  auto window_width = AniInfoManager::GetIns().GetWidth();
  auto window_height = AniInfoManager::GetIns().GetHeight();
  auto frame_count = AniInfoManager::GetIns().GetDuration() * AniInfoManager::GetIns().GetFrameRate();
  auto frame_rate = AniInfoManager::GetIns().GetFrameRate();

  HINSTANCE hInstance = GetModuleHandle(NULL);
  dx_render_ = std::make_shared<D3DRender>(hInstance, L"R2GrapDx", window_width, window_height, frame_count, frame_rate);
}

int R2grapDx::run(){
  auto layers_count = reader_->getLayersCount();
  std::vector<std::shared_ptr<RenderContent>> contents;
  for (auto i = 0; i < layers_count; i++) {
    auto layer_info = reader_->GetLayersInfo(i).get();
    contents.emplace_back(std::make_shared<RenderContent>(layer_info));
  }
  RenderContent::UpdateTransRenderData(contents, objs_);

  if(!dx_render_->Init(objs_)) return 0;

  return dx_render_->Run();
}

}