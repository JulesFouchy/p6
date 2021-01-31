# p6

A take on improving the (already amazing) [p5](https://p5js.org/) API.

Core design principles :
  - **Simplicity first** : optimization considerations should not complexify the API, but should still be a goal if they can be transparent to the user.
  - **Vectors are amazing** : They should be used as often as possible to abstract away the coordinate system and simplify the equations.
  - **Prepare for other contexts** : p6 should be a great environment to get started with programming in general, and graphics programming in particular. It should introduce concepts and allow a smooth transition to other coding environments :
    - Scripting in softwares like Blender, Illustrator or After Effects.
    - Per-pixel thinking ([Shadertoy](https://www.shadertoy.com/) like).
    - Low-Level graphics programming with GPU APIs like OpenGL.
  - **Low-Level access** : nothing should stand in the way of you writting raw OpenGL calls
