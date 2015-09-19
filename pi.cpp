// ----------------------------------------
// PLNET ISNA
// Krzysztof Jankowski <kj@p1x.in>
//
// (c)2015 P1X
// http://p1x.in
//
// Repo:
// https://github.com/w84death/plant-isna
//
// Linux:
// gcc -Os pi.cpp -o pi-linux.app -lglut -lGL -lGLU -lm
//
// OSX:
// gcc -o pi-osx pi.cpp -framework GLUT -framework OpenGL
//
// ----------------------------------------

// LIBS
// ----------------------------------------

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <time.h>
#include <math.h>

// SYSTEM SETTINGS
// ----------------------------------------------------------------------------

bool fullscreen_mode  = false;
char win_title[]      = "PLANET ISNA";
static float VERSION  = 0.1f;
int win_width         = 800;
int win_height        = 600;
int win_x             = 50;
int win_y             = 80;

// RENDERING SETTINGS
// ----------------------------------------------------------------------------

int frames = 0;
int cycles = 0;
static int RENDER_FPS = 1000/30;
static int LOGIC_FPS  = 1000/24;

// CAMERA SETTINGS
// ----------------------------------------------------------------------------

static float FOV      = 45.0f;
int cam_mode          = 0;
float cam_speed       = 0.8f;
float cam_move_speed  = 0.6f;
float cam_look_move_speed  = 0.6f;
float cam_pos[]       = {35.0f, 22.0f, 70.0f, 35.0f, 22.0f, 70.0f};
float cam_look_pos[]  = {35.0f, 22.0f, 0.0f, 35.0f, 22.0f, 0.0f};
float cam_aim[]       = {0.0f, 0.0f};
float cam_for_enable  = false;
float cam_fog_start   = 6.0f;
float cam_fog_end     = 40.0f;
float cam_clear_color[4] = {0.3f, 0.05f, 0.6f, 1.0f};

// PI SETTINGS
// ----------------------------------------------------------------------------
static int CELLS_ARRAY_SIZE[]     = {80, 30};
static int CELLS_MAX              = CELLS_ARRAY_SIZE[0]*CELLS_ARRAY_SIZE[1];
static int CELLS_HALF[]           = {CELLS_ARRAY_SIZE[0] * 0.5, CELLS_ARRAY_SIZE[1] * 0.5};
static int LAYERS                 = 5;

int pi_buffor[80][30][4];
int pi_layers[5][80][30][4];

static int C_SPACE                = 0;
static int C_STAR                 = 2;
static int C_SHIP                 = 4;
static int C_MISSILE              = 8;
static int C_HUD                  = 16;

static float C_STAR_0_DENS        = 0.05f;
static float C_STAR_1_DENS        = 0.01f;

int STATE                         = 0;
static int S_INT                  = 0;
static int S_MENU                 = 2;
static int S_SIMULATION           = 4;
static int S_CREDITS              = 8;







// FUNCTIONS LIST
// ----------------------------------------------------------------------------

void display();
void render_loop();
void reshape();
void setup_lighting();
void setup_gl();
void setup_scene();
void camera_move();
void draw_floor();
void fullscreen_toggle();
void mouse();
void special_keys();
















// MATH HELPERS
// ----------------------------------------------------------------------------



float random_f(){
   float r = (float)rand() / (float)RAND_MAX;
   return r;
}


















// ARENA
// ----------------------------------------------------------------------------


void pi_setup(){
  int r;
  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
    pi_buffor[x][y][0] = C_SPACE;
    pi_buffor[x][y][1] = 0;
    pi_buffor[x][y][2] = 15;
    pi_buffor[x][y][3] = 0;
    for (int b = 0; b<LAYERS; b++){
      
      if (b==0){
        r = random_f() < C_STAR_0_DENS ? C_STAR : C_SPACE;
        pi_layers[b][x][y][0] = r;
        pi_layers[b][x][y][1] = 31;
      }
      if (b==1){
        r = random_f() < C_STAR_1_DENS ? C_STAR : C_SPACE;
        pi_layers[b][x][y][0] = r;
        pi_layers[b][x][y][1] = 7;
      }
      if (b>1){
        pi_layers[b][x][y][0] = C_SPACE;
        pi_layers[b][x][y][1] = 0;
      }
      pi_layers[b][x][y][2] = 15;
      pi_layers[b][x][y][3] = 0;
    }
  }}
}

void pi_draw_tile(int x, int y, char sprite){
  glPushMatrix();

    // glTranslatef (x, y*2, 0.0f);

    // glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
    glEnable ( GL_COLOR_MATERIAL ) ;
    glColor4f(0.7f, 0.7f, 1.0f, 1.0f);

    // float size = 0.5f;

    // glBegin(GL_POLYGON);
    //    glVertex3d(-size, size*2, 0.0f);
    //    glVertex3d(-size, -size*2, 0.0f);
    //    glVertex3d(size, -size*2, 0.0f);
    //    glVertex3d(size, size*2, 0.0f);
    // glEnd();

    //GLUT_BITMAP_9_BY_15
    glRasterPos2f(x*0.9, y*1.6);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, sprite);
    glPopMatrix();
}

void  pi_draw_buffor(){
  int cell_type;
  int cell_char;

  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
    cell_type = pi_buffor[x][y][0];
    if (cell_type > C_SPACE){
      cell_char = pi_buffor[x][y][1];
      pi_draw_tile(x, y, cell_char);
    }
  }}
}


void pi_fill_buffor(){
  int temp_type;
  int temp_char;
  int temp_color_f;
  int temp_color_b;

  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
     pi_buffor[x][y][0] = C_SPACE;
    for (int b = 0; b<LAYERS; b++){
      temp_type = pi_layers[b][x][y][0];
      temp_char = pi_layers[b][x][y][1];
      temp_color_f = pi_layers[b][x][y][2];
      temp_color_b = pi_layers[b][x][y][3];

      if(temp_type > C_SPACE){
        pi_buffor[x][y][0] = temp_type;
        pi_buffor[x][y][1] = temp_char;
        pi_buffor[x][y][2] = temp_color_f;
        pi_buffor[x][y][3] = temp_color_b;
      }
    }
  }}
}


void pi_automate(int layer){

  int cell_type;
  int cell_char;

  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){

    cell_type = pi_layers[layer][x][y][0]; 
    cell_char = pi_layers[layer][x][y][1]; 

    if (layer == 0 or layer == 1){
      if (cell_type == C_STAR) {
        pi_layers[layer][x][y][0] = C_SPACE;
        if(y>0){
          pi_layers[layer][x][y-1][0] = cell_type;
          pi_layers[layer][x][y-1][1] = cell_char;
        }else{
          pi_layers[layer][x][CELLS_ARRAY_SIZE[1]-1][0] = cell_type;
          pi_layers[layer][x][CELLS_ARRAY_SIZE[1]-1][1] = cell_char;
        }
      }
    }


  }}
}

void pi_draw(){
  pi_draw_buffor();
  frames++;
};

void pi_loop(){

  if (cycles % 2 == 0) pi_automate(0);
  pi_automate(1);

  pi_fill_buffor();
  cycles++;
};









// INPUTS
// ----------------------------------------------------------------------------

void mouse(int button, int state, int x, int y) {
  // void
}

void special_keys(int key, int x, int y) {
   switch (key) {
      case GLUT_KEY_F1:
         fullscreen_mode = !fullscreen_mode;
         fullscreen_toggle();
         break;
      case GLUT_KEY_RIGHT:
        
        break;
      case GLUT_KEY_LEFT:
        
        break;
      case GLUT_KEY_UP:
        
        break;
      case GLUT_KEY_DOWN:
        
        break;
      case GLUT_KEY_F2:
        
        break;
      case GLUT_KEY_F3:
        cam_mode = 0;
        break;
      case GLUT_KEY_F4:
        cam_mode = 1;
        break;
      case GLUT_KEY_F5:
        cam_mode = 2;
        break;

   }
}

void keyboard(unsigned char key, int x, int y) {
   switch (key) {
      case 27:     // ESC key
         //exit(0);
         break;
      case 13: // enter
         break;
      case 113: // q

        break;
      case 101: // e

        break;
      case 97: // a

        break;
      case 100: // d

        break;

      case 119: // w

        break;
      case 115: // s

        break;
   }
}









// OPEN GL
// ----------------------------------------------------------------------------

void reshape(GLsizei width, GLsizei height) {
  if (width == 0) width = 12;
  if (height == 0) height = 12;

  glViewport(0, 0, width, height);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (FOV, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void fullscreen_toggle(){
  if (fullscreen_mode) {
    glutFullScreen();
  } else {
    glutReshapeWindow(win_width, win_height);
    glutPositionWindow(win_x, win_y);
  }
}

void setup_lighting(){
  float amb = 0.6f;
  float diff = 0.3f;
  float spec = 0.2f;

  GLfloat global_ambient[]  = {amb, amb, amb, 0.2f};
  GLfloat light_ambient[]   = {amb, amb, amb, 1.0f };
  GLfloat light_diffuse[]   = {diff, diff, diff, 1.0f };
  GLfloat light_specular[]  = {spec, spec, spec, 1.0f };
  GLfloat light_position[]  = {0.0f, 0.0f, 0.0f, 0.0f };

  glEnable(GL_LIGHTING);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);

  if (cam_for_enable){
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, cam_clear_color);
    glFogf(GL_FOG_START, cam_fog_start);
    glFogf(GL_FOG_END, cam_fog_end);
    glEnable(GL_FOG);
  }
}

void setup_gl(){
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void setup_app() {
  glutCreateWindow(win_title);
  fullscreen_toggle();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special_keys);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
}

void setup_menu(){

}

void camera_move(){
  glLoadIdentity();
  float treshold = 0.05f;
  for (int i = 0; i < 3; i++){
    if(cam_pos[i+3] > cam_pos[i]){
      cam_pos[i+3] -= cam_move_speed;
    }
    if(cam_pos[i+3] < cam_pos[i]){
      cam_pos[i+3] += cam_move_speed;
    }
  }

  for (int i = 0; i < 3; i++){
    if(cam_look_pos[i+3] > cam_look_pos[i]){
      cam_look_pos[i+3] -= cam_look_move_speed;
    }
    if(cam_look_pos[i+3] < cam_look_pos[i]){
      cam_look_pos[i+3] += cam_look_move_speed;
    }
  }


  gluLookAt(cam_pos[3], cam_pos[4], cam_pos[5],
    cam_look_pos[3], cam_look_pos[4], cam_look_pos[5],
    0.0, 1.0, 0.0);
}

void setup_scene(){
  setup_lighting();
  STATE = S_SIMULATION;
}














// MAIN LOOPS
// ----------------------------------------------------------------------------

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

  glLoadIdentity();
  camera_move();

  if (STATE == S_SIMULATION){
    pi_draw();
  }

  glFlush();
  glutSwapBuffers();
}

void render_loop(int value) {
  glutPostRedisplay();
  glutTimerFunc(RENDER_FPS, render_loop, 0);
}

void logic_loop(int value){
  pi_loop();
  glutTimerFunc(LOGIC_FPS, logic_loop, 0);
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  setup_app();
  setup_menu();
  setup_scene();

  pi_setup();

  glutTimerFunc(0, render_loop, 0);
  glutTimerFunc(0, logic_loop, 0);
  glutMainLoop();
   return 0;
}



// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.