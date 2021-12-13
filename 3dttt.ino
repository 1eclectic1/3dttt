#include <Adafruit_NeoPixel.h>

#define b1Pin  2    // the number of the pushbutton pin
#define b2Pin  4    // the number of the pushbutton pin
#define PIN        6
Adafruit_NeoPixel pixels(64, PIN, NEO_GRB + NEO_KHZ800);
long lastDebounceTime = 0;  // the last time the output pin was toggled
int b1State = digitalRead(b1Pin);             // the current reading from the input pin
int lastB1State = b1State;   // the previous reading from the input pin
int b2State = digitalRead(b2Pin);             // the current reading from the input pin
int lastB2State = b2State;   // the previous reading from the input pin
int b1reading = b1State;
int b2reading = b2State;
int brite = 2;

int gameboard[9];
int num[64];
int lane[76];
const int gamepix[9] = {
  53,54,55,57,58,59,61,62,63};
const int winner[8][3] = {
  53,54,55,57,58,59,61,62,63,53,57,61,54,58,62,55,59,63,53,58,63,55,58,61};

const int ntest[9] = {
  4,15,3,10,5,0,2,1,5};

const int nmove[16] = {
  11,64,13,52,4,61,16,49,22,43,23,42,26,39,27,38};

const int move[76][4] = {
  21, 42, 63, 0, 22, 41, 60, 3, 25, 38, 51, 12, 26, 37, 48, 15, 
  20, 40, 60, 0, 21, 41, 61, 1, 22, 42, 62, 2, 23, 43, 63, 3, 
  18, 33, 48, 3, 22, 37, 52, 7, 26, 41, 56, 11, 30, 45, 60, 15, 
  24, 36, 48, 12, 25, 37, 49, 13, 26, 38, 50, 14, 27, 39, 51, 15, 
  17, 34, 51, 0, 21, 38, 55, 4, 25, 42, 59, 8, 29, 46, 63, 12, 
  16, 32, 48, 0, 17, 33, 49, 1, 18, 34, 50, 2, 19, 35, 51, 3, 
  20, 36, 52, 4, 21, 37, 53, 5, 22, 38, 54, 6, 23, 39, 55, 7, 
  24, 40, 56, 8, 25, 41, 57, 9, 26, 42, 58, 10, 27, 43, 59, 11, 
  28, 44, 60, 12, 29, 45, 61, 13, 30, 46, 62, 14, 31, 47, 63, 15, 
  1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 8, 13, 14, 15, 12, 
  4, 8, 12, 0, 5, 9, 13, 1, 6, 10, 14, 2, 7, 11, 15, 3, 
  5, 10, 15, 0, 6, 9, 12, 3, 17, 18, 19, 16, 21, 22, 23, 20, 
  25, 26, 27, 24, 29, 30, 31, 28, 20, 24, 28, 16, 21, 25, 29, 17, 
  22, 26, 30, 18, 23, 27, 31, 19, 21, 26, 31, 16, 22, 25, 28, 19, 
  33, 34, 35, 32, 37, 38, 39, 36, 41, 42, 43, 40, 45, 46, 47, 44, 
  36, 40, 44, 32, 37, 41, 45, 33, 38, 42, 46, 34, 39, 43, 47, 35, 
  37, 42, 47, 32, 38, 41, 44, 35, 49, 50, 51, 48, 53, 54, 55, 52, 
  57, 58, 59, 56, 61, 62, 63, 60, 52, 56, 60, 48, 53, 57, 61, 49, 
  54, 58, 62, 50, 55, 59, 63, 51, 53, 58, 63, 48, 54, 57, 60, 51};


void setup() {
  pinMode(b1Pin, INPUT_PULLUP);
  pinMode(b2Pin, INPUT_PULLUP);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {

  int rc;
  clean();
  for(int i=0; i<16; i++) {
    pixels.setPixelColor(63-i, pixels.Color(brite*3, 0, 0));
    pixels.show();
  }
  for(int i=0; i<16; i++) {
    pixels.setPixelColor(63-i, pixels.Color(brite*20, 0, 0));
    pixels.show();
    if (select() == 2) {
      clean();
      rc=pick(i+1);
      break;
    } 
    else {
      pixels.setPixelColor(63-i, pixels.Color(brite*2, 0, 0));
    }
  }
}

void setbrite() {

  int rc;
  clean();
  for(int i=0; i<10; i++) {
    pixels.setPixelColor(63-i, pixels.Color(brite*2, 0, 0));
    pixels.show();
  }
  for(int i=0; i<10; i++) {
    pixels.setPixelColor(63-i, pixels.Color(brite*10, 0, 0));
    pixels.show();
    if (select() == 2) {
      clean();
      brite = i+1;
      break;
    } 
    else {
      pixels.setPixelColor(63-i, pixels.Color(brite*2, 0, 0));
    }
  }
}

int pick(int game) {
  int rc;
  switch(game) {
  case 1:
    setbrite(); // 1 player easiest - try to lose
    break;
  case 2:
    rc = xtt3d(1); // 1 player hardest level
    break;
  case 3:
    rc = xtt3d(2); // 1 player hardest level computer first
    break;
  case 4:
    rc = xtt3d(3); // 2 player head to head
    break;
  case 5:
    rc = xtt3d(4); // 1 player medium - finish traps
    break;
  case 6:
    rc = xtt3d(5); // 1 player medium - block player if possible
    break;
  case 7:
    rc = xtt3d(6); // 1 player easy - check for a winning move
    break;
  case 8:
    rc = regularTTT(); // 2 player 3x3
    break;
  case 10:
    rain();
    break;
  case 11:
    rc = startwinkle();
    break;
  case 12:
    rc = test12();
    break;
  case 13:
    rc = test13();
    break;
  case 14:
    rc = test14();
    break;
  case 15:
    rc = test15();
    break;
  case 16:
    rc = test16();  // lights off delay loop
    break;
  }
}

int xtt3d(int flag) {
  clean();
  int rc;
  for(int i=0; i<64; i++) {
    pixels.setPixelColor(i, pixels.Color(3, 3, 3));
    num[i] = 0;
  }
  pixels.show();
  if (flag == 2) rc=computermove(flag);
  while (1 == 1) {
    for(int i=0; i<64; i++) {
      if(num[i] == 0) {
        pixels.setPixelColor(i, pixels.Color(20, 20,0));
        pixels.show();
        rc=select();
        pixels.setPixelColor(i, pixels.Color(3, 3, 3));
        pixels.show();
        if(rc==2) {
          num[i] = 1;
          pixels.setPixelColor(i, pixels.Color(20, 0, 0));
          pixels.show();
          rc = computermove(flag);
          if (rc == 9) {
            return(0);
            break;
          }
        }
      }
    }
  }
}

int computermove(int level) {
  int rc;
  // did player win?
  for (int i=0; i < 76; i++) {
    int j1 = move[i][0];
    int j2 = move[i][1];
    int j3 = move[i][2];
    int j4 = move[i][3];
    lane[i] = num[j1]+num[j2]+num[j3]+num[j4];
    if (lane[i] == 4) {
      rc = w3d(i, 1);
      return(9);
    }
  }
  // Nope - do we have  winning move?
  if (level<6) {
    for (int i=0; i < 76; i++) {
      if (lane[i] == 15) {
        rc = w3d(i, 2);
        return(9);
      }
    }
  }
  // can anyone win now?
  int winlist[7] {
    0,1,2,3,5,10,15                                                                };
  int wintest = 0;
  for (int i=0; i<76; i++) {
    for (int j=0; j<7; j++) {
      if (lane[i] == winlist[j]) wintest++;
    }
  }
  if (wintest == 0) {
    rc = w3d(1,3);
    return(9);
  }
  // can we block?
  if (level < 5) {
    for (int i=0; i < 76; i++) {
      if (lane[i] == 3) {
        int j1 = move[i][0];
        int j2 = move[i][1];
        int j3 = move[i][2];
        int j4 = move[i][3];
        if (num[j1]==0) {
          num[j1] = 5;
          pixels.setPixelColor(j1, pixels.Color(0, 20, 0));
        }
        if (num[j2]==0) {
          num[j2] = 5;
          pixels.setPixelColor(j2, pixels.Color(0, 20, 0));
        }
        if (num[j3]==0) {
          num[j3] = 5;
          pixels.setPixelColor(j3, pixels.Color(0, 20, 0));
        }
        if (num[j4]==0) {
          num[j4] = 5;
          pixels.setPixelColor(j4, pixels.Color(0, 20, 0));
        }
        pixels.show();

        return(0);
      }
    }
  }
  // Can we finish a trap??

  if (level<4) {
    for (int i=0; i < 76; i++) {
      for (int j=0; j<76; j++) {
        if (lane[i] == 10 && lane[j] == 10 && i != j) {
          for (int k=0; k<4; k++) {
            for (int l=0; l<4; l++) {
              int j1 = move[j][l];
              if (move[i][k] == j1 && num[j1] == 0 ) {
                num[j1] = 5;
                for (int m=0; m<4; m++) {
                  if (num[move[i][m]] == 5) {
                    pixels.setPixelColor(move[i][m], pixels.Color(0, 80, 0));
                  }
                }
                for (int m=0; m<4; m++) {
                  if (num[move[j][m]] == 5) {
                    pixels.setPixelColor(move[j][m], pixels.Color(0, 80, 0));
                  }
                }
                pixels.show();
                return(0);
              }
            }
          }
        }
      }
    }
  }
  // Random move for now
  int mymove = random(63);
  for (int i=0; i<64; i++) {
    mymove = i+ mymove;
    if (mymove >= 64) {
      mymove = mymove - 64;
    }
    if (num[mymove] == 0) {
      num[mymove] = 5;
      pixels.setPixelColor(mymove, pixels.Color(0, 20, 0));
      pixels.show();
      return(0);
    }
  }
  return(0);
}

int w3d(int i, int who) {
  int c1;
  int c2;
  int c3 = 0;
  int c4 = 0;
  int j1 = move[i][0];
  int j2 = move[i][1];
  int j3 = move[i][2];
  int j4 = move[i][3];
  switch(who) {
  case 1:
    c1 = 100;
    c2 = 0;
    break;
  case 2:
    c1 = 0;
    c2 = 100;
    break;
  case 3:
    c1 = 0;
    c2 = 100;
    c3 = 100;
    c4 = 0;
    j1 = 63;
    j2 = 60;
    j3 = 51;
    j4 = 48;
  }
  while (1 == 1) {
    pixels.setPixelColor(j1, pixels.Color(c1,c2,0));
    pixels.setPixelColor(j2, pixels.Color(c1,c2,0));
    pixels.setPixelColor(j3, pixels.Color(c1,c2,0));
    pixels.setPixelColor(j4, pixels.Color(c1,c2,0));
    pixels.show();
    delay(500);
    pixels.setPixelColor(j1, pixels.Color(c3,c4,0));
    pixels.setPixelColor(j2, pixels.Color(c3,c4,0));
    pixels.setPixelColor(j3, pixels.Color(c3,c4,0));
    pixels.setPixelColor(j4, pixels.Color(c3,c4,0));
    pixels.show();
    delay(500);
    b1reading = digitalRead(b1Pin);
    if (b1reading != b1State) {
      int rc = select();
      return(1);
    }
  }
}

int regularTTT() {
  int movenum = 0;
  int player;
  int rc;
  for(int i=0; i<9; i++) {
    pixels.setPixelColor(gamepix[i], pixels.Color(5, 5, 5));
    gameboard[i] = 0;
  }
  pixels.show();
  player = 1;
  while (1 == 1) {
    for(int i=0; i<9; i++) {
      if(gameboard[i] == 0) {
        pixels.setPixelColor(gamepix[i], pixels.Color(20, 20,0));
        pixels.show();
        rc = 2;
        if (movenum < 8) {
          rc=select();
        }
        pixels.setPixelColor(gamepix[i], pixels.Color(5, 5, 5));
        pixels.show();
        if(rc==2 || movenum == 9) {
          movenum = movenum + 1;
          gameboard[i] = player;
          if (player == 1) {
            player = 4;
            pixels.setPixelColor(gamepix[i], pixels.Color(20, 0, 0));
            pixels.show();
          } 
          else {
            player = 1;
            pixels.setPixelColor(gamepix[i], pixels.Color(0, 20, 0));
          }
          rc = checkWin(movenum);
          if (rc == 0) {
            break;
          }
          return(0);
        }
      }
    }
  }
}

int checkWin(int who) {
  int test[8];
  if (who == 9) {
    while (1==1) {
      pixels.setPixelColor(61, pixels.Color(100,0,0));
      pixels.setPixelColor(55, pixels.Color(100,0,0));
      pixels.setPixelColor(63, pixels.Color(0,100,0));
      pixels.setPixelColor(53, pixels.Color(0,100,0));
      pixels.show();
      delay(500);
      pixels.setPixelColor(61, pixels.Color(0,100,0));
      pixels.setPixelColor(55, pixels.Color(0,100,0));
      pixels.setPixelColor(63, pixels.Color(100,0,0));
      pixels.setPixelColor(53, pixels.Color(100,0,0));
      pixels.show();
      delay(500);
      b1reading = digitalRead(b1Pin);
      if (b1reading != b1State) {
        int rc = select();
        return(1);    
      }
    }
  }
  test[0] = gameboard[0]+gameboard[1]+gameboard[2]; 
  test[1] = gameboard[3]+gameboard[4]+gameboard[5]; 
  test[2] = gameboard[6]+gameboard[7]+gameboard[8]; 
  test[3] = gameboard[0]+gameboard[3]+gameboard[6]; 
  test[4] = gameboard[1]+gameboard[4]+gameboard[7]; 
  test[5] = gameboard[2]+gameboard[5]+gameboard[8]; 
  test[6] = gameboard[0]+gameboard[4]+gameboard[8]; 
  test[7] = gameboard[2]+gameboard[4]+gameboard[6];
  for (int xx = 0; xx< 8; xx++) {
    if ((test[xx] == 3) || (test[xx] == 12)) {
      while (1==1) {
        for(int xy = 0; xy< 3; xy++) {
          if (test[xx] == 3) {
            pixels.setPixelColor(winner[xx][xy], pixels.Color(100,0,0));
          }
          if (test[xx] == 12) {
            pixels.setPixelColor(winner[xx][xy], pixels.Color(0,100,0));
          }
        }
        pixels.show();
        delay(500);
        for(int xy = 0; xy< 3; xy++) {
          pixels.setPixelColor(winner[xx][xy], pixels.Color(2, 2, 2));
        }
        pixels.show();
        delay(500);
        b1reading = digitalRead(b1Pin);
        if (b1reading != b1State) {
          int rc = select();
          return(1);    
        }
      }
    }
  }
  return(0);
}


int select() {
  long debounceDelay = 50;    // the debounce time; increase if the output flickers
  while (1==1) {  // Forever 
    b1reading = digitalRead(b1Pin);
    b2reading = digitalRead(b2Pin);

    if ((millis() - lastDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:

      // if the button 1 state has changed:
      if (b1reading != b1State) {
        lastDebounceTime = millis();
        b1State = b1reading;
        if (b1State == LOW) {
          return 1;
        }
      }

      // if the button 2 state has changed:
      if (b2reading != b2State) {
        lastDebounceTime = millis();
        b2State = b2reading;
        if (b2State == LOW) {
          return 2; 
        }
      }
    }
  } 
}



int startwinkle() {
  int rr;
  int cc;
  int ll;
  int c1;
  int c2;
  int c3;
  int brightness = brite*3;
  for (rr=0; rr<4; rr++) {
    for (cc=0; cc<4; cc++) {
      for (ll=0; ll<4; ll++) {
        LED(ll, rr, cc, brightness, brightness, brightness);
      }
    }
  }   
  while (1 == 1) {
    ll = random(4);
    rr = random(4);
    cc = random(4);
    c1 = random(brightness);
    c2 = random(brightness);
    c3 = random(brightness);
    if (c3>brightness/2) {
      c1=0; 
      c2=0; 
      c3=0;
    }
    LED(ll, rr, cc, c1, c2, c3);
    delay(50);
    b1reading = digitalRead(b1Pin);
    if (b1reading != b1State) {
      int rc = select();
      return(0);
    }
  }
  return(0);
}

int test12() {
  int g1[64] {
    1,1,1,1,1,0,0,1,1,0,0,1,1,1,1,1,
    1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,
    1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,
    1,1,1,1,1,0,0,1,1,0,0,1,1,1,1,1                                                                                    };
  int g2[64] {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,
    0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0                                                                                    };
  int pix;
  int brightness = brite*5;
  while (1==1) {
    for (int i=0; i<64; i++) {
      if (g1[i] == 1) {
        pixels.setPixelColor(i, pixels.Color(brightness, brightness, brightness)); 
      } 
      else
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));            
    }
    pixels.show();
    delay(500);
    for (int i=0; i<64; i++) {
      if (g2[i] == 1) {
        pixels.setPixelColor(i, pixels.Color(brightness, brightness, brightness)); 
      } 
      else
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));            
    }
    pixels.show();
    delay(500);
    b1reading = digitalRead(b1Pin);
    if (b1reading != b1State) {
      int rc = select();
      return(0);    
    }
  }
}

int test13() {
  int pix;
  int b = brite*25;
  while (1==1) {
    for (int p=0; p<3; p++) {
      for (int r=0; r<4; r++) {
        for (int c=0; c<4; c++) {
          for (int l=0; l<4; l++) {
            if (p == 0) {
              LED(l, r, c, b, b, b);
            }
            if (p == 1) {
              LED(r, l, c, b, b, b);
            }
            if (p == 2) {
              LED(c, r, l, b, b, b);
            }
          }
          delay(100);
          clean();
          b1reading = digitalRead(b1Pin);
          if (b1reading != b1State) {
            int rc = select();
            return(0);
          }
        }
      }   
    }
  }
}

int test14() {
  int lev[16] = {
    1,3,2,0,1,3,2,0,1,3,2,0,1,3,2,0                                                                                                       };
  int pix;
  int brightness = 25;
  int b1 = random(brightness);
  int b2 = random(brightness);
  int b3 = random(brightness);
  int d1 = -1;
  int d2 = 3;
  int d3 = -2;
  while (1 == 1) {
    for (int l=0; l<4; l++) {
      for (int c=0; c<4; c++) {
        b1 = b1 + d1;
        if (b1<0 || b1 > brightness) {
          d1 = -d1;
          b1 = b1 + d1;
        }
        b2 = b2 + d2;
        if (b2<0 || b2 > brightness) {
          d2 = -d2;
          b2 = b2 + d2;
        }
        b3 = b3 + d3;
        if (b3<0 || b3 > brightness) {
          d3 = -d3;
          b3 = b3 + d3;
        }
        for (int r=0; r<4; r++) {
          LED(lev[(l + c)],r,c,brite*b1,brite*b2,brite*b3);
        }
      }
      delay(100);
      clean();
    }
    b1reading = digitalRead(b1Pin);
    if (b1reading != b1State) {
      int rc = select();
      return(0);
    }
  } 
}

int test15() {
  int rr;
  while (1==1) {
    for (rr = 0; rr<64; rr++) {
      pixels.setPixelColor(rr, pixels.Color(brite*20, brite*20, brite*20));
      pixels.show();
      delay(50);
      b1reading = digitalRead(b1Pin);
      if (b1reading != b1State) {
        int rc = select();
        return(0);
      }
      clean();
    }
  }
}

int test16() {
  int rr;
  clean();
  while (1==1) {
    delay(1000);
    b1reading = digitalRead(b1Pin);
    if (b1reading != b1State) {
      int rc = select();
      return(0);
    }
  }
}

void LED(int level, int row, int column, byte red, byte green, byte blue) {
  int pix = level*16 + row*4 + column;
  pixels.setPixelColor(pix, pixels.Color(red, blue, green));
  pixels.show(); 
}

void clean() {
  pixels.clear();
  pixels.show();
}

void rain() {
  int y[4][4], Rbrite;

  for (int i = 0; i< 4; i++) {
    for (int j = 0; j< 4; j++) {
      y[i][j] = 0;
    }
  }

  clean();

  while (1==1) {

    for (int i = 0; i< 4; i++) {
      for (int j = 0; j< 4; j++) {

        switch(y[i][j]) {
        case 0:
          Rbrite = random(25);
          if (Rbrite == 3) { 
            y[i][j] = 8; 
          }
          break;
        case 1:
          y[i][j] = 0;
          LED(0,i,j,0,0,0);
          break;
        case 2:
          y[i][j] = 1;
          LED(1,i,j,0,0,0);
          break;
        case 3:
          y[i][j] = 2;
          LED(2,i,j,0,0,0);
          break;
        case 4:
          y[i][j] = 3;
          LED(3,i,j,0,0,0);
          break;
        case 5:
          y[i][j] = 4;
          LED(0,i,j,brite,brite,brite);
          break;
        case 6:
          y[i][j] = 5;
          LED(1,i,j,brite*2,brite*2,brite*2);
          break;
        case 7:
          y[i][j] = 6;
          LED(2,i,j,brite*3,brite*3,brite*3);
          break;
        case 8:
          y[i][j] = 7;
          LED(3,i,j,brite*4,brite*4,brite*6);
          break;
        }
      }
    }

    delay(160);

    b1reading = digitalRead(b1Pin);
    if (b1reading != b1State) {
      int rc = select();
      return;
    }
  }
}

