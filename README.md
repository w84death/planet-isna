![Planet ISNA](https://i.imgur.com/nBYru84.png)
# PLANET ISNA

Experimental text-mode ANSI space shooter

## Watch the video: [https://youtu.be/f3qq_6dWxcA](https://youtu.be/f3qq_6dWxcA)

Whats so special?

- all done using text chars
- engine works like cellular automata (with layers)
- each ship is basicly a group of different types of cells
 - body cells (if they are destoyed whole ship is destoryed)
 - armour cells
 - guns cells (they generates lasers cells when pressed "fire")

## Concept art
It all started as an ANSI art done in [PableDraw](http://picoe.ca/products/pablodraw/)

![concept art](https://i.imgur.com/uKBKky7.png)

# Charset
OpenGL Glut doesnt have all the ANSI chars. It only support few simple characters. For now I'm using them but in the end I probably switch to the sprite rendering my own font (copy of DOS ANSI).

![GLUT charset](http://openglut.sourceforge.net/openglut_bitmap_9by15.png)

Each ship is made of chars that defines its construction. SPRITE array that defines each ship.

![Chip](https://i.imgur.com/mD2FZuq.png)

You can easly edit it:
- each cell has 3 params {char_id, forground_color, background_color}
- chart id from [GLUT charset](http://openglut.sourceforge.net/openglut_bitmap_9by15.png)
- some are restricted to:
 - type 1 is a weapon
 - type 2 is a body (important bits!)
 - any other is armour (or cosmetic stuff)


## How to compile

### Linux:
``gcc -Os pi.cpp -o pi-linux.app -lglut -lGL -lGLU -lm``

### OSX:
``gcc -o pi-osx pi.cpp -framework GLUT -framework OpenGL``
