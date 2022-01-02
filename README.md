# p6

A take on improving the (already amazing) [p5](https://p5js.org/) API.

You can find examples of how to use the library [here](https://github.com/JulesFouchy/p6-docs).

## Core design principles
  - **Simplicity first** : optimization considerations should not complexify the API, but should still be a goal if they can be transparent to the user.
  - **Vectors are amazing** : They should be used as often as possible to abstract away the coordinate system and simplify the equations.
  - **Prepare for other contexts** : p6 should be a great environment to get started with programming in general, and graphics programming in particular. It should introduce concepts and allow a smooth transition to other coding environments such as :
    - Scripting in softwares like Blender, Illustrator or After Effects.
    - Per-pixel thinking ([Shadertoy](https://www.shadertoy.com/) like).
    - Low-Level graphics programming with GPU APIs like OpenGL.
  - **Low-Level access** : nothing should stand in the way of you writting raw OpenGL calls

## Thinks I dislike about p5

- It is written in a language that doesn't allow operator overloading.
- The Vector class isn't used enough (like as a valid parameter for all the functions)
- There is no buit-in matrix type and you can't access the transform matrix.
- Ellipses are described by diameter instead of radius. If we change that we also have to change *rect* to take the inner radius as a parameter (a.k.a distance between center and side). Or we make overloads based on parameter name (would be nice if allowed by any language).
- *rect* is described by top-left corner by default, which is insconsistent with *ellipse* and less practical than giving a center in most cases in my opinion.
- *ellipse* and *rect* miss a rotation parameter.
- There is no proper Color type
- Colors are described with values in the range [0, 255] instead of [0, 1] by default. Hexadecimal strings are nice though.
- If you describe colors with a string, you can't add a second parameter to specify an alpha value.
- *lerp* is a weird and confusing name. I much prefer *mix*, which also has the advantage of being the one used in *glsl*.
- The default *noise* should use Simplex Noise instead of Classic Perlin Noise. Also, more noise types should be available (voronoise etc.)
- The coordinate system should be expressed in relative coordinates (-1 to 1 for y and -aspectRatio to aspectRatio for x) [Maybe we could have the option to change the state so that x becomes the axis of reference going from -1 to 1 (usefull for vertical compositions)]
- The y axis should point up, like in the classic mathematical convention, and the OpenGL one.
- Exporting an image sequence is completely broken in p5.
- There is no time() function that gives the physical time elapsed since the start of the sketch. It is the natural counterpart of *framecount* when we are dealing with realtime rendering. It would be nice to abstract both of these concepts behind a time() function, and adding a MODE to choose if we want realtime or fixed timestep (framecount).
- There are inconsistencies between Classic and WebGL modes (like the coordinate system).
- Circles are ugly in WebGL mode (not enough subdivisions / no adaptive subdivisions)
- p5.Graphics are not practical to use / introduce in existing code : you have to prefix every drawing call with "pg.". I would rather wrap my code in a pair of beginDraw() / endDraw() functions, just as if you were binding / unbinding another render target. Or even nicer, just one *draw()* function that takes in a function pointer that does the drawing code. (It would enforce wrapping your code in a scope, and would make sure you don't forget to call *endDraw()*).
- Javascript's modulo (%) uses the wrong convention for negative numbers (it introduces a discontinuity of behaviour between positive and negative numbers)
- Pixel arrays are 1D and are indexed as such. I think this 1D nature should be hidden to the user and we should be able to use 2D indices.
- noise() isn't defined for negative inputs

## Thinks I dislike about computer graphics in general

- Lacks a Custom Blend Mode option

## Documentation

### width, height and aspect_ratio

I am strongly convinced that you should always be working in relative coordinates and that any call to `window_size()` is a code smell. The only use case that I can think of for `window_size()` is if you want to create an `Image` that has a size proportional to that of the window.
