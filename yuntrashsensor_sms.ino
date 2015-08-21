// Process.h gives us access to the Process class, which lets
// us communicate with the shell
#include <Bridge.h>
#include <Process.h>
#include "Ultrasonic.h"

Ultrasonic ultrasonic(7);
// Distance to trash before you send a text alert
const int dist = 4;

int textsent = 0;

////////////////////////////
//         Twilio         //
////////////////////////////

//Authentification for the Twilio
String accountSid = "Account_SID_Here";
String authToken = "Auth_Token_Here";
//the number should be formatted like +12345678912
String sendNumber = "Number_Sending_To";
String fromNumber = "Twilio_Number_Here";
//Twilio API URL
String TwilioURL = "https://api.twilio.com/2010-04-01/Accounts/";


// This only runs once at the very beginning
void setup()  {
  Bridge.begin();
  Serial.begin(9600);

  while (!Serial);

  delay(2000);
}

// This repeats
void loop() 
{
  Serial.println("Taking Range!");
  // Gather Data
  takeRange();

  delay(5000);
}



// Here are the data bucket creation and posting functions
// They do not need to be edited - everything you would need to change for 
// your situation can be found above

void takeRange ()
{
  // Take the range in inches
  // Replace "Inches" with "Centimeters" for range in cm
  long RangeInInches = ultrasonic.MeasureInInches();

  Serial.print("Distance: ");
  Serial.println(RangeInInches);

  if (RangeInInches < dist)
  {
    Serial.println("Take out the trash!");
    if (textsent == 0)
    {
    // This function will send the sms
    // The first argument is the number to send to
    // The second argument is the body of the text message, which must be within URLEncode()
    Serial.println("Sending SMS");
    sendSMS(sendNumber, URLEncode("The trash is full"));
    textsent = 1;
    }
  }
  else
  {
    textsent = 0;
  }
}

void sendSMS(String number,String message) 
{
  // Initialize the process
  Process sendsms;

  String sms;

  sms = "curl";
  sms += " ";
  sms += "-k";
  sms += " ";
  sms+= "-XPOST";
  sms += " ";
  sms += TwilioURL + accountSid + "/Messages.json";
  sms += " ";
  sms += "-d";
  sms += " ";
  sms += "\"Body=" + message + "\"";
  sms += " ";
  sms += "-d";
  sms += " ";
  sms += "\"To=" + sendNumber + "\"";
  sms += " ";
  sms += "-d";
  sms += " ";
  sms += "\"From=" + fromNumber + "\"";
  sms += " ";
  sms += "-u";
  sms += " ";
  sms += "\'" + accountSid + ":" + authToken + "\'";

  Serial.println(sms);
  sendsms.runShellCommand(sms);
  while(sendsms.running());

  Serial.println("SMS sent");
  Serial.flush();
} 

String URLEncode(const char* msg)
{
  const char *hex = "0123456789abcdef";
  String encodedMsg = "";

  while (*msg!='\0'){
    if( ('a' <= *msg && *msg <= 'z')
      || ('A' <= *msg && *msg <= 'Z')
      || ('0' <= *msg && *msg <= '9') ) {
      encodedMsg += *msg;
    } 
    else {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 15];
    }
    msg++;
  }
  return encodedMsg;
}
