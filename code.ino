//Importing the necessary libraries.
// Library used for connecting the wifi securely to the esp32 kit.
#include <WiFi.h>
#include <WiFiClientSecure.h>

//Connecting the TelegramBot with the esp32 kit.
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
// Inorder to send the html request for fetching or posting the data from or into the google sheets.
#include <HTTPClient.h>

// These are the cells with username,password,totalamount at initial stages.
const char*cellAddress1 = "A1";
const char*cellAddress2 = "B1";
const char*cellAddress3 = "D2";
// Inorder to check whether the user is verified or not.
int User_Verified = 0;
// WI-FI will help us to connect the outer world.
const char* ssid_name = "ESP32";
const char* password = "Acharya_Teja123";

//Bot has been created from the telegram using BotFather.
// Inorder to access that bot we are using these credentials.
#define BOTtoken "5675384134:AAEFt79mx2sS9XoC0FNwkblwHGyFC75-sn4"  
#define CHAT_ID "1245117506"

// Google script ID and required credentials
String GOOGLESCRIPT_ID1 = "AKfycbyqvXnUtfN594wXWoSgRo7G1IAX7xIJk6vrYqibPQMuJDmqVUbgAfIXkcXahg9Yyo_WVg";
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
// Getting checked the messages for every 1 second.
int botDelay = 1000;
int username_status=0,password_status=0;
unsigned long lastTimeBotRan;

// details of the account.
String username_from_bot,password_from_bot;
int credit_status=0,debit_status=0,creditNotes,debitNotes;
String username_from_sheet,password_from_sheet;
int no_of_notes;
int password_update=0,username_update=0;
String updatedPassword;

// Handle what happens when you receive new messages
String handleNewMessages(int numNewMessages) {
  Serial.println(String(numNewMessages));
  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    // text of the message entered.
    String msg = bot.messages[i].text;
    // this becomes activate when user is asked to type the username.
    if (username_status==1){
      username_from_bot = msg;
      Serial.print("Entered Username from bot: ");
      Serial.println(username_from_bot);
      username_status=0;
    }
    // this becomes activate when user is asked to type the password.
    if (password_status==1){
      password_from_bot = msg;
      Serial.print("Entered Password from bot: ");
      Serial.println(password_from_bot);
      password_status = 0;
    }
    // if user is crediting the money into the bank account it activates.
    if (credit_status==1){
      creditNotes = msg.toInt();
      credit_status=0;
      no_of_notes = no_of_notes + creditNotes;
      //sends the message that money has been credited.
      bot.sendMessage(chat_id,"Dear Customer, Your a/c with username "+username_from_bot+" Credited by Rs."+String(creditNotes*100),"");
      //sends the message of final balance in  the bank account.
      bot.sendMessage(chat_id,"Dear Customer, Your a/c with username "+username_from_bot+" , Aval.Balance= "+String(no_of_notes*100),"");
      //updating the transaction details in the spreadsheet.
      String urlFinal4 = "https://script.google.com/macros/s/"+GOOGLESCRIPT_ID1+"/exec?transaction="+String(creditNotes*100)+"&balance="+String(no_of_notes*100);
      Serial.print("Posting data to the spreadsheet");
      HTTPClient http;
      http.begin (urlFinal4.c_str () );
      http.setFollowRedirects (HTTPC_STRICT_FOLLOW_REDIRECTS) ;
      int httpCode4 = http.GET () ;
      Serial.println(httpCode4);
      //ending the http request.
      http.end();
      //updating the final balance or total amount in the spreadsheet.
      String urlFinal6 = "https://script.google.com/macros/s/"+GOOGLESCRIPT_ID1+"/exec?finalbalance="+String(no_of_notes*100);
      Serial.print("Posting data to the spreadsheet");
      http.begin (urlFinal6.c_str () );
      http.setFollowRedirects (HTTPC_STRICT_FOLLOW_REDIRECTS) ;
      int httpCode6 = http.GET () ;
      Serial.println(httpCode6);
      //ending the http request
      http.end();
    }
    // it becomes activate when you are doing withdrawl of money.
    if (debit_status==1){
      debitNotes = msg.toInt();
      debit_status=0;
      if (debitNotes>no_of_notes){
        // as money is not sufficient it was not able to perform the action.
        bot.sendMessage(chat_id,"Insuffienct Balance.","");
      }
      else{

        no_of_notes = no_of_notes -debitNotes;
        bot.sendMessage(chat_id,"Dear Customer, Your a/c with username "+username_from_bot+" debited by Rs."+String(debitNotes*100),"");
        bot.sendMessage(chat_id,"Dear Customer, Your a/c with username "+username_from_bot+" , Aval.Balance= "+String(no_of_notes*100),"");
        // updating the transactions in the google spreadsheet.
        String urlFinal5 = "https://script.google.com/macros/s/"+GOOGLESCRIPT_ID1+"/exec?transaction="+String(debitNotes*-100)+"&balance="+String(no_of_notes*100);
        Serial.print("Posting data to the spreadsheet");
        HTTPClient http;
        http.begin (urlFinal5.c_str () );
        http.setFollowRedirects (HTTPC_STRICT_FOLLOW_REDIRECTS) ;
        int httpCode5 = http.GET () ;
        Serial.println(httpCode5);
        //ending the http request.
        http.end();
        // updating the final balance or total amount in the amount.
        String urlFinal7 = "https://script.google.com/macros/s/"+GOOGLESCRIPT_ID1+"/exec?finalbalance="+String(no_of_notes*100);
        Serial.print("Posting data to the spreadsheet");
        http.begin (urlFinal7.c_str () );
        http.setFollowRedirects (HTTPC_STRICT_FOLLOW_REDIRECTS) ;
        int httpCode7 = http.GET () ;
        Serial.println(httpCode7);
        // ending the http request.
        http.end();
      }
    }
    // get activated when user wants to update the password.
    if (password_update==1){
      updatedPassword = msg;
      password_update=0;
      // getting updated password posted in the google spreadsheet.
      String urlFinal8 = "https://script.google.com/macros/s/"+GOOGLESCRIPT_ID1+"/exec?passwordUpdate="+updatedPassword;
      Serial.print("Posting data to the spreadsheet");
      HTTPClient http;
      http.begin (urlFinal8.c_str () );
      http.setFollowRedirects (HTTPC_STRICT_FOLLOW_REDIRECTS) ;
      int httpCode8 = http.GET () ;
      Serial.println(httpCode8);
      //ending the request.
      http.end();
      bot.sendMessage(chat_id,"Password is updated Successfully!!");
      User_Verified = 0;
      digitalWrite(2,LOW);
    }
    String from_name = bot.messages[i].from_name;
    // basic one,.
    if (msg == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/username inorder to verify the username through input\n";
      welcome += "/password inorder to verify the password through input\n";
      welcome += "/login inorder to verify the credentials. \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    if (msg == "/username") {
      bot.sendMessage(chat_id, "Enter the username", "");
      username_status = 1;
    }
    if (msg == "/password") {
      bot.sendMessage(chat_id, "Enter the password", "");
      password_status = 1;
    }

    if (msg =="/login"){
      bot.sendMessage(chat_id,"Verifying the Credentials","");
      if (User_Verified==1){
        bot.sendMessage(chat_id,"You have already Logged into your account","");
        continue;
      }
      if (username_from_sheet==username_from_bot && password_from_sheet==password_from_bot){
        digitalWrite(2,HIGH);
        User_Verified = 1;
        String message = "Congratulations!! " + from_name + ".\n";
        message += "You have Successfully logged into your bank account\n";
        message += "/checkBalance inorder to find the balance of the bank account\n";
        message += "/credit inorder to credit the money into your bank account\n";
        message += "/debit inorder to debit the money from your bank account\n";
        message += "/updatePassword In order to change the password of the bank account\n";
        message += "/logout inorder to logout from the bank account. \n";
        bot.sendMessage(chat_id,message,"");
      }
      else{
        bot.sendMessage(chat_id,"Sorry! Entered Credentials are wrong. Re-Enter Correctly");
      }
    }
    if (msg == "/logout"){
      if (User_Verified==1){
        User_Verified = 0;
        digitalWrite(2,LOW);
        String message2 = "You have Successfully logged out from the bank account";
        bot.sendMessage(chat_id,message2,"");
      }
      else{
        bot.sendMessage(chat_id,"First Login to your account inorder to logout","");
      }
    }
    if (msg=="/checkBalance"){
      if (User_Verified==1){
        bot.sendMessage(chat_id,"Dear Customer, Your a/c with username "+username_from_bot+" , Aval.Balance= "+String(no_of_notes*100),"");
      }
      else{
        bot.sendMessage(chat_id,"Enter to your bank account using valid credentials inorder to perform any action.","");
      }
    }
    if (msg=="/credit"){
      if (User_Verified==1){
        bot.sendMessage(chat_id,"Enter the number of 100 rupee notes you wanna credit from the bank account: ","");
        credit_status=1;
      }
      else{
        bot.sendMessage(chat_id,"Enter to your bank account using valid credentials inorder to perform any action.","");
      }
    }
    if (msg =="/debit"){
      if (User_Verified==1){
        bot.sendMessage(chat_id,"Enter the number of 100 rupees notes you wanna debit from the bank account: ","");
        debit_status = 1;
      }
      else{
        bot.sendMessage(chat_id,"Enter to your bank account using valid credentials inorder to perform any action.","");
      }
    }
    if (msg == "/updatePassword"){
      if (User_Verified==1){
        bot.sendMessage(chat_id,"Enter the new password to which you wanna update: ","");
        password_update=1;
      }
    }
  }
}
void setup() {
  pinMode(2,OUTPUT);
  Serial.begin(115200);
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_name, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  if (WiFi.status()==WL_CONNECTED){
    String urlFinal1 = "https://script.google.com/macros/s/"+GOOGLESCRIPT_ID1+"/exec?read="+cellAddress1;
    String urlFinal2 = "https://script.google.com/macros/s/"+GOOGLESCRIPT_ID1+"/exec?read="+cellAddress2;
    String urlFinal3 = "https://script.google.com/macros/s/"+GOOGLESCRIPT_ID1+"/exec?read="+cellAddress3;
    Serial.println("Credentials");
    delay(1000);
    HTTPClient http;
    http.begin (urlFinal1.c_str () );
    delay(1000);
    http.setFollowRedirects (HTTPC_STRICT_FOLLOW_REDIRECTS) ;
    int httpCode1 = http.GET () ;
    if (httpCode1 >0){
      username_from_sheet = http.getString();    
      Serial.println(httpCode1);
      Serial.println(username_from_sheet);
    }
    else{
      Serial.println("ERROR ON THE REQUEST");
    }
    http.end();
    http.begin (urlFinal2.c_str ());
    delay(1000);
    http.setFollowRedirects (HTTPC_STRICT_FOLLOW_REDIRECTS) ;
    int httpCode2 = http.GET () ;
    String payload2;
    if (httpCode2 >0){
      password_from_sheet = http.getString();   
      Serial.println(httpCode2);
      Serial.println(password_from_sheet);
    }
    else{
      Serial.println("ERROR ON THE REQUEST");
    }
    http.end();
    http.begin (urlFinal3.c_str ());
    delay(1000);
    http.setFollowRedirects (HTTPC_STRICT_FOLLOW_REDIRECTS) ;
    int httpCode3 = http.GET () ;
    if (httpCode3 >0){
      String balance = http.getString();   
      Serial.println(httpCode3);
      no_of_notes = balance.toInt()/100;
      Serial.println(no_of_notes);
    }
    else{
      Serial.println("ERROR ON THE REQUEST");
    }
    http.end();
  }
}
void loop() {
  if (WiFi.status () == WL_CONNECTED) {
    if (millis() > lastTimeBotRan + botDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  }

}