#ifndef WEBPAGE_H_
#define WEBPAGE_H_

// DO NOT AUTO FORMAT THIS
static const char webPageHTML[] =
    "<html>\n"
    "<head>\n"
    "<style type=\"text/css\">\n"
    "body {\n"
    "font-family: sans-serif;\n"
    "margin: 50px;\n"
    "padding: 20px;\n"
    "line-height: 250%\n"
    "}\n"
    "</style>\n"
    "<title>Arduino Setup</title>\n"
    "</head>\n"
    "<body>\n"
    "<h2>WIFI CREDENTIALS</h2>\n"
    "NETWORK NAME: <input id=\"network\"><br>PASSWORD: <input id=\"password\"><br><button type=\"button\" onclick=\"SendText()\">Enter</button>\n"
    "<script>\n"
    "var network = document.querySelector('#network');\n"
    "var password = document.querySelector('#password');\n"
    "function SendText() {\n"
    "nocache = \"&nocache=\" + Math.random() * 1000000;\n"
    "var request = new XMLHttpRequest();\n"
    "netText = \"&txt=\" + \"?\" + network.value + \"!\" + password.value + \",\" + \",&end=end\";\n"
    "request.open(\"GET\", \"ajax_inputs\" + netText + nocache, true);\n"
    "request.send(null)\n"
    "network.value = ''\n"
    "password.value = ''\n"
    "}\n"
    "</script>\n"
    "</body>\n"
    "</html>\n";

#endif /* WEBPAGE_H_ */
