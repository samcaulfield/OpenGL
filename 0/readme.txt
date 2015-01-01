Blue Screen of Life

Initialises glut and OpenGL to minimal levels so that a rendering window may be
displayed. No drawing is done.

The first few lines of the main function tell glut to get us an OpenGL 4.0 core,
forward-compatible context. This gives us a relatively up-to-date context with
deprecated OpenGL features removed. The call to glutSetOption simply makes the
glutMainLoop function return after the window is closed, so we can do cleanup in
main. The call to glutInitDisplayMode gets us an OpenGL context with depth
buffering (new objects drawn over old ones), double-buffering (reduces
flickering), and RGBA (red, green, blue, alpha) colours.

The reshape function is called every time the rendering window is resized
because of the call to glutReshapeFunc.

The call to glClearColor sets the colour that glClear sets the window background
to. The parameters of glClearColor are the RGBA values of a colour.

The call to glViewport in resize tells OpenGL where in the window we want our
scene to be rendered.

The call to glutSwapBuffers in display makes use of the double-buffered context
we received from glut. Double buffering is basically where our drawing is done
to a buffer in memory that does not map to the screen yet. This is called the
back buffer. The front buffer is the memory buffer that is mapped to the screen.
When a new image is to be displayed, the pointers to the front and back buffers
are switched, so that the screen contents are instantly available. This reduces
flickering as drawing is not being done directly to the screen.

Framerate is measured by incrementing the frame counter each frame and then
printing and resetting it every second in the timer function.

