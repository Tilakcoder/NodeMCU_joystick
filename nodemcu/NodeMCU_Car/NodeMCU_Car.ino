#define ENA   14          // Enable/speed motors Right        GPIO14(D5)
#define ENB   12          // Enable/speed motors Left         GPIO12(D6)
#define IN_1  15          // L298N in1 motors Right           GPIO15(D8)
#define IN_2  13          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  2           // L298N in3 motors Left            GPIO2(D4)
#define IN_4  0           // L298N in4 motors Left            GPIO0(D3)

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

String command;           //String to store app command state.
String Y;
String resp;
int speedCar = 800;         // 400 - 1023.
int speed_Coeff = 3;

const char* ssid = "Girish Car";
ESP8266WebServer server(80);

void setup() {

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);

  Serial.begin(115200);

  // Connecting WiFi

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Starting WEB-server
  server.on ( "/", HTTP_handleRoot );
  server.onNotFound ( HTTP_handleRoot );
  server.begin();
}

void loop() {
  server.handleClient();

  command = server.arg("X");
  Y = server.arg("Y");
}

void HTTP_handleRoot(void) {

  if ( server.hasArg("X")) {
    command = server.arg("X");
    int pp1 = command.toInt();
    //     Serial.println(command.toInt());
    Y = server.arg("Y");
    int pp2 = Y.toInt();

    int p1 = pp2 + pp1;
    int p2 = pp2 - pp1;

    digitalWrite(IN_1, p1 >= 0);
    digitalWrite(IN_2, p1 < 0);

    digitalWrite(IN_3, p2 >= 0);
    digitalWrite(IN_4, p2 < 0);

    p1 = (abs(p1) > 100) ? 100 : p1;
    p2 = (abs(p2) > 100) ? 100 : p2;

    analogWrite(ENA, (abs(p1) * 255) / 100);
    analogWrite(ENB, (abs(p2) * 255) / 100);
    resp = "";
  }

  if( server.hasArg("MKS")){
    resp = "<!DOCTYPE html>"
"<html lang='en'>"
""
"<head>"
    "<meta charset='UTF-8'>"
    "<meta http-equiv='X-UA-Compatible' content='IE=edge'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>Car Remote</title>"
    "<style>"
        "body,"
        "#main {"
            "display: flex;"
            "justify-content: right;"
            "align-items: center;"
        "} "

        ".panel {"
            "height: 90vh;"
        "} "

        "#main {"
            "border: 1px solid black;"
            "width: 70vh;"
            "justify-content: center;"
            "position: relative;"
            "height: 70vh;"
            "margin-right: 50px;"
        "} "
        
        "#joystick {"
            "position: absolute;"
            "display: inline-block;"
            "width: 100px;"
            "height: 100px;"
            "background: red;"
            "border-radius: 50px;"
        "}"
        ".buts>button {"
            "display: block;"
            "margin: 10px;"
            "font-size: 20px;"
        "}"
    "</style>"

"</head>"

"<body>"
    "<div class='panel' style='flex: 1;'>"
        "<div class='buts'>"
            "<button id='start' onclick='start()'>Start</button>"
            "<button id='stop' onclick='end()' disabled>Stop</button>"
            "<div>"
                "192.168.4.1"
            "</div>"
        "</div>"
    "</div>"
    "<div class='panel' id='main' ontouchmove='throttel(event)' ontouchend='throttelEnd()'>"
        "<div id='joystick'></div>"
    "</div>"

    "<script>"
        "body = document.getElementById('main');"
        "joystick = document.getElementById('joystick');"
        "values = { 'X': 0, 'Y': 0, 'aleron': 0, 'elevator': 0 };"

        "power = false;"

        "const Http = new XMLHttpRequest();"
        "var pre = false;"

        "bd = body.getBoundingClientRect();"
        "X = bd.width / 2;"
        "Y = bd.height / 2;"
        "compile(X, Y);"

        "url = 'http://192.168.4.1/?X=' + Math.round(values['aleron']) + '&Y=' + Math.round(values['elevator']);"
        "Http.onreadystatechange = function () {"
            "if (this.readyState == 4) {"
                "pre = false;"
            "}"
        "};"
        
        " function compile(X, Y) {"
            "X = (X > bd.width) ? bd.width : X;"
            "X = (X < 0) ? 0 : X;"
            "values['X'] = X;"
            "X = (X / bd.width) * 200;"
            "X -= 100;"
            "values['aleron'] = X;"
            "Y = (Y > bd.height) ? bd.height : Y;"
            "Y = (Y < 0) ? 0 : Y;"
            "values['Y'] = Y;"
            "Y = (Y / bd.height) * -200;"
            "Y += 100;"
            "values['elevator'] = Y;"
            "place();"
        "} "
        "function throttel(e) {"
            "e.preventDefault();"
            "bd = body.getBoundingClientRect();"
            "Y = e.targetTouches[0].clientY - bd.top;"
            "X = e.targetTouches[0].clientX - bd.left;"
            "compile(X, Y);"
        "}"
        "function throttelEnd() {"
            "bd = body.getBoundingClientRect();"
            "X = bd.width / 2;"
            "Y = bd.height / 2;"
            "compile(X, Y);"
        "} "
        "function place() {"
            "dime = joystick.getBoundingClientRect();"
            "joystick.style.left = (values['X'] - (dime.width / 2)) + 'px';"
            "joystick.style.top = (values['Y'] - (dime.height / 2)) + 'px';"
        "} "
        "c = 0; "
        "function repeat() {"
            "if (!power) {"
                "return;"
            "}"
            "if (c % 7 == 0 && !pre) {"
                "url = 'http://192.168.4.1/?X=' + Math.round(values['aleron']) + '&Y=' + Math.round(values['elevator']);"
                "pre = true;"
                "Http.open('GET', url);"
                "Http.send();"
            "}"
            "c += 1;"
            "window.requestAnimationFrame(repeat);"
        "} "
        "function start() {"
            "power = true;"
            "window.requestAnimationFrame(repeat);"
            "document.getElementById('stop').disabled = false;"
            "document.getElementById('start').disabled = true;"
        "} "
        "function end() {"
            "power = false;"
            "document.getElementById('stop').disabled = true;"
            "document.getElementById('start').disabled = false;"
        "}"
    "</script>"
"</body>"
"</html>";
  }
  server.send ( 200, "text/html", resp );
  delay(1);
}
