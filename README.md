# opengl-npr-renderer

Non-photorealistic real-time renderer. C++20, OpenGL 4.1 core, GLFW, GLEW.
Toon shading with procedural hatching, outlines from inflated back faces, and an
instanced particle system for fire.

<!-- Captures go here once recorded:
![Toon-shaded forest](docs/forest.png)
![Fire particle system](docs/fire.gif)
-->

## How a frame is drawn

Four passes over the scene, in this order:

1. **Outlines.** The meshes are drawn with vertices pushed outwards along their
   normals and front-face culling enabled, so only the inflated back faces reach
   the screen.
2. **Shading.** The same meshes again, culled normally and textured, with the
   toon lighting and the hatching done in the fragment shader. This overdraws
   the interior of the previous pass and leaves only a contour.
3. **Particles**, when enabled. Instanced camera-facing quads, alpha blended,
   with depth writes turned off so overlapping particles accumulate instead of
   occluding each other.
4. **Moon.** One blended quad, also with depth writes off.

## Techniques

**Toon shading.** The diffuse term is quantized into discrete bands, and each
band selects a blend between a cool ambient and a warm key light. The result
reads as flat painted regions instead of a gradient.

**Procedural hatching.** Stroke lines are generated in the fragment shader from
a world-space pattern, broken up by a per-cell hash so they do not form
continuous stripes, and scaled by how dark the surface already is. No hatching
texture and nothing authored by hand.

**Outlines from inflated back faces.** No edge detection and no extra
framebuffer. The whole cost is one additional draw of the scene, and the contour
keeps a constant width without any screen-space work.

**Instanced billboards.** A single quad lives in the vertex buffer. Each
particle is an instance, feeding position, size and a packed color through
per-instance attributes. The quad is turned towards the camera by reading the
right and up vectors straight out of the model-view matrix in the vertex shader,
so no per-particle matrix is ever built or uploaded.

**Particle pool.** Fixed size, recycled by index, so nothing is allocated at
runtime once it is up. The streamed vertex buffers are orphaned before each
upload to avoid stalling while the GPU is still reading the previous frame.

**Fire.** Particles spawn over a disc, rise with jittered velocity, lose
horizontal momentum to drag, shrink, and shift from yellow to red as they age.
Its shader program is only compiled the first time the effect is switched on.

**Moon.** A distant quad whose disc and halo come from radial falloffs in the
fragment shader, fully discarded outside the glow.

**Loading.** OBJ and MTL through tinyobjloader, images through `stb_image`.
Textures are cached by path, and materials without a map fall back to a plain
white one.

## Building

Arch:

```sh
yay -S base-devel glfw glew pkgconf
```

Debian:

```sh
sudo apt install build-essential libglfw3-dev libglew-dev pkg-config
```

macOS:

```sh
brew install glfw glew
```

Then:

```sh
make
```

## Running

The forest mesh is a 214 MB ASCII OBJ, too big to version, so it lives in the
releases. Fetch it once:

```sh
make assets
```

Build and launch:

```sh
make run
```

Any other OBJ works too:

```sh
./main objects/cylinder.obj
```

### Controls

| Input           | Action          |
| --------------- | --------------- |
| Mouse           | Look            |
| `W` `A` `S` `D` | Move            |
| `Space`         | Up              |
| `Left Shift`    | Down            |
| `N`             | Toggle the fire |
| `Escape`        | Quit            |

## Layout

```
main.cc      render loop and the draw passes
src/         camera, shader programs, particles, math, OBJ and image loading
shaders/     color (bands + hatching), outline, fire, moon
objects/     meshes and materials
textures/    diffuse maps referenced by the materials
```

## Credits

The scene is "Free Low Poly Forest" by
[purepoly](https://sketchfab.com/purepoly), under
[CC-BY-4.0](http://creativecommons.org/licenses/by/4.0/). Required attribution:

> This work is based on
> ["Free Low Poly Forest"](https://sketchfab.com/3d-models/free-low-poly-forest-6dc8c85121234cb59dbd53a673fa2b8f)
> by [purepoly](https://sketchfab.com/purepoly) licensed under
> [CC-BY-4.0](http://creativecommons.org/licenses/by/4.0/)

Full text in [ASSETS-LICENSE.txt](ASSETS-LICENSE.txt). Third-party code:
[tinyobjloader](https://github.com/tinyobjloader/tinyobjloader),
[stb_image](https://github.com/nothings/stb).

## License

Code under the [MIT License](LICENSE). Assets keep their own, see above.

Built for a graphics programming course at EPITA.
