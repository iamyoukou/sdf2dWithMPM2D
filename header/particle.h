#pragma once

#include "constants.h"
#include "common.h"

/* The particle class contains data commun to all simulation.
The material subclasses contains particular data and methods. */

class Particle {
public:
  /* Data */
  float Vp0; // Initial volume (cste)
  float Mp;  // Particle mass (cste)

  Vector2f Xp; // Particle position
  Vector2f Vp; // Particle Velocity
  Matrix2f Bp; // ~ Particle velocity field

  float t_life; // the lapsed time since a particle is created

  /* Constructors */
  Particle(){};
  Particle(const float inVp0, const float inMp, const Vector2f &inXp,
           const Vector2f &inVp, const Matrix2f &inBp);
  ~Particle(){};

  /* Static function */
  static int myRand(int down, int up) {
    return (rand() % (up - down - 1) + down);
  }

  // Generate point positions from image
  static std::vector<Vector2f> ptsFromImg(std::string name, Vector2f trans,
                                          Vector2f scale, float theta) {
    std::vector<Vector2f> ps;
    cv::Mat inImg = cv::imread(name);

    int rndSize = 200; // resolution along each axis
    int imgWidth = inImg.size().width;
    int imgHeight = inImg.size().height;

    for (int i = 0; i < rndSize; i++) {
      for (int j = 0; j < rndSize; j++) {
        int x, y;
        x = myRand(0, imgWidth - 1);
        y = myRand(0, imgHeight - 1);

        cv::Vec3b pixelSrc = inImg.at<cv::Vec3b>(cv::Point(x, y));

        int color = pixelSrc[0] + pixelSrc[1] + pixelSrc[2];
        // some threshold
        if (color < 10) {
          Vector2f pos;

          float fx, fy;
          fx = (float)x / (float)imgWidth; // to [0, 1.0]
          fy = (float)y / (float)imgHeight;

          // rotate
          float rx, ry;
          float rad = 3.1415f / 180.f * theta;
          // glm::vec2 rt = glm::rotate(glm::vec2(fx, fy), rad);
          // fx = rt.x;
          // fy = rt.y;
          rx = glm::cos(rad) * fx - glm::sin(rad) * fy;
          ry = glm::sin(rad) * fx + glm::cos(rad) * fy;
          fx = rx;
          fy = ry;

          // rescale
          fx *= scale[0];
          fy *= scale[0];

          // translate
          fx += trans[0];
          fy += trans[1];

          // back to world space
          pos = Vector2f(fx * (float)X_GRID, fy * (float)Y_GRID);

          ps.push_back(pos);
        }
      } // end inner for
    }   // end outer for

    return ps;
  } // end ptsFromImg
};

/* ----------------------------------------------------------------------------------------------
 */

/* WATER */
class Water : public Particle {
public:
  /* Data */
  float Ap; // For computation purpose
  float Jp; // Deformation gradient (det)

  static float grey[3]; // Drawing colors
  static float green[3];
  static float blue[3];
  static float h_color;
  static float l_color;
  static float d_color;

  /* Constructors */
  Water() : Particle(){};
  Water(const float inVp0, const float inMp, const Vector2f &inXp,
        const Vector2f &inVp, const Matrix2f &inBp);
  Water(Particle p);
  ~Water(){};

  /* Functions */
  void ConstitutiveModel();                  // Deformation gradient increment
  void UpdateDeformation(const Matrix2f &T); // Deformation gradient update

  void DrawParticle();

  /* Static Functions */
  static std::vector<Water> InitializeParticles() {
    std::vector<Water> outParticles;

    // Vector2f v = Vector2f(0.f, 0.f); // Initial velocity
    // Matrix2f a = Matrix2f(0);
    //
    // // get position from image
    // std::vector<Vector2f> pts = ptsFromImg(
    //     "../image/wo.png", Vector2f(0.1f, 0.45f), Vector2f(0.5, 0.5), 30.f);
    //
    // for (int i = 0; i < pts.size(); i++) {
    //   outParticles.push_back(Water(1.14f, 0.0005f, pts[i], v, a));
    // }

    return outParticles;
  }

  // Add particle mid-simulation
  // Use frameNumber as a parameter to modify rndTheta
  static std::vector<Water> AddParticles(int deg, int radius, int pNum) {
    std::vector<Water> outParticles;

    Matrix2f a = Matrix2f(0);

    float rndTheta = float(myRand(deg, 90)) * (3.1415f / 180.f); // radian

    for (int p = 0; p < pNum; p++) {
      Vector2f v =
          Vector2f(glm::cos(rndTheta) * float(radius),
                   -glm::sin(rndTheta) * float(radius)); // Initial velocity

      float rndx = float(myRand(-7, 7));
      float rndy = float(myRand(-5, 5));
      Vector2f pos = Vector2f(110.f + rndx, 187.f + rndy); // new positions

      outParticles.push_back(Particle(1.14f, 0.0005f, pos, v, a));
    }

    return outParticles;
  }
};

/* DRY SAND */
class DrySand : public Particle {
public:
  /* Data */
  Matrix2f Ap; // For computation purpose

  Matrix2f Fe, FeTr; // (Trial) Elastic deformation
  Matrix2f Fp, FpTr; // (Trial) Plastic deformation

  float q, alpha; // Hardening paremeters

  float r; // Color

  /* Constructors */
  DrySand() : Particle(){};
  DrySand(const float inVp0, const float inMp, const Vector2f &inXp,
          const Vector2f &inVp, const Matrix2f &inBp);
  DrySand(Particle p);
  ~DrySand(){};

  /* Functions */
  void ConstitutiveModel();                  // Deformation gradient increment
  void UpdateDeformation(const Matrix2f &T); // Deformation gradient update
  void Plasticity();                         // Update plastic dissipation
  void Projection                            // Return mapping algorithm
      (const Vector2f &Eps, Vector2f *T, float *dq);

  void DrawParticle();

  /* Static Functions */
  static std::vector<DrySand> InitializeParticles() {
    std::vector<DrySand> outParticles;
    // DefaultPRNG PRNG;

    // std::vector<sPoint> P_c = GeneratePoissonPoints(2000, PRNG);

    Vector2f v = Vector2f(0); // Initial velocity
    Matrix2f a = Matrix2f(0);

    std::vector<Vector2f> pts = ptsFromImg(
        "../image/dai.png", Vector2f(0.f, 0.65f), Vector2f(0.5f, 0.5f), -30.f);

    int NP = static_cast<int>(pts.size());

    float W_COL = X_GRID / 8.0f;
    float H_COL = (Y_GRID - 2 * CUB) * 0.9f;
    float X_COL = (X_GRID - W_COL) / 2.0f;
    float Y_COL = CUB;

    float VOL = W_COL * H_COL / static_cast<float>(NP);
    float MASS = VOL * RHO_dry_sand / 100.0f;

    for (int i = 0; i < pts.size(); i++) {
      // Vector2f pos =
      //     Vector2f(P_c[p].x * W_COL + X_COL, P_c[p].y * H_COL + Y_COL);
      outParticles.push_back(DrySand(VOL, MASS, pts[i], v, a));
    }

    return outParticles;
  }

  static std::vector<DrySand> AddParticles() // Add particle mid-simulation
  {
    std::vector<DrySand> outParticles;
    return outParticles;
  }
};

/* SNOW */
class Snow : public Particle {
public:
  /* Data */
  Matrix2f Ap; // For computation purpose

  Matrix2f Fe, FeTr; // (Trial) Elastic deformation
  Matrix2f Fp, FpTr; // (Trial) Plastic deformation
  float Je, Jp;      // Deformation gradients

  float lam; // Lame parameters
  float mu;

  float s;       // size
  float r, g, b; // size and color

  /* Constructors */
  Snow() : Particle(){};
  Snow(const float inVp0, const float inMp, const Vector2f &inXp,
       const Vector2f &inVp, const Matrix2f &inBp);
  Snow(Particle p);
  ~Snow(){};

  /* Functions */
  void ConstitutiveModel();                  // Deformation gradient increment
  void UpdateDeformation(const Matrix2f &T); // Deformation gradient update
  void Plasticity();                         // Update plastic dissipation

  void DrawParticle();

  /* Static Functions */
  static std::vector<Snow> InitializeParticles() {
    std::vector<Snow> outParticles;

    // get position from image
    std::vector<Vector2f> pts =
        ptsFromImg("../image/men.png", Vector2f(0.25f, 0.45f),
                   Vector2f(0.5f, 0.5f), -30.f);

    // DefaultPRNG PRNG;
    //
    // std::vector<sPoint> P_c = GeneratePoissonPoints(3000, PRNG, 30, true);
    int NP = static_cast<int>(pts.size());

    float R_BALL = fmin(float(X_GRID), float(Y_GRID)) * 0.33f;
    float X_BALL = X_GRID * 0.3f;
    float Y_BALL = Y_GRID * 0.45f;

    float VOL = 2 * PI * R_BALL * R_BALL / static_cast<float>(NP);
    float MASS = VOL * RHO_snow / 100.0f;

    Vector2f v = Vector2f(0.f, 0.f); // Initial velocity
    Matrix2f a = Matrix2f(0);

    for (int i = 0; i < pts.size(); i++) {
      outParticles.push_back(Snow(VOL, MASS, pts[i], v, a));
    }

    // for (int p = 0; p < NP; p++) {
    //   Vector2f pos = Vector2f(P_c[p].x * R_BALL + X_BALL,
    //                           P_c[p].y * R_BALL + Y_GRID - Y_BALL);
    //   outParticles.push_back(Snow(VOL, MASS, pos, v, a));
    // }
    // for (int p = 0; p < NP; p++) {
    //   Vector2f pos = Vector2f(P_c[p].x * R_BALL + X_GRID - X_BALL,
    //                           P_c[p].y * R_BALL + Y_BALL);
    //   outParticles.push_back(Snow(VOL, MASS, pos, -v, a));
    // }

    return outParticles;
  }

  static std::vector<Snow> AddParticles() // Add particle mid-simulation
  {
    std::vector<Snow> outParticles;
    return outParticles;
  }
}; // end class Snow

/* ELASTIC */
class Elastic : public Particle {
public:
  /* Data */
  Matrix2f Ap;   // For computation purpose
  Matrix2f Fe;   // Elastic deformation
  float lam, mu; // Lame parameter
  float r, g, b; // color

  /* Constructors */
  Elastic() : Particle(){};
  Elastic(const float inVp0, const float inMp, const Vector2f &inXp,
          const Vector2f &inVp, const Matrix2f &inBp, const float inlam,
          const float inmu, const float inr, const float ing, const float inb);
  Elastic(Particle p);
  ~Elastic(){};

  /* Functions */
  void ConstitutiveModel();                  // Deformation gradient increment
  void UpdateDeformation(const Matrix2f &T); // Deformation gradient update

  void DrawParticle();

  /* Static Functions */
  static std::vector<Elastic> InitializeParticles() {
    std::vector<Elastic> outParticles;

    std::vector<Vector2f> pts = ptsFromImg(
        "../image/ni.png", Vector2f(0.45f, 0.45f), Vector2f(0.5f, 0.5f), 30.f);

    // Create cube point cloud
    // std::vector<Vector2f> positions;
    // for (float i = 0; i < fmax(X_GRID, Y_GRID) / 8.0f; i++)
    //   for (float j = 0; j < fmax(X_GRID, Y_GRID) / 8.0f; j++)
    //     positions.push_back(Vector2f(i, j));

    float VOL =
        (float)fmax(X_GRID, Y_GRID) * (float)fmax(X_GRID, Y_GRID) / 16.0f;
    float MASS = VOL * RHO_elastic / 100.0f;

    Vector2f v = Vector2f(0, 0); // Initial velocity
    Matrix2f a = Matrix2f(0);

    // for (size_t p = 0, plen = positions.size(); p < plen; p++) { // 1st cube
    //   Vector2f pos = Vector2f(positions[p][0] + X_GRID * 0.1f,
    //                           positions[p][1] + Y_GRID / 3.0f);
    //   outParticles.push_back(Elastic(VOL, MASS, pos, v, a, LAM_elastic *
    //   0.1f,
    //                                  MU_elastic * 0.1f, 1, 0, 0));
    // }
    // for (size_t p = 0, plen = positions.size(); p < plen; p++) { // 2nd cube
    //   Vector2f pos = Vector2f(positions[p][0] + X_GRID * 0.325f,
    //                           positions[p][1] + Y_GRID / 2.0f);
    //   outParticles.push_back(
    //       Elastic(VOL, MASS, pos, v, a, LAM_elastic, MU_elastic, 0, 0, 1));
    // }
    // for (size_t p = 0, plen = positions.size(); p < plen; p++) { // 3rd cube
    //   Vector2f pos = Vector2f(positions[p][0] + X_GRID * 0.55f,
    //                           positions[p][1] + Y_GRID * 2 / 3.0f);
    //   outParticles.push_back(Elastic(VOL, MASS, pos, v, a, 100 * LAM_elastic,
    //                                  100 * MU_elastic, 0, 1, 0));
    // }
    for (int i = 0; i < pts.size(); i++) {
      int ir, ig, ib;
      ir = 50;
      ig = myRand(150, 170);
      ib = 50;

      float fr, fg, fb;
      fr = float(ir) / 255.f;
      fg = float(ig) / 255.f;
      fb = float(ib) / 255.f;

      outParticles.push_back(Elastic(VOL, MASS, pts[i], v, a, 100 * LAM_elastic,
                                     100 * MU_elastic, fr, fg, fb));
    }

    return outParticles;
  }

  static std::vector<Elastic> AddParticles() // Add particle mid-simulation
  {
    std::vector<Elastic> outParticles;
    return outParticles;
  }
};
