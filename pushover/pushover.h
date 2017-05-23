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
 * @param retry the pushover.net retry parameter
 * @param expire the pushover.net expire parameter
 * @param key the pushover.net user key
 * @param token the pushover.net application token
 * @param emergency receipt
 */
std::string push_emergency(const char* retry, const char* expire, const char* key,
      const char* token);
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
