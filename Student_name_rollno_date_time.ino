#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <unordered_map>
#include <unordered_set>

ESP8266WiFiMulti wifiMulti;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "in.pool.ntp.org", 19800, 60000);


// Define the days of the week and the corresponding timetable
String days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

String timetable[7][24] = {
  // Sunday
  {"/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA","/Attendance/NA", "/Attendance/NA", "Attendance/NA", "/Attendance/NA"},
  // Monday {"10AM":"Chemistry","11AM":"EG","12PM":"Maths"}
  {"/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/Chemistry", "/Attendance/EG", "/Attendance/Maths", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA"},
  // Tuesday {"10AM":"EG","11AM":"Physics","12PM":"Maths"}
  {"/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/EG", "/Attendance/Physics", "/Attendance/Maths", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA"},
  // Wednesday {"10AM":"PCE","11AM":"CP","12PM":"Maths"}}
  {"/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/PCE", "/Attendance/CP", "/Attendance/Maths", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA"},
  // Thursday {"10AM":"CP","11AM":"PCE","12PM":"Chemistry"}
  {"/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/CP", "/Attendance/PCE", "/Attendance/Chemistry", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA"},
  // Friday :{"10AM":"Maths","11AM":"Physics","12PM":"CP"}
  {"/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/Maths", "/Attendance/Physics", "/Attendance/CP", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA"},
  // Saturday {"10AM":"PCE","11AM":"EG","12PM":"Physics"}
  {"/Attendance/Maths", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/PCE", "/Attendance/EG", "/Attendance/Physics", "/Attendance/Maths", "/Attendance/Chemistry", "/Attendance/MD", "/Attendance/SOM", "/Attendance/KOM", "/Attendance/TOM", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA", "/Attendance/NA"},
};

FirebaseData firebaseData;

// Map of BSSIDs to student names
std::unordered_map<std::string, std::string> bssidToName {
  {"80:AD:16:85:1A:BF", "Class-A/Student Name: Omkar Mondkar"},
  {"1A:C7:10:47:BA:E6", "Class-A/Student Name: Jagdish Mondkar"},
  {"F4:F2:6D:7F:B9:F0", "Class-A/Student Name: Sandeep Mondkar"},
  {"00:17:7C:36:D7:89", "Class-A/Student Name: Manoj Nikam"},
  {"72:E5:DF:6C:59:AA", "Class-A/Student Name: Yashraj Sawant"},
  {"AA:48:FA:FF:85:97", "Class-A/Student Name: Card_1"},
  {"0E:E0:DC:98:60:93", "Class-A/Student Name: Anushka Sonawale"},
  // Add more mappings as needed
};

// Set of names that have already been sent to Firebase
std::unordered_set<std::string> sentNames;

// Map of BSSIDs to student names
std::unordered_map<std::string, std::string> bssidToRollno {
  {"80:AD:16:85:1A:BF", "Roll no: 33"},
  {"1A:C7:10:47:BA:E6", "Roll no: 32"},
  {"F4:F2:6D:7F:B9:F0", "Roll no: 34"},
  {"00:17:7C:36:D7:89", "Roll no: 35"},
  {"72:E5:DF:6C:59:AA", "Roll no: 36"},
  {"AA:48:FA:FF:85:97", "Roll no: 37"},
  {"0E:E0:DC:98:60:93", "Roll no: 38"},
  // Add more mappings as needed
};

// Set of names that have already been sent to Firebase
std::unordered_set<std::string> sentRollno;

// int count = 0;

String jsonStr;

void setup() {
  Serial.begin(115200);

  wifiMulti.addAP("Redmi", "12345678");

  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }

  Serial.println("Connected to WiFi");

  timeClient.begin();


}

void loop() {
  Firebase.begin("https://attendance-78c23-default-rtdb.firebaseio.com/", "6fzMnlDV1UHQg1N49HmopAEJZpXcpvNhbWE2bFbo");

  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  Serial.println("MDNS responder started");

  Serial.println("Starting scan...");

  int networksFound = WiFi.scanNetworks();

  Serial.print(networksFound);
  Serial.println(" networks found");

  for (int i = 0; i < networksFound; i++) {
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID(i));
    Serial.print("MAC address: ");
    String bssid = WiFi.BSSIDstr(i);
    Serial.println(bssid);

    if (bssidToName.find(bssid.c_str()) != bssidToName.end()) {
        std::string name = bssidToName[std::string(bssid.c_str())];
        Serial.println(name.c_str());
        if (sentNames.find(name) == sentNames.end()) {        
          //   String branch = "/branch/mechanical";  
          // Firebase.pushString(firebaseData, "Class_A" , name.c_str() + branch.c_str());


            timeClient.update();

            time_t epochTime = timeClient.getEpochTime();
            Serial.print("Epoch Time: ");
            Serial.println(epochTime);

            //Get a time structure
            struct tm *ptm = gmtime ((time_t *)&epochTime); 

            int monthDay = ptm->tm_mday;
            Serial.print("Month day: ");
            Serial.println(monthDay);

            int currentMonth = ptm->tm_mon+1;
            Serial.print("Month: ");
            Serial.println(currentMonth);

            String currentMonthName = months[currentMonth-1];
            Serial.print("Month name: ");
            Serial.println(currentMonthName);

            int currentYear = ptm->tm_year+1900;
            Serial.print("Year: ");
            Serial.println(currentYear);
            //Print complete date:
            String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
            Serial.print("Current date: ");
            Serial.println(currentDate);

          
          String timestamp = timeClient.getFormattedTime();
          Serial.print(timestamp);
          


          // Get the current day of the week (0 = Sunday, 1 = Monday, etc.)
          int dayOfWeek = timeClient.getDay();
          // Get the current hour and minute
          int currentHour = timeClient.getHours();
          int currentMinute = timeClient.getMinutes();

          // Get the current subject based on the timetable
          String current_subject = timetable[dayOfWeek][currentHour];
          Serial.println(current_subject);

          Firebase.pushString(firebaseData, name.c_str(), "Subject: " + current_subject + " at Date&Time: " + currentDate + ", " + timestamp );

          // Firebase.pushString(firebaseData, name.c_str() + current_subject , " Time: " + timestamp );
          // delay(1000);

          

            // Update the count
            // count++;
            // Firebase.pushJSON(firebaseData, name.c_str() + current_subject , " Days:"+ count);


            // // Get the current value from Firebase
            // if (Firebase.getInt(firebaseData, name.c_str() + current_subject)) {
            //   int currentValue = Firebase.getInt(firebaseData, name.c_str() + current_subject);
            //   Serial.print("Current value: ");
            //   Serial.println(currentValue);

            //   // Increment the value by 1
            //   int newValue = currentValue + 1;
            //   Serial.print("New value: ");
            //   Serial.println(newValue);

            //   // Send the new value back to Firebase
            //   Firebase.setInt(firebaseData, name.c_str() + current_subject, newValue);
              
            // } else {
            //   Serial.println(firebaseData.errorReason());
            // }

            // // Wait for some time before looping again
            // delay(5000);


            // Get the current count
            int count = Firebase.getInt(firebaseData, name.c_str() + current_subject);
            Serial.print("Current count: ");
            Serial.println(count);

            // Increment the count and push it to Firebase
            count++;
            Firebase.setInt(firebaseData, name.c_str() + current_subject, count);
            Serial.print("New count: ");
            Serial.println(count);



          if (bssidToRollno.find(bssid.c_str()) != bssidToRollno.end()) {
            std::string Rollno = bssidToRollno[std::string(bssid.c_str())];
            Serial.println(Rollno.c_str());
            if (sentRollno.find(Rollno) == sentRollno.end()) {
          
          
            Firebase.pushString(firebaseData, name.c_str(), Rollno.c_str());
            sentRollno.insert(Rollno);
            }
            delay(1000);
          }
          delay(1000);


          Firebase.pushString(firebaseData, name.c_str(), "Branch : Mechanical");
          delay(1000);


          Firebase.pushString(firebaseData, name.c_str(), "Year : 2");
          delay(1000);






          sentNames.insert(name);
        }
        delay(1000);
    }
    delay(1000);
  }

  Serial.println("Scan complete");
  delay(15000);
}
