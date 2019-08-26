/*
 * pushover.h
 *
 *  Created on: 13.02.2017
 *      Author: CBe
 */

#ifndef PUSHOVER_PUSHOVER_H_
#define PUSHOVER_PUSHOVER_H_

#include <string>

/** @brief Send an pushover emergency
 * @param title your message's title
 * @param message your message
 * @param priority send as -2 to generate no notification/alert, -1 to always send as a quiet notification, 
 *        1 to display as high-priority and bypass the user's quiet hours, or 2 to also require confirmation from the user
 * @param retry the pushover.net retry parameter
 * @param expire the pushover.net expire parameter
 * @param key the pushover.net user key
 * @param token the pushover.net application token
 * @param device the pushover.net device list e.g "dev1,dev2" or NULL for all devices
 */
std::string push_emergency(const char* title, const char* message, const char* priority, const char* retry, 
      const char* expire, const char* key, const char* token, const char* device);
/** @brief Cancel an emergency
 * @param receipt emergency receipt to poll for
 */
void cancel_emergency(const std::string& receipt, const char* token);
/** @brief Poll for acknowledgement
 * @param receipt emergency receipt to poll for
 * @param token the pushover.net application token
 * @return true if notification was acknowledged otherwise false
 */
bool poll_receipt(const std::string& receipt, const char* token);

#endif /* PUSHOVER_PUSHOVER_H_ */
