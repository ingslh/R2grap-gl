#pragma once
#include <mutex>
#include <memory>
namespace R2grap{
enum rDataType{
  tNoneData = 0,
  tVerticesData,
  tColorData,
  tTransformData,
};
class BaseRenderData{
public:
  rDataType GetType() const {
    return type_;
  }

  std::recursive_mutex& GetEditingLock(){
    return mutex_;
  }

private:
  rDataType type_;
  mutable std::recursive_mutex mutex_;
};
typedef std::shared_ptr<BaseRenderData> BaseRenderDataPtr;
}