#include <LovyanGFX.hpp>

#include "cat_32x24.h"
#include "cat_80x60.h"
#include "cat_160x120.h"
#include "cat_320x240.h"

static LGFX lcd;

static auto transpalette = 0;

#define DEF_CAT_FILE(str)    \
  {                          \
    cat0_##str##_width,      \
        cat0_##str##_height, \
        cat0_##str##_d,      \
        cat1_##str##_d,      \
        cat2_##str##_d,      \
        cat3_##str##_d,      \
        cat4_##str##_d,      \
        cat_sleep_##str##_d  \
  }

typedef struct
{
  const int width;
  const int height;
  const unsigned char *cat[6];
} CAT_FILE;

CAT_FILE cats[4] = {
    DEF_CAT_FILE(32x24),
    DEF_CAT_FILE(80x60),
    DEF_CAT_FILE(160x120),
    DEF_CAT_FILE(320x240),
};

#define CAT_WIDTH (cats[catSize].width)
#define CAT_HEIGHT (cats[catSize].height - 1)
#define CAT_IMAGE cats[catSize].cat

#define DELAY_TIME 50

void setup()
{
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);

  lcd.init();
  lcd.setRotation(1);
  lcd.setFont(&fonts::FreeMono9pt7b);
  lcd.fillScreen(lcd.color888(0, 0, 0));
}

#define INTERVAL_TIME 10000

void loop()
{
  unsigned long currTime;
  unsigned long drawTime;
  unsigned long stateTime;
  int catSize = 0;
  bool run = true;
  int catForm = 0;
  int waitTime = DELAY_TIME;
  int key = 0;

  currTime = drawTime = stateTime = millis();

  while (true)
  {
    int catSizeOld = catSize;

    while (true)
    {
      currTime = millis();
      if (drawTime > currTime || (currTime - drawTime) > waitTime)
      {
        break;
      }
      delay(1);

      if (stateTime > currTime || (currTime - stateTime) > INTERVAL_TIME)
      {
        run = !run;
        stateTime = currTime;
      }

      key = catSize * 10000 + waitTime;
      if (digitalRead(WIO_5S_LEFT) == LOW)
        catSize--;
      if (digitalRead(WIO_5S_RIGHT) == LOW)
        catSize++;
      if (digitalRead(WIO_5S_DOWN) == LOW)
        waitTime--;
      if (digitalRead(WIO_5S_UP) == LOW)
        waitTime++;

      catSize = min(3, max(0, catSize));
      waitTime = min(1000, max(0, waitTime));

      if (key != (catSize * 10000 + waitTime))
      {
        delay(100);
      }
    }

    if (catSizeOld != catSize)
    {
      lcd.fillScreen(0);
      delay(300);
    }

    int ofsX = (lcd.width() - CAT_WIDTH) / 2;
    int ofsY = (lcd.height() - CAT_HEIGHT) / 2;

    lcd.startWrite();
    lcd.drawBitmap(ofsX,
                   ofsY,
                   CAT_IMAGE[run ? catForm : 5],
                   CAT_WIDTH,
                   CAT_HEIGHT,
                   TFT_WHITE,
                   TFT_BLACK);
    lcd.endWrite();
    drawTime = millis();

    catForm = (catForm + 1) % 5;
  }
}
