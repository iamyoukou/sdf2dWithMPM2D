#include "solver.h"
#include "solver.cpp" // to avoid template link error
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

/* Declarations */
void initGLContext();
GLFWwindow *initGLFWContext();
void drawSdf(GLFWwindow *);
Solver<Snow> *SnowSolver;
Solver<Water> *WaterSolver;
Solver<DrySand> *SandSolver;
Solver<Elastic> *ElasticSolver;
int t_count = 0;
int frameNumber = 0;
bool mask1 = true, mask2 = true, mask3 = true; // for animation controlling
bool startWater = false;
bool isAddPartices = true, isMovePolygon = false;

// for create new particles
int deg = 30;
int radius = 40;
int pNum = 8;

/* For video */
void images2video() {
  // the first 2 frames are always broken
  // don't know why
  // so I just start from the third frame
  std::string command =
      "ffmpeg -r 60 -start_number 2 -i ../result/sim%04d.png -vcodec mpeg4 "
      "-b 30M -s 600x600 ../video/result.mp4";
  system(command.c_str());

  // remove images
  // command = "rm ./result/*.png";
  // system(command.c_str());
}

/* -----------------------------------------------------------------------
|					MATERIAL POINT METHOD ALGORITHM
|
----------------------------------------------------------------------- */

void Initialization() {
  std::vector<Border> inBorders = Border::InitializeBorders();
  std::vector<Node> inNodes = Node::InitializeNodes();
  std::vector<Polygon> inPolygons = Polygon::InitializePolygons();

  // // for snow solver
  // std::vector<Snow> inSnowParticles = Snow::InitializeParticles();
  // SnowSolver = new Solver(inBorders, inNodes, inSnowParticles, inPolygons);
  // SnowSolver->computeSdf();

  // for water solver
  std::vector<Water> inWaterParticles = Water::InitializeParticles();
  WaterSolver = new Solver(inBorders, inNodes, inWaterParticles, inPolygons);
  WaterSolver->computeSdf();

  // for sand
  // std::vector<DrySand> inSandParticles = DrySand::InitializeParticles();
  // SandSolver = new Solver(inBorders, inNodes, inSandParticles, inPolygons);
  // SandSolver->computeSdf();

  // for elastic
  // std::vector<Elastic> inElasticParticles = Elastic::InitializeParticles();
  // ElasticSolver =
  //     new Solver(inBorders, inNodes, inElasticParticles, inPolygons);
  // ElasticSolver->computeSdf();
}

void Update() {
  /* for snow solver */
  // // for user-defined special effects
  // if (frameNumber == 51 && mask1) {
  //   SnowSolver->polygons[0].v = glm::vec2(0.f, 0.f);
  //   mask1 = false;
  // }
  // // stop the knife for a few frames
  // // then pull it out
  // else if (frameNumber == 81 && mask2) {
  //   SnowSolver->polygons[0].v = glm::vec2(-40.f, -40.f);
  //
  //   // stop the movement of all particles
  //   for (size_t i = 0; i < SnowSolver->plen; i++) {
  //     SnowSolver->particles[i].Vp = Vector2f(0.f, 0.f);
  //   }
  //
  //   mask2 = false;
  // }
  // // stop the knife again
  // else if (frameNumber == 121 && mask3) {
  //   SnowSolver->polygons[0].v = glm::vec2(0.f, 0.f);
  //   mask3 = false;
  // }
  //
  // // move objects
  // SnowSolver->MovePolygons();
  //
  // // for dynamic objects, update sdf every frame
  // SnowSolver->computeSdf();
  //
  // // MPM steps
  // // Transfer data from Particles to Grid Nodes
  // SnowSolver->P2G();
  // // Update nodes data
  // SnowSolver->UpdateNodes();
  // // Transfer data from Grid Nodes to Particles
  // SnowSolver->G2P();
  // // Update particles data
  // SnowSolver->UpdateParticles();
  /* end for snow solver */

  /* for water solver */
  if (frameNumber == 1) {
    startWater = true;
  }

  if (startWater) {
    if (frameNumber == 31) {
      isMovePolygon = true;
    } else if (frameNumber == 56) {
      // isMovePolygon = false;
      // WaterSolver->polygons[0].omega = 0.f;
    } else if (frameNumber == 401) {
      // isMovePolygon = true;
    }

    // just a trick to reduce rendering time
    if (isMovePolygon) {
      WaterSolver->MovePolygons();
      WaterSolver->computeSdf();
    }

    // manually change parameters
    if (frameNumber == 100) {
      WaterSolver->dt_rob = DT_ROB * 2;
      deg = 35;
      radius = 35;
      pNum = 7;
    } else if (frameNumber == 150) {
      WaterSolver->dt_rob = DT_ROB * 3;
      deg = 40;
      radius = 30;
      pNum = 6;
    } else if (frameNumber == 200) {
      WaterSolver->dt_rob = DT_ROB * 4;
      deg = 50;
      radius = 25;
      pNum = 5;
    } else if (frameNumber == 250) {
      WaterSolver->dt_rob = DT_ROB * 5;
      deg = 60;
      radius = 20;
      pNum = 4;
    } else if (frameNumber == 300) {
      WaterSolver->dt_rob = DT_ROB * 6;
      deg = 70;
      radius = 15;
      pNum = 3;
    } else if (frameNumber == 350) {
      WaterSolver->dt_rob = DT_ROB * 7;
      deg = 80;
      radius = 10;
      pNum = 2;
    } else if (frameNumber == 400) {
      WaterSolver->dt_rob = DT_ROB * 8;
      deg = 85;
      radius = 10;
      isAddPartices = false;
    } else if (frameNumber == 450) {
      WaterSolver->dt_rob = DT_ROB * 9;
      deg = 85;
      radius = 10;
    } else if (frameNumber == 500) {
      WaterSolver->dt_rob = DT_ROB * 10;
      deg = 85;
      radius = 10;
    }

    // add water particles
    if (t_count % WaterSolver->dt_rob == 0 &&
        WaterSolver->particles.size() < 6000) {

      // Add water particles
      if (isAddPartices) {
        std::vector<Water> new_p = Water::AddParticles(deg, radius, pNum + 4);

        // Add particles to solver
        for (size_t p = 0; p < new_p.size(); p++) {
          WaterSolver->particles.push_back(new_p[p]);
        }

        WaterSolver->plen = WaterSolver->particles.size();
      }
    }

    if (WaterSolver->particles.size() > 0) {
      WaterSolver->P2G();
      WaterSolver->UpdateNodes();
      WaterSolver->G2P();
      WaterSolver->UpdateParticles();
    }
  }
  /* end for water solver */

  // water
  // WaterSolver->P2G();
  // WaterSolver->UpdateNodes();
  // WaterSolver->G2P();
  // WaterSolver->UpdateParticles();

  // sand
  // SandSolver->P2G();
  // SandSolver->UpdateNodes();
  // SandSolver->G2P();
  // SandSolver->UpdateParticles();

  // Elastic
  // ElasticSolver->P2G();
  // ElasticSolver->UpdateNodes();
  // ElasticSolver->G2P();
  // ElasticSolver->UpdateParticles();
}

// Add particle during the simulation
// If new particles are needed during simulation, use this method,
// and modify the AddParticles() method in particle.h
// void AddParticles() {
//   // DT_ROB gives the rate of insertion. Maximum number of particles
//   if (t_count % DT_ROB == 0 && SnowSolver->particles.size() < 3000) {
//     // Add water particles
//     std::vector<Water> new_p = Water::AddParticles();
//
//     // Add particles to solver
//     for (size_t p = 0; p < new_p.size(); p++)
//       SnowSolver->particles.push_back(new_p[p]);
//   }
// }

/* -----------------------------------------------------------------------
|								MAIN
|
----------------------------------------------------------------------- */

int main(int argc, char **argv) {
  /* Initialize SnowSolver */
  Initialization();

/* [1] : output data to .ply file (to read in Houdini for example) */
#if WRITE_TO_FILE
  int frame_count = 0;
  while (1) {
    AddParticles();
    Update();
    if (t_count % (int)(DT_render / DT) == 0) // Record frame at desired
      rate SnowSolver->WriteToFile(frame_count++);
    SnowSolver->ResetGrid();
    t_count++;
  }

#else
  /* [2] : show result on OpenGL window, and record an .mp4 if selected */
  const int frameLimit = 500;

  GLFWwindow *window = initGLFWContext();
  initGLContext();

  while (!glfwWindowShouldClose(window) && frameNumber < frameLimit) {

    glClear(GL_COLOR_BUFFER_BIT);

    // If new particles are needed during simulation,
    // activate this method
    // AddParticles();

    // P2G, compute grid forces, etc.
    // std::cout << "t_count = " << t_count << '\n';
    Update();

    // Display frame at desired rate
    // t_count accounts for the t_count-th time step
    // (DT_render / DT) time steps equal to one frame (counted by frameNumber)
    // By default, 1 frame = 66 time steps
    if (t_count % (int)(DT_render / DT) == 0) {
      // SnowSolver->Draw();

      // if (frameNumber == 81)
      // if (WaterSolver->particles.size() > 0)
      WaterSolver->Draw();
      // SandSolver->Draw();
      // ElasticSolver->Draw();

      // for testing sdf
      // drawSdf(window);

      glfwSwapBuffers(window);

#if RECORD_VIDEO
      std::string dir = "./result/sim";

      // zero padding
      std::string num = std::to_string(frameNumber);
      num = std::string(4 - num.length(), '0') + num;
      std::string output = dir + num + ".png";

      // use opencv to save a frame
      // the frame size becomes twice from frame 2,
      // thus multiplying 2 is needed.
      // don't know why
      cv::Mat outImg(Y_WINDOW * 2, X_WINDOW * 2, CV_8UC3);
      glPixelStorei(GL_PACK_ALIGNMENT, 4);
      glReadPixels(0, 0, X_WINDOW * 2, Y_WINDOW * 2, GL_BGR, GL_UNSIGNED_BYTE,
                   (GLvoid *)outImg.data);
      cv::flip(outImg, outImg, 0);
      cv::imwrite(output, outImg);

      std::cout << "Frame " << frameNumber << " saved." << '\n';
#endif

      glfwPollEvents();

      frameNumber++;
    } // end outer if

    // Don't forget to reset grid every frame !!
    // SnowSolver->ResetGrid();
    WaterSolver->ResetGrid();
    // SandSolver->ResetGrid();
    // ElasticSolver->ResetGrid();

    t_count++;
  } // end while

#if RECORD_VIDEO
  images2video();
#endif

  glfwTerminate();
#endif

  delete SnowSolver;
  delete WaterSolver;
  delete ElasticSolver;
  delete SandSolver;

  return 0;
}

/* -----------------------------------------------------------------------
|						     	  OPENGL
|
----------------------------------------------------------------------- */

/* Define window. */
// Set parameters for window
GLFWwindow *initGLFWContext() {
  if (!glfwInit())
    exit(EXIT_FAILURE);

  /* Create Window*/
  GLFWwindow *window =
      glfwCreateWindow(X_WINDOW, Y_WINDOW, "SnowSolver", NULL, NULL);

  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);

  return window;
}

/* Define view parameters */
// Set parameters for opengl environment
void initGLContext() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, X_GRID, 0, Y_GRID, -1, 1);                   // original
  glViewport(0, 0, (GLsizei)X_WINDOW, (GLsizei)Y_WINDOW); // transfo

  glClearColor(.659f, .694f, .82f, .0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

// for testing sdf
void drawSdf(GLFWwindow *wnd) {
  double xpos, ypos;
  glfwGetCursorPos(wnd, &xpos, &ypos);

  float fx, fy;
  fx = xpos / (float)X_WINDOW;
  fy = ypos / (float)Y_WINDOW;

  // Note: (0, 0) is at right-bottom
  float wx, wy;
  wx = fx * X_GRID;
  wy = (1.f - fy) * Y_GRID;

  // for test
  // draw sdf gradient at a given point
  glm::vec2 start = glm::vec2(wx, wy);
  float dist = WaterSolver->getDistance(start);
  glm::vec2 grad = WaterSolver->getGradient(start);
  glm::vec2 end = start + dist * grad;

  // std::cout << "dist = " << dist << '\n';

  glLineWidth(4);
  glColor3f(1.f, 0.f, 0.f);

  glBegin(GL_LINES);
  glVertex2f(start.x, start.y); // start point
  glVertex2f(end.x, end.y);     // end point
  glEnd();
}
