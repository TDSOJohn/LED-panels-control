#include <DMD2.h>
#include <SPI.h>
#include <fonts/Arial14.h>


#define DISPLAYS_WIDE 2
#define DISPLAYS_HIGH 1

#define ROW_LENGTH 64

SoftDMD dmd(DISPLAYS_WIDE, DISPLAYS_HIGH);
DMDFrame ca_frame(64, 16);
DMDFrame text_frame(64, 16);

uint8_t RULE = 30;
uint8_t DATA[ROW_LENGTH * 2];

uint8_t* current_cell_states;
uint8_t* next_cell_states;
uint8_t* temp;

int speed = 60;

enum Mode {
  CA,
  NAME_STROBE,
};

struct RandData {
  uint8_t delay_time;
  uint8_t cycles;
  uint8_t pattern;
  uint8_t corruption_prob;
  uint8_t corruption_rate;
};

RandData data;

Mode current_mode = Mode::CA;


int stateBasedOnNeighbors(int rule, int left, int center, int right) {
  return(rule >> (left << 2 | center << 1 | right)) & 1;
}

void generateRandData() {
  data.delay_time = random(50, 120);
  data.cycles = random(20, 70);
  data.pattern = random(10);
  data.corruption_prob = random(20, 50);
  data.corruption_rate = random(20);
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

void corrupt_frame(DMDFrame& frame) {
  for(int i = 0; i < data.corruption_rate; i++) {
    uint8_t x = random(64);
    uint8_t y = random(16);
    uint8_t size = random(12);
    frame.drawLine(x, y, x + size, y);
  }
  delay(random(200));
}

void setup() {
  dmd.setBrightness(5);
  text_frame.selectFont(Arial14);
  dmd.begin();

  current_cell_states = DATA;
  next_cell_states = DATA + ROW_LENGTH;

  randomSeed(analogRead(A0));

  setStartingValues(true);
  generateRandData();
}

uint8_t row_to_update = 0;

int counter = 0;
int artist_counter = 0;

void loop() {
  counter++;
  // CA1D
  if(current_mode == Mode::CA) {
    if(row_to_update == 15)
      ca_frame.scrollY(-1);

    for(int i = 0; i < ROW_LENGTH; i++)
      ca_frame.setPixel(i, row_to_update, (current_cell_states[i]? GRAPHICS_ON : GRAPHICS_OFF));
    
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
  
    if(random(100) > data.pattern)
      dmd.copyFrame(ca_frame, 0, 0);
  
    if(counter >= data.cycles) {
      counter = 0;
      current_mode = Mode::NAME_STROBE;
      generateRandData();
    }
  } else {
    // TEXT STROBE
    if(random(100) < data.corruption_prob) {
      corrupt_frame(text_frame);
    }
    if(artist_counter == 0)
      text_frame.drawString(0, 1, " DJ MASDA", DMDGraphicsMode::GRAPHICS_XOR);
    if(artist_counter == 1)
      text_frame.drawString(0, 1, " HIRAKU", DMDGraphicsMode::GRAPHICS_XOR);
    if(artist_counter == 2)
      text_frame.drawString(0, 1, "PIERMATTEI", DMDGraphicsMode::GRAPHICS_XOR);
    if(artist_counter == 3)
      text_frame.drawString(0, 1, " ROND", DMDGraphicsMode::GRAPHICS_XOR);
    if(artist_counter == 4)
      text_frame.drawString(0, 1, " SNDCRFT", DMDGraphicsMode::GRAPHICS_XOR);
    if(artist_counter == 5)
      text_frame.drawString(0, 1, " KEVAN", DMDGraphicsMode::GRAPHICS_XOR);
    if(artist_counter == 6)
      text_frame.drawString(0, 1, " 2VIBES", DMDGraphicsMode::GRAPHICS_XOR);

    dmd.copyFrame(text_frame, 0, 0);
    delay(data.delay_time * 1.3);
    text_frame.clearScreen();
    dmd.copyFrame(text_frame, 0, 0);
    if(counter >= data.cycles / 3) {
      counter = 0;
      current_mode = Mode::CA;
      generateRandData();
      artist_counter++;
      artist_counter = artist_counter % 7;
    }
  }

  delay(data.delay_time);
}
