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
    // square.add(glm::vec2(150.f, 10.f), NORMAL_VTX);
    // square.add(glm::vec2(100.f, 60.f), NORMAL_VTX);
    // square.add(glm::vec2(150.f, 110.f), NORMAL_VTX);
    // square.add(glm::vec2(200.f, 60.f), NORMAL_VTX);
    // square.computeAabb();
    // square.translate(glm::vec2(25.f, 50.f));
    // square.scale(0.5f);
    // square.r = 0.616f;
    // square.g = 0.29f;
    // square.b = 0.663f;
    // square.v = glm::vec2(0.f, 0.f);
    // square.omega = 0.f;

    Polygon fan;
    // vertices for sdf
    fan.add(glm::vec2(11.8f, 4.7f), SDF_VTX);  // 0
    fan.add(glm::vec2(25.f, 17.7f), SDF_VTX);  // 1
    fan.add(glm::vec2(36.1f, 6.8f), SDF_VTX);  // 2
    fan.add(glm::vec2(55.9f, 4.7f), SDF_VTX);  // 3
    fan.add(glm::vec2(42.7f, 17.9f), SDF_VTX); // 4
    fan.add(glm::vec2(53.8f, 29.1f), SDF_VTX); // 5
    fan.add(glm::vec2(55.7f, 48.6f), SDF_VTX); // 6
    fan.add(glm::vec2(42.6f, 35.7f), SDF_VTX); // 7
    fan.add(glm::vec2(31.5f, 46.6f), SDF_VTX); // 8
    fan.add(glm::vec2(11.9f, 48.7f), SDF_VTX); // 9
    fan.add(glm::vec2(24.9f, 35.3f), SDF_VTX); // 10
    fan.add(glm::vec2(13.8f, 24.3f), SDF_VTX); // 11
    // vertices for drawing
    // triangle 1
    fan.add(glm::vec2(11.8f, 4.7f), NORMAL_VTX);  // 0
    fan.add(glm::vec2(25.f, 17.7f), NORMAL_VTX);  // 1
    fan.add(glm::vec2(13.8f, 24.3f), NORMAL_VTX); // 11
    // triangle 2
    fan.add(glm::vec2(13.8f, 24.3f), NORMAL_VTX); // 11
    fan.add(glm::vec2(25.f, 17.7f), NORMAL_VTX);  // 1
    fan.add(glm::vec2(24.9f, 35.3f), NORMAL_VTX); // 10
    // triangle 3
    fan.add(glm::vec2(25.f, 17.7f), NORMAL_VTX);  // 1
    fan.add(glm::vec2(42.7f, 17.9f), NORMAL_VTX); // 4
    fan.add(glm::vec2(24.9f, 35.3f), NORMAL_VTX); // 10
    // triangle 4
    fan.add(glm::vec2(25.f, 17.7f), NORMAL_VTX);  // 1
    fan.add(glm::vec2(36.1f, 6.8f), NORMAL_VTX);  // 2
    fan.add(glm::vec2(42.7f, 17.9f), NORMAL_VTX); // 4
    // triangle 5
    fan.add(glm::vec2(36.1f, 6.8f), NORMAL_VTX);  // 2
    fan.add(glm::vec2(55.9f, 4.7f), NORMAL_VTX);  // 3
    fan.add(glm::vec2(42.7f, 17.9f), NORMAL_VTX); // 4
    // triangle 6
    fan.add(glm::vec2(11.9f, 48.7f), NORMAL_VTX); // 9
    fan.add(glm::vec2(24.9f, 35.3f), NORMAL_VTX); // 10
    fan.add(glm::vec2(31.5f, 46.6f), NORMAL_VTX); // 8
    // triangle 7
    fan.add(glm::vec2(24.9f, 35.3f), NORMAL_VTX); // 10
    fan.add(glm::vec2(42.6f, 35.7f), NORMAL_VTX); // 7
    fan.add(glm::vec2(31.5f, 46.6f), NORMAL_VTX); // 8
    // triangle 8
    fan.add(glm::vec2(24.9f, 35.3f), NORMAL_VTX); // 10
    fan.add(glm::vec2(42.7f, 17.9f), NORMAL_VTX); // 4
    fan.add(glm::vec2(42.6f, 35.7f), NORMAL_VTX); // 7
    // triangle 9
    fan.add(glm::vec2(42.6f, 35.7f), NORMAL_VTX); // 7
    fan.add(glm::vec2(42.7f, 17.9f), NORMAL_VTX); // 4
    fan.add(glm::vec2(53.8f, 29.1f), NORMAL_VTX); // 5
    // triangle 10
    fan.add(glm::vec2(42.6f, 35.7f), NORMAL_VTX); // 7
    fan.add(glm::vec2(53.8f, 29.1f), NORMAL_VTX); // 5
    fan.add(glm::vec2(55.7f, 48.6f), NORMAL_VTX); // 6
    fan.computeAabb();
    // transform
    fan.translate(glm::vec2(140.f, 70.f));
    fan.scale(2.f);
    fan.rotate(45.f);
    // other properties
    fan.r = 0.616f;
    fan.g = 0.29f;
    fan.b = 0.663f;
    fan.v = glm::vec2(0.f, 0.f);
    fan.omega = 0.f;

    // faucet
    Polygon faucet;
    faucet.isConvex = false;
    // vertices for sdf
    faucet.add(glm::vec2(115.5f, 161.5f), SDF_VTX); // 0
    faucet.add(glm::vec2(140.f, 161.5f), SDF_VTX);  // 1
    faucet.add(glm::vec2(140.f, 144.5f), SDF_VTX);  // 2
    faucet.add(glm::vec2(187.5f, 144.5f), SDF_VTX); // 3
    faucet.add(glm::vec2(187.5f, 126.f), SDF_VTX);  // 4
    faucet.add(glm::vec2(170.5f, 126.f), SDF_VTX);  // 5
    faucet.add(glm::vec2(170.5f, 116.5f), SDF_VTX); // 6
    faucet.add(glm::vec2(178.5f, 116.5f), SDF_VTX); // 7
    faucet.add(glm::vec2(178.5f, 109.5f), SDF_VTX); // 8
    faucet.add(glm::vec2(152.5f, 109.5f), SDF_VTX); // 9
    faucet.add(glm::vec2(152.5f, 116.5f), SDF_VTX); // 10
    faucet.add(glm::vec2(160.f, 116.5f), SDF_VTX);  // 11
    faucet.add(glm::vec2(160.f, 126.f), SDF_VTX);   // 12
    faucet.add(glm::vec2(115.5f, 126.f), SDF_VTX);  // 13
    // vertices for drawing
    // triangle 1
    faucet.add(glm::vec2(115.5f, 161.5f), NORMAL_VTX); // 0
    faucet.add(glm::vec2(140.f, 161.5f), NORMAL_VTX);  // 1
    faucet.add(glm::vec2(140.f, 144.5f), NORMAL_VTX);  // 2
    // triangle 2
    faucet.add(glm::vec2(115.5f, 161.5f), NORMAL_VTX); // 0
    faucet.add(glm::vec2(140.f, 144.5f), NORMAL_VTX);  // 2
    faucet.add(glm::vec2(115.5f, 126.f), NORMAL_VTX);  // 13
    // triangle 3
    faucet.add(glm::vec2(140.f, 144.5f), NORMAL_VTX); // 2
    faucet.add(glm::vec2(160.f, 126.f), NORMAL_VTX);  // 12
    faucet.add(glm::vec2(115.5f, 126.f), NORMAL_VTX); // 13
    // triangle 4
    faucet.add(glm::vec2(140.f, 144.5f), NORMAL_VTX);  // 2
    faucet.add(glm::vec2(187.5f, 144.5f), NORMAL_VTX); // 3
    faucet.add(glm::vec2(160.f, 126.f), NORMAL_VTX);   // 12
    // triangle 5
    faucet.add(glm::vec2(187.5f, 144.5f), NORMAL_VTX); // 3
    faucet.add(glm::vec2(170.5f, 126.f), NORMAL_VTX);  // 5
    faucet.add(glm::vec2(160.f, 126.f), NORMAL_VTX);   // 12
    // triangle 6
    faucet.add(glm::vec2(187.5f, 144.5f), NORMAL_VTX); // 3
    faucet.add(glm::vec2(187.5f, 126.f), NORMAL_VTX);  // 4
    faucet.add(glm::vec2(170.5f, 126.f), NORMAL_VTX);  // 5
    // triangle 7
    faucet.add(glm::vec2(170.5f, 126.f), NORMAL_VTX); // 5
    faucet.add(glm::vec2(160.f, 116.5f), NORMAL_VTX); // 11
    faucet.add(glm::vec2(160.f, 126.f), NORMAL_VTX);  // 12
    // triangle 8
    faucet.add(glm::vec2(170.5f, 126.f), NORMAL_VTX);  // 5
    faucet.add(glm::vec2(170.5f, 116.5f), NORMAL_VTX); // 6
    faucet.add(glm::vec2(160.f, 116.5f), NORMAL_VTX);  // 11
    // triangle 9
    faucet.add(glm::vec2(152.5f, 109.5f), NORMAL_VTX); // 9
    faucet.add(glm::vec2(152.5f, 116.5f), NORMAL_VTX); // 10
    faucet.add(glm::vec2(160.f, 116.5f), NORMAL_VTX);  // 11
    // triangle 10
    faucet.add(glm::vec2(178.5f, 109.5f), NORMAL_VTX); // 8
    faucet.add(glm::vec2(152.5f, 109.5f), NORMAL_VTX); // 9
    faucet.add(glm::vec2(160.f, 116.5f), NORMAL_VTX);  // 11
    // triangle 11
    faucet.add(glm::vec2(170.5f, 116.5f), NORMAL_VTX); // 6
    faucet.add(glm::vec2(178.5f, 109.5f), NORMAL_VTX); // 8
    faucet.add(glm::vec2(160.f, 116.5f), NORMAL_VTX);  // 11
    // triangle 12
    faucet.add(glm::vec2(170.5f, 116.5f), NORMAL_VTX); // 6
    faucet.add(glm::vec2(178.5f, 116.5f), NORMAL_VTX); // 7
    faucet.add(glm::vec2(178.5f, 109.5f), NORMAL_VTX); // 8
    faucet.computeAabb();
    // transform
    faucet.translate(glm::vec2(-60.f, 75.f));
    faucet.rotate(-180.f);
    faucet.scale(0.75f);
    // other properties
    faucet.r = 0.616f;
    faucet.g = 0.29f;
    faucet.b = 0.663f;
    faucet.v = glm::vec2(0.f, 0.f);
    faucet.omega = 0.f;

    // // Knife A
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
    polys.push_back(fan);
    // polys.push_back(knifeA);
    polys.push_back(faucet);

    return polys;
  }

  static std::vector<Polygon> EmptyPolygons() {
    std::vector<Polygon> polys;

    return polys;
  }
}; // end class Polygon
