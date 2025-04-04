### Information for libries:
I used compiled binaries libraries for linux and can **only** be run on linux machine and they all in lib folder and their include are in include directory.

Also libs I used are: SDL3, SDL3_image and SDL3_ttf that also use one more library called freetype library.

### How to build a .exe and run it:
Open terminal in root directory of the project where is main CMakeLists.txt located in and open a terminal and type this command:
```sh
mkdir build && cd build && cmake .. && make && ./sdl_proj
```
