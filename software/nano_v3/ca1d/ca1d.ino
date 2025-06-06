#include <DMD2.h>
#include <SPI.h>
#include <fonts/Arial14.h>


#define DISPLAYS_WIDE 2
#define DISPLAYS_HIGH 1

#define ROW_LENGTH 64

SoftDMD dmd(DISPLAYS_WIDE, DISPLAYS_HIGH);
DMDFrame frame(64, 16);

uint8_t RULE = 30;
uint8_t DATA[ROW_LENGTH * 2];

uint8_t* current_cell_states;
uint8_t* next_cell_states;
uint8_t* temp;

int speed = 60;


int stateBasedOnNeighbors(int rule, int left, int center, int right) {
  return(rule >> (left << 2 | center << 1 | right)) & 1;
}

void setStartingValues(bool rand_start) {
  if(rand_start) {
    for(int i = 0; i < ROW_LENGTH; i++)
      current_cell_states[i] = rand() % 2;
  } else {
    for(int i = 0; i < ROW_LENGTH; i++)
      current_cell_states[i] = 0;
    current_cell_states[ROW_LENGTH / 2] = 1;
  }
}

void setup() {
  dmd.setBrightness(5);
  dmd.begin();

  current_cell_states = DATA;
  next_cell_states = DATA + ROW_LENGTH;

  setStartingValues(true);
}

uint8_t row_to_update = 0;

uint8_t message_counter = 0;

void loop() {
  if(row_to_update == 15)
    frame.scrollY(-1);

  for(int i = 0; i < ROW_LENGTH; i++)
    frame.setPixel(i, row_to_update, (current_cell_states[i]? GRAPHICS_ON : GRAPHICS_OFF));
  
  for(int i = 0; i < ROW_LENGTH; i++) {
    int left = current_cell_states[((i - 1) + ROW_LENGTH) % ROW_LENGTH];
    int curr = current_cell_states[i];
    int right = current_cell_states[(i + 1) % ROW_LENGTH];
    
    next_cell_states[i] = stateBasedOnNeighbors(RULE, left, curr, right);
  }

  temp = current_cell_states;
  current_cell_states = next_cell_states;
  next_cell_states = temp;

  if(row_to_update < 15)
    row_to_update++;

  dmd.copyFrame(frame, 0, 0);

  delay(speed);
}
