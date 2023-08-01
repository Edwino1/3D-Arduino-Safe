/*Program: EGR396ProjectSlaveMain
 * Author: Samuel Lobert and Edwin Odiakosa 
 * Date: 11/20/22
 * 
 * Purpose control the slave arduino nano for the final safe project 
 * 
 */
  //Nano SDA is A4 and SCL is A5
 //Ground each arduino to each other 

 # include <Wire.h>
 #include <Servo.h>
 #include <MFRC522.h>

  //code for rfid in slave 
  String myUID = ""; // The user identification number 
  #define SS_PIN 10 //Communication pin from master 
  #define RST_PIN 9 //Reset pin from master 
  MFRC522 myRFID(SS_PIN, RST_PIN);   // Create MFRC522 instance.
  // End of RFID

  int masterCase = 0; 
  int slaveCase = 0; 
  bool isOpen = false; 
  Servo myServo; 
  
 
 void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  Wire.begin(1); // begin w/ address 1 
  myRFID.PCD_Init(); //initiate the RFID reader

  pinMode(4, OUTPUT); // GREEN LED #1
  pinMode(2, OUTPUT); // GREEN LED #2

  pinMode(3, OUTPUT); //Servo pin 
  myServo.attach(3); 
  pinMode(A0, INPUT); // input pin for the door potentiometer

}

void loop() {
  // put your main code here, to run repeatedly:
  lookForMaster(); 

  switch (masterCase){
    case 0:
      //This should not happen something has gone wrong 
      Serial.println("masterCase is 0, This should not happen"); 
      break;
    case 1: 
      //The keypad is good turn first led on 
      //Swipe the rfid card 
      digitalWrite(4, HIGH); 
      readCard(); 
      break; 
    case 2: 
      //The door is unlocked, tell the master when the door is locked again. 
      isOpen = true; 
      digitalWrite(2, HIGH); //Turn on the other led 
      //Read the potentiometer 
      int potVal = analogRead(A0); // This is a number between 0 and 1023 

      int potDegrees = map(potVal, 0, 1023, 0, 180); //map the pot to a degree val for the servo
      if (isOpen && potVal > 0) {
         myServo.write(potDegrees);
         delay(10); 
          
      } else {
        isOpen = false; 
      }
     

      //if the potentiometer is turned back to zero then the door is relocked. 
      if (! isOpen) {
        //the door is locked 
        slaveCase = 6; 
        Wire.onRequest(request);
      }
      

      break; 
  }
  
}

void readCard() {
  //Look for the user to scan their card 

  if (! myRFID.PICC_IsNewCardPresent()) {
    return;
  }

  //read the available card 
  if (! myRFID.PICC_ReadCardSerial()); {
    return;
  }

 // Print the RFID card's Identification num to the serial
 String info = "";  
 

  Serial.print("UID tag: "); 
  for (byte i = 0; i < myRFID.uid.size; i++) {
    Serial.print(myRFID.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(myRFID.uid.uidByte[i], HEX); 
    info.concat(String(myRFID.uid.uidByte[i] < 0x10 ? " 0" : " "));
    info.concat(String(myRFID.uid.uidByte[i], HEX));
  }

  info.toUpperCase(); 
  //Allow access if the correct UID is read
  if (info.substring(1) == myUID) {
    //Do the next steps for access
        slaveCase = 4; 
        Wire.onRequest(request);
    Serial.println("Access Granted"); 
    delay(3000); 
  } else {
    //The card is denied
        slaveCase = 5; 
        Wire.onRequest(request); 
    Serial.println("Sorry Try Again"); 
    delay(1000); 
  }
}

void incomingInfo(){
  masterCase = Wire.read();
  Serial.println("Message Recieved!"); 
  Serial.println(masterCase); 
  //Wire.onRequest(request); this calls the function to send back to the master
  
}

void request() {
  Serial.print("The slave is sending: "); 
  Serial.println(slaveCase); 
  Wire.write(slaveCase); 
  Serial.println(slaveCase); 
}

void lookForMaster() {
   Wire.onReceive(incomingInfo); 
   delay(100); 
}
