#include "RenderDataFactory.h"
#include <mutex>

namespace R2grap{

template<typename T>
void SafeDelete(BaseRenderData* data){
  auto concreate = static_cast<T*>(data);
  {
    std::lock_guard<std::recursive_mutex> guard(concreate->BaseRenderData::GetEditingLock());
    delete data;
  }
}

VerticesRenderData* RenderDataFactory::CreateVerticesData(const LayersInfo* layer){
  return new VerticesRenderData(layer);
}

ColorRenderData* RenderDataFactory::CreateColorData(const LayersInfo* layer){
  return new ColorRenderData(layer);
}

TransformRenderData* RenderDataFactory::CreateTransformData(const LayersInfo* layer){
  return new TransformRenderData(layer);
}

TransformRenderData* RenderDataFactory::CreateTransformData(const Transform* trans, unsigned int ind, float inpos, float outpos){
  return new TransformRenderData(trans, ind, inpos, outpos);
}

bool RenderDataFactory::ReleaseRenderData(BaseRenderData* data){
  if(!data) return false;

  switch (data->GetType())
  {
  case rDataType::tVerticesData:
    SafeDelete<VerticesRenderData>(data);
    break;
  case rDataType::tColorData:
    SafeDelete<ColorRenderData>(data);
    break;
  case rDataType::tTransformData:
    SafeDelete<TransformRenderData>(data);
    break;
  default:
    return false;
  }
  return true;
}

}