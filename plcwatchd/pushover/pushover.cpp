/*
 * pushover.cpp
 *
 *  Created on: 13.02.2017
 *      Author: CBe
 */

#include <string>
#include <iostream>
#include <curl/curl.h>
#include "pushover.h"
#include "Jzon.hpp"

using namespace std;

/** @brief Parse json response, got from libcurl
 */
static size_t curl_process(void *contents, size_t size, size_t nmemb,
      Jzon::Node *node) {
   size_t realsize = size * nmemb;

   string response;
   response.append((char*) contents, realsize);
//   cout << "Response: " << response << endl;

   Jzon::Parser json;
   *node = json.parseString(response);

   return realsize;
}

string push_emergency(const char* retry, const char* expire,
      const char* key, const char* token) {
   string receipt;
   CURL *curl;
   CURLcode res;

   /* In windows, this will init the winsock stuff */
   curl_global_init(CURL_GLOBAL_ALL);

   /* get a curl handle */
   curl = curl_easy_init();
   if (curl) {
      string post_data = "token=";
      post_data += token;
      post_data += "&user=";
      post_data += key;
      post_data += "&priority=2";
      post_data += "&retry=";
      post_data += retry;
      post_data += "&expire=";
      post_data += expire;
      post_data += "&title=Homeautomation crashed";
      post_data += "&message=Acknowledge this message to request RUN.";

      cout << "Request: " << post_data << endl;

      // The pushover response is parsed into a Jzon::Node
      Jzon::Node response;

      /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
      curl_easy_setopt(curl, CURLOPT_URL,
            "https://api.pushover.net/1/messages.json");
      /* Now specify the POST data */
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
      /* Set a function that will be called to store the output */
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_process);
      /* set the curl_process parameter */
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
      /* complete connection within 10 seconds */
      curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if (res != CURLE_OK) {
         cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
               << endl;
      } else {
         // evaluate json response, extract receipt
         if (!response.has("status") || response.get("status").toInt() != 1) {
            if (response.has("errors")) {
               Jzon::Node errors = response.get("errors");
               for (Jzon::Node::iterator it = errors.begin();
                     it != errors.end(); ++it) {
                  cout << "push_emergency(): " << it->second.toString() << endl;
               }
            } else {
               cout << "push_emergency(): Unable to access pushover.net"
                     << endl;
            }
         }
         if (response.has("receipt")) {
            receipt = response.get("receipt").toString();
            cout << "Receipt: " << receipt << endl;
         }
      }

      /* always cleanup */
      curl_easy_cleanup(curl);
   }
   curl_global_cleanup();

   return receipt;
}

void cancel_emergency(const string& receipt, const char* token) {
   CURL *curl;
   CURLcode res;

   /* In windows, this will init the winsock stuff */
   curl_global_init(CURL_GLOBAL_ALL);

   /* get a curl handle */
   curl = curl_easy_init();
   if (curl) {
      string get_data = "https://api.pushover.net/1/receipts/" + receipt
            + "/cancel.json";
      string post_data = "token=";
      post_data += token;

      cout << "Request: " << get_data << "?" << post_data << endl;

      // The pushover response is parsed into a Jzon::Node
      Jzon::Node response;

      /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
      curl_easy_setopt(curl, CURLOPT_URL, get_data.c_str());
      /* Now specify the POST data */
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
      /* Set a function that will be called to store the output */
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_process);
      /* set the curl_process parameter */
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
      /* complete connection within 10 seconds */
      curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if (res != CURLE_OK) {
         cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
               << endl;
      } else {
         // evaluate json response, extract receipt
         if (!response.has("status") || response.get("status").toInt() != 1) {
            if (response.has("errors")) {
               Jzon::Node errors = response.get("errors");
               for (Jzon::Node::iterator it = errors.begin();
                     it != errors.end(); ++it) {
                  cout << "cancel_emergency(): " << it->second.toString()
                        << endl;
               }
            } else {
               cout << "cancel_emergency(): Unable to access pushover.net"
                     << endl;
            }
         }
      }

      /* always cleanup */
      curl_easy_cleanup(curl);
   }
   curl_global_cleanup();
}

bool poll_receipt(const string& receipt, const char* token) {
   bool acknowledged = false;

   CURL *curl;
   CURLcode res;

   /* In windows, this will init the winsock stuff */
   curl_global_init(CURL_GLOBAL_ALL);

   /* get a curl handle */
   curl = curl_easy_init();
   if (curl) {
      string get_data = "https://api.pushover.net/1/receipts/" + receipt
            + ".json?token=";
      get_data += token;

      cout << "Request: " << get_data << endl;

      // The pushover response is parsed into a Jzon::Node
      Jzon::Node response;

      /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
      curl_easy_setopt(curl, CURLOPT_URL, get_data.c_str());
      /* Set a function that will be called to store the output */
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_process);
      /* set the curl_process parameter */
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
      /* complete connection within 10 seconds */
      curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if (res != CURLE_OK) {
         cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
               << endl;
      } else {
         // evaluate json response, extract receipt
         if (!response.has("status") || response.get("status").toInt() != 1) {
            if (response.has("errors")) {
               Jzon::Node errors = response.get("errors");
               for (Jzon::Node::iterator it = errors.begin();
                     it != errors.end(); ++it) {
                  cout << "poll_receipt(): " << it->second.toString() << endl;
               }
            } else {
               cout << "poll_receipt(): Unable to access pushover.net" << endl;
            }
         }
         if (response.has("acknowledged")
               && (response.get("acknowledged").toInt() == 1)) {
            cout << "poll_receipt(): emergency acknowledged" << endl;
            acknowledged = true;
         }
         if (response.has("expired")
               && (response.get("expired").toInt() == 1)) {
            cout << "poll_receipt(): emergency expired" << endl;
         }
      }

      /* always cleanup */
      curl_easy_cleanup(curl);
   }
   curl_global_cleanup();

   return acknowledged;
}
