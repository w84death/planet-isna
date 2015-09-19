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

bool fullscreen_mode  = true;
char win_title[]      = "PLANET ISNA";
static float VERSION  = 0.3f;
int win_width         = 800;
int win_height        = 600;
int win_x             = 50;
int win_y             = 80;

// RENDERING SETTINGS
// ----------------------------------------------------------------------------

int frames = 0;
int cycles = 0;
static int RENDER_FPS = 1000/48;
static int LOGIC_FPS  = 1000/30;

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
static int AI_FPS                 = 1000/4;

static int CELLS_ARRAY_SIZE[]     = {100, 46};
static int CELLS_MAX              = CELLS_ARRAY_SIZE[0]*CELLS_ARRAY_SIZE[1];
static int CELLS_HALF[]           = {CELLS_ARRAY_SIZE[0] * 0.5, CELLS_ARRAY_SIZE[1] * 0.5};
static int LAYERS                 = 5;
static int GAME_LAYER             = 2;
static int MISSILE_LAYER          = 3;
static int HUD_LAYER              = 4;

int pi_buffor[100][46][4];
int pi_layers[5][100][46][5];

static int C_SPACE                = 0;
static int C_STAR                 = 1;
static int C_SHIP                 = 2;
static int C_WEAPON               = 3;
static int C_BODY                 = 4;
static int C_MISSILE              = 5;
static int C_HUD                  = 6;
static int C_EXPLOSION            = 7;

static float C_STAR_0_DENS        = 0.005f;
static float C_STAR_1_DENS        = 0.01f;

int STATE                         = 0;
static int S_INT                  = 0;
static int S_MENU                 = 2;
static int S_SIMULATION           = 4;
static int S_CREDITS              = 8;

static float COLOURS[16][3]        = {
  // NORMAL
  {0.0f,                0.0f,                 0.0f}, // black
  {(float)170 / 255.0f, 0.0f,                    0.0f}, // red
  {0.0f,                (float)170 / 255.0f,  0.0f}, // green
  {(float)170 / 255.0f, (float)85 / 255.0f,   0.0f}, // brown
  {0.0f,                0.0f,                 (float)170 / 255.0f}, // blue
  {0.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 0.0f},
  {0.5f, 0.5f, 0.5f}, // gray
  // LIGHT
  {(float)85 / 255.0f, (float)85 / 255.0f, (float)85 / 255.0f}, // gray
  {0.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 0.0f},
  {1.0f, 1.0f, 1.0f},
};

static int HUD_LOGO[3][5] = {
  {2, 2, 2, 2, 2},
  {2,80,49,88, 2},
  {2, 2, 2, 2, 2}};

static int HUD_LOGO_SIZE[2] = {3,5};

//hlorotfortum wariagatum

static int SPRITES[12][5][5][3] = {
  {
    {{21,15,0}, {2,4,0}, {28,4,0}, {2,4,0}, {22,15,0}},
    {{1,15,0}, {2,4,0}, {2,15,4}, {2,4,0},  {1,15,0}},
    {{0,0,0}, {13,15,0}, {2,15,4}, {12,15,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {2,15,4}, {0,0,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}}
  },
  {
    {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {1,1,0}, {0,0,0}, {0,0,0}},
    {{0,0,0}, {29,1,0}, {2,0,1}, {29,1,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {16,1,0}, {0,0,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}}
  },
  {
    {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {29,1,0}, {0,0,0}, {0,0,0}},
    {{1,1,0}, {2,1,0}, {2,0,1}, {2,1,0}, {1,1,0}},
    {{0,0,0}, {16,1,0}, {29,1,0}, {16,1,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}}
  },
  {
    {{0,0,0}, {0,0,0}, {1,1,0}, {0,0,0}, {0,0,0}},
    {{0,0,0}, {29,1,0}, {29,1,0}, {29,1,0}, {0,0,0}},
    {{2,1,0}, {2,1,0}, {2,0,1}, {2,1,0}, {2,1,0}},
    {{2,1,0}, {29,1,0}, {29,1,0}, {29,1,0}, {2,1,0}},
    {{0,0,0}, {2,1,0}, {2,1,0}, {2,1,0}, {0,0,0}}
  }};

static int SPRITES_SIZE[2] = {5,5};

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


















// ENGINE
// ----------------------------------------------------------------------------
void pi_draw_hud_element(){
  int sprite;

  for (int y = 0; y < HUD_LOGO_SIZE[0]; y++){
  for (int x = 0; x < HUD_LOGO_SIZE[1]; x++){
    sprite = HUD_LOGO[y][x];
    pi_layers[HUD_LAYER][x][y][0] = C_HUD;
    pi_layers[HUD_LAYER][x][y][1] = sprite;
    pi_layers[HUD_LAYER][x][y][2] = 15;
    pi_layers[HUD_LAYER][x][y][3] = 0;

  }}
};

void pi_insert_entity(int id, int sprite_id, int start_x, int start_y){
  int sprite;
  int colour_f;
  int colour_b;
  int px;
  int py;
  int type = C_SHIP;

  for (int y = 0; y < SPRITES_SIZE[0]; y++){
  for (int x = 0; x < SPRITES_SIZE[1]; x++){
    sprite = SPRITES[sprite_id][y][x][0];
    colour_f = SPRITES[sprite_id][y][x][1];
    colour_b = SPRITES[sprite_id][y][x][2];
    px = x+start_x;
    py = y+start_y;
    if (sprite > 0){
      type = C_SHIP;
      if (sprite == 1) type = C_WEAPON;
      if (sprite == 2) type = C_BODY;
      pi_layers[GAME_LAYER][px][py][0] = type;
      pi_layers[GAME_LAYER][px][py][1] = sprite;
      pi_layers[GAME_LAYER][px][py][2] = colour_f;
      pi_layers[GAME_LAYER][px][py][3] = colour_b;
      pi_layers[GAME_LAYER][px][py][4] = id;
    }
  }}
};

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
        pi_layers[b][x][y][2] = 8;

      }
      if (b==1){
        r = random_f() < C_STAR_1_DENS ? C_STAR : C_SPACE;
        pi_layers[b][x][y][0] = r;
        pi_layers[b][x][y][1] = 7;
        pi_layers[b][x][y][2] = 7;
      }

      if (b>1){
        pi_layers[b][x][y][0] = C_SPACE;
        pi_layers[b][x][y][1] = 0;
        pi_layers[b][x][y][2] = 0;
      }

      pi_layers[b][x][y][3] = 0;
      pi_layers[b][x][y][4] = 0;

    }
  }}

  pi_draw_hud_element();
  pi_insert_entity(1, 0, 30, 4);
  pi_insert_entity(2, 1, 10, 80);
  pi_insert_entity(3, 2, 35, 70);
  pi_insert_entity(4, 3, 66, 77);
  pi_insert_entity(5, 1, 15, 60);
}

void pi_draw_tile(int x, int y, char sprite, int colour_f, int colour_b){
  
  if ( colour_b > 0) {
    glPushMatrix();
      // BACK
      glTranslatef (x*0.9f-0.1f, y*1.5f-0.3f, 0.0f);
    glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE ) ;
    glEnable ( GL_COLOR_MATERIAL ) ;

      glColor4f(COLOURS[colour_b][0], COLOURS[colour_b][1], COLOURS[colour_b][2], 1.0f);

      float size_x = 0.9f;
      float size_y = 1.5f;

      glBegin(GL_POLYGON);
         glVertex3d(0, size_y, 0.0f);
         glVertex3d(0, 0, 0.0f);
         glVertex3d(size_x, 0, 0.0f);
         glVertex3d(size_x, size_y, 0.0f);
      glEnd();

    glPopMatrix();
  }
  glPushMatrix();
    // FRONT
    glEnable ( GL_COLOR_MATERIAL ) ;
    glColor4f(COLOURS[colour_f][0], COLOURS[colour_f][1], COLOURS[colour_f][2], 1.0f);
    glRasterPos2f(x*0.9f, y*1.5f);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, sprite);
  glPopMatrix();
}

void  pi_draw_buffor(){
  int cell_type;
  int cell_char;
  int cell_colour_f;
  int cell_colour_b;

  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
    cell_type = pi_buffor[x][y][0];
    cell_char = pi_buffor[x][y][1];
    cell_colour_f = pi_buffor[x][y][2];
    cell_colour_b = pi_buffor[x][y][3];
    if (cell_type > C_SPACE){
      pi_draw_tile(x, y, cell_char, cell_colour_f, cell_colour_b);
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
    pi_buffor[x][y][3] = 0;
    for (int b = 0; b<LAYERS; b++){
      temp_type = pi_layers[b][x][y][0];
      if(temp_type > C_SPACE){
        temp_char = pi_layers[b][x][y][1];
        temp_color_f = pi_layers[b][x][y][2];
        temp_color_b = pi_layers[b][x][y][3];
        pi_buffor[x][y][0] = temp_type;
        pi_buffor[x][y][1] = temp_char;
        pi_buffor[x][y][2] = temp_color_f;
        pi_buffor[x][y][3] = temp_color_b;
      }
    }
  }}
}

 bool pi_check_ship(int id){
  int cell_id;
  int cell_type;
  int parts = 0;

  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
    cell_type = pi_layers[GAME_LAYER][x][y][0];
    cell_id = pi_layers[GAME_LAYER][x][y][4];
    if(cell_type == C_BODY and cell_id == id){
      parts++;
    }
  }}

  if (parts == 0){
    for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
    for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
      cell_id = pi_layers[GAME_LAYER][x][y][4];
      if(cell_id == id){
        pi_layers[GAME_LAYER][x][y][0] = C_SPACE;
      }
    }}
    return false;
  }
  return true;
}

void pi_move_cell(int layer, int id, int x1, int y1, int x2, int y2){
  int cell_id;
  int cell_char;
  int cell_type;
  int cell_colour_f;
  int cell_colour_b;

  cell_type = pi_layers[layer][x1][y1][0];
  cell_char = pi_layers[layer][x1][y1][1];
  cell_colour_f = pi_layers[layer][x1][y1][2];
  cell_colour_b = pi_layers[layer][x1][y1][3];

  if (x2 >= CELLS_ARRAY_SIZE[0]){
    x2 = 0;
  }

  if (x2 < 0){
    x2 = CELLS_ARRAY_SIZE[0]-1;
  }

  if (y2 >= 0 and y2 < CELLS_ARRAY_SIZE[1]){
    if (pi_layers[layer][x2][y2][0] > C_SPACE or (cell_type == C_MISSILE and pi_layers[GAME_LAYER][x2][y2][0] > C_SPACE)){
      pi_layers[layer][x2][y2][0] = C_SPACE;
      if(cell_type == C_MISSILE){
        pi_layers[GAME_LAYER][x2][y2][0] = C_SPACE;
        cell_id = pi_layers[GAME_LAYER][x2][y2][4];
        pi_check_ship(cell_id);
        pi_layers[GAME_LAYER][x2][y2][4] = 0;
      }
      pi_buffor[x2][y2][0] = C_EXPLOSION;
      pi_buffor[x2][y2][1] = 43;
      pi_buffor[x2][y2][2] = 16;
      pi_buffor[x2][y2][3] = 0;
    }else{
      pi_layers[layer][x2][y2][0] = cell_type;
      pi_layers[layer][x2][y2][1] = cell_char;
      pi_layers[layer][x2][y2][2] = cell_colour_f;
      pi_layers[layer][x2][y2][3] = cell_colour_b;
      pi_layers[layer][x2][y2][4] = id;
    }
  }

  pi_layers[layer][x1][y1][0] = C_SPACE;
  pi_layers[layer][x1][y1][4] = 0;
}

void pi_automate(int layer){

  int cell_type;
  int cell_char;
  int cell_x;
  int cell_id;

  if (layer == HUD_LAYER) return;

  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){

    cell_type = pi_layers[layer][x][y][0]; 
    cell_char = pi_layers[layer][x][y][1]; 

    if (layer == 0 or layer == 1){
      if (cell_type == C_STAR) {
        // clear old space
        pi_layers[layer][x][y][0] = C_SPACE;
        if(y>0){
          // move cell one tile ahead
          pi_layers[layer][x][y-1][0] = cell_type;
          pi_layers[layer][x][y-1][1] = cell_char;
        }else{
          // restart cell in the begining
          cell_x = (int)(random_f()*CELLS_ARRAY_SIZE[0]);
          pi_layers[layer][cell_x][CELLS_ARRAY_SIZE[1]-1][0] = cell_type;
          pi_layers[layer][cell_x][CELLS_ARRAY_SIZE[1]-1][1] = cell_char;
        }
      }
    }

    if (layer == MISSILE_LAYER and cell_type == C_MISSILE){
      cell_id = pi_layers[layer][x][y][4]; 
      if (cell_id > 1){
        pi_move_cell(layer, cell_id, x, y, x, y-1);
      }
    }
  }}

  if (layer == MISSILE_LAYER){
    for (int y = CELLS_ARRAY_SIZE[1]; y >= 0; --y){
    for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
      cell_id = pi_layers[layer][x][y][4]; 
      if (cell_id == 1){
        pi_move_cell(layer, cell_id, x, y, x, y+1);
      }
    }}
  }
}




void pi_move_entitie(int id, bool vertical, bool change){
  int cell_id;
  int cell_type;

  // RIGHT
  if (vertical and change){
    for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
    for (int x = CELLS_ARRAY_SIZE[0]; x >= 0; --x){
      cell_id = pi_layers[GAME_LAYER][x][y][4];
      if (cell_id == id){
        pi_move_cell(GAME_LAYER, id, x, y, x+1, y);
      }
    }}
  }

  // LEFT
  if (vertical and !change){
    for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
    for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
      cell_id = pi_layers[GAME_LAYER][x][y][4];
      if (cell_id == id){
        pi_move_cell(GAME_LAYER, id, x, y, x-1, y);
      }
    }}
  }
}

void pi_shoot_bullet(int id){
  int cell_type;
  int cell_id;

  for (int y = 0; y < CELLS_ARRAY_SIZE[1]; y++){
  for (int x = 0; x < CELLS_ARRAY_SIZE[0]; x++){
    cell_type = pi_layers[GAME_LAYER][x][y][0];
    cell_id = pi_layers[GAME_LAYER][x][y][4];
    if (cell_type == C_WEAPON and cell_id == id){

      pi_layers[MISSILE_LAYER][x][y][0] = C_MISSILE;
      pi_layers[MISSILE_LAYER][x][y][1] = 25;
      pi_layers[MISSILE_LAYER][x][y][2] = 15;
      pi_layers[MISSILE_LAYER][x][y][3] = 0;
      pi_layers[MISSILE_LAYER][x][y][4] = id;

    }
  }}
}

void pi_ai(int id){
  if(cycles % 2 == 0) pi_shoot_bullet(id);
  if (random_f()<0.5f) pi_move_entitie(id, true, random_f() < 0.5f ? true : false);
}

void pi_draw(){
  pi_draw_buffor();
  frames++;
};

void pi_loop(){

  if (cycles % 6 == 0 and cycles % 2 == 0) pi_automate(0);
  pi_automate(1);
  pi_automate(MISSILE_LAYER);
  pi_fill_buffor();
  cycles++;
};

void pi_ai_loop(){
  pi_ai(2);
  pi_ai(3);
  pi_ai(4);
  pi_ai(5);
  pi_shoot_bullet(1);
}








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
        pi_move_entitie(1, true, true);
        break;
      case GLUT_KEY_LEFT:
        pi_move_entitie(1, true, false);
        break;
      case GLUT_KEY_UP:
        
        break;
      case GLUT_KEY_DOWN:
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
        pi_shoot_bullet(1);
        break;
      case 101: // e
        break;
      case 97: // a

        break;
      case 100: // d

        break;

      case 119: // w
        pi_shoot_bullet(1);
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
  //gluPerspective (FOV, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
  gluOrtho2D(0.0, width, 0.0, height);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
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

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glLoadIdentity();

  glScalef(11.0f, 11.0f, 1.0f);
  glTranslatef(1.0f, 1.0f, 0.0f);

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

void ai_loop(int value){
  pi_ai_loop();
  glutTimerFunc(AI_FPS, ai_loop, 0);
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  setup_app();
  setup_menu();
  setup_scene();

  pi_setup();

  glutTimerFunc(0, render_loop, 0);
  glutTimerFunc(0, logic_loop, 0);
  glutTimerFunc(0, ai_loop, 0);
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