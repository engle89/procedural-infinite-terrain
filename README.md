# procedural-infinite-terrain
Procedural infinite terrain implementation with:
1.marching cube algorithm
2.triplanar texture
3.phong lighting

Here is the demo:
![alt text](demo.gif)

The following diagram is a high level design for my project:
![alt text](design.png)
I use the basic framework from http://www.rastertek.com/tutdx11s2ter.html and add worldclass as chunk management, cubeclass as chunk generator, and marchingcube vertex, geometry and pixel shader.
For the marching cube algortihm, I recommend reading the following paper:
Voxel-Based Terrain for Real-Time Virtual Simulations By ERIC STEPHEN LENGYEL.

