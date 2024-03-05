struct taskButton {
  int sw, redLED, greenLED, blueLED, color, hour, minute, second;
  unsigned long lastTimePressed;
  bool reminderMessageSent, editTime, editLabel;
  String ButtonLabel;
};

#define BUZZER_P 15
#define BUZZER_N 17

#define RESET_DURATION 3000
#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7


#include "credentials.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <UniversalTelegramBot.h>


taskButton button1;
taskButton button2;
taskButton button3;
const int gracePeriod = 15; // The LED will turn RED and a notification will be sent this many minutes after the task was supposed to be completed

const long utcOffsetInSeconds = 3600 * -5; // Adjust this value according to your time zone
//const int daylightOffsetInSeconds = 3600 * -4; // Adjust this value for your daylight saving time offset
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
const unsigned long BOT_MTBS = 1000; // mean time between scan messages
unsigned long bot_lasttime;          // last time messages' scan has been done
String keyboardJson;
String chat_id;

void IRAM_ATTR button1WasPressed(){
  if(millis() - button1.lastTimePressed > 100){
    button1.lastTimePressed = millis();
    button1.color = GREEN;
  }
}
void IRAM_ATTR button2WasPressed(){
  if(millis() - button2.lastTimePressed > 100){
    button2.lastTimePressed = millis();
    button2.color = GREEN;
  }
}
void IRAM_ATTR button3WasPressed(){
  if(millis() - button3.lastTimePressed > 100){
    button3.lastTimePressed = millis();
    button3.color = GREEN;
  }
}

void handleNewMessages(int numNewMessages){
  for (int i = 0; i < numNewMessages; i++)
  {
    // Get all the important data from the message
    int message_id = bot.messages[i].message_id;
    chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == ""){
      from_name = "Guest";
    }
    Serial.print("message received from: ");
    Serial.println(from_name);
    Serial.print("on chat id: ");
    Serial.println(chat_id);
    Serial.print("text: ");
    Serial.println(text);
    if(chat_id == DM_CHAT_ID){ // only allows one whitelisted chat_id to use this bot. All others will be unauthorized
      // Inline buttons with callbacks when pressed will raise a callback_query message
      if (bot.messages[i].type == "callback_query")
      {
        Serial.println("callback query");
        if (text == "Back to Settings")
        {
          String keyboardJson = "[[{ \"text\" : \"" + button1.ButtonLabel + "\", \"callback_data\" : \"Edit Button 1\" }],[{ \"text\" : \"" + button2.ButtonLabel + "\", \"callback_data\" : \"Edit Button 2\" }],[{ \"text\" : \"" + button3.ButtonLabel + "\", \"callback_data\" : \"Edit Button 3\"}]]";
          bot.sendMessageWithInlineKeyboard(chat_id, "Settings", "", keyboardJson, message_id);
        }     
        if (text == "Edit Button 1"){
          String keyboardJson = "[[{ \"text\" : \"Reset " + button1.ButtonLabel + "\", \"callback_data\" : \"Reset Button 1\" }],[{ \"text\" : \"Edit time: " + String(button1.hour) + ":" + (button1.minute < 10 ? "0" : "") + String(button1.minute) + "\", \"callback_data\" : \"Edit Button 1 Time\" }],[{ \"text\" : \"Back to Settings\", \"callback_data\" : \"Back to Settings\" }]]";
          bot.sendMessageWithInlineKeyboard(chat_id, button1.ButtonLabel, "", keyboardJson, message_id);
        }
        if (text == "Edit Button 2"){
          String keyboardJson = "[[{ \"text\" : \"Reset " + button2.ButtonLabel + "\", \"callback_data\" : \"Reset Button 2\" }],[{ \"text\" : \"Edit time: " + String(button2.hour) + ":" + (button2.minute < 10 ? "0" : "") + String(button2.minute) + "\", \"callback_data\" : \"Edit Button 2 Time\" }],[{ \"text\" : \"Back to Settings\", \"callback_data\" : \"Back to Settings\" }]]";
          bot.sendMessageWithInlineKeyboard(chat_id, button2.ButtonLabel, "", keyboardJson, message_id);
        }
        if (text == "Edit Button 3"){
          String keyboardJson = "[[{ \"text\" : \"Reset " + button3.ButtonLabel + "\", \"callback_data\" : \"Reset Button 3\" }],[{ \"text\" : \"Edit time: " + String(button3.hour) + ":" + (button3.minute < 10 ? "0" : "") + String(button3.minute) + "\", \"callback_data\" : \"Edit Button 3 Time\" }],[{ \"text\" : \"Back to Settings\", \"callback_data\" : \"Back to Settings\" }]]";
          bot.sendMessageWithInlineKeyboard(chat_id, button3.ButtonLabel, "", keyboardJson, message_id);
        }
        if (text == "Reset Button 1"){
          button1.color = BLACK; //button3.reminderMessageSent = false;
        }
        if (text == "Reset Button 2"){
          button2.color = BLACK; //button3.reminderMessageSent = false;
        }
        if (text == "Reset Button 3"){
          button3.color = BLACK; //button3.reminderMessageSent = false;
        }
        if (text == "Edit Button 1 Time"){
          button1.editTime = true;
          bot.sendMessage(chat_id, "When do you want to " + button1.ButtonLabel + "?\nReply directly to this message with a 24 hour formated time (hh:mm)", "Markdown");
        }
        if (text == "Edit Button 2 Time"){
          button2.editTime = true;
          bot.sendMessage(chat_id, "When do you want to " + button2.ButtonLabel + "?\nReply directly to this message with a 24 hour formated time (hh:mm)", "Markdown");
        }
        if (text == "Edit Button 3 Time"){
          button3.editTime = true;
          bot.sendMessage(chat_id, "When do you want to " + button3.ButtonLabel + "?\nReply directly to this message with a 24 hour formated time (hh:mm)", "Markdown");
        }
        if (text == "Edit Button 1 Label"){
          
        }
      }
      else{ 
        if(button1.editTime){
          String hourString = text.substring(0,2);
          String minuteString = text.substring(3);
          button1.hour = hourString.toInt();
          button1.minute = minuteString.toInt();
          String message = button1.ButtonLabel + " was changed to " + hourString + ":" + minuteString;
          bot.sendMessage(chat_id, message, "");
          button1.editTime = false;
        }
        if(button2.editTime){
          String hourString = text.substring(0,2);
          String minuteString = text.substring(3);
          button2.hour = hourString.toInt();
          button2.minute = minuteString.toInt();
          String message = button2.ButtonLabel + " was changed to " + hourString + ":" + minuteString;
          bot.sendMessage(chat_id, message, "");
          button2.editTime = false;
        }
        if(button3.editTime){
          String hourString = text.substring(0,2);
          String minuteString = text.substring(3);
          button3.hour = hourString.toInt();
          button3.minute = minuteString.toInt();
          String message = button3.ButtonLabel + " was changed to " + hourString + ":" + minuteString;
          bot.sendMessage(chat_id, message, "");
          button3.editTime = false;
        }
        if (text == "/settings"){
          String keyboardJson = "[[{ \"text\" : \"" + button1.ButtonLabel + "\", \"callback_data\" : \"Edit Button 1\" }],[{ \"text\" : \"" + button2.ButtonLabel + "\", \"callback_data\" : \"Edit Button 2\" }],[{ \"text\" : \"" + button3.ButtonLabel + "\", \"callback_data\" : \"Edit Button 3\"}]]";
          bot.sendMessageWithInlineKeyboard(chat_id, "Settings", "", keyboardJson);
        }
      }
    }
    else{
      Serial.println("unauthorized chat id");
      bot.sendMessage(chat_id, "You are not authorized to use this telegram bot", "");
    }
  }
}

void setup() {
  Serial.begin(115200);

  button1.sw = 5;  button1.redLED = 6;  button1.greenLED = 9;  button1.blueLED = 10; button1.hour = 7;   button1.minute = 0; button1.reminderMessageSent = false; button1.editTime = false; button1.editLabel = false; button1.ButtonLabel = "feed Luna breakfast";
  button2.sw = 11; button2.redLED = 12; button2.greenLED = 13; button2.blueLED = 37; button2.hour = 13;  button2.minute = 0; button2.reminderMessageSent = false; button2.editTime = false; button1.editLabel = false; button2.ButtonLabel = "feed Luna lunch";
  button3.sw = 35; button3.redLED = 36; button3.greenLED = 8;  button3.blueLED = 14; button3.hour = 18;  button3.minute = 0; button3.reminderMessageSent = false; button3.editTime = false; button1.editLabel = false; button3.ButtonLabel = "feed Luna dinner";
  
  pinMode(button1.sw, INPUT_PULLUP);
  pinMode(button1.redLED, OUTPUT);
  pinMode(button1.greenLED, OUTPUT);
  pinMode(button1.blueLED, OUTPUT);

  pinMode(button2.sw, INPUT_PULLUP);
  pinMode(button2.redLED, OUTPUT);
  pinMode(button2.greenLED, OUTPUT);
  pinMode(button2.blueLED, OUTPUT);

  pinMode(button3.sw, INPUT_PULLUP);
  pinMode(button3.redLED, OUTPUT);
  pinMode(button3.greenLED, OUTPUT);
  pinMode(button3.blueLED, OUTPUT);

  pinMode(BUZZER_P,OUTPUT);
  pinMode(BUZZER_N,OUTPUT);

  attachInterrupt(button1.sw, button1WasPressed, FALLING);
  attachInterrupt(button2.sw, button2WasPressed, FALLING);
  attachInterrupt(button3.sw, button3WasPressed, FALLING);
  
  // Set all button colors
  setLED(1, BLACK);
  setLED(2, BLACK);
  setLED(3, BLACK);
  
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected");
  // Initialize NTP client
  timeClient.begin();
  timeClient.setTimeOffset(utcOffsetInSeconds);
  timeClient.forceUpdate();
  //timeClient.setUpdateInterval(1000 * 30); // Update interval in milliseconds. Requests an update from the NTP server. This function needs to be called periodically to keep the time updated.
  
  // assign button colors before main loop
  button1.color = BLACK;
  button2.color = BLACK;
  button3.color = BLACK;
}

void loop() {
  timeClient.update();
  
  if (millis() - bot_lasttime > BOT_MTBS){
    //Serial.println(timeClient.getFormattedTime());    
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    //Serial.println(numNewMessages);
    while (numNewMessages){
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }

  // Reset all button colors
  if(timeClient.getHours() == 0 && timeClient.getMinutes() == 0){ // reset all button colors at midnight
  //if(timeClient.getSeconds() == 0){
    button1.color = BLACK; button1.reminderMessageSent = false;
    button2.color = BLACK; button2.reminderMessageSent = false;
    button3.color = BLACK; button3.reminderMessageSent = false;    
  }

  // Assign button 1 color
  if(button1.color == GREEN){
    if(!digitalRead(button1.sw) && millis() - button1.lastTimePressed >= RESET_DURATION){ // If the button is green, and the button has been pressed for at least RESET_DURATION milliseconds, reset the button
      button1.color = BLACK;
    }
  }
  else{
    int graceHour, graceMinute;
    if(button1.minute + gracePeriod >= 60){
      graceHour = button1.hour + 1;
      graceMinute = button1.minute + gracePeriod - 60;
    }
    else{
      graceHour = button1.hour;
      graceMinute = button1.minute + gracePeriod;
    }

    if(timeClient.getHours() >= graceHour && timeClient.getMinutes() >= graceMinute){
    //if(timeClient.getSeconds() >= 20){ // condensed timeline for code development
      button1.color = RED;
      if(!button1.reminderMessageSent){
        bot.sendMessage(chat_id, "Remember to " + button1.ButtonLabel, "");
        button1.reminderMessageSent = true;
      }      
    }
    else if(timeClient.getHours() >= button1.hour && timeClient.getMinutes() >= button1.minute){
    //else if(timeClient.getSeconds() >= 15) { // condensed timeline for code development
      button1.color = BLUE;
    }
    else{
      button1.color = BLACK;
      button1.reminderMessageSent = false;
    }
  }

  // Assign button 2 color
  if(button2.color == GREEN){
    if(!digitalRead(button2.sw) && millis() - button2.lastTimePressed >= RESET_DURATION){ // If the button is green, and the button has been pressed for at least RESET_DURATION milliseconds, reset the button
      button2.color = BLACK;
    }
  }
  else{
    int graceHour, graceMinute;
    if(button2.minute + gracePeriod >= 60){
      graceHour = button2.hour + 1;
      graceMinute = button2.minute + gracePeriod - 60;
    }
    else{
      graceHour = button2.hour;
      graceMinute = button2.minute + gracePeriod;
    }
    if(timeClient.getHours() >= graceHour && timeClient.getMinutes() >= graceMinute){
    //if(timeClient.getSeconds() >= 35){ // condensed timeline for code development
      button2.color = RED;
      if(!button2.reminderMessageSent){
        bot.sendMessage(chat_id, "Remember to " + button2.ButtonLabel, "");
        button2.reminderMessageSent = true;
      }   
    }
    else if(timeClient.getHours() >= button2.hour && timeClient.getMinutes() >= button2.minute){
    //else if(timeClient.getSeconds() >= 30) { // condensed timeline for code development
      button2.color = BLUE;
    }
    else{
      button2.color = BLACK;
      button2.reminderMessageSent = false;
    }
  }

  // Assign button 3 color
  if(button3.color == GREEN){
    if(!digitalRead(button3.sw) && millis() - button3.lastTimePressed >= RESET_DURATION){ // If the button is green, and the button has been pressed for at least RESET_DURATION milliseconds, reset the button
      button3.color = BLACK;
    }
  }
  else{
    int graceHour, graceMinute;
    if(button3.minute + gracePeriod >= 60){
      graceHour = button3.hour + 1;
      graceMinute = button3.minute + gracePeriod - 60;
    }
    else{
      graceHour = button3.hour;
      graceMinute = button3.minute + gracePeriod;
    }
    if(timeClient.getHours() >= graceHour && timeClient.getMinutes() >= graceMinute){
    //if(timeClient.getSeconds() >= 50){ // condensed timeline for code development
      button3.color = RED;
      if(!button3.reminderMessageSent){
        bot.sendMessage(chat_id, "Remember to " + button3.ButtonLabel, "");
        button3.reminderMessageSent = true;
      }   
    }
    else if(timeClient.getHours() >= button3.hour && timeClient.getMinutes() >= button3.minute){
    //else if(timeClient.getSeconds() >= 45) { // condensed timeline for code development
      button3.color = BLUE;
    }
    else{
      button3.color = BLACK;
      button3.reminderMessageSent = false;
    }
  }

  // Set all button colors
  setLED(1, button1.color);
  setLED(2, button2.color);
  setLED(3, button3.color);
}


void setLED(int buttonNumber, int color){
  bool redLEDState = color & RED, greenLEDState = color & GREEN, blueLEDState = color & BLUE;

  switch(buttonNumber){
    case 1:
      digitalWrite(button1.redLED, redLEDState); digitalWrite(button1.greenLED, greenLEDState); digitalWrite(button1.blueLED, blueLEDState);
      break;
    case 2:
      digitalWrite(button2.redLED, redLEDState); digitalWrite(button2.greenLED, greenLEDState); digitalWrite(button2.blueLED, blueLEDState);
      break;
    case 3:
      digitalWrite(button3.redLED, redLEDState); digitalWrite(button3.greenLED, greenLEDState); digitalWrite(button3.blueLED, blueLEDState);
      break;
    default:
      digitalWrite(button1.redLED, LOW); digitalWrite(button1.greenLED, LOW); digitalWrite(button1.blueLED, LOW);
      digitalWrite(button2.redLED, LOW); digitalWrite(button2.greenLED, LOW); digitalWrite(button2.blueLED, LOW);
      digitalWrite(button3.redLED, LOW); digitalWrite(button3.greenLED, LOW); digitalWrite(button3.blueLED, LOW);
      break;
  }
}