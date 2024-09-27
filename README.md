# Raycasting

This project is inspired by Tsoding's series on **Raycasting in TypeScript** on youtube: *https://www.youtube.com/watch?v=K1xEkA46CuM&list=PLpM-Dvs8t0VZ08cYW6yqNTWLKqLssw0nm&pp=iAQB*

Also follows **Lode's Computer Graphics Tutorial**: *https://lodev.org/cgtutor/raycasting.html*

**Implemented and planned to implement:** <br>
<span style="color:green">[x]</span> Player movement
- [x] Wall collision
- [x] Reading a map from a text file
- [ ] Connecting multiple maps
- [x] Raycasting the walls
- [x] Loading images to textures and using it to give textures to the walls
- [ ] Better wrapper for scene objects connected to images
- [ ] Raycasting the floors and ceilings
- [ ] Add light sources
- [ ] Depth-buffering
- [ ] Add interactible objects
- [ ] Add a player inventory
- [ ] Add a hotbar and render what the player is holding
- [ ] Improving player movement
- [ ] Add enemies
- [ ] Add animated textures support

This project uses SDL2 and SDL2_image libraries. All dependecies are included in the repository.
It uses C++20 and uses mingw64 compiler. To compile and run just run a command **make** (or **mingw32_make**) and run it with **.\main**. 
Project is made for Windows but should be able to run fine with Linux.