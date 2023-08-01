/*Program EGR 396 Project Main V1
 * Authors: Samuel Lobert and Edwin Odiakosa
 * Purpose: Create code to replicate the functionality of a safe deposit box
 */

// inlcude the needed libraries 
# include <SPI.h>
#include <MFRC522.h> 
# include <Keypad.h> 
# include <Wire.h>
#include <LiquidCrystal.h>

//Global variables
//Variables for Keypad code 
//4X4 Keypad 
char matrix[3][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},

  };
 const byte ROWS = 3;
 const byte COLS = 3;
 char password[4] = "4444"; // THIS IS WHERE WE SET YOUR 4 DIGIT PASSWORD
 String pass = "3333";
 byte data_count = 0, master_count = 0;
 bool Pass_is_good;
 char Data[5]; //5 digit array for holding the input data 

 byte rowPins[ROWS] = {10,9,8}; // connect the row pins on the keypad to these pins in order. NOTE This order may change 
 byte colPins[COLS] = {7, 6, 13}; // connect the column pins on the keypad to these pins in order NOTE This order may change 
 Keypad myKeypad = Keypad(makeKeymap(matrix), rowPins, colPins, ROWS, COLS); //initialize the keypad
//// //END of Keypad 

 // initialize the library by connecting any needed LCD interface pin with the arduino pin numbers 

 // initialize the library by connecting any needed LCD interface pin with the arduino pin numbers 
  const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

   
  bool val = false;
  int recFromSlave = 0;
  int p = 1; 
  int w = 1; 
  int y = 1; 
  
  
void setup() 
{
    lcd.begin(16, 2);  // set up the LCD's number of columns and rows:
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Hello!"); // Print a message to the LCD.
    delay(2000);
    Serial.begin(9600); 
     
    Wire.begin(); 
    Serial.println("I am Here");

    
    
   
    
}

void loop() 
{

    
    while (w == 1) {
      callKeypad(); }

  
  while (y == 1) {
   Serial.println("Back in the loop!"); 
    delay(2000); 
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("Scan RFID"); 
    reciveFromSlave(); 
    if (recFromSlave == 4) {
      Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      lcd.clear();
      lcd.setCursor(0,0); 
      lcd.print("RFID Accepted");
      y++;
      break;
    }
  }
    
   switch(recFromSlave) {
    case 0:
      //This should not be zero at this point error 
      Serial.println("recFromSlave is 0 after call, error occurs"); 
      callKeypad(); //Take the program to start if error happens 
      break; 
    case 4: //the rfid is accepted 
      lcd.clear(); 
      lcd.setCursor(0,0); 
      lcd.print("ACCEPTED!"); 
      delay(2000); 
      callSlave(2); // tell the slave case 2 which is potentiometer and servo
      recFromSlave = 9;
      break;
    case 5: 
      callSlave(1); //tell slave to repeat the rfid request 
       lcd.clear(); 
      lcd.setCursor(0,0); 
      lcd.print("DENIED!");
      lcd.setCursor(0,1); 
      lcd.print("Try Again"); 
      callSlave(1); 
      recFromSlave = 9;
      break; 
   }
//
//   //If you reach this point it means the rfid and the keypad are cleared
//   // prompt user to turn the handle
//   //wait for the slave to say the door is relocked 
//
//   reciveFromSlave(); //
//   if (recFromSlave == 6) {
//    //The door is now relocked 
//    callKeypad(); 
//   } else {
//    callSlave(1); // swipe the rfid again 
//   }
//  delay(1000); 
//   

}

//Keypad function 
void callKeypad() {
  // This should probably be in a while loop untill a condition is met where we have the correct combo
   lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Input Code"); 
    lcd.setCursor(0,1); //Second line display 

  while (p ==1) {
   char keyVal = myKeypad.getKey(); //read the keypad
  
  if (keyVal) { 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Input Code"); 
    lcd.setCursor(0,1); //Second line display 
     
    Data[data_count] = keyVal; //Keep track of the last 4 keys pressed 
    data_count++; 

    Serial.print("The dataCount is: ");
    Serial.println(data_count);
   lcd.print(Data);
   Serial.println((String)Data); 
  }
 int j = -1;
  if (data_count == 4) { // When the 4th key is pressed check and see if the password works 
      
        if(((String)Data == pass)) //The password works then turn on the green LED and print correct 
          {
              delay(1000);
              clearData();
              Serial.println("Correct!!!");
              lcd.clear(); 
              lcd.setCursor(0,0);
              lcd.print("Correct!!!"); 
              val = true; 
              
              p = 7; 
              w = 5; 
              
              callSlave(7); // Tell the slave case 7 which is the case for turn the 1st led green 
              delay(50);
              Serial.println("Call_Slave should have been sent");
              break;
             } else {
              Serial.println("NOT CORRECT");
             lcd.clear(); 
             lcd.setCursor(0,0);
             lcd.print("Incorrect");
            delay(1000);
            clearData(); // clear the array so the next 4 digit combo can be entered 
            callKeypad(); //I think this should go here 
             }
      
  }
  }
  
  Serial.println("This should never happen"); //flag this doesn't work like I'm thinking 
  reciveFromSlave(); 
  delay(50);
}

void clearData(){ //Clear the data for the kaypad array 
  while(data_count !=0){
    Data[data_count--] = 0; 
    
  }
  return;
}


int callSlave (int caseVal) {
  Serial.println("You have reached call slave");
  Wire.beginTransmission(8);
  Wire.write(caseVal); 
  delay(500);
  Wire.endTransmission(); 
  Serial.println("Message Sent!     :"); 
  Serial.println(caseVal);
  delay(1000);

}

void reciveFromSlave() {
  while(recFromSlave == 0) {
  Serial.println("I'm at recieve from slave"); 
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Scan RFID");
  Wire.requestFrom(8, 1);    // Request 6 bytes from slave device number two

    // Slave may send less than requested
    while(Wire.available()) {
        recFromSlave = Wire.read();    // Receive a byte as character
        Serial.print(recFromSlave);         // Print the character
    }

    delay(50); 
  }
}
