CXX=llvm-g++
COMPILE=-g -c -std=c++17 \
-I/usr/local/Cellar/glew/2.1.0_1/include \
-I/usr/local/Cellar/glfw/3.3.2/include \
-I/usr/local/Cellar/eigen/3.3.7/include/eigen3 \
-I/usr/local/Cellar/opencv/4.3.0/include/opencv4 \
-I/usr/local/Cellar/glm/0.9.9.8/include \
-I/Users/YJ-work/sdf2dWithMPM2D/header
LINK=-L/usr/local/Cellar/glew/2.1.0_1/lib -lGLEW \
-L/usr/local/Cellar/glfw/3.3.2/lib -lglfw \
-L/usr/local/Cellar/opencv/4.3.0/lib -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc \
-framework GLUT -framework OpenGL -framework Cocoa
SRC_DIR=/Users/YJ-work/sdf2dWithMPM2D/src

all: main

main: main.o border.o node.o particle.o solver.o algebra.o sdf.o
	$(CXX) $(LINK) $^ -o $@
	rm -rv *.o

main.o: $(SRC_DIR)/main.cpp
	$(CXX) $(COMPILE) $^ -o $@

border.o: $(SRC_DIR)/border.cpp
	$(CXX) $(COMPILE) $^ -o $@

node.o: $(SRC_DIR)/node.cpp
	$(CXX) $(COMPILE) $^ -o $@

particle.o: $(SRC_DIR)/particle.cpp
	$(CXX) $(COMPILE) $^ -o $@

solver.o: $(SRC_DIR)/solver.cpp
	$(CXX) $(COMPILE) $^ -o $@

algebra.o: $(SRC_DIR)/algebra.cpp
	$(CXX) $(COMPILE) $^ -o $@

sdf.o: $(SRC_DIR)/sdf.cpp
	$(CXX) $(COMPILE) $^ -o $@
