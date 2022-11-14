// Author - wa1tf0rme
// The code initializes the serial on the 9600 baud rate, then prints the next baud rate, switches, and prints the part of the flag. The goal is to get a full flag without any gibberish.


void setup() {
  
  Serial.begin(9600); 

}

void loop() {
  int angleArray[8] = {1200,19200,2400,4800,38400,300,600,750};
  int w =angleArray[random(8)];
  Serial.println(w);
  delay(250);
  Serial.println("Can you catch up with my serial?)");
  Serial.println("");
  Serial.flush(); 
  Serial.begin(w);
  while(Serial.available()) Serial.read(); 
  w =angleArray[random(8)];
  Serial.println(w);
  delay(270);
  Serial.flush(); 
  Serial.begin(w);
  while(Serial.available()) Serial.read();
  Serial.println("HW_PWN{");        
  w =angleArray[random(8)];
  Serial.println(w);
  delay(270);
  Serial.flush(); 
  Serial.begin(w);
  while(Serial.available()) Serial.read();
  Serial.println("UAR7");
  w =angleArray[random(8)];
  Serial.println(w);
  delay(230);
  Serial.flush(); 
  Serial.begin(w);
  while(Serial.available()) Serial.read();
  Serial.println("_15_fas71_and");
  w =angleArray[random(8)];
  Serial.println(w);
  delay(500);
  Serial.flush(); 
  Serial.begin(w);
  while(Serial.available()) Serial.read();
  Serial.println("_unpr");
  w =angleArray[random(8)];
  Serial.println(w);
  delay(500);
  Serial.flush(); 
  Serial.begin(w);
  while(Serial.available()) Serial.read();
  Serial.println("3d1c7ab13}");
}
