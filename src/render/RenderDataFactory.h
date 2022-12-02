#pragma once
#include "LayersInfo.h"
#include "PathInfo.h"
#include "VertRenderData.h"
#include "ColorRenderData.h"
#include "TransRenderData.h"

namespace R2grap{
class ColorRenderData;
class VerticesRenderData;
class TransformRenderData;
class RenderDataFactory{
public:
  RenderDataFactory()= default;
  virtual ~RenderDataFactory()= default;//auto call derived class destructor func

public:
  VerticesRenderData* CreateVerticesData(const LayersInfo* layer);
  ColorRenderData* CreateColorData(const LayersInfo* layer);
  TransformRenderData* CreateTransformData(const LayersInfo* layer);
  TransformRenderData* CreateTransformData(const ShapeGroup* group, unsigned int ind, float inpos, float outpos);
  static bool ReleaseRenderData(BaseRenderData* data);
};

typedef std::shared_ptr<VerticesRenderData> VerticesRenderDataPtr;
typedef std::shared_ptr<ColorRenderData> ColorRenderDataPtr;
typedef std::shared_ptr<TransformRenderData> TransformRenderDataPtr;

class SRenderDataFactory{
private:
  SRenderDataFactory(){
    render_data_factory_ = new RenderDataFactory();
  }

  ~SRenderDataFactory(){
      delete render_data_factory_;
  }

public:
  static SRenderDataFactory &GetIns(){
    static SRenderDataFactory kIns;
    return kIns;
  }

  VerticesRenderDataPtr CreateVerticesData(const LayersInfo* layer){
    return VerticesRenderDataPtr(render_data_factory_->CreateVerticesData(layer), [=](VerticesRenderData *data){
      render_data_factory_->ReleaseRenderData(data);
    });
  }

  ColorRenderDataPtr CreateColorData(const LayersInfo* layer){
    return ColorRenderDataPtr(render_data_factory_->CreateColorData(layer), [=](ColorRenderData *data){
      render_data_factory_->ReleaseRenderData(data);
    });
  }

  TransformRenderDataPtr CreateTransformData(const LayersInfo* layer){
    return TransformRenderDataPtr(render_data_factory_->CreateTransformData(layer),[=](TransformRenderData* data){
      render_data_factory_->ReleaseRenderData((BaseRenderData*)data);
    });
  }

  TransformRenderDataPtr CreateTransformData(const ShapeGroup* group, unsigned int ind, float inpos, float outpos){
    return TransformRenderDataPtr(render_data_factory_->CreateTransformData(group, ind, inpos, outpos),[=](TransformRenderData* data){
      render_data_factory_->ReleaseRenderData((BaseRenderData*)data);
    });
  }

private:
  RenderDataFactory* render_data_factory_;
};

}