#include "sdf.h"
#include "solver.h"

glm::vec2 worldOrigin(0.f, 0.f);

/* Functions in class Polygon */
void Polygon::computeAabb() {
  lb = vertices[0];
  rt = vertices[1];

  for (int i = 0; i < vertices.size(); i++) {
    glm::vec2 vtx = vertices[i];

    lb.x = (lb.x < vtx.x) ? lb.x : vtx.x;
    lb.y = (lb.y < vtx.y) ? lb.y : vtx.y;

    rt.x = (rt.x > vtx.x) ? rt.x : vtx.x;
    rt.y = (rt.y > vtx.y) ? rt.y : vtx.y;
  }
}

void Polygon::add(glm::vec2 vtx) { vertices.push_back(vtx); }

void Polygon::translate(glm::vec2 xy) {
  for (int i = 0; i < vertices.size(); i++) {
    vertices[i] += xy;
  }

  // update aabb
  lb += xy;
  rt += xy;
}

glm::vec2 Polygon::applyRotate(glm::vec2 vtx, float theta, glm::vec2 offset) {
  float x, y;

  // translate to origin
  x = vtx.x - offset.x;
  y = vtx.y - offset.y;

  // rotate
  vtx.x = x * cos(theta) - y * sin(theta);
  vtx.y = x * sin(theta) + y * cos(theta);

  // translate back
  vtx.x += offset.x;
  vtx.y += offset.y;

  return vtx;
}

void Polygon::rotate(float theta) {
  // degree to radian
  theta = 3.1415926f / 180.f * theta;

  glm::vec2 center = (lb + rt) * 0.5f;
  glm::vec2 offset = center - worldOrigin; // offset from origin

  float x, y;

  for (int i = 0; i < vertices.size(); i++) {
    vertices[i] = applyRotate(vertices[i], theta, offset);
  }

  // update aabb
  computeAabb();
}

glm::vec2 Polygon::applyScale(glm::vec2 vtx, float factor, glm::vec2 offset) {
  vtx -= offset; // translate to origin
  vtx *= factor;
  vtx += offset; // translate back

  return vtx;
}

void Polygon::scale(float factor) {
  glm::vec2 center = (lb + rt) * 0.5f;
  glm::vec2 offset = center - worldOrigin; // offset from origin

  for (int i = 0; i < vertices.size(); i++) {
    vertices[i] = applyScale(vertices[i], factor, offset);
  }

  // update aabb
  lb = applyScale(lb, factor, offset);
  rt = applyScale(rt, factor, offset);
}

void Polygon::DrawPolygon() {
  glColor3f(.616f, 0.29f, 0.663f);

  glBegin(GL_POLYGON);
  for (size_t i = 0; i < vertices.size(); i++) {
    glVertex2f(vertices[i].x, vertices[i].y);
  }
  glEnd();
}
/* end Functions in class Polygon */

bool intersect(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4) {
  glm::vec3 a(p1, 0), b(p2, 0), c(p3, 0), d(p4, 0);

  float crossAcAb, crossAbAd, crossDaDc, crossDcDb;
  crossAcAb = glm::cross(c - a, b - a).z;
  crossAbAd = glm::cross(b - a, d - a).z;
  crossDaDc = glm::cross(a - d, c - d).z;
  crossDcDb = glm::cross(c - d, b - d).z;

  if (crossAcAb * crossAbAd > 0 && crossDaDc * crossDcDb > 0) {
    return true;
  } else {
    return false;
  }
}

bool inside_polygon(glm::vec2 p, Polygon &poly) {
  int count = 0;
  glm::vec2 q(1234567.f, 1234567.f); // a point at the infinity
  int len = poly.vertices.size();

  for (int i = 0; i < len; i++) {
    glm::vec2 &start = poly.vertices[i];
    glm::vec2 &end = poly.vertices[(i + 1) % len];
    count += intersect(p, q, start, end);
  }

  return count % 2 == 1;
}

float nearest_distance(glm::vec2 p, Polygon &poly) {
  float dist = 9999.f;
  int len = poly.vertices.size();

  for (int i = 0; i < len; i++) {
    glm::vec2 &a = poly.vertices[i];
    glm::vec2 &b = poly.vertices[(i + 1) % len];

    //当前处理的边定义为 ab
    glm::vec2 ab = b - a;
    float abLength = glm::length(ab);
    glm::vec2 dir = glm::normalize(ab);

    //求点 p 在边 ab 上的投影点 q
    //于是，线段 pq 的长度就是 p 点的有向距离
    glm::vec2 ap = p - a;
    float frac = glm::dot(ap, dir);
    frac = glm::clamp(frac, 0.f, abLength);
    glm::vec2 q = a + frac * dir;
    glm::vec2 pq = q - p;

    dist = glm::min(dist, length(pq));
  }

  return dist;
}

// p is world position
// float getDistance(glm::vec2 p) {
//   // world position to sdf index
//   int idx_x, idx_y;
//   idx_x = int(glm::floor(p.x / sdfCellSize));
//   idx_y = int(glm::floor(p.y / sdfCellSize));
//
//   if (idx_x < 0 || idx_x > sdfWidth - 1) {
//     return 9999.f;
//   } else if (idx_y < 0.f || idx_y > sdfHeight - 1) {
//     return 9999.f;
//   } else {
//     return grid.sdf.at<float>(Point(idx_x, idx_y));
//   }
// }

// p is world position

void computeSdf(int sdfWidth, int sdfHeight, float sdfCellSize,
                std::vector<Polygon> &polygons, Solver &slv) {
  // compute sdf for (sdfWidth * sdfHeight) world space points
  // the interval between two adjacent points is sdfCellSize
  for (int x = 0; x < sdfWidth; x++) {
    for (int y = 0; y < sdfHeight; y++) {
      // std::cout << x << ", " << y << '\n';

      // world space point
      glm::vec2 p = worldOrigin + glm::vec2(sdfCellSize * x, sdfCellSize * y);

      float fDist = 9999.f;
      float temp = 0.f;

      for (int i = 0; i < polygons.size(); i++) {
        // std::cout << i << '\n';
        temp = (inside_polygon(p, polygons[i])) ? -1.f : 1.f;
        temp *= nearest_distance(p, polygons[i]);
        // std::cout << "here" << '\n';

        // fDist = glm::min(temp, fDist);
        if (temp < fDist) {
          fDist = temp;

          // change to Node
          // the nearest polygon to this node
          // grid.polyPtrs.set(y, x, polygons[i]);
        }
      }

      // save sdf distance
      int idx = x + y * (sdfWidth - 1); // # of nodes in a row is (sdfWidth - 1)
      slv.nodes[idx].sdfDist = fDist;
      // std::cout << "(" << x << ", " << y << ") distance = " << fDist << '\n';
    }
  }
}
