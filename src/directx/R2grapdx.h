#include "D3DRender.h"
#include <string>


namespace R2grap{

class JsonReader;
class R2grapDX{
public:
  R2grapDx(const std::string& filename);
  void run();

private:
  std::shared_ptr<JsonReader> reader_;
  std::shared_ptr<D3DRender> dx_render_;
}

}