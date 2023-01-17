# R2grap-gl

A 2D After Effect shape animation renderer.

<img src="doc/gif/thinking.gif" width = "350" height = "300" alt="thinking" />

## Features
1. Support opengl,directX,metal graphics api;
2. Fit AE’s layer-groud(s)-path clip construction；


## File Directory 
```
R2grap-gl
   |___assets //some configured animation json files
   |___doc
   |___include 
   |___lib
   |___script //used to ouput animation json form After Effect
   |___src 
         |___base    //some basic process, generate bezier curve and irregular polygon
         |___codec   //analysis json and construct animation data structure
         |___render  //contain render data generator(vertcies\color\transform) for opengl
         |___shader  //glsl,hlsl
         |___metal   //metal render process
         |___directx //directx render process
         |___opengl  //opengl render process
