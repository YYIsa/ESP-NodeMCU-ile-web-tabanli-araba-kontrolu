/*  NodeMCU Access Point (AP) modunda çalışmaktadır.
    Kablosuz ağlardan "Robot"a bağlanın. Şifre: 12345678
    Tarayıcıdan 192.168.4.1 adresine girin.
*/
#include <ESP8266WiFi.h> // NodeMCU kurulumuyla gelir.
#include <WiFiClient.h> // NodeMCU kurulumuyla gelir.
#include <ESP8266WebServer.h> // NodeMCU kurulumuyla gelir.
#include <WebSocketsServer.h> // WebSockets v2.3.6
#include <Hash.h> // NodeMCU kurulumuyla gelir.

const char* ssid = "Robot";
const char* password = "12345678"; // Şifre en az sekiz haneli olmalı.

const byte IN1 = D0, IN2 = D1, IN3 = D2, IN4 = D3; // PWM pinleri. (enA ve enB'de jumper takılı).

int hiz = 150; 
char karakter;

static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>Wifi Robot</title>
<style>
"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }"
#JD {
  text-align: center;
}
#JD {
  text-align: center;
  font-family: "Lucida Sans Unicode", "Lucida Grande", sans-serif;
  font-size: 24px;
}
.foot {
  text-align: center;
  font-family: "Comic Sans MS", cursive;
  font-size: 30px;
  color: #F00;
}
.button {
    border: none;
    color: white;
    padding: 17px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
    margin: 4px 2px;
    cursor: pointer;
    border-radius: 12px;
  width: 100%;
}
.red {background-color: #F00;}
.green {background-color: #090;}
.yellow {background-color:#F90;}
.blue {background-color:#03C;}
</style>
<script>
var websock;
function start() {
  websock = new WebSocket('ws://' + window.location.hostname + ':81/');
  websock.onopen = function(evt) { console.log('websock open'); };
  websock.onclose = function(evt) { console.log('websock close'); };
  websock.onerror = function(evt) { console.log(evt); };
  websock.onmessage = function(evt) {
    console.log(evt);
    var e = document.getElementById('ledstatus');
    if (evt.karakter === 'ledon') {
      e.style.color = 'red';
    }
    else if (evt.karakter === 'ledoff') {
      e.style.color = 'black';
    }
    else {
      console.log('unknown event');
    }
  };
}
function buttonclick(e) {
  websock.send(e.id);
}
</script>
</head>
<body onload="javascript:start();">
&nbsp;
<table width="100%" border="0">
  <tr>
    <td bgcolor="#FFFFFF" id="JD">Wifi Robot </td>
  </tr>
</table>
<table width="100" height="249" border="0" align="center">
    <td>&nbsp;</td>
    <td align="center" valign="middle"><form name="form1" method="post" action="">
      <label>
        <button id="a"  type="button" onclick="buttonclick(this);" class="button green">ileri</button> 
      </label>
    </form></td>
    <td>&nbsp;</td>
  </tr>   
    <td align="center" valign="middle"><form name="form1" method="post" action="">
      <label>
        <button id="c"  type="button" onclick="buttonclick(this);" class="button green">Sol  </button> 
      </label>
    </form></td>
    <td align="center" valign="middle"><form name="form1" method="post" action="">
      <label>
        <button id="e"  type="button" onclick="buttonclick(this);" class="button red">Dur  </button> 
      </label>
    </form></td>
    <td align="center" valign="middle"><form name="form1" method="post" action="">
      <label>
        <button id="d"  type="button" onclick="buttonclick(this);" class="button green">Sag  </button> 
      </label>
    </form></td>
  </tr>
   <td>&nbsp;</td>
    <td align="center" valign="middle"><form name="form1" method="post" action="">
      <label>
        <button id="b"  type="button" onclick="buttonclick(this);" class="button green">Geri </button> 
      </label>
    </form></td>     
    <td>&nbsp;</td>
  </tr>  
</table>
</body>
</html>
)rawliteral";

WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);

void setup() {
 Serial.begin(9600);

 pinMode(IN1, OUTPUT);
 pinMode(IN2, OUTPUT);
 pinMode(IN3, OUTPUT);
 pinMode(IN4, OUTPUT);

  WiFi.mode(WIFI_AP);           // Access point modunda.
  WiFi.softAP(ssid, password);  // Kablosuz ağı başlat.

  IPAddress IP = WiFi.softAPIP(); 

  server.on("/", [](){
  server.send(200, "text/html", INDEX_HTML);
  });
  
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketFonksiyonu);
}

void loop() {

if (karakter == 'a') { // ileri.
      analogWrite(IN1, hiz);
      analogWrite(IN2, 0);
      analogWrite(IN3, hiz);
      analogWrite(IN4, 0);
    } else if (karakter == 'b') { // Geri.
      analogWrite(IN1, 0);
      analogWrite(IN2, hiz);
      analogWrite(IN3, 0);
      analogWrite(IN4, hiz);
    } else if (karakter == 'c') { // Sol.
      analogWrite(IN1, hiz);
      analogWrite(IN2, 0);
      analogWrite(IN3, 0);
      analogWrite(IN4, hiz);
    } else if (karakter == 'd') { // Sağ.
      analogWrite(IN1, 0);
      analogWrite(IN2, hiz);
      analogWrite(IN3, hiz);
      analogWrite(IN4, 0);
    } else if (karakter == 'e') { // Dur.
      analogWrite(IN1, 0);
      analogWrite(IN2, 0);
      analogWrite(IN3, 0);
      analogWrite(IN4, 0);
    }
     
   webSocket.loop();
   server.handleClient();
}

void webSocketFonksiyonu(byte sayi, WStype_t type,byte * veri, size_t length){
  switch(type) {
    case WStype_DISCONNECTED:
      break;
    case WStype_CONNECTED:
      {IPAddress ip = webSocket.remoteIP(sayi);}
      break;
    case WStype_TEXT:
      karakter = veri[0];
      Serial.println(karakter);
      webSocket.broadcastTXT(veri, length);     
      break;
    case WStype_BIN:
      hexdump(veri, length);
      webSocket.sendBIN(sayi, veri, length);
      break;
      default:
      break;
  }
}
