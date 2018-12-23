// This sketch demonstrates the operation of the laser turret. Pressing P activates the slope change, pressing O activates the height change. The images to put in the Data folder represent a wall and the 3D model of the laser turret.



PImage immagine, IM;
float x = 1000;
float y=500;
float a=0;
float b=0;
float c=0;
float d=0;
int mode= 0;

int KEY_LEFT = 0;
int KEY_RIGHT = 1;
int KEY_UP = 2;
int KEY_DOWN = 3;
public boolean[] keysPressed = new boolean[] {false, false, false, false};

void setup() {
  fullScreen();
  background(255);
  stroke(255, 0, 0);
  IM=loadImage("3.jpg");
  immagine = loadImage("3imm.png");
}
void updateRotation() {
  float speed = 3.0f;
  if (keysPressed[KEY_LEFT]) {
    a += speed;
    b -= speed;
  } 
  if (keysPressed[KEY_RIGHT]) {
    a -= speed;
    b += speed;
  }
  if (keysPressed[KEY_UP]) {
    c += speed;
    d -= speed;
  }
  if (keysPressed[KEY_DOWN]) {
    c -= speed;
    d += speed;
  }
}

void updateMovement() {
  int dirX = 0;
  int dirY = 0;
  float speed = 10.0f; 
  if (keysPressed[KEY_LEFT]) {
    dirX = -1;
  } 
  if (keysPressed[KEY_RIGHT]) {
    dirX = 1;
  }
  if (keysPressed[KEY_UP]) {
    dirY = -1;
  }
  if (keysPressed[KEY_DOWN]) {
    dirY = 1;
  }
  x += speed * dirX;
  y += speed * dirY;
}

void draw() {
  if(mode == 0){
    updateMovement();
  } else {
    updateRotation();
  }
  background(255);
  image(IM, 0, 0);
  image(immagine, 0, 0);

  line(0, y+a, width, y+b);
  line(x+c, 0, x+d, height);
}

void keyPressed() {
  if (key == CODED) {
    if (keyCode == UP) {
      keysPressed[KEY_UP] = true;
      keysPressed[KEY_DOWN] = false;
    } else if (keyCode == DOWN) {
      keysPressed[KEY_DOWN] = true;
      keysPressed[KEY_UP] = false;
    } else if (keyCode == LEFT) {
      keysPressed[KEY_LEFT] = true;
      keysPressed[KEY_RIGHT] = false;
    } else if (keyCode == RIGHT) {
      keysPressed[KEY_RIGHT] = true;
      keysPressed[KEY_LEFT] = false;
    }
  }

  switch(key) {
  case 'o':
    mode=0;
    break;
  case 'p':
    mode=1;
    break;
  case 'i':  
    a=0;
    b=0;
    c=0;
    d=0;
    x=1000;
    y=500;
    break;
  }
}

void keyReleased() {
  if (key == CODED) {
    if (keyCode == UP) {
      keysPressed[KEY_UP] = false;
    } else if (keyCode == DOWN) {
      keysPressed[KEY_DOWN] = false;
    } else if (keyCode == LEFT) {
      keysPressed[KEY_LEFT] = false;
    } else if (keyCode == RIGHT) {
      keysPressed[KEY_RIGHT] = false;
    }
  }
}

void keyPressedAncoraPiuVecchio() {
  switch(key) {
  case 'o':
    mode=0;
    break;
  case 'p':
    mode=1;
    break;
  case 'i':  
    a=0;
    b=0;
    c=0;
    d=0;
    x=1000;
    y=500;
    break;
  }
  if (mode==0) {
    if (key == CODED) {
      if (keyCode == UP) {
        y=y-3;
      } else if (keyCode == DOWN) {
        y=y+3;
      } else if (keyCode == LEFT) {
        x=x-3;
      } else if (keyCode == RIGHT) {
        x=x+3;
      }
    }
  }
  if (mode==1) {
    if (key == CODED) {
    } else if (keyCode == LEFT) {
      c=c-3;
      d=d+3;
    } else if (keyCode == RIGHT) {
      c=c+3;
      d=d-3;
    }
  }
}