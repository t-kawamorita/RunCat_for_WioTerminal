#include <LovyanGFX.hpp>

#include "cat_32x24.h"
#include "cat_80x60.h"
#include "cat_160x120.h"
#include "cat_320x240.h"

static LGFX lcd;
static LGFX_Sprite canvas(&lcd);
static LGFX_Sprite cat0(&canvas);

static auto transpalette = 0;

#define DEF_CAT_FILE(str) { cat0_##str##_width, \
                            cat0_##str##_height, \
                            cat0_##str##_d, \
                            cat1_##str##_d, \
                            cat2_##str##_d, \
                            cat3_##str##_d, \
                            cat4_##str##_d, \
                            cat_sleep_##str##_d }

typedef struct {
  const int width;
  const int height;
  const unsigned char* cat[6];
} CAT_FILE;

CAT_FILE cats[4] = {
  DEF_CAT_FILE(32x24) ,
  DEF_CAT_FILE(80x60) ,
  DEF_CAT_FILE(160x120) ,
  DEF_CAT_FILE(320x240) ,
};

int cursor = 0;

#define CAT_WIDTH (cats[cursor].width)
#define CAT_HEIGHT (cats[cursor].height)
#define CAT_IMAGE cats[cursor].cat

void setup() {
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);

  lcd.init();
  lcd.setRotation(1);
  lcd.setFont(&fonts::FreeMono9pt7b);
  lcd.fillScreen(lcd.color888(0,0,0));

  canvas.setColorDepth(lgfx::palette_4bit);
  canvas.createSprite(lcd.width(), lcd.height());
  canvas.fillScreen(transpalette);
  cat0.setColorDepth(lgfx::palette_4bit);  
  cat0.createSprite(CAT_WIDTH, CAT_HEIGHT);
}



void drawCat(unsigned int n)
{
  unsigned char* p;
  if ( n <= 4 ) {
    p = (unsigned char*)CAT_IMAGE[n];
  } else {
    p = (unsigned char*)CAT_IMAGE[5];
  }

  for (int y=0; y<CAT_HEIGHT; y++) {
    for (int x=0; x<CAT_WIDTH; ) {
      unsigned char bits = *p++;
      for ( int b=0; b<8; b++) {
        if ( bits & 0x80 ) {
          cat0.drawPixel( x, y, 15 );
        } else {
          cat0.drawPixel( x, y, transpalette);
        }
        bits <<=1;
        x++;
      }
    }
  }
}

void loop() {
  int run = 0;
  int limit = 100;
  int c = 0; 
  
  while ( true ) {
    int ocursor = cursor;
    if ( digitalRead(WIO_5S_LEFT) == LOW ) {
      cursor = cursor > 0 ? cursor-1 : cursor;
    }
    if ( digitalRead(WIO_5S_RIGHT) == LOW ) {
      cursor = cursor < 3 ? cursor+1 : cursor;
    }
    if ( ocursor != cursor ) {
      cat0.deleteSprite();
      cat0.createSprite(CAT_WIDTH, CAT_HEIGHT);
      canvas.fillScreen(transpalette);
      delay(300);
    }

    int ofsX = (lcd.width() - CAT_WIDTH) / 2;
    int ofsY = (lcd.height() - CAT_HEIGHT) / 2;

    drawCat( run ? c : 5 );
    lcd.startWrite();
    canvas.pushSprite(0,0);
    cat0.pushSprite(ofsX,ofsY);
    lcd.endWrite();

    if ( limit == 0 ) {
      run = 1 - run;
      limit = 100;
    }
    c = (c+1) % 5;
    limit--;
  }
}
