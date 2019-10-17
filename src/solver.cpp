#include "solver.h"

/* Constructors */
template <typename Type>
Solver<Type>::Solver(const std::vector<Border> &inBorders,
                     const std::vector<Node> &inNodes,
                     const std::vector<Type> &inParticles,
                     const std::vector<Polygon> &inPolygons) {
  borders = inBorders;
  nodes = inNodes;
  particles = inParticles;
  polygons = inPolygons;

  blen = borders.size();
  ilen = nodes.size();
  plen = particles.size();
  polylen = polygons.size();

  dt_rob = DT_ROB;
}

/* -----------------------------------------------------------------------
|			          Functions for computing sdf
|
----------------------------------------------------------------------- */
template <typename Type> void Solver<Type>::computeSdf() {
  // compute sdf for (sdfWidth * sdfHeight) world space points
  // the interval between two adjacent points is sdfCellSize
  for (int x = 0; x < sdfWidth; x++) {
    for (int y = 0; y < sdfHeight; y++) {
      // std::cout << x << ", " << y << '\n';

      // world space point
      glm::vec2 p = worldOrigin + glm::vec2(sdfCellSize * x, sdfCellSize * y);

      float fDist = 9999.f;
      float temp = 0.f;

      // node index
      int idx = x + y * (sdfWidth - 1); // # of nodes in a row is (sdfWidth - 1)

      for (int i = 0; i < polygons.size(); i++) {
        // std::cout << i << '\n';
        temp = (inside_polygon(p, polygons[i])) ? -1.f : 1.f;
        temp *= nearest_distance(p, polygons[i]);
        // std::cout << "here" << '\n';

        // fDist = glm::min(temp, fDist);
        if (temp < fDist) {
          fDist = temp;

          // the nearest polygon from this node
          nodes[idx].nearestPolygonPtr = &polygons[i];
        }
      }

      // save sdf distance
      nodes[idx].sdfDist = fDist;
      // std::cout << "(" << x << ", " << y << ") distance = " << fDist << '\n';
    }
  }
}

template <typename Type>
bool Solver<Type>::intersect(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3,
                             glm::vec2 p4) {
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

template <typename Type>
bool Solver<Type>::inside_polygon(glm::vec2 p, Polygon &poly) {
  int count = 0;
  glm::vec2 q(1234567.f, 1234567.f); // a point at the infinity
  int len = poly.sdfVertices.size();

  for (int i = 0; i < len; i++) {
    glm::vec2 &start = poly.sdfVertices[i];
    glm::vec2 &end = poly.sdfVertices[(i + 1) % len];
    count += intersect(p, q, start, end);
  }

  return count % 2 == 1;
}

template <typename Type>
float Solver<Type>::nearest_distance(glm::vec2 p, Polygon &poly) {
  float dist = 9999.f;
  int len = poly.sdfVertices.size();

  for (int i = 0; i < len; i++) {
    glm::vec2 &a = poly.sdfVertices[i];
    glm::vec2 &b = poly.sdfVertices[(i + 1) % len];

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
template <typename Type> float Solver<Type>::getDistance(glm::vec2 p) {

  // world position to sdf index
  int idx_x, idx_y;
  idx_x = int(glm::floor(p.x / sdfCellSize));
  idx_y = int(glm::floor(p.y / sdfCellSize));

  if (idx_x < 0 || idx_x > sdfWidth - 1) {
    return 9999.f;
  } else if (idx_y < 0.f || idx_y > sdfHeight - 1) {
    return 9999.f;
  } else {
    int idx = idx_x + idx_y * (sdfWidth - 1);
    // std::cout << idx << '\n';
    return nodes[idx].sdfDist;
  }
}

// p is world position
template <typename Type> glm::vec2 Solver<Type>::getGradient(glm::vec2 p) {
  glm::vec2 gd;

  // world position to sdf position
  glm::vec2 sdfPos, sdfPosFloor;
  sdfPos = p / sdfCellSize;
  sdfPosFloor = glm::floor(sdfPos);

  float fx, fy;
  fx = sdfPos.x - sdfPosFloor.x;
  fy = sdfPos.y - sdfPosFloor.y;

  float temp1 = getDistance(glm::vec2(p.x + 1.f, p.y)) -
                getDistance(glm::vec2(p.x - 1.f, p.y));
  float temp2 = getDistance(glm::vec2(p.x + 1.f, p.y + 1.f)) -
                getDistance(glm::vec2(p.x - 1.f, p.y + 1.f));
  gd.x = glm::lerp(temp1, temp2, fy);

  float temp3 = getDistance(glm::vec2(p.x, p.y + 1.f)) -
                getDistance(glm::vec2(p.x, p.y - 1.f));
  float temp4 = getDistance(glm::vec2(p.x + 1.f, p.y + 1.f)) -
                getDistance(glm::vec2(p.x + 1.f, p.y - 1.f));
  gd.y = glm::lerp(temp3, temp4, fx);

  gd = glm::normalize(-gd);

  return gd;
}

// p is world position
template <typename Type> Polygon *Solver<Type>::getPolygon(glm::vec2 p) {
  // world position to sdf index
  int idx_x, idx_y;
  idx_x = int(glm::floor(p.x / sdfCellSize));
  idx_y = int(glm::floor(p.y / sdfCellSize));

  if (idx_x < 0 || idx_x > sdfWidth - 1) {
    return NULL;
  } else if (idx_y < 0.f || idx_y > sdfHeight - 1) {
    return NULL;
  } else {
    int idx = idx_x + idx_y * (sdfWidth - 1);
    return nodes[idx].nearestPolygonPtr;
  }
}

template <typename Type> void Solver<Type>::applySdfCollision(Node &node) {
  // node world space position
  glm::vec2 pos(node.Xi[0], node.Xi[1]);
  glm::vec2 v(node.Vi[0], node.Vi[1]);

  // std::cout << glm::to_string(pos) << '\n';

  // distance
  float dist = getDistance(pos);
  glm::vec2 n;
  Polygon *co; // collision object
  bool isCollisionOn = false;

  glm::vec2 vco(0.f, 0.f);

  // 基于 sdf 的碰撞检测
  // 和边界处的碰撞检测
  // 唯一不同的是法向量 n 的计算方式
  // 而碰撞后的速度，采用同样��处理
  // for sdf collision detection
  // narrow band threshold
  if (glm::abs(dist) < NARROW_BAND) {
    n = -getGradient(pos);
    co = getPolygon(pos); // get collision object
    isCollisionOn = true;

    // object velocity
    // linear
    glm::vec2 vlin = co->v;

    // rotational
    glm::vec2 center = (co->lb + co->rt) * 0.5f;
    glm::vec2 r = pos - center;

    glm::vec2 vrot;
    vrot.x = -r.y / length(r);
    vrot.y = r.x / length(r);
    // glm::vec2 vrot(0.f, 0.f);

    vrot *= co->omega * length(r);

    vco = vlin + vrot;

    // relative velocity
    glm::vec2 vrel = v - vco;

    float vnLength = glm::dot(n, vrel);
    // std::cout << vnLength << ", " << isCollisionOn << '\n';

    // if not separating
    if (vnLength < 0.f && isCollisionOn) {
      glm::vec2 vt = vrel - vnLength * n;
      float mu = 0.55f;

      if (length(vt) <= -mu * vnLength) {
        vrel = glm::vec2(0.f, 0.f);
      } else {
        vrel = vt + mu * vnLength * normalize(vt);
      }

      // back to particle velocity
      v = vrel + vco;

      // std::cout << v.x << ", " << v.y << '\n';

      node.Vi[0] = v.x;
      node.Vi[1] = v.y;

      // std::cout << "node velocity: " << node.Vi[0] << ", " << node.Vi[1]
      //           << '\n';
    }
  }
}

/* -----------------------------------------------------------------------
|					MATERIAL POINT METHOD ALGORITHM
|
----------------------------------------------------------------------- */

// Transfer from Particles to Grid nodes
template <typename Type> void Solver<Type>::P2G() {
  // plen is computed here for when we add particles mid-simulaion
  // If there is no need to add new particles during simulation,
  // write this line in constructors
  // plen = particles.size();

#pragma omp parallel for
  for (int p = 0; p < plen; p++) {
    // Pre-update Ap (in particle loop)
    particles[p].ConstitutiveModel();

    // Index of bottom-left node closest to the particle
    int node_base = (X_GRID + 1) * static_cast<int>(particles[p].Xp[1] -
                                                    Translation_xp[1]) +
                    static_cast<int>(particles[p].Xp[0] - Translation_xp[0]);

    // Loop over all the close nodes (depend on interpolation through bni)
    for (int y = bni; y < 3; y++) {
      for (int x = bni; x < 3; x++) {
        // Index of the node
        int node_id = node_base + x + (X_GRID + 1) * y;

        // Distance and weight
        Vector2f dist = particles[p].Xp - nodes[node_id].Xi;
        float Wip = getWip(dist);
        Vector2f dWip = getdWip(dist);

        // Pre-compute node mass, node velocity and pre-update force increment
        // (APIC)
        float inMi = Wip * particles[p].Mp;
        Vector2f inVi = Wip * particles[p].Mp *
                        (particles[p].Vp +
                         Dp_scal * H_INV * H_INV * particles[p].Bp * (-dist));

        Vector2f inFi = particles[p].Ap * dWip;

// Udpate mass, velocity and force
// (atomic operation because 2 particles (i.e threads) can have nodes in commun)
#pragma omp atomic
        nodes[node_id].Mi += inMi;

#pragma omp atomic
        nodes[node_id].Vi[0] += inVi[0];
#pragma omp atomic
        nodes[node_id].Vi[1] += inVi[1];

#pragma omp atomic
        nodes[node_id].Fi[0] += inFi[0];
#pragma omp atomic
        nodes[node_id].Fi[1] += inFi[1];
      }
    }
  }
}

// Update node force and velocity
template <typename Type> void Solver<Type>::UpdateNodes() {
// Dynamic parallelization because not all the nodes are active
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < ilen; i++) {
    if (nodes[i].Mi > 0) {
      // Finish updating velocity, force, and apply updated force
      // (not done before because the loop was on particles)
      nodes[i].Vi /= nodes[i].Mi;
      nodes[i].Fi = DT * (-nodes[i].Fi / nodes[i].Mi + G);
      nodes[i].Vi += nodes[i].Fi;

      // Apply collisions and frictions
      applySdfCollision(nodes[i]); // sdf-based

      nodes[i].NodeCollisions(); // borders
#if FRICTION
      nodes[i].NodeFrictions();
#else
      nodes[i].Vi_fri = nodes[i].Vi_col;
#endif
    }
  }
}

// Transfer from Grid nodes to Particles
template <typename Type> void Solver<Type>::G2P() {
#pragma omp parallel for
  for (int p = 0; p < plen; p++) {
    // Index of bottom-left node closest to the particle
    int node_base = (X_GRID + 1) * static_cast<int>(particles[p].Xp[1] -
                                                    Translation_xp[1]) +
                    static_cast<int>(particles[p].Xp[0] - Translation_xp[0]);

    // Set velocity and velocity field to 0 for sum update
    particles[p].Vp.setZeros();
    particles[p].Bp.setZeros();

    // Loop over all the close nodes (depend on interpolation through bni)
    for (int y = bni; y < 3; y++) {
      for (int x = bni; x < 3; x++) {
        // Index of the node
        int node_id = node_base + x + (X_GRID + 1) * y;

        // Distance and weight
        Vector2f dist = particles[p].Xp - nodes[node_id].Xi;
        float Wip = getWip(dist);

        // Update velocity and velocity field (APIC)
        particles[p].Vp += Wip * nodes[node_id].Vi_fri;
        particles[p].Bp += Wip * (nodes[node_id].Vi_fri.outer_product(-dist));
      }
    }
  }
}

// Update particle deformation data and position
template <typename Type> void Solver<Type>::UpdateParticles() {
#pragma omp parallel for
  for (int p = 0; p < plen; p++) {
    // Index of bottom-left node closest to the particle
    int node_base = (X_GRID + 1) * static_cast<int>(particles[p].Xp[1] -
                                                    Translation_xp[1]) +
                    static_cast<int>(particles[p].Xp[0] - Translation_xp[0]);

    // Save position to compute nodes-particle distances and update position in
    // one loop
    Vector2f Xp_buff = particles[p].Xp;
    particles[p].Xp.setZeros();
    //  T ~ nodal deformation
    Matrix2f T;

    // Loop over all the close nodes (depend on interpolation through bni)
    for (int y = bni; y < 3; y++) {
      for (int x = bni; x < 3; x++) {
        // Index of the node
        int node_id = node_base + x + (X_GRID + 1) * y;

        // Distance and weight
        Vector2f dist = Xp_buff - nodes[node_id].Xi;
        float Wip = getWip(dist);
        Vector2f dWip = getdWip(dist);

        // Update position and nodal deformation
        particles[p].Xp +=
            Wip * (nodes[node_id].Xi + DT * nodes[node_id].Vi_col);
        T += nodes[node_id].Vi_col.outer_product(dWip);
      }
    }

    // Update particle deformation gradient (elasticity, plasticity etc...)
    particles[p].UpdateDeformation(T);

    // time lapsed for a particle
    particles[p].t_life += 0.00001f;

    // if a particle has moved into an object
    // push it out
    glm::vec2 pos = glm::vec2(particles[p].Xp[0], particles[p].Xp[1]);
    float newDist = getDistance(pos);
    if (newDist < 0.f) {
      newDist *= 2.f; // for visualization convenience
      glm::vec2 newGrad = getGradient(pos);
      pos += newDist * newGrad;

      particles[p].Xp[0] = pos.x;
      particles[p].Xp[1] = pos.y;
    }
  }
}

// Reset active nodes data
template <typename Type> void Solver<Type>::ResetGrid() {
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < ilen; i++)
    if (nodes[i].Mi > 0)
      nodes[i].ResetNode();
}

/* -----------------------------------------------------------------------
|								  OTHER
|
----------------------------------------------------------------------- */
template <typename Type> void Solver<Type>::MovePolygons() {
  for (size_t i = 0; i < polylen; i++) {
    polygons[i].translate(DT * polygons[i].v);
    polygons[i].rotate(polygons[i].omega);
  }
}

/* -----------------------------------------------------------------------
|								  DRAWING
|
----------------------------------------------------------------------- */

// Draw particles, border and nodes (if selected).
template <typename Type> void Solver<Type>::Draw() {
  // Draw borders
  for (size_t b = 0; b < blen; b++) {
    borders[b].DrawBorder();
  }

  // Draw polygons
  for (size_t i = 0; i < polylen; i++) {
    polygons[i].DrawPolygon();
  }

  // Draw nodes
  // #if DRAW_NODES
  //   for (size_t i = 0; i < ilen; i++)
  //     nodes[i].DrawNode();
  // #endif

  // Draw particles
  for (size_t p = 0; p < plen; p++) {
    particles[p].DrawParticle();
  }
}

// Write particle position to .ply file (used in Houdini for ex)
// void Solver::WriteToFile(int frame) {
//   std::ofstream output;
//   std::string fileName = "out/ply/frame_" + std::to_string(frame) + ".ply";
//
//   output.open(fileName);
//   output << "ply" << std::endl;
//   output << "format ascii 1.0" << std::endl;
//   output << "element vertex " << particles.size() << std::endl;
//   output << "property float x" << std::endl;
//   output << "property float y" << std::endl;
//   output << "property float z" << std::endl;
//   output << "element face 0" << std::endl;
//   output << "property list uint int vertex_indices" << std::endl;
//   output << "end_header" << std::endl;
//
//   for (int p = 0; p < plen; p++) {
//     std::string coordinates = std::to_string(particles[p].Xp[0]) + " " +
//                               std::to_string(particles[p].Xp[1]) + " " + "0";
//     output << coordinates << std::endl;
//   }
//
//   output.close();
//   std::cout << " Frame #: " << frame << std::endl;
// }
