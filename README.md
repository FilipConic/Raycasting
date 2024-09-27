# Raycasting
<style>
r {color : Red}
g {color : Green}
</style>


This project is inspired by Tsoding's series on **Raycasting in TypeScript** on youtube: *https://www.youtube.com/watch?v=K1xEkA46CuM&list=PLpM-Dvs8t0VZ08cYW6yqNTWLKqLssw0nm&pp=iAQB* <br>
Also follows **Lode's Computer Graphics Tutorial**: *https://lodev.org/cgtutor/raycasting.html*<br>

**Implemented and planned to implement:** <br>
<g>[x]</g> Player movement<br>
- [x] Wall collision<br>
- [x] Reading a map from a text file<br>
<r>[ ]</r> Connecting multiple maps<br>
- [x] Raycasting the walls<br>
- [x] Loading images to textures and using it to give textures to the walls<br>
- [ ] Better wrapper for scene objects connected to images<br>
- [ ] Raycasting the floors and ceilings<br>
- [ ] Add light sources<br>
- [ ] Depth-buffering<br>
- [ ] Add interactible objects<br>
- [ ] Add a player inventory<br>
- [ ] Add a hotbar and render what the player is holding<br>
- [ ] Improving player movement<br>
- [ ] Add enemies<br>
- [ ] Add animated textures support<br>

This project uses SDL2 and SDL2_image libraries. All dependecies are included in the repository.<br>
It uses C++20 and uses mingw64 compiler. To compile and run just run a command **make** (or **mingw32_make**) and run it with **.\main**. <br>
Project is made for Windows but should be able to run fine with Linux.<br>