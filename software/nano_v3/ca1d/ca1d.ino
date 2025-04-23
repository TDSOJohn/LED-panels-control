#include <DMD2.h>
#include <SPI.h>

#include <DMXSerial.h>


#define DISPLAYS_WIDE 2
#define DISPLAYS_HIGH 1

SoftDMD dmd(DISPLAYS_WIDE, DISPLAYS_HIGH);
DMDFrame frame(64, 16);


#define ROW_LENGTH 64

enum RenderState {
  Play,
  Pause,
  Stop,
};

RenderState renderState;

uint8_t CURRENT_RULE = 30;
uint8_t DATA[ROW_LENGTH * 2];

uint8_t* current_cell_states;
uint8_t* next_cell_states;
uint8_t* temp;

int startChannel = 1;

int speed = 255;


int stateBasedOnNeighbors(int rule, int left, int center, int right) {
  return(rule >> (left << 2 | center << 1 | right)) & 1;
}

void setStartingValues(bool rand_start) {
  renderState = Pause;
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
  DMXSerial.init(DMXReceiver);

  dmd.setBrightness(255);
  dmd.begin();

  current_cell_states = DATA;
  next_cell_states = DATA + ROW_LENGTH;

  current_cell_states[ROW_LENGTH / 2] = 1;

  DMXSerial.write(startChannel, 45);
  DMXSerial.write(startChannel + 1, 0);
  DMXSerial.write(startChannel + 2, 0);

  renderState = Play;
}

uint8_t row_to_update = 0;

uint8_t restart_timer = 0;

void loop() {
  speed = 1320 - (DMXSerial.read(startChannel) + 5) * 5;
  uint8_t ch2_read = DMXSerial.read(startChannel + 1);
  if(ch2_read < 64)
    CURRENT_RULE = 30;
  else if(ch2_read >= 64 && ch2_read < 128)
    CURRENT_RULE = 57;
  else if(ch2_read >= 128 && ch2_read < 192)
    CURRENT_RULE = 73;
  else
    CURRENT_RULE = 90;
  
  uint8_t ch3_read = DMXSerial.read(startChannel + 2);

  if((ch3_read > 225) && (ch3_read <= 240))
    setStartingValues(true);
  else if(ch3_read > 240)
    setStartingValues(false);

  if(renderState == Pause) {
    if(ch3_read == 0)
      renderState = Play;
  }

  if(renderState == Play) {
    if(row_to_update == 15)
      frame.scrollY(-1);

    for(int i = 0; i < ROW_LENGTH; i++)
      frame.setPixel(i, row_to_update, (current_cell_states[i]? GRAPHICS_ON : GRAPHICS_OFF));
    
    for(int i = 0; i < ROW_LENGTH; i++) {
      int left = current_cell_states[((i - 1) + ROW_LENGTH) % ROW_LENGTH];
      int curr = current_cell_states[i];
      int right = current_cell_states[(i + 1) % ROW_LENGTH];
      
      next_cell_states[i] = stateBasedOnNeighbors(CURRENT_RULE, left, curr, right);
    }

    dmd.copyFrame(frame, 0, 0);

    temp = current_cell_states;
    current_cell_states = next_cell_states;
    next_cell_states = temp;

    if(row_to_update < 15)
      row_to_update++;

    delay(speed);
  }
}
