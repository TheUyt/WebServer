/*
 Arduino with Ethernet Shield
 */

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

int InsideLightPin = 26;
int OutsideLightPin = 27;
int WtrHtrPin = 31;
int CoopHtrPin = 32;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 10, 110, 7, 54 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 10, 110, 7, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);                             //server port     
String readString;
//File webFile; //File name for reading/writing data files to sd card
// Set up SD variables using the SD Utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
SdFile file;
char str[3]; //Strings for SD Card Files Date/Time
// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))
void error_P(const char* str) {
  PgmPrint("error: ");
  SerialPrintln_P(str);
  if (card.errorCode()) {
    PgmPrint("SD error: ");
    Serial.print(card.errorCode(), HEX), Serial.print(','), Serial.println(card.errorData(), HEX);
    
    //Display Error on Screen and flash between Green/Red
    while (1){ 
     }
    }
  }

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(115200);

  pinMode(53, OUTPUT);
  pinMode(InsideLightPin, OUTPUT);
  pinMode(OutsideLightPin, OUTPUT);
  pinMode(WtrHtrPin, OUTPUT);
  pinMode(CoopHtrPin, OUTPUT);
  
  
  // start the Ethernet connection and the server:
  if (!SD.begin(4)) {
  Serial.println("initialization failed!");
  return;
  }
  Serial.println("initialization done.");
  
  /*Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());*/
  
   //disable w5100 SPI while starting w5100 (Ethernet) or will have problems
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  //Start Ethernet SPI
  Serial.println(F("Starting Ethernet..."));
  Ethernet.begin(mac, ip);
  Serial.print("server is at "), Serial.println(Ethernet.localIP());
  // this corrects a bug in the Ethernet.begin() function
  // even tho the call to Ethernet.localIP() does the same thing
  digitalWrite(53,HIGH);
  pinMode(28,INPUT);
  
//==============================================  
  PgmPrint("Free RAM: ");
  Serial.println(FreeRam());  

  if (!card.init(SPI_HALF_SPEED, 4)) error("card.init failed!");
  // initialize a FAT volume
  if (!volume.init(&card)) error("vol.init failed!");

  PgmPrint("Volume is FAT");
  Serial.println(volume.fatType(),DEC);
  Serial.println();
  
  if (!root.openRoot(&volume)) error("openRoot failed");

  // list file in root with date and size
  PgmPrintln("Files found in root:");
  root.ls(LS_DATE | LS_SIZE);
  Serial.println();
    
  // Recursive list of all directories
  PgmPrintln("Files found in all dirs:");
  root.ls(LS_R);
  
  Serial.println();
  PgmPrintln("Done");
  
  Serial.print("server is (still?) at "), Serial.println(Ethernet.localIP()); 
}

#define BUFSIZ 100 // buffer for HTTP reqeusts

void loop(void) {
  char clientline[BUFSIZ];
  char *filename;
  char *webcmd;
  int index = 0;
  int image = 0;
  
  // Create a client connection
  EthernetClient client = server.available();
  
  if (client) { // got client?
  
  boolean current_line_is_blank = true; // an http request ends with a blank line
  index = 0; // reset the input buffer
  
    while (client.connected()) {   
      if (client.available()) {
        char c = client.read();
        
        if (readString.length() < 100) { //read char by char HTTP request
        Serial.println(readString.length());
        readString += c;  //store characters to string 
        //Serial.print(c); // print HTTP request character to serial monitor
        } 
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        //if HTTP request has ended
        if (c != '\n' && c != '\r') {
          clientline[index] = c;
          index++;
          // are we too big for the buffer? start tossing out data
          if (index >= BUFSIZ)
            index = BUFSIZ - 1;
            
            // continue to read more data!
          continue;
        }
   
         // got a \n or \r new line, which means the string is done
        clientline[index] = 0;
        filename = 0;
                
        Serial.println(readString); //print to serial monitor for debugging
        //Serial.println(clientline); // Print it out for debugging
         
        //select proper header for file to be sent to browser 
        if ((strstr(clientline, "GET / ")) || (strstr(clientline, "GET /?")) != 0) {// Look for substring such as a request to get the root file
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: keep-alive");
          client.println();
          // Send rest of web page
          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<center>");
          client.println("<body bgcolor=Thistle>");
          client.println("<font color=white>");
          client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
          client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
          //client.print("<p><img src='SCRCH.GIF' alt='Scratching Chicken' align='left' width='110' height='89'/>");
          client.println("<p><img src='LAYCH.GIF' alt='Laying Chicken' align='left' width='110' height='89' />");
          client.println("<img src='LAYCH.GIF' alt='Laying Chicken' align='right' width='110' height='89' /></p>");
          client.println("<TITLE>EGG PLANT</TITLE>");
          //client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          client.println("</HEAD>");
          client.println("<BODY>");
          client.println("<H1>The Egg Plant Project</H1>");
          client.println("<hr />");
          client.println("<br />");
          client.println("<table border=1 width=325>");
          client.println("<tr><th align=center>Item</th><th align=center>Status</th><th align=center>Action</th></tr>");
          client.println("<tr><td align=center>Door Open Switch</td><td align=center>Status</td><td align=center>N/A</td></tr>");
          client.println("<tr><td align=center>Door Closed Switch</td><td align=center>Status</td><td align=center>N/A</td></tr>");
          client.println("<tr><td align=center>Door Open Button</td><td align=center>Status</td><td align=center>N/A</td></tr>");
          client.println("<tr><td align=center>Door Closed Button</td><td align=center>Status</td><td align=center>N/A</td></tr>");
          client.println("<tr><td align=center>Inside Light</td><td align=center>Status</td><td align=center><a href=\"/?Insideon\"\">Turn On Inside Light</a>      <a href=\"/?Insideoff\"\">Turn Off Inside Light</a></td></tr>");
          client.println("<tr><td align=center>Outside Light</td><td align=center>Status</td><td align=center><a href=\"/?Outsideon\"\">Turn On Outside Light</a>    <a href=\"/?Outsideoff\"\">Turn Off Outside Light</a></td></tr>");
          client.println("<tr><td align=center>Water Heater</td><td align=center>Status</td><td align=center><a href=\"/?WtrHtron\"\">Turn On Water Heater</a>    <a href=\"/?WtrHtroff\"\">Turn Off Water Heater</a></td></tr>");
          client.println("<tr><td align=center>Coop Heater</td><td align=center>Status</td><td align=center><a href=\"/?CoopHtron\"\">Turn On Coop Heater</a>      <a href=\"/?CoopHtroff\"\">Turn Off Coop Heater</a></td></tr>");
          client.println("</BODY>");
          client.println("</HTML>");
          }
         
          else if ((strstr(clientline,"GET /")) &&! (strstr(clientline,"GET /?")) != 0) {
          // this time no space after the /, so a sub-file
          Serial.println(clientline);

            if (!filename) filename = clientline + 5; // look after the "GET /" (5 chars)
            // a little trick, look for the " HTTP/1.1" string and
            // turn the first character of the substring into a 0 to clear it out.
            (strstr(clientline, " HTTP"))[0] = 0;
            Serial.print("extracted filename: "), Serial.println(clientline);

            // print the file we want
            Serial.print("filename: "), Serial.println(filename);
                
            if ((! file.open(&root, filename, O_READ))) {
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-Type: text/html");
              client.println();
              client.println("<p>File Not Found!</p>");
              break;
            }

            Serial.println("Opened!");

              client.println("HTTP/1.1 200 OK");
            if (strstr(filename, ".htm") != 0)
              client.println("Content-Type: text/html");
            else if (strstr(filename, ".css") != 0)
              client.println("Content-Type: text/css");
            else if (strstr(filename, ".png") != 0)
              client.println("Content-Type: image/png");
            else if (strstr(filename, ".jpg") != 0)
              client.println("Content-Type: image/jpeg");
            else if (strstr(filename, ".gif") != 0)
              client.println("Content-Type: image/gif");
            else if (strstr(filename, ".3gp") != 0)
              client.println("Content-Type: video/mpeg");
            else if (strstr(filename, ".pdf") != 0)
              client.println("Content-Type: application/pdf");
            else if (strstr(filename, ".js") != 0)
              client.println("Content-Type: application/x-javascript");
            else if (strstr(filename, ".xml") != 0)
              client.println("Content-Type: application/xml");
            else
              client.println("Content-Type: text");

            client.println();

            int16_t c;
            while ((c = file.read()) >= 0) {
              // uncomment the serial to debug (slow!)
              //Serial.print((char)c); //Debug Only
              client.print((char)c);
            }
            file.close();
            } /*else {
            // everything else is a 404
            client.println("HTTP/1.1 404 Not Found");
            client.println("Content-Type: text/html");
            client.println();
            client.println("<p>File IS Not Found!</p>");
            }  */    
        
            //client.println("</BODY>");
            //client.println("</HTML>");    
     
           delay(1);
           //stopping client
           client.stop();
           
           //controls the Arduino if you press the buttons
           if (readString.indexOf("?Insideon") >0){
               digitalWrite(InsideLightPin, HIGH);
           }
           if (readString.indexOf("?Insideoff") >0){
               digitalWrite(InsideLightPin, LOW);
           }
           if (readString.indexOf("?Outsideon") >0){
               digitalWrite(OutsideLightPin, HIGH);
           }
           if (readString.indexOf("?Outsideoff") >0){
               digitalWrite(OutsideLightPin, LOW);
           }
           if (readString.indexOf("?WtrHtron") >0){
               digitalWrite(WtrHtrPin, HIGH);
           }
           if (readString.indexOf("?WtrHtroff") >0){
               digitalWrite(WtrHtrPin, LOW);
           }
           if (readString.indexOf("?CoopHtron") >0){
               digitalWrite(CoopHtrPin, HIGH);
           }
           if (readString.indexOf("?CoopHtroff") >0){
               digitalWrite(CoopHtrPin, LOW);
           }
           //clearing string for next read
           readString=""; 
         }
       }
  }
}
