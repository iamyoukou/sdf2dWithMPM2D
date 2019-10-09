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

    // Polygon square;
    // square.isConvex = true;
    // square.add(glm::vec2(146.5f, 12.f));
    // square.add(glm::vec2(104.f, 49.f));
    // square.add(glm::vec2(150.5f, 84.f));
    // square.add(glm::vec2(200.5f, 56.f));
    // square.computeAabb();
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

    // Arrow A
    Polygon arrowA;
    arrowA.isConvex = false;
    // When compute sdf, use the original convex, i.e. 12 vertices
    arrowA.add(glm::vec2(91.5f, 150.f), true);   // 0
    arrowA.add(glm::vec2(85.5f, 138.5f), true);  // 1
    arrowA.add(glm::vec2(104.f, 138.5f), true);  // 2
    arrowA.add(glm::vec2(109.5f, 148.5f), true); // 3
    arrowA.add(glm::vec2(201.f, 148.5f), true);  // 4
    arrowA.add(glm::vec2(201.f, 138.5f), true);  // 5
    arrowA.add(glm::vec2(225.f, 150.f), true);   // 6
    arrowA.add(glm::vec2(201.f, 163.f), true);   // 7
    arrowA.add(glm::vec2(201.f, 151.5f), true);  // 8
    arrowA.add(glm::vec2(109.5f, 151.5f), true); // 9
    arrowA.add(glm::vec2(104.f, 162.f), true);   // 10
    arrowA.add(glm::vec2(85.5f, 162.f), true);   // 11
    // Since opengl cannot draw concaves,
    // I separate the polygon into several triangles
    // triangle 1
    arrowA.add(glm::vec2(91.5f, 150.f), false);  // 0
    arrowA.add(glm::vec2(85.5f, 138.5f), false); // 1
    arrowA.add(glm::vec2(104.f, 138.5f), false); // 2
    // triagnle 2
    arrowA.add(glm::vec2(91.5f, 150.f), false);   // 0
    arrowA.add(glm::vec2(104.f, 138.5f), false);  // 2
    arrowA.add(glm::vec2(109.5f, 148.5f), false); // 3
    // triangle 3
    arrowA.add(glm::vec2(91.5f, 150.f), false);   // 0
    arrowA.add(glm::vec2(109.5f, 148.5f), false); // 3
    arrowA.add(glm::vec2(109.5f, 151.5f), false); // 9
    // triagnle 4
    arrowA.add(glm::vec2(91.5f, 150.f), false);   // 0
    arrowA.add(glm::vec2(109.5f, 151.5f), false); // 9
    arrowA.add(glm::vec2(104.f, 162.f), false);   // 10
    // triangle 5
    arrowA.add(glm::vec2(91.5f, 150.f), false); // 0
    arrowA.add(glm::vec2(104.f, 162.f), false); // 10
    arrowA.add(glm::vec2(85.5f, 162.f), false); // 11
    // triangle 6
    arrowA.add(glm::vec2(109.5f, 148.5f), false); // 3
    arrowA.add(glm::vec2(201.f, 151.5f), false);  // 8
    arrowA.add(glm::vec2(109.5f, 151.5f), false); // 9
    // triangle 7
    arrowA.add(glm::vec2(109.5f, 148.5f), false); // 3
    arrowA.add(glm::vec2(201.f, 148.5f), false);  // 4
    arrowA.add(glm::vec2(201.f, 151.5f), false);  // 8
    // triangle 8
    arrowA.add(glm::vec2(201.f, 151.5f), false); // 8
    arrowA.add(glm::vec2(225.f, 150.f), false);  // 6
    arrowA.add(glm::vec2(201.f, 163.f), false);  // 7
    // triangle 9
    arrowA.add(glm::vec2(201.f, 148.5f), false); // 4
    arrowA.add(glm::vec2(225.f, 150.f), false);  // 6
    arrowA.add(glm::vec2(201.f, 151.5f), false); // 8
    // triangle 10
    arrowA.add(glm::vec2(201.f, 138.5f), false); // 5
    arrowA.add(glm::vec2(225.f, 150.f), false);  // 6
    arrowA.add(glm::vec2(201.f, 148.5f), false); // 4
    // when finish adding vertices, don't forget to call computeAabb()
    arrowA.computeAabb();
    // transform
    arrowA.translate(glm::vec2(0.f, -25.f));
    arrowA.rotate(45.f);
    arrowA.scale(0.375f);
    // other properties
    arrowA.r = 0.616f;
    arrowA.g = 0.29f;
    arrowA.b = 0.663f;
    arrowA.v = glm::vec2(0.f, 40.f);
    arrowA.omega = 1.f;

    // Arrow B

    // Arrow C

    // polys.push_back(square);
    // polys.push_back(object2);
    // polys.push_back(object3);
    polys.push_back(arrowA);

    return polys;
  }
}; // end class Polygon
