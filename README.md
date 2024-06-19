# CSCI-490J5/631 Computer Graphics & Rendering

A graphics engine built entirely from scratch, pixel-by-pixel. Project is constructed
over 5 assignments, each covering an area within computer graphics that ultimately
builds up to a complete 3D software renderer with Phong lighting. It also comes with
a rendering language with a parser to construct scenes using various primitives. All
the code within this project is written entirely from scratch, using only C++'s standard
library and Win32 to run.

Covers Bresenham's algorithm to render lines, a flood fill algorithm to fill flat 2D
polygons, linear transforms for 3D shapes, polygon clipping, scan conversion, DDA,
dynamic shaders, point lights, ambient lights, and diffuse lighting.

Due to the pace and difficulty of the course, I didn't spend much time optimizing or
organizing the code. To be quite honest, it's not even that good. The main hurdle for
each assignment was completing it on time. As a consequence, things are written in
places they shouldn't be, little thought was made about architecture, and the pipeline
isn't well-laid out. The parser that handles converting scene files into renderable
elements doesn't follow the specification perfectly, but it does work.

### Potential Refactor

- [ ] **Runtime Environment Redesign**
      Before even considering remaking the project, this is probably step one since
      the runtime environment isn't just poorly thought out, not thought was even made.
      Therefore, I will probably come back and restructure how the renderer runs from
      top to bottom and maybe even put in the nuts and bolts for multiplatform support
      on Linux and MacOSX.

- [ ] **Render Language Parser**
      The render files that we used in this course were brilliant because we can design
      3D scenes without actually having to modify the source code and recompile to tweak
      lighting or object positions. The language the professor used for this course is
      designed to be easy enough to parse for the brave souls (and idiots like myself)
      to reimplement if they choose to work on a platform that doesn't support his
      preimplemented runtime framework (which is Linux, and coincidentally not that
      platform I wrote this project on). Now that I have experience in writing compilers,
      I think I can design a different render language with some additional features
      that might be nice to have (such as model loading).

- [ ] **Render Pipeline**
      Since this renderer is a software renderer, its not particular fast. Although
      my code isn't optimized (nor did I turn optimization *on*) I would like to be
      able to do some optimizations for the render pipeline such that it can run
      somewhat realtime. Perhaps add hardware rendering in the mix? Though, that's
      something I've done in the past and wouldn't actually add any value to what
      this project was actually meant to demonstrate.
