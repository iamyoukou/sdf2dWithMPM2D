#include "sdf.h"
#include "solver.h"

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

void Polygon::add(glm::vec2 vtx, bool isSdf) {
  if (isSdf) {
    sdfVertices.push_back(vtx);
  } else {
    vertices.push_back(vtx);
  }
}

void Polygon::translate(glm::vec2 xy) {
  // vertices and sdfVertices share the same transformation
  for (int i = 0; i < vertices.size(); i++) {
    vertices[i] += xy;
  }

  for (int i = 0; i < sdfVertices.size(); i++) {
    sdfVertices[i] += xy;
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

  for (int i = 0; i < sdfVertices.size(); i++) {
    sdfVertices[i] = applyRotate(sdfVertices[i], theta, offset);
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

  for (int i = 0; i < sdfVertices.size(); i++) {
    sdfVertices[i] = applyScale(sdfVertices[i], factor, offset);
  }

  // update aabb
  lb = applyScale(lb, factor, offset);
  rt = applyScale(rt, factor, offset);
}

void Polygon::DrawPolygon() {
  glColor3f(r, g, b);

  GLenum mode = isConvex ? GL_POLYGON : GL_TRIANGLES;

  glBegin(mode);
  for (size_t i = 0; i < vertices.size(); i++) {
    glVertex2f(vertices[i].x, vertices[i].y);
  }
  glEnd();
}
/* end Functions in class Polygon */
