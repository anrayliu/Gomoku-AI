#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdlib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned int MSG_LEN = 16;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.display();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  custom_loop();
}

void loop() {}

void custom_loop() {
  char msg[MSG_LEN];
  int x;
  int y;
  short v;
  int w = -1;
  int h = -1;
  int win_con;
  short** board;
  int movex;
  int movey;

  while (true) {
    read_message(msg);
    
    if (strcmp(msg, "set") == 0) {
      read_message(msg);
      x = atoi(msg);
      showC(msg);

      read_message(msg);
      y = atoi(msg);
      show(msg);

      board[y][x] = 1;

      if (check_board(board, x, y, 1, w, h, win_con)) {
        showC("You won!");
        break;
      }

      move(board, w, h, &movex, &movey, win_con);

      Serial.write("<set>");
      Serial.write(to_str(movex, movey));

      board[movey][movex] = 2;

      if (check_board(board, movex, movey, 2, w, h, win_con)) {
        showC("I won!");
      }

    } else if (strcmp(msg, "reset") == 0) {
      if (w != -1 || h != -1) {
        for (int i = 0; i < h; i ++) {
          delete[] board[i];
        }
        delete board;
      }

      read_message(msg);
      w = atoi(msg);
      showC(msg);

      read_message(msg);
      h = atoi(msg);
      show(msg);

      read_message(msg);
      win_con = atoi(msg);
      show(msg);

      board = new short*[h];
      for (int i = 0; i < h; i ++) {
        board[i] = new short[w];
        for (int j = 0; j < w; j ++) {
          board[i][j] = 0;
        }
      }

    }
  }
}

// doesn't account for tiles put in middle

bool check_board(short** board, int x, int y, short v, int w, int h, int win_con) {
  short possibilities[8]{};

  for (short i = 1; i < win_con; i ++) {
    if ((y - i < 0 || board[y - i][x] != v) && (possibilities[0] == 0)) {
      possibilities[0] = i;
    }
    if ((y + i > h - 1 || board[y + i][x] != v) && (possibilities[4] == 0)) {
      possibilities[4] = i;
    }
    if ((x - i < 0 || board[y][x - i] != v) && (possibilities[1] == 0)) {
      possibilities[1] = i;
    }
    if ((x + i > w - 1 || board[y][x + i] != v) && (possibilities[5] == 0)) {
      possibilities[5] = i;
    }
    if ((y - i < 0 || x - i < 0 || board[y - i][x - i] != v) && (possibilities[2] == 0)) {
      possibilities[2] = i;
    }
    if ((y + i > h - 1 || x + i > w - 1 || board[y + i][x + i] != v) && (possibilities[6] == 0)) {
      possibilities[6] = i;
    }
    if ((y + i > h - 1 || x - i < 0 || board[y + i][x - i] != v) && (possibilities[3] == 0)) {
      possibilities[3] = i;
    }
    if ((y - i < 0 || x + i > w - 1 || board[y - i][x + i] != v) && (possibilities[7] == 0)) {
      possibilities[7] = i;
    }
  }

  for (short k : possibilities) {
    if (k == 0) {
      return true;
    }
  }

  for (short j = 0; j < 4; j ++) {
    if (possibilities[j] - 1 + possibilities[j + 4] == win_con) {
      return true;
    }
  }

  return false;
}

void show(const char* message) {
  display.println(message);
  display.display();
}

void showC(const char* message) {
  display.setCursor(0, 0);
  display.clearDisplay();
  display.println(message);
  display.display();
}

char* to_str(int a, int b) { 
  static char buffer[16];
  snprintf(buffer, sizeof(buffer), "<%d %d>", a, b);
  return buffer;
}

void read_message(char* message) {
  unsigned int index = 0;
  char recv_char;
  bool start_tracking = false;

  while (true) {
    if (Serial.available()) {
      recv_char = Serial.read();

      if (start_tracking) {
        if (recv_char != '>' && index < MSG_LEN - 1) {
          message[index++] = recv_char;
        } else {
          message[index] = '\0';
          return;
        }
      } else if (recv_char == '<') {
        start_tracking = true;
      }

    }
  }
}

void move(short** board, int w, int h, int* movex, int* movey, int win_con) {
  for (int k = win_con; k > 0; k --) {
    for (int i = 0; i < h; i ++) {
      for (int j = 0; j < w; j ++) {
        if (board[i][j] == 0) {
          if (check_board(board, j, i, 2, w, h, k) || check_board(board, j, i, 1, w, h, k)) {
            (*movex) = j;
            (*movey) = i;
            return;
          }
        }
      }
    }
  }
}


