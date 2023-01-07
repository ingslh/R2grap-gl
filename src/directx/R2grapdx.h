#include "D3DRender.h"
#include <string>
#include <vector>


namespace R2grap{

struct RePathObj;
class JsonReader;
class R2grapDx{
public:
  R2grapDx(const std::string& filename);
  int run();

private:
  std::shared_ptr<JsonReader> reader_;
  std::shared_ptr<D3DRender> dx_render_;
  std::vector<RePathObj> objs_;
};

}