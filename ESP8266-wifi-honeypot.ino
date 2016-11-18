#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern "C" {
#include<user_interface.h>
}

#define OLED_RESET 0
Adafruit_SSD1306 display(OLED_RESET);


const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

unsigned long lastlistrefresh;

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>Hello</title></head><body><center>"
  "<img src=\"data:image/png;base64,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"
  "BbhiLgYaTUXX8QDXJHsVGYbyoTbvETPRnZBBRKJep99nLal66YKAJf8btWGf2eCPgfxhwj127ksZgHGZgKvbAYKSY4erRyTBzuKyyh0e5XG1papAZYFimwRigtVZxYZXeWFvPt1hr9admwSGgBXP"
  "xCN74rK+I+0+IPOM5PRxjsR/2xgCMxAC0v03TEDmRTRnt5hGRRpQUSaU7TeHV+BCP402EBjT+vwHcodBNKFelMUpPQB2bQJUYBQu7oRoCDFnYIAQQsNCEzdjINtB7eB8t/b+Z+v9hwNvKnRCoZd+"
  "2ZAUZtXvHVWERJiajGgrLkXez2JKsdvxEvIrDd1jX/wXzxWZZd0+XhwAAAFR0RVh0Y29tbWVudABGaWxlIHNvdXJjZTogaHR0cDovL2VuY3ljbG9wZWRpYWRyYW1hdGljYS5jaC9GaWxlOlRyYW5"
  "zcGFyZW50X1Ryb2xsZmFjZTIucG5nWHlUBwAAACV0RVh0ZGF0ZTpjcmVhdGUAMjAxMS0wOS0xOVQwMDo0MjowMiswMjowMGB1GVIAAAAldEVYdGRhdGU6bW9kaWZ5ADIwMTEtMDctMDZUMTU6NDQ"
  "6MDArMDI6MDCWuCgKAAAARnRFWHRzb2Z0d2FyZQBJbWFnZU1hZ2ljayA2LjYuMC00IDIwMTAtMTEtMTYgUTE2IGh0dHA6Ly93d3cuaW1hZ2VtYWdpY2sub3JnS+hB2QAAABh0RVh0VGh1bWI6OkR"
  "vY3VtZW50OjpQYWdlcwAxp/+7LwAAABh0RVh0VGh1bWI6OkltYWdlOjpoZWlnaHQAMzU1PlV8RAAAABd0RVh0VGh1bWI6OkltYWdlOjpXaWR0aAA0MzYo5c9wAAAAGXRFWHRUaHVtYjo6TWltZXR"
  "5cGUAaW1hZ2UvcG5nP7JWTgAAABd0RVh0VGh1bWI6Ok1UaW1lADEzMDk5NTk4NDAlmHpxAAAAEnRFWHRUaHVtYjo6U2l6ZQA1OC4xS0IaDTFIAAAATHRFWHRUaHVtYjo6VVJJAGZpbGU6Ly8vaG9"
  "tZS9kYXRhL25mcy93aWtpL2ltYWdlcy8yLzI4L1RyYW5zcGFyZW50X1Ryb2xsZmFjZTIucG5nmqoNlQAAAABJRU5ErkJggg==\"/>"
  "<br/><h1>Woopsy! No Internet Here!</h1></center></body></html>";


#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup() {
  Wire.begin(D5, D6);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Free Wifi");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  // replay to all requests with same HTML
  webServer.onNotFound([]() {
    client_status();
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.begin();
  client_status();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  if(millis() - lastlistrefresh > 3000) {
    client_status();
  }
}


void client_status()
{
  lastlistrefresh = millis();
  unsigned char number_client;
  struct station_info *stat_info;
  
  struct ip_addr *IPaddress;
  IPAddress address;
  int i=1;
  
  number_client= WiFi.softAPgetStationNum(); // Count of stations which are connected to ESP8266 soft-AP
  stat_info = wifi_softap_get_station_info();
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
    
  display.print(number_client);
  display.println(" connected:");
  while (stat_info != NULL) {
    
    IPaddress = &stat_info->ip;
    address = IPaddress->addr;
    
    display.print((address));
    display.print(" ");
    
    display.print(stat_info->bssid[0],HEX);
    display.print(stat_info->bssid[1],HEX);
    display.print(stat_info->bssid[2],HEX);
    display.print(stat_info->bssid[3],HEX);
    display.print(stat_info->bssid[4],HEX);
    display.print(stat_info->bssid[5],HEX);
    
    stat_info = STAILQ_NEXT(stat_info, next);
    i++;
    display.println();
  
  }
  display.display();
  

  wifi_softap_free_station_info();

} 
