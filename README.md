# R2grap-gl

A 2D animation renderer, based on opengl.

<img src="doc/gif/thinking.gif" width = "350" height = "300" alt="thinking" />



```
R2grap-gl
   |___assets //some configured animation json files
   |___doc
   |___include 
   |___lib
   |___script //used to ouput animation json form After Effect
   |___src 
         |___base   //some basic process, generate bezier curve and irregular polygon
         |___codec  //analysis json and construct animation data structure
         |___render //contain render data generator(vertcies\color\transform) for opengl
         |___shader //glsl
