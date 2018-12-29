#include <SPI.h>
#include <WiFi101.h>
#include <PubSubClient.h>
#include <aREST.h>
#include "WebPage.h"

// AP Connect
WiFiServer server( 80 );
String     HTTP_req;
boolean    readingNetwork = false;
boolean    readingPassword = false;
String     password = "";
String     network = "";
boolean    needCredentials = true;
boolean    needWiFi = false;
boolean    needAP = true;
int        numReconnects = 0;

// aRest
int          status = WL_IDLE_STATUS;
WiFiClient   wifiClient;
PubSubClient client( wifiClient );
String       local_ip = "";
aREST        rest = aREST( client );
char *       device_id = "fa3698";
int          numRequests = 0;
boolean      serverIsLive = false;

// Callback function for the cloud connection
void callback( char *topic, byte *payload, unsigned int length );

// Function to open a container
int openContainer( String command );

// AP functions
void launchAP();
void getCredentials();
void getWiFi();
void printWiFiStatus();
void printAPStatus();

void setup( void )
{
    // Start Serial
    Serial.begin( 115200 );

    // Set callback
    client.setCallback( callback );

    // Functions for the REST API
    rest.function( "container", openContainer );
    rest.variable( "numRequests", &numRequests );
    rest.variable( "numReconnects", &numReconnects );

    // Give name and ID to device (ID should be 6 characters long)
    rest.set_id( device_id );
    rest.set_name( "mkr1000" );

    for( uint8_t i = 0; i < 8; i++ ) {
        pinMode( i, OUTPUT );
        digitalWrite( i, HIGH );
    }
}

void loop()
{
    if( needAP )
        launchAP();
    else {
        if( needCredentials ) getCredentials();

        if( needWiFi ) getWiFi();

        if( !needWiFi && !needCredentials ) {
            if( WiFi.status() != WL_CONNECTED ) {
                needWiFi = true;
                serverIsLive = false;
                numReconnects++;
            }
        }

        if( !needWiFi && !needCredentials && !serverIsLive ) {
            server.begin();
            Serial.println( "Local server started on IP:" );
            serverIsLive = true;
        }
        else if( serverIsLive ) {
            // Connect to the cloud
            rest.handle( client );

            // Handle REST calls
            WiFiClient localClient = server.available();
            if( !localClient ) {
                return;
            }
            while( !localClient.available() ) {
                delay( 1 );
            }
            rest.handle( localClient );
        }
    }
}

// Handles message arrived on subscribed topic(s)
void callback( char *topic, byte *payload, unsigned int length )
{

    // Handle
    rest.handle_callback( client, topic, payload, length );
}

// Convert IP address to String
String ipToString( IPAddress address )
{
    return String( address[0] ) + "." + String( address[1] ) + "." +
           String( address[2] ) + "." + String( address[3] );
}

// Opens and closes a container
int openContainer( String command )
{
    int container = command.toInt();
    container--;
    numRequests++;
    if( container >= 0 && container < 8 ) {
        digitalWrite( container, LOW );
        delay( 1000 );
        digitalWrite( container, HIGH );
        return 1;
    }
    return 0;
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
    WiFiClient wC = server.available();
    if( wC ) {
        Serial.println( "new client" );
        String currentLine = "";
        while( wC.connected() ) {
            if( wC.available() ) {
                char c = wC.read();
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
                        wC.stop();
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
                        wC.println( webPageHTML );
                        wC.println();
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
        wC.stop();
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
