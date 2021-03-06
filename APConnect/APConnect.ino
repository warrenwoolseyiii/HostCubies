#include <SPI.h>
#include <WiFi101.h>
#include "WebPage.h"

int        status = WL_IDLE_STATUS;
WiFiServer server( 80 );
String     HTTP_req;
boolean    readingNetwork = false;
boolean    readingPassword = false;
String     password = "";
String     network = "";
boolean    needCredentials = true;
boolean    needWiFi = false;
boolean    needAP = true;

void setup()
{
    Serial.begin( 9600 );
}

void loop()
{
    if( needAP )
        launchAP();
    else {
        if( needCredentials ) getCredentials();

        if( needWiFi ) getWiFi();

        if( !needWiFi && !needCredentials )
            if( WiFi.status() != WL_CONNECTED ) needWiFi = true;
    }
}

void launchAP()
{
    char apssid[] = "HostCubbyAP";

    needAP = true;
    Serial.println( "Access Point Web Server" );

    if( WiFi.status() == WL_NO_SHIELD )
        Serial.println( "WiFi shield not present" );

    Serial.print( "Creating access point named: " );
    Serial.println( apssid );

    if( WiFi.beginAP( apssid ) != WL_AP_LISTENING )
        Serial.println( "Creating access point failed" );
    else
        needAP = false;

    Serial.flush();
    delay( 1000 );

    if( !needAP ) {
        server.begin();
        printAPStatus();
    }
}

void getCredentials()
{
    WiFiClient client = server.available();
    if( client ) {
        Serial.println( "new client" );
        String currentLine = "";
        while( client.connected() ) {
            if( client.available() ) {
                char c = client.read();
                Serial.print( c );
                if( c == '?' ) readingNetwork = true;
                if( readingNetwork ) {
                    if( c == '!' ) {
                        readingPassword = true;
                        readingNetwork = false;
                    }
                    else if( c != '?' ) {
                        network += c;
                    }
                }
                if( readingPassword ) {
                    if( c == ',' ) {
                        Serial.println();
                        Serial.print( "Network Name: " );
                        Serial.println( network );
                        Serial.print( "Password: " );
                        Serial.println( password );
                        Serial.println();
                        client.stop();
                        WiFi.end();
                        readingPassword = false;
                        needWiFi = true;
                    }
                    else if( c != '!' ) {
                        password += c;
                    }
                }
                if( c == '\n' ) {
                    if( currentLine.length() == 0 ) {
                        client.println( webPageHTML );
                        client.println();
                        break;
                    }
                    else {
                        currentLine = "";
                    }
                }
                else if( c != '\r' ) {
                    currentLine += c;
                }
            }
        }
        client.stop();
        Serial.println( "client disconnected" );
        Serial.println();
    }
}

void getWiFi()
{
    if( network == "" or password == "" ) {
        Serial.println( "Invalid WiFi credentials" );
        while( true )
            ;
    }
    else
        needCredentials = false;

    while( WiFi.status() != WL_CONNECTED ) {
        Serial.print( "Wifi Status: " );
        Serial.println( WiFi.status() );
        Serial.print( "Attempting to connect to SSID: " );
        Serial.println( network );
        WiFi.begin( network, password );
    }
    Serial.println( "WiFi connection successful" );
    printWiFiStatus();
    needWiFi = false;
    delay( 1000 );
}

void printWiFiStatus()
{
    Serial.print( "SSID: " );
    Serial.println( WiFi.SSID() );
    IPAddress ip = WiFi.localIP();
    Serial.print( "IP Address: " );
    Serial.println( ip );
    Serial.print( "signal strength (RSSI):" );
    Serial.print( WiFi.RSSI() );
    Serial.println( " dBm" );
}

void printAPStatus()
{
    Serial.print( "SSID: " );
    Serial.println( WiFi.SSID() );
    IPAddress ip = WiFi.localIP();
    Serial.print( "IP Address: " );
    Serial.println( ip );
    Serial.print( "signal strength (RSSI):" );
    Serial.print( WiFi.RSSI() );
    Serial.println( " dBm" );
    Serial.print( "To connect, open a browser to http://" );
    Serial.println( ip );
}
