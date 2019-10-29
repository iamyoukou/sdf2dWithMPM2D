# SDF-based collision detection with MPM2D
I integrate an SDF-based collision detection into an MPM simulator.

The SDF algorithm is inspired by [Yuanming Hu's Taichi](https://github.com/yuanming-hu/taichi).

And the MPM algorithm is from [Elias-Gu's MPM2D](https://github.com/Elias-Gu/MPM2D).

# About the computation of SDF
The original method used to compute SDF is too expensive, because it iterates all the grid points.
It takes more than `60s` to render a frame in the demo.

I wrote an refined version which only iterates some area around a polygon.
This area is a little bigger than the AABB of the polygon.
Now it only takes `8~9s` to render a frame. 

Another way to reduce the computation may be the one from [Fuhrmann,2003](https://pdfs.semanticscholar.org/ec41/48aed023dfe1ba6f42a198613800fe29ae37.pdf).
Instead of the AABB of the whole polygon,
they compute SDF for AABBs of all the triangle faces of the polygon.
But I didn't try it yet.

From my point of view,
if the inner part of a polygon occpies a lot of area,
Fuhrmann's method is better than the one I wrote.
Otherwise, such as the one I used in the demo, mine is better.

# Result
![result](./video/output.gif)
