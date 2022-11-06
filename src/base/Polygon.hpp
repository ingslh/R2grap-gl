#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <earcut.hpp>

namespace mapbox {
  namespace util {
    template <>struct nth<0, glm::vec2> {
      inline static auto get(const glm::vec2 &t) {
        return t.x;
      };
    };
    template <>struct nth<1, glm::vec2> {
      inline static auto get(const glm::vec2 &t) {
        return t.y;
      };
    };

  } // namespace util
} // namespace mapbox

namespace R2grap{
using Point = glm::vec2;
class PolygonArray{
public:
  PolygonArray(const std::vector<glm::vec2>& vert){
    vertices_.emplace_back(vert);
  }

  std::vector<std::vector<Point>> getVertices() const {return vertices_;}
  void addBezierVert(const std::vector<glm::vec2>& vert) { vertices_.emplace_back(vert); }
  
private:
  std::vector<std::vector<Point>> vertices_;
};
} // namespace R2grap