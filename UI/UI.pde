import processing.serial.*;
Serial mingi;
String msg;
String temp = "00.0 C";
String humid = "00.0 %";
String wateringRaw = "0!";
int isWatering = 0;
String[] data = new String[3];

void setup(){
  size(375,500);
  String portName="COM3";
  mingi=new Serial(this,portName,9600);
}

void draw(){
  String[] tempData = split(humid, "%");
  float floatHumid = float(tempData[0]);
  String[] waterTemp = split(wateringRaw, "!");
  isWatering = int(waterTemp[0]);
  background(162, 100 + 1 * int(floatHumid), 100 + 2 * int(floatHumid)); //초록의 값을 낯추면 진한 파랑이 됨.
  noStroke();
  fill(255);
  ellipse(175,385,100,100);
  
  textSize(20);
  text("Temperature", 25,120);
  text("Humid", 25,250);
  
  if(mingi.available() > 0){
    msg = mingi.readStringUntil('!');
    if(msg != null){
      print(temp + ", " + humid + ", " + (isWatering == 1 ? "Pump ON" : "Pump OFF"));
      data = split(msg, ',');
      temp = data[0];
      humid = data[1];
      wateringRaw = data[2];
    }
  }
    
  textSize(70);
  text(temp,150,50);
  text(humid,150, 260);
  
  textSize(50);
  fill(162,235,246);
  textSize(25);
  text("Water it!",132,395);
}

void mousePressed(){
  if(175-50<=mouseX && mouseX<=175+50 && 385-50<=mouseY && mouseY<=385+50){
    if(isWatering == 0){
      mingi.write("9");
    }
  }
}
