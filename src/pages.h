#ifndef _PAGES_H
#define _PAGES_H
#include <string.h>

String body_page_wifi_scan(String red){
    String body = "<body>";
    body+="    <div class='center container border-gradient'>";
    body+="    <center><h1>Ingrese Wifi</h1></center>";
    body+="    <form action='guardar_conf' method='get'>";
    body+="    <center>SSID <select placeholder='Nombre Wifi' name='ssid' type='text'>";
    body+="        <option value='a'>Selecione WiFi</option>";
    body+=red;
    body+="    </select>";
    body+="    </center>";
    body+="    <br>";
    body+="    <center>PASS <input placeholder='Password' name='pass' type='password'></center><br>";
    body+="    <center><input class='btn' type='submit' value='GUARDAR'/></center><br>";
    body+="    </form>";
    
    return body;
}

String footer_html = "    </div>"
"    <footer>"
"        <p>Dev: Cristian Carreras</p>"
"    </footer>   "
"</body>"
""
"</html>";

String header_html = "<!DOCTYPE html>"
"<html lang='es'>"
"<head>"
"<meta charset=' UTF-8 '>"
"    <meta http-equiv='X-UA-Compatible' content='IE=edge'>"
"    <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"    <title>Temperatura & Humedad</title>"
"    <style>"
"        * {"
"            margin: 0;"
"            padding: 0;"
"            color: rgb(93, 190, 214);"
"            margin-top: 10px;"
"        }"
"        body {"
"            font-family: Arial, Helvetica, sans-serif;"
"            font-size: 14px;"
"            background-color: #333333;"
"        }"
"        h1 {"
"            "
"            text-align: center;"
"        }"
"        .container {"
"            width: 50%;"
"            height: 100%;"
"            display: flex;"
"            flex-direction: column;"
"            justify-content: center;"
"            align-items: center;"
"            margin: auto;"
"            border-radius: 10px;"
"        }"
"        .title {"
"            border: 1px solid;"
"            border-image-slice: 1;"
"            border-image-source: linear-gradient(to left, #54d0d4, #9ad53a);"
"            border-radius: 5px;"
"            font-size: large;"
"            text-align: center;"
"            width: 80%;"
"            margin: 5%;"
""
"        }"
"        .content-table {"
"            font-size:x-large;"
"            margin-bottom: 5%;"
"        }"
"        .border-gradient {"
"        border: 5px solid;"
"        border-image-slice: 1;"
"        border-image-source: linear-gradient(to left, #54d0d4, #9ad53a);"
"        }"
"        footer{"
"            position: absolute;"
"            bottom: 0;"
"            width: 100%;"
"            height: 5%;"
"            text-align: center;"
"            font-size: small;"
"        }"
"        p{"
"            color:rgb(93, 190, 214);"
"        }"
"        .emoji{"
"            font-size: 200%;"
"        }"
"        #value-sensor{"
"            font-weight: bold;"
"            font-size: xx-large;"
"        }"
"       tr:nth-child(odd){"
"            background-color:  #474747;"
"        }"
""
"    </style>"
"</head>";

String body_page_tempHum(int temp, int hum){
    String body ="<body>";
    body +="    <div class='container border-gradient'>";
    body +="        <div class='content-table'>";
    body +="            <br>";
    body +="            <span class='emoji'>&#127777</span>";
    body +="                <span> Temp </span>";
    body +="                <span id='value-sensor'> ";
    body += String(temp);
    body +="°C</span>";
    body +="                <br>";
    body +="                <br>";
    body +="                <span class='emoji'>&#128167</span>";
    body +="                <span> Humedad </span>";
    body +="                <span id='value-sensor'> ";
    body +=String(hum);
    body +="%</span>";
    body +="       </div>";
    body +="    </div>";
    return body;
}

#endif