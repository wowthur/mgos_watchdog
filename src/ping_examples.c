// extern "C" {
// #include "ping.h"
// }

// #include "os_type.h"
// #include <ESP8266WiFi.h>
 
// ping_option pingOps; // Struct used by ping_start, see ping.h
// static os_timer_t ping_timer;

// // Set all your options here:
// #define MY_SSID "your_WiFi_SSID_goes_here"
// #define PASSWORD "your_password_goes_here"
// #define PING_DELAY_S 1 //amount of time between pings and before a ping times out, sec
// #define PING_COUNT 4 //count of pings per request
// #define DELAY 3000 // Time to wait after completing a full ping request (doing PING_COUNT pings) and before the next one
// IPAddress PING_TARGET(192,168,0,1); // The IP address you're trying to ping

// // This function is called when ping_timer fires:
// static void ICACHE_FLASH_ATTR ping_cb(void *arg)
// {
//   os_timer_disarm(&ping_timer);
//   Serial.println("---");
//   Serial.print("Pinging: ");
//   Serial.print(PING_TARGET);
//   Serial.println(" with 32 bytes of data:");
//   ping_start(&pingOps);
// }

// // This function is called when a ping is received or the request times out:
// static void ICACHE_FLASH_ATTR ping_recv (void* arg, void *pdata)
// {
//   struct ping_resp *pingrsp = (struct ping_resp *)pdata;

//   if (pingrsp->bytes > 0)
//   {
//     Serial.print("Reply from: ");
//     Serial.print(PING_TARGET);
//     Serial.print(": ");
//     Serial.print("bytes=");
//     Serial.print(pingrsp->bytes);
//     Serial.print(" time=");
//     Serial.print(pingrsp->resp_time);
//     Serial.println("ms");
//   }
//   else
//   {
//     Serial.println("Request timed out");
//   }
// }

// // This function is called after the ping request is completed
// // (i.e., after PING_COUNT pings are done)
// static void ICACHE_FLASH_ATTR ping_sent (void* arg, void *pdata)
// {
//   struct ping_resp *pingrsp = (struct ping_resp *)pdata;

//   Serial.println();
//   Serial.print("Ping statistics for: ");
//   Serial.println(PING_TARGET);
//   Serial.print("Packets: Sent = ");
//   Serial.print(pingrsp->total_count);
//   Serial.print(", Recieved = ");
//   Serial.print(pingrsp->total_count-pingrsp->timeout_count);
//   Serial.print(", Lost = ");
//   Serial.print(pingrsp->timeout_count);
//   Serial.print(" (");
//   Serial.print(float(pingrsp->timeout_count)/pingrsp->total_count*100);
//   Serial.println("% loss)");

//   // Ping request complete, rearm the timer so that a new request is sent in DELAY seconds
//   os_timer_arm(&ping_timer, DELAY, 1);
// }

// // Function for connecting to the designated WiFi SSID:
// void connectToWifi() {
//   WiFi.begin(MY_SSID, PASSWORD);
//   Serial.println("");
 
//   // Wait for connection
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
 
//   Serial.println("");
//   Serial.print("Connected to ");
//   Serial.println(MY_SSID);
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP());
// }

// void setup() {
//   Serial.begin(115200);
//   connectToWifi();

//   // Setup the ping options:
//   pingOps.count = PING_COUNT;
//   pingOps.ip = uint32_t(PING_TARGET);
//   pingOps.coarse_time = PING_DELAY_S;
//   ping_regist_sent(&pingOps, ping_sent);
//   ping_regist_recv(&pingOps, ping_recv);

//   os_timer_disarm(&ping_timer);
//   os_timer_setfn(&ping_timer, (os_timer_func_t *)ping_cb, (void *)0);

//   // Timer ping_timer will fire after DELAY seconds elapse, calling function ping_cb
//   // The timer is disarmed in the first line of ping_cb, so it doesn't actually fire every DELAY seconds continiously
//   os_timer_arm(&ping_timer, DELAY, 1);
// }

// void loop() {
//   // Anything else you want to do!
// }
