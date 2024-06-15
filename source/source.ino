#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define WATERBTN 2
#define MODEBTN 3
#define MOIST A1
#define VALVE 5

bool state = true;
bool prevWaterBtn = HIGH;
bool prevModeBtn = HIGH;
float currentTemp, currentHumid;
int currentGround;
short watering = 0;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup(){
    pinMode(VALVE, OUTPUT);
    pinMode(WATERBTN, INPUT_PULLUP);
    pinMode(MODEBTN, INPUT_PULLUP);

    lcd.begin();
    dht.begin();
    Serial.begin(9600);
}

void loop(){
    unsigned long start = millis();
    while(millis() - start < 1000){
        bool modeTrig = digitalRead(MODEBTN);        // check display mode button
        if (modeTrig == LOW && prevModeBtn == HIGH){
                prevModeBtn = LOW;
                state = !state;                     // change display mode
                displayMode(currentTemp, currentHumid, currentGround);
        }
        else if (modeTrig == HIGH && prevModeBtn == LOW) {
                prevModeBtn = HIGH;
        }

        bool waterTrig = digitalRead(WATERBTN);     // check water button
        if(waterTrig == LOW && prevWaterBtn == HIGH && watering == 0){
            prevWaterBtn = LOW;
            waterPlant();                       // water the plant
        }
        else if(waterTrig == HIGH && prevWaterBtn == LOW){
            prevWaterBtn = HIGH;
        }
        sendData(currentTemp, currentHumid, watering); 
        if(Serial.available() && Serial.read() && watering == 0) waterPlant();
        while(currentGround < 15 && watering == 0) waterPlant();
    }
    currentTemp = dht.readTemperature();     // get datas to show/send
    currentHumid = dht.readHumidity();
    currentGround = getGroundMoist();

    displayMode(currentTemp, currentHumid, currentGround);       // show datas w/ lcd
}

void waterPlant(){                          // water plant
    watering = 1;
    lcd.clear();
    for(int i=0; i<5; i++){
        currentTemp = dht.readTemperature();     // get datas to show/send
        currentHumid = dht.readHumidity();
        currentGround = getGroundMoist();
        lcd.setCursor(0, 0);
        lcd.print("Watering...");
        lcd.setCursor(0, 1);
        lcd.print("Moist :         ");           // clear 2nd row
        lcd.setCursor(0, 1);
        lcd.print("Moist : ");
        lcd.print(currentGround);
        lcd.print("%");
        unsigned long start = millis();
        while(millis() - start < 1000){
            digitalWrite(VALVE, HIGH);
            sendData(currentTemp, currentHumid, watering);
        }
    }
    digitalWrite(VALVE, LOW);
    watering = 0;
}

int getGroundMoist(){                       // return ground moisture (0~100)
    int value = analogRead(MOIST);
    int mapped = map(value, 0, 1023, 0, 100);
    return mapped;
}

void displayMode(float temp, float humid, int ground){      // display data
    if(state){
        lcd.setCursor(0, 0);
        lcd.print("Temp(C) : ");
        lcd.print(temp);
        lcd.setCursor(0, 1);
        lcd.print("Humid(%) : ");
        lcd.print(humid);
    }
    else{
        lcd.setCursor(0, 0);
        lcd.print("Ground Moist(%) ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(ground);
        lcd.print("%");
    }
}

void sendData(float temp, float humid, short isWatering){     // send data
    char buffer[10];                        // create buffer to make format
    char temp1[5], temp2[5];                // var to store data as str
    dtostrf(temp, 4, 1, temp1);             // change datatype float -> str
    dtostrf(humid, 4, 1, temp2);
    sprintf(buffer, "%s C,%s %%,%d!", temp1, temp2, isWatering); // build format
    Serial.println(buffer);                 // send data via serial
}