// Import the Serial library
import processing.serial.*;

// Declare the Serial object
Serial myPort;

// Declare a string to store the data received from the serial port
String val;

// Declare a PrintWriter object to log events to a file
PrintWriter output;
String dateTime;

// Setup function runs once when the program starts
void setup() {
  // Initialize the Serial object with the appropriate port and baud rate
  myPort = new Serial(this, "COM3", 9600);
  
  // Create a new file named "log.txt" to log events to
  output = createWriter("log.txt");
  
  // Get the current time and format it
  dateTime = nf(day(), 2) + "/" + nf(month(), 2) + "/" + year() + " " + nf(hour(), 2) + ":" + nf(minute(), 2) + ":" + nf(second(), 2);
  
  // Print a message indicating when the program started to the console and log it to the file
  output.println("Program started at " + dateTime);
  
  // Flush the output to ensure that the message is written to the file
  output.flush();
  
  // Set the size of the window to 1000 x 800 pixels
  size(1000, 800);
  
  // Set the background color to white
  background(255, 255, 255);
  
  // Draw a rectangle for Zone 1
  rect(50, 50, 300, 600);
  
  // Set the text color to black and the size to 40
  fill(0);
  textSize(40);
  
  // Add the text "Zone 1" to the rectangle for Zone 1
  text("Zone 1", 150, 200);
  
  // Draw a rectangle for the entry/exit zone
  fill(255, 255, 255);
  rect(350, 50, 300, 600);
  
  // Add the text "Entry Exit" to the rectangle for the entry/exit zone
  fill(0);
  textSize(40);
  text("Entry Exit", 410, 200);
  
  // Draw a rectangle for Zone 2
  fill(255, 255, 255);
  rect(650, 50, 300, 600);
  
  // Add the text "Zone 2" to the rectangle for Zone 2
  fill(0);
  textSize(40);
  text("Zone 2", 750, 200);
}

// Draw function runs repeatedly, updating the display based on the data received from the serial port
void draw() {
  // Check if data is available from the serial port
  if (myPort.available() > 0) {
    // Read the data from the serial port and store it in the val variable
    val = myPort.readStringUntil('\n');
    
// Remove any leading or trailing whitespace from the val variable
val = trim(val);

// Check if val is not null
if (val != null) {

  // Print the value to the console
  println(val);

  // Check if the value equals "Main Door: Motion detected."
  if (val.equals("Main Door: Motion detected.")) {

    // Set the fill color to red and draw a rectangle
    fill(255, 0, 0);
    rect(350, 50, 300, 600);

    // Set the fill color to black and set the text size to 40
    fill(0);
    textSize(40);

    // Add text to the rectangle
    text("Entry Exit", 410, 200);

    // Log the event with a message
    logEvent("Entry Exit Zone sensor activated");
  }

  // Check if the value equals "Overheating!!!!"
  if (val.equals("Emergency Zone 2")) {

    // Set the fill color to red and draw a rectangle
    fill(255, 0, 0);
    rect(650, 50, 300, 600);

    // Set the fill color to black and set the text size to 40
    fill(0);
    textSize(40);

    // Add text to the rectangle
    text("Zone 2", 700, 200);

    // Log the event with a message
    logEvent("Zone 2 Detection");
  }

  // Check if the value equals "Zone 1 Detection"
  if (val.equals("Zone 1 Detection")) {

    // Set the fill color to red and draw a rectangle
    fill(255, 0, 0);
    rect(50, 50, 300, 600);

    // Set the fill color to black and set the text size to 40
    fill(0);
    textSize(40);

    // Add text to the rectangle
    text("Zone 1", 150, 200);

    // Log the event with a message
    logEvent("Zone 1 Detection");
  }

  // Check if the value equals "Alarm Disarmed"
  if (val.equals("Alarm Disarmed")) {

    // Set the fill color to white and draw three rectangles
    fill(255, 255, 255);
    rect(650, 50, 300, 600);
    rect(50, 50, 300, 600);
    rect(350, 50, 300, 600);

    // Set the fill color to black and set the text size to 40
    fill(0);
    textSize(40);

    // Add text to the rectangles
    text("Zone 1", 150, 200);
    text("Entry Exit", 410, 200);
    text("Zone 2", 700, 200);
      }
    } 
  }
}

// Function to log an event with a timestamp
void logEvent(String event){

  // Log the event with its timestamp
  println(dateTime + "ms - " + event);
  output.println(dateTime + "ms - " + event);
  output.flush();
} 
