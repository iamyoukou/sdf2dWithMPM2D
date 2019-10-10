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

class Polygon {
public:
  std::vector<glm::vec2> vertices;    // for drawing
  std::vector<glm::vec2> sdfVertices; // for computing sdf
  glm::vec2 lb, rt;                   // aabb, left-bottom, right-top
  glm::vec2 v;                        // linear velocity
  float omega;                        // rotate velocity
  float r, g, b;
  std::string name; // for test purpose
  bool isConvex;

  /* Constructors */
  Polygon(){};
  ~Polygon(){};

  /* Functions */
  void computeAabb();
  void add(glm::vec2, bool);
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
    square.isConvex = true;
    square.add(glm::vec2(146.5f, 12.f), NORMAL_VTX);
    square.add(glm::vec2(104.f, 49.f), NORMAL_VTX);
    square.add(glm::vec2(150.5f, 84.f), NORMAL_VTX);
    square.add(glm::vec2(200.5f, 56.f), NORMAL_VTX);
    square.computeAabb();
    square.translate(glm::vec2(0.f, 50.f));
    square.r = 0.616f;
    square.g = 0.29f;
    square.b = 0.663f;
    square.v = glm::vec2(0.f, 0.f);
    square.omega = 1.f;
    //
    // Polygon object2;
    // object2.add(glm::vec2(29.5f, 87.5f));
    // object2.add(glm::vec2(29.5f, 136.5f));
    // object2.add(glm::vec2(84.5f, 175.f));
    // object2.add(glm::vec2(132.5f, 147.f));
    // object2.computeAabb();
    //
    // Polygon object3;
    // object3.add(glm::vec2(207.5f, 118.f));
    // object3.add(glm::vec2(233.f, 186.f));
    // object3.add(glm::vec2(284.5f, 139.f));
    // object3.computeAabb();

    // Knife A
    // Polygon knifeA;
    // knifeA.isConvex = false;
    // // vertices for sdf
    // // When compute sdf, use the original convex
    // knifeA.add(glm::vec2(68.5f, 161.f), SDF_VTX);   // 0
    // knifeA.add(glm::vec2(131.5f, 161.f), SDF_VTX);  // 1
    // knifeA.add(glm::vec2(131.5f, 173.f), SDF_VTX);  // 2
    // knifeA.add(glm::vec2(143.5f, 173.f), SDF_VTX);  // 3
    // knifeA.add(glm::vec2(143.5f, 164.5f), SDF_VTX); // 4
    // knifeA.add(glm::vec2(220.5f, 164.5f), SDF_VTX); // 5
    // knifeA.add(glm::vec2(244.f, 151.f), SDF_VTX);   // 6
    // knifeA.add(glm::vec2(220.5f, 137.5f), SDF_VTX); // 7
    // knifeA.add(glm::vec2(143.5f, 137.5f), SDF_VTX); // 8
    // knifeA.add(glm::vec2(143.5f, 129.f), SDF_VTX);  // 9
    // knifeA.add(glm::vec2(131.5f, 129.f), SDF_VTX);  // 10
    // knifeA.add(glm::vec2(131.5f, 141.f), SDF_VTX);  // 11
    // knifeA.add(glm::vec2(68.5f, 141.f), SDF_VTX);   // 12
    // // vertices for drawing
    // // Since opengl cannot draw concaves,
    // // I separate the polygon into several triangles
    // // triangle 1
    // knifeA.add(glm::vec2(68.5f, 161.f), NORMAL_VTX);  // 0
    // knifeA.add(glm::vec2(131.5f, 141.f), NORMAL_VTX); // 11
    // knifeA.add(glm::vec2(68.5f, 141.f), NORMAL_VTX);  // 12
    // // triangle 2
    // knifeA.add(glm::vec2(68.5f, 161.f), NORMAL_VTX);  // 0
    // knifeA.add(glm::vec2(131.5f, 161.f), NORMAL_VTX); // 1
    // knifeA.add(glm::vec2(131.5f, 141.f), NORMAL_VTX); // 11
    // // triangle 3
    // knifeA.add(glm::vec2(131.5f, 173.f), NORMAL_VTX); // 2
    // knifeA.add(glm::vec2(143.5f, 173.f), NORMAL_VTX); // 3
    // knifeA.add(glm::vec2(131.5f, 129.f), NORMAL_VTX); // 10
    // // triangle 4
    // knifeA.add(glm::vec2(143.5f, 173.f), NORMAL_VTX); // 3
    // knifeA.add(glm::vec2(143.5f, 129.f), NORMAL_VTX); // 9
    // knifeA.add(glm::vec2(131.5f, 129.f), NORMAL_VTX); // 10
    // // triangle 5
    // knifeA.add(glm::vec2(143.5f, 164.5f), NORMAL_VTX); // 4
    // knifeA.add(glm::vec2(220.5f, 164.5f), NORMAL_VTX); // 5
    // knifeA.add(glm::vec2(143.5f, 137.5f), NORMAL_VTX); // 8
    // // triangle 6
    // knifeA.add(glm::vec2(220.5f, 164.5f), NORMAL_VTX); // 5
    // knifeA.add(glm::vec2(220.5f, 137.5f), NORMAL_VTX); // 7
    // knifeA.add(glm::vec2(143.5f, 137.5f), NORMAL_VTX); // 8
    // // triangle 7
    // knifeA.add(glm::vec2(220.5f, 164.5f), NORMAL_VTX); // 5
    // knifeA.add(glm::vec2(244.f, 151.f), NORMAL_VTX);   // 6
    // knifeA.add(glm::vec2(220.5f, 137.5f), NORMAL_VTX); // 7
    // // when finish adding vertices, don't forget to call computeAabb()
    // knifeA.computeAabb();
    // // transform
    // knifeA.translate(glm::vec2(-100.f, -25.f));
    // knifeA.rotate(45.f);
    // knifeA.scale(0.2f);
    // // other properties
    // knifeA.r = 0.616f;
    // knifeA.g = 0.29f;
    // knifeA.b = 0.663f;
    // knifeA.v = glm::vec2(40.f, 40.f);
    // knifeA.omega = 1.f;

    // polys.push_back(square);
    // polys.push_back(object2);
    // polys.push_back(object3);
    // polys.push_back(knifeA);

    return polys;
  }
}; // end class Polygon
