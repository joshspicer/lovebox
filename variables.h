#include <Arduino.h>

const char *ssid = "<TODO>";
const char *password = "<TODO>";
String me = "1";
String peer = "2";
String baseAddress = "http://<TODO>:3000";

String getMeAddress = baseAddress + "/?user=" + me;
String provisionMeAddress = baseAddress + "/provision?user=" + me;
String postPeerAddress = baseAddress + "/?user=" + peer;
