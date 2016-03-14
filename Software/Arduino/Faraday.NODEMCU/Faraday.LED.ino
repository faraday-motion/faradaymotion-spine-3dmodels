#if defined(ENABLELED)

void setupLeds()
{
  leds.init(LEDSTOTAL);
}

void showLedState()
{
  if (isControllerEnabled() == false)
  {
    if (controlDead == false)
      ledReadyState();
    else
      ledDeadState();
  }
  showLedLights();
}

void showLedLights()
{
  //Front lights
  for (int i = LEDSFRONTINDEX; i < LEDSFRONTINDEX + LEDSFRONTCOUNT; i++) {
    setPixelColor(i, 100, 100, 100);
  }
  for (int i = LEDSBACKINDEX; i < LEDSBACKINDEX + LEDSBACKCOUNT; i++) {
    //If braking, put brake lights
    if (motorDirection == 2)
      setPixelColor(i, 255, 0, 0);
    else
      setPixelColor(i, 100, 0, 0);
  }
}


void setLedControls()
{
  if (isControllerEnabled())
  {
    int pixelCountTarget = map(motorTargetPercent, 0, 100, 0, LEDSCONTROLCOUNT);
    int pixelCountActual = map(motorPercent, 0, 100, 0, LEDSCONTROLCOUNT);
    for (int i = LEDSCONTROLINDEX; i < LEDSCONTROLINDEX + LEDSCONTROLCOUNT; i++) {
      setPixelColor(i, 0, 0, 0);
      yield();
    }

    //Set the led 0 for all states
    if (motorDirection == 0) {
      setPixelColor(LEDSCONTROLINDEX, 150, 150, 0);
    }
    else if (motorDirection == 1) {
      setPixelColor(LEDSCONTROLINDEX, 0, 200, 0);
    }
    else if (motorDirection == 2) {
      setPixelColor(LEDSCONTROLINDEX, 200, 0, 0);
    }

    //Going forward
    if (motorDirection == 1)
      for (int i = LEDSCONTROLINDEX + 1; i < LEDSCONTROLINDEX + LEDSCONTROLCOUNT; i++) {
        if (i < pixelCountActual) {
          setPixelColor(i, 0, 200, 0);
        }
        else if (i < pixelCountTarget) {
          setPixelColor(i, 0, 100, 0);
        }
        yield();
      }
    else if (motorDirection == 2) {
      //Braking
      for (int i = LEDSCONTROLINDEX + 1; i < LEDSCONTROLINDEX + LEDSCONTROLCOUNT; i++) {
        if (LEDSCONTROLCOUNT - i < pixelCountActual) {
          setPixelColor(i, 200, 0, 0);
        }
        else if (LEDSCONTROLCOUNT - i < pixelCountTarget) {
          setPixelColor(i, 100, 0, 0);
        }
        yield();
      }
    }
  }
}
  byte readyColorCounter = 0;
  byte readyColor = 1;
  byte readyColorStep = 3;
  const byte readyColorMin = 0;
  const byte readyColorMax = 90;

  void ledReadyState()
  {
    for (int i = LEDSCONTROLINDEX; i < LEDSCONTROLCOUNT; i++) {
      pixels[i] = Wheel(readyColorCounter);
    }
    if (readyColorCounter <= readyColorMin)
      readyColor = +readyColorStep;
    else if (readyColorCounter >= readyColorMax)
      readyColor = -readyColorStep;
    readyColorCounter += readyColor;
  }

  void ledDeadState()
  {
    uint8_t i;
    static uint8_t c = 0;
    static uint8_t d = 5;
    for (i = LEDSCONTROLINDEX; i < LEDSCONTROLCOUNT; i++)
      setPixelColor(i, c, 0, 0);
    c += d;
    if ( (c >= 255) || (c <= 0) ) d = -d;
  }

  void setPixelColor(byte index, byte r, byte g, byte b)
  {
    pixels[index].R = r;
    pixels[index].G = g;
    pixels[index].B = b;
  }

  Pixel_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    Pixel_t p;
    if (WheelPos < 85) {
      p.R = 255 - WheelPos * 3;
      p.G = 0;
      p.B = WheelPos * 3;
      return p;
    } else if (WheelPos < 170) {
      WheelPos -= 85;
      p.R = 0;
      p.G = WheelPos * 3;
      p.B = 255 - WheelPos * 3;
      return p;
    } else {
      WheelPos -= 170;
      p.R = WheelPos * 3;
      p.G = 255 - WheelPos * 3;
      p.B = 0;
      return p;
    }
  }

#endif

