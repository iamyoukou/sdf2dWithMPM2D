#include "solver.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

/* Declarations */
void initGLContext();
GLFWwindow *initGLFWContext();
Solver *Simulation;
int t_count = 0;
int frameNumber = 0;

/* For video */
void images2video() {
  std::string command =
      "ffmpeg -r 60 -start_number 0 -i ../result/sim%04d.png -vcodec mpeg4 "
      "-b 20M -s 600x600 ./result.mp4";
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
  std::vector<Material> inParticles = Material::InitializeParticles();

  Simulation = new Solver(inBorders, inNodes, inParticles);
}

void Update() {
  Simulation->P2G();         // Transfer data from Particles to Grid Nodes
  Simulation->UpdateNodes(); // Update nodes data
  Simulation->G2P();         // Transfer data from Grid Nodes to Particles

  // BUG: Something wrong in this method.
  // Simulation->UpdateParticles(); // Update particles data
}

// Add particle during the simulation
// If new particles are needed during simulation, use this method,
// and modify the AddParticles() method in particle.h
void AddParticles() {
  // DT_ROB gives the rate of insertion. Maximum number of particles
  if (t_count % DT_ROB == 0 && Simulation->particles.size() < 3000) {
    // Create particles
    std::vector<Material> new_p = Material::AddParticles();

    // Add particles to solver
    for (size_t p = 0; p < new_p.size(); p++)
      Simulation->particles.push_back(new_p[p]);
  }
}

/* -----------------------------------------------------------------------
|								MAIN
|
----------------------------------------------------------------------- */

int main(int argc, char **argv) {
  /* Initialize Simulation */
  Initialization();

/* [1] : output data to .ply file (to read in Houdini for example) */
#if WRITE_TO_FILE
  int frame_count = 0;
  while (1) {
    AddParticles();
    Update();
    if (t_count % (int)(DT_render / DT) == 0) // Record frame at desired
      rate Simulation->WriteToFile(frame_count++);
    Simulation->ResetGrid();
    t_count++;
  }

#else
  /* [2] : show result on OpenGL window, and record an .mp4 if selected */
  GLFWwindow *window = initGLFWContext();
  initGLContext();
  while (!glfwWindowShouldClose(window)) {

    glClear(GL_COLOR_BUFFER_BIT);

    // If new particles are needed during simulation,
    // activate this method
    // AddParticles();

    // P2G, compute grid forces, etc.
    Update(); // BUG

    if (t_count % (int)(DT_render / DT) == 0) // Display frame at desired rate
    {
      Simulation->Draw();
      glfwSwapBuffers(window);

#if RECORD_VIDEO
      std::string dir = "../result/sim";

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

      frameNumber++;
#endif

      glfwPollEvents();
    }

    // Don't forget to reset grid every frame !!
    Simulation->ResetGrid();

    t_count++;
  }

#if RECORD_VIDEO
  images2video();
#endif

  glfwTerminate();
#endif

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
      glfwCreateWindow(X_WINDOW, Y_WINDOW, "Simulation", NULL, NULL);

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