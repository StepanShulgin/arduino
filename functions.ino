void modesTick() {
  button.tick();
  boolean changeFlag = false;
  if (button.isClick()) {
    mode++;
    if (mode > 8) mode = 0;
    changeFlag = true;
  }
  if (button.isHolded()) {
    mode = 0;
    changeFlag = true;
  }

  if (changeFlag) {
    if (mode == 0) {
      lcd.clear();
      loadClock();
      drawClock(hrs, mins, 0, 0, 1);
      drawData();
      drawSensors();
    } else {
      lcd.clear();
      loadPlot();
      redrawPlot();
    }
  }
}

void redrawPlot() {
  lcd.clear();
  switch (mode) {
    case 1: drawPlot(0, 3, 15, 4, TEMP_MIN, TEMP_MAX, (int*)tempHour, "t hr");
      break;
    case 2: drawPlot(0, 3, 15, 4, TEMP_MIN, TEMP_MAX, (int*)tempWeek, "t Wk");
      break;
    case 3: drawPlot(0, 3, 15, 4, HUM_MIN, HUM_MAX, (int*)humHour, "h hr");
      break;
    case 4: drawPlot(0, 3, 15, 4, HUM_MIN, HUM_MAX, (int*)humWeek, "h Wk");
      break;
    case 5: drawPlot(0, 3, 15, 4, PRESS_MIN, PRESS_MAX, (int*)pressHour, "p hr");
      break;
    case 6: drawPlot(0, 3, 15, 4, PRESS_MIN, PRESS_MAX, (int*)pressWeek, "p Wk");
      break;
    case 7: drawPlot(0, 3, 15, 4, CO2_MIN, CO2_MAX, (int*)co2Hour, "c hr");
      break;
    case 8: drawPlot(0, 3, 15, 4, CO2_MIN, CO2_MAX, (int*)co2Week, "c Wk");
      break;
  }
}

void readSensors() {
  bme.takeForcedMeasurement();
  dispTemp = bme.readTemperature();
  dispHum = bme.readHumidity();
  dispPres = (float)bme.readPressure() * 0.00750062;
  dispCO2 = mhz19.getPPM();
}

void drawSensors() {
  lcd.setCursor(0, 2);
  lcd.print(String(dispTemp, 1));
  lcd.write(223);
  lcd.setCursor(6, 2);
  lcd.print(" " + String(dispHum) + "%  ");

  lcd.print(String(dispCO2) + " ppm");
  if (dispCO2 < 1000) lcd.print(" ");

  lcd.setCursor(0, 3);
  lcd.print(String(dispPres) + " mm  rain ");
  lcd.print(F("       "));
  lcd.setCursor(13, 3);
}

void plotSensorsTick() {
  // 4 минутный таймер
  if (hourPlotTimer.isReady()) {
    for (byte i = 0; i < 14; i++) {
      tempHour[i] = tempHour[i + 1];
      humHour[i] = humHour[i + 1];
      pressHour[i] = pressHour[i + 1];
      co2Hour[i] = co2Hour[i + 1];
    }
    tempHour[14] = dispTemp;
    humHour[14] = dispHum;
    co2Hour[14] = dispCO2;
  
    else pressHour[14] = dispPres;
  }

  // 11.2 часовой таймер
  if (weekPlotTimer.isReady()) {
    long averTemp = 0, averHum = 0, averPress = 0, averCO2 = 0;

    for (byte i = 0; i < 15; i++) {
      averTemp += tempHour[i];
      averHum += humHour[i];
      averPress += pressHour[i];
      averCO2 += co2Hour[i];
    }
    averTemp /= 15;
    averHum /= 15;
    averPress /= 15;
    averCO2 /= 15;

    for (byte i = 0; i < 14; i++) {
      tempWeek[i] = tempWeek[i + 1];
      humWeek[i] = humWeek[i + 1];
      pressWeek[i] = pressWeek[i + 1];
      co2Week[i] = co2Weeky[i + 1];
    }
    tempWeek[14] = averTemp;
    humWeek[14] = averHum;
    pressWeek[14] = averPress;
    co2Week[14] = averCO2;
  }
}

boolean dotFlag;
void clockTick() {
  dotFlag = !dotFlag;
  if (dotFlag) {          // каждую секунду пересчёт времени
    secs++;
    if (secs > 59) {      // каждую минуту
      secs = 0;
      mins++;
      if (mins <= 59 && mode == 0) drawClock(hrs, mins, 0, 0, 1);
    }
    if (mins > 59) {      // каждый час
      now = rtc.now();
      secs = now.second();
      mins = now.minute();
      hrs = now.hour();
      if (mode == 0) drawClock(hrs, mins, 0, 0, 1);
      if (hrs > 23) {
        hrs = 0;
      }
      if (mode == 0) drawData();
    }
    
  }
  if (mode == 0) drawdots(7, 0, dotFlag);
}
