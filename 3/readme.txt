t3h c00b 0f d00m

Draws a cube that rotates on two axes (so as to be inspectable visually).

The cube uses indexed buffering. This reduces the amount of data that has to be
sent to the GPU. Instead of defining three verices (and colours) per triangle
(3 * 2 * 6 * 2 = 72 vertices) we only need to define eight vertices. A second
buffer is then generated that tells OpenGL how to reuse the vertices to make up
a cube (8 * 6 = 48 vertices). This saves quite a bit of memory on the GPU per
cube. Note the introduction of depth testing to make the cube render properly.
The concept is simple, fragments are "kicked off the screen" by new incoming
fragments that are closer to the "camera".

