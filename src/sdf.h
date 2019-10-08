#pragma once

#include "algebra.h"
#include "constants.h"
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>

extern glm::vec2 worldOrigin;

class Polygon {
public:
  std::vector<glm::vec2> vertices;
  glm::vec2 lb, rt; // aabb, left-bottom, right-top
  glm::vec2 v;      // linear velocity
  float omega;      // rotate velocity
  float r, g, b;
  std::string name; // for test purpose

  /* Constructors */
  Polygon(){};
  ~Polygon(){};

  /* Functions */
  void computeAabb();
  void add(glm::vec2);
  void translate(glm::vec2);
  glm::vec2 applyRotate(glm::vec2, float, glm::vec2);
  void rotate(float);
  glm::vec2 applyScale(glm::vec2, float, glm::vec2);
  void scale(float);
  void DrawPolygon();

  /* Static Functions */
  static std::vector<Polygon> InitializePolygons() {
    std::vector<Polygon> polys;

    Polygon square;
    square.add(glm::vec2(146.5f, 12.f));
    square.add(glm::vec2(104.f, 49.f));
    square.add(glm::vec2(150.5f, 84.f));
    square.add(glm::vec2(200.5f, 56.f));
    square.computeAabb();

    polys.push_back(square);

    return polys;
  }
}; // end class Polygon

bool intersect(glm::vec2, glm::vec2, glm::vec2, glm::vec2);
bool inside_polygon(glm::vec2, Polygon &);
float nearest_distance(glm::vec2, Polygon &);
