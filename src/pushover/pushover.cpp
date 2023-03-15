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

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "log.hpp"

using namespace std;

/** @brief Parse json response, got from libcurl
 */
static size_t curl_process(void *contents, size_t size, size_t nmemb,
      std::string *curl_response) {
   size_t realsize = size * nmemb;

   // congatenate response
   curl_response->append((char*) contents, realsize);
   //tcout() << "Response: " << *curl_response << endl;

   return realsize;
}

string push_emergency(const char* title, const char* message, const char* priority, const char* retry,
      const char* expire, const char* key, const char* token, const char* device) {
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
      post_data += "&priority=";
      post_data += priority;
      post_data += "&retry=";
      post_data += retry;
      post_data += "&expire=";
      post_data += expire;
      if (device) {
         post_data += "&device=";
         post_data += device;
      }
      post_data += "&title=";
      post_data += title;
      post_data += "&message=";
      post_data += message;

      tcout() << "Request: " << post_data << endl;

      // The pushover response is parsed into a rapidjson::Document
      std::string response;

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
      rapidjson::Document document;
      res = curl_easy_perform(curl);
      document.Parse(response.c_str());
      /* Check for errors */
      if (res != CURLE_OK) {
         tcerr() << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
      } else if(document.HasParseError()) {
         tcerr() << "document has parse error" << endl;
      } else {
         // evaluate json document, extract receipt
         if (!document.HasMember("status") || document["status"].GetInt() != 1) {
            if (document.HasMember("errors")) {
               const rapidjson::Value& errors = document["errors"];
               for(rapidjson::Value::ConstValueIterator itr = errors.Begin(); itr != errors.End(); ++itr) {
                  tcout() << "push_emergency(): " << itr->GetString() << endl;
               }
            } else {
               tcout() << "push_emergency(): Unable to access pushover.net" << endl;
            }
         }
         if (document.HasMember("receipt")) {
            receipt = document["receipt"].GetString();
            tcout() << "Receipt: " << receipt << endl;
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

      tcout() << "Request: " << get_data << "?" << post_data << endl;

      // The pushover response is parsed into a rapidjson::Document
      std::string response;

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
      rapidjson::Document document;
      res = curl_easy_perform(curl);
      document.Parse(response.c_str());
      /* Check for errors */
      if (res != CURLE_OK) {
         tcerr() << "curl_easy_perform() failed: " << curl_easy_strerror(res)
               << endl;
      } else if(document.HasParseError()) {
         tcerr() << "document has parse error" << endl;
      }else {
         // evaluate json document, extract receipt
         if (!document.HasMember("status") || document["status"].GetInt() != 1) {
            if (document.HasMember("errors")) {
               const rapidjson::Value& errors = document["errors"];
               for(rapidjson::Value::ConstValueIterator itr = errors.Begin(); itr != errors.End(); ++itr) {
                  tcout() << "cancel_emergency(): " << itr->GetString() << endl;
               }
            } else {
               tcout() << "cancel_emergency(): Unable to access pushover.net" << endl;
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

      tcout() << "Request: " << get_data << endl;

      // The pushover response is parsed into a rapidjson::Document
      std::string response;

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
      rapidjson::Document document;
      res = curl_easy_perform(curl);
      document.Parse(response.c_str());
      /* Check for errors */
      if (res != CURLE_OK) {
         tcerr() << "curl_easy_perform() failed: " << curl_easy_strerror(res)
               << endl;
      } else if(document.HasParseError()) {
         tcerr() << "document has parse error" << endl;
      }else {
         // evaluate json document, extract receipt
         if (!document.HasMember("status") || document["status"].GetInt() != 1) {
            if (document.HasMember("errors")) {
               const rapidjson::Value& errors = document["errors"];
               for(rapidjson::Value::ConstValueIterator itr = errors.Begin(); itr != errors.End(); ++itr) {
                  tcout() << "poll_receipt(): " << itr->GetString() << endl;
               }
            } else {
               tcout() << "poll_receipt(): Unable to access pushover.net" << endl;
            }
         }
         if (document.HasMember("acknowledged")
               && (document["acknowledged"].GetInt() == 1)) {
            tcout() << "poll_receipt(): emergency acknowledged" << endl;
            acknowledged = true;
         }
         if (document.HasMember("expired")
               && (document["expired"].GetInt() == 1)) {
            tcout() << "poll_receipt(): emergency expired" << endl;
         }
      }

      /* always cleanup */
      curl_easy_cleanup(curl);
   }
   curl_global_cleanup();

   return acknowledged;
}
