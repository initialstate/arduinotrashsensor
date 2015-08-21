// Process.h gives us access to the Process class, which lets
// us communicate with the shell
#include <Bridge.h>
#include <Process.h>
#include <Console.h>
#include "Ultrasonic.h"

// Initialize Ultrasonic Ranger pin
Ultrasonic ultrasonic(7);
// Distance to trash before you send a text alert
const int dist = 4;

////////////////////////////
// Initial State Streamer //
////////////////////////////

// URL to IS Bucket API
String ISBucketURL = "https://groker.initialstate.com/api/buckets";
// URL to IS Event API
String ISEventURL = "https://groker.initialstate.com/api/events";
// Access key (the one you find in your account settings):
String accessKey = "Your_Access_Key_Here";
// Bucket key (hidden reference to your bucket that allows appending):
String bucketKey = "yun_trash";
// Bucket name (name your data will be associated with in Initial State):
String bucketName = ":put_litter_in_its_place: Trash Sensor";
// How many signals are in your stream? You can have as few or as many as you want
const int NUM_SIGNALS = 2;
// What are the names of your signals (i.e. "Temperature", "Humidity", etc.)
String signalName[NUM_SIGNALS] = {"Dist (in)", "Empty trash?"};
// This array is to store our signal data later
String signalData[NUM_SIGNALS];


// This only runs once at the very beginning
void setup()  
{ 
  Bridge.begin();
  Console.begin();

  while (!Console);
  
  // Post Bucket
  Console.println("Posting Bucket!");
  // The postBucket() function creates a bucket 
  // (unnecessary if the bucket already exists)
  postBucket();

  // Wait 2 seconds for the sensors to initialize
  delay(2000);

  // Take the range and post the data once before the loop with a 15 minute wait begins
  // Posting twice makes sure we don't accidentally miss 
  // the first messages on the Console monitor
  takeRange();
  postData();
}

// This repeats
void loop() 
{
  // Wait 2 seconds
  delay(2000);

  Console.println("Taking Range!");
  // Gather Data
  takeRange();

  // Post Data
  Console.println("Posting Data!");
  // The postData() function streams our events
  postData(); 

  // Wait for 894 seconds before collecting and sending the next batch
  // This makes total time between values 15 minutes
  delay(894000);
}



// Here are the data bucket creation and posting functions
// They do not need to be edited - everything you would need to change for 
// your situation can be found above

void takeRange ()
{
  // Take the range in inches
  // Replace "Inches" with "Centimeters" for range in cm
  long RangeInInches = ultrasonic.MeasureInInches();

  signalData[0] = String(RangeInInches);

  // If the range is less than our allowable distance, send a text
  if (RangeInInches < dist)
  {
    signalData[1] = String(":weary: Yes");
  }
  else
  {
    signalData[1] = String(":smile: No");
  }
}

void postBucket()
{
  // Initialize the process
  Process isbucket;

  isbucket.begin("curl");
  isbucket.addParameter("-k");
  isbucket.addParameter("-v");
  isbucket.addParameter("-X");
  isbucket.addParameter("POST");
  isbucket.addParameter("-H");
  isbucket.addParameter("Content-Type:application/json");
  isbucket.addParameter("-H");
  isbucket.addParameter("Accept-Version:0.0.1");

  // IS Access Key Header
  isbucket.addParameter("-H");
  isbucket.addParameter("X-IS-AccessKey:" + accessKey);

  // IS Bucket Key Header
  isbucket.addParameter("-d");
  isbucket.addParameter("{\"bucketKey\": \"" + bucketKey + "\", \"bucketName\": \"" + bucketName + "\"}");
  
  isbucket.addParameter("https://groker.initialstate.com/api/buckets");
  
  // Run the process
  isbucket.run();

}
  
void postData()
{
  for (int i=0; i<NUM_SIGNALS; i++)
  {
    // Initialize the process
    Process isstreamer;
    
    isstreamer.begin("curl");
    isstreamer.addParameter("-k");
    isstreamer.addParameter("-v");
    isstreamer.addParameter("-X");
    isstreamer.addParameter("POST");
    isstreamer.addParameter("-H");
    isstreamer.addParameter("Content-Type:application/json");
    isstreamer.addParameter("-H");
    isstreamer.addParameter("Accept-Version:0.0.1");

    // IS Access Key Header
    isstreamer.addParameter("-H");
    isstreamer.addParameter("X-IS-AccessKey:" + accessKey);

    // IS Bucket Key Header
    // Note that bucketName is not needed here
    isstreamer.addParameter("-H");
    isstreamer.addParameter("X-IS-BucketKey:" + bucketKey);

    isstreamer.addParameter("-d");

    // Initialize a string to hold our signal data
    String jsonData;

    jsonData = "[";
    jsonData += "{\"key\": \"" + signalName[i] + "\", \"value\": \"" + signalData[i] + "\"}";
    jsonData += "]";

    isstreamer.addParameter(jsonData);

    isstreamer.addParameter("https://groker.initialstate.com/api/events");

    // Print posted data for debug
    Console.print("Sending event stream #");
    Console.print(i);
    Console.println(jsonData);

    // Run the process
    isstreamer.run();

    delay(2000);
  }
}
