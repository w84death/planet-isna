![Planet ISNA](https://i.imgur.com/nBYru84.png)
# PLANET ISNA

Experimental text-mode ANSI space shooter

Whats so special?

- all done using text chars
- engine works like cellular automata (with layers)
- each ship is basicly a group of different types of cells
 - body cells (if they are destoyed whole ship is destoryed)
 - armour cells
 - guns cells (they generates lasers cells when pressed "fire")


## how to compile

### Linux:
``gcc -Os pi.cpp -o pi-linux.app -lglut -lGL -lGLU -lm``

### OSX:
``gcc -o pi-osx pi.cpp -framework GLUT -framework OpenGL``
