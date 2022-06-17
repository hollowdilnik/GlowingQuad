# GlowingQuad
![Glow](Resources/glow.jpg)
Unreal Engine implementation of the fake volumetric glow / flare effect from Doom 3 (FlareDeform).

Following [the original implementation](https://github.com/TTimo/doom3.gpl/blob/master/neo/renderer/tr_deform.cpp#L509) and [an article](http://yzergame.com/doomGlare.html) by Tim Sabo.

This is a pretty cheap way to fake bloom for a single quad. Doesn't require `MobileHDR`, any post processing or fancy materials. This is done by deforming a procedural mesh each frame, drawn with a simple additive or translucent material. Might come in handy for the retro look, low-end mobile devices or for mobile VR.

Tested on UE 4.27.2 with Oculus Quest 2, should work with other engine versions and devices without too much trouble.

### You can:
- Draw one- and two-sided quads
- Draw customizable colors
- Optionally skip drawing the inner quad and just draw the glow
- Use a separate material for the inner quad (and access the UVs)
- Customize the fade angle range
- Change the glow size base on the distance from the camera
- Make the glow size ignore actor's scaling and remain in world space units
- Draw a debug wireframe
- Customize the quad shape (ever wanted a glowing trapezoid in your game? I got you!)

### How to use
Refer to the GlowShowcase map in the plugin content folder for examples.

### TODO:
- Merging multiple quads in a single procedural mesh to reduce draw calls
- Calculate the UV coordinates for the glow mesh
