CXX=llvm-g++
COMPILE=-c -std=c++17 \
-I/usr/local/Cellar/glew/2.1.0/include \
-I/usr/local/Cellar/glfw/3.3/include \
-I/usr/local/Cellar/eigen/3.3.7/include/eigen3 \
-I/Users/YJ-work/MPM2D-master/MPM2D/ext \
-I/Users/YJ-work/MPM2D-master/MPM2D/ext/Algebra \
-I/usr/local/Cellar/freeimage/3.18.0/include
LINK=-L/usr/local/Cellar/glew/2.1.0/lib -lGLEW \
-L/usr/local/Cellar/glfw/3.3/lib -lglfw \
-L/usr/local/Cellar/freeimage/3.18.0/lib -lfreeimage \
-framework GLUT -framework OpenGL -framework Cocoa

all: main

main: main.o border.o node.o particle.o solver.o algebra.o
	$(CXX) $(LINK) $^ -o $@
	rm -rv *.o

main.o: main.cpp
	$(CXX) $(COMPILE) $^ -o $@

border.o: border.cpp
	$(CXX) $(COMPILE) $^ -o $@

node.o: node.cpp
	$(CXX) $(COMPILE) $^ -o $@

particle.o: particle.cpp
	$(CXX) $(COMPILE) $^ -o $@

solver.o: solver.cpp
	$(CXX) $(COMPILE) $^ -o $@

algebra.o: /Users/YJ-work/MPM2D-master/MPM2D/ext/Algebra/algebra.cpp
	$(CXX) $(COMPILE) $^ -o $@
