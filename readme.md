# Running

### Dependencies

- GLEW
- SDL2
- glm

For debian based distributions they are:
`libglew-dev libsdl2-dev libglm-dev`

Make sure you have SDL2 development libraries installed and then run the command bellow.   
`git clone "https://github.com/gbrls/myncraft/" && cd myncraft && make all`

# Progress
I usually write my code by expanding upon smaller functional iterations, so it's easy to document.

- [Day 0 (OpenGL "Hello World")](https://github.com/gbrls/myncraft/blob/4d16cab566b8c2e36e23c29ca5cf64d35191eda8/src/main.cpp)  
![](screenshots/day0.jpg)  

- [Day 1 (Minecraft "Hello Cube")](https://github.com/gbrls/myncraft/tree/48595563b984af3f6ec80cef53db212969c2136f)  
![](screenshots/day1.jpg)

- Day 2.  
During this day I worked mostly on camera controls and projections, so the graphics didn't really changed.

- Day 3.  
I worked on mesh generation with a DFS to mesh only the visible faces.

- [Day 4](https://github.com/gbrls/myncraft/tree/13989dc41ad2b0b874482c4888649fb4aaf86e07)  
I changed the project structure a little bit and started messing around with terrain generation.  
![](screenshots/day4.jpg)  
![](screenshots/day4-mesh.jpg)  
Efficient mesh generation, but it's worse than greedy meshing.  

- [Day 5](https://github.com/gbrls/myncraft/tree/41265aacc674c7665069df5608533dee03ffbcdc)  
I implemented a texture array, instead of a single texture.  
A simple pause was implemented in order to take screenshots.  
A simple tree generation algorithm was implemented, currently the trees cannot occupy multiple chunks, this is a problem.
![](screenshots/day5.jpg)  


