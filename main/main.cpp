#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <csignal>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "snap7.h"
#include "pushover.h"

using namespace std;

static TS7Client s7Client;

/** @brief check the snap7 library function result and printout the related error message
 * @param result error code
 * @param function Name of the function the result is from
 */
static bool check(int result, const char * function) {
   if (result != 0) {
      if (result < 0) {
         cerr << function << ": library error (-1)" << endl;
      } else {
         cerr << function << ": " << CliErrorText(result) << endl;
      }
   }
   return result == 0;
}

/** @brief Cleanup Snap7 connection and exit
 * @param s Received signal
 */
static void signal_handler(int s) {
   cout << "SIG " << s << " received!" << endl;
   if (s7Client.Connected()) {
      cout << "Disconnect from PLC" << endl;
      check(s7Client.Disconnect(), "s7Client.Disconnect()");
   }
   exit(EXIT_FAILURE);
}

/** @brief Fork process to background
 *
 * @link http://www.enderunix.org/docs/eng/daemon.php
 */
void daemonize() {
   if (getppid() == 1) {
      return; /* already a daemon */
   }

   cout << "daemonize" << endl;

   int i = fork();
   if (i < 0) {
      exit(EXIT_FAILURE); /* fork error */
   } else if (i > 0) {
      exit(EXIT_SUCCESS); /* parent exits */
   }

   /* child (daemon) continues */
   setsid(); /* obtain a new process group */

   for (i = getdtablesize(); i >= 0; --i) {
      close(i); /* close all descriptors */
   }

   /* handle standart I/O */
   i = open("/dev/null", O_RDWR);
   if (dup(i) < 0) {
   }
   if (dup(i) < 0) {
   }

   if (chdir("/") < 0) {
   } /* change working directory */
   umask(0022);

   /* ignore signals from parent tty */
   struct sigaction action;
   sigemptyset(&action.sa_mask);
   action.sa_flags = 0;
   action.sa_handler = SIG_IGN;

   sigaction(SIGCHLD, &action, NULL); /* ignore child */
   sigaction(SIGTSTP, &action, NULL); /* ignore tty signals */
   sigaction(SIGTTOU, &action, NULL);
   sigaction(SIGTTIN, &action, NULL);
}

static void usage() {
   cout << "Usage: plcwatchd [-v -d] -k key -t token [-c sec -e sec ] "
         << "-i ip [-r num -s num]" << endl << endl
         << "  -v   verbose" << endl
         << "  -d   daemonize" << endl
         << "  -k   key - pushover.net user key" << endl
         << "  -t   token - pushover.net appliacation token" << endl
         << "  -c   retry - pushover.net retry parameter, default 60s" << endl
         << "  -e   expire - pushover.net expire parameter, default 600" << endl
         << "  -i   ip - address of the plc" << endl
         << "  -r   rack - rack of the plc, default 0" << endl
         << "  -s   slot - slot of the plc, default 2" << endl;
}

int main(int argc, char *argv[]) {

   int status = EXIT_SUCCESS;
   int rack = 0;
   int slot = 2;
   const char* retry = "60";
   const char* expire = "600";
   char* ip = NULL;
   char* key = NULL;
   char* token = NULL;
   int option = 0;
   bool daemon = false;
   bool verbose = false;

   //save cout / cerr stream buffer
   streambuf* strm_out_buffer = cout.rdbuf();
   streambuf* strm_err_buffer = cerr.rdbuf();
   ofstream file("/dev/null");

   // catch signals to close established Snap7 client connection ...
   signal(SIGABRT, &signal_handler);
   signal(SIGTERM, &signal_handler);
   signal(SIGINT, &signal_handler);

   if (argc <= 1) {
      usage();
      status = EXIT_FAILURE;
      goto ext;
   }

   while ((option = getopt(argc, argv, "dvi:r:s:k:t:c:e:")) != -1) {
      switch (option) {
      case 'v':
         verbose = true;
         break;
      case 'd':
         daemon = true;
         break;
      case 'i':
         ip = optarg;
         break;
      case 'r':
         rack = atoi(optarg);
         break;
      case 's':
         slot = atoi(optarg);
         break;
      case 'k':
         key = optarg;
         break;
      case 't':
         token = optarg;
         break;
      case 'c':
         retry = optarg;
         break;
      case 'e':
         expire = optarg;
         break;
      default:
         usage();
         status = EXIT_FAILURE;
         goto ext;
      }
   }

   // mandatory parameters available?
   if (rack == -1 || slot == -1 || !ip || !key || !token) {
      usage();
      status = EXIT_FAILURE;
      goto ext;
   }

   // daemonize process and run forever
   if (daemon) {
      daemonize();
   }

   if (!verbose || daemon) {
      // redirect cout / cerr to /dev/null
      cout.rdbuf(file.rdbuf());
      cerr.rdbuf(file.rdbuf());
   }

   // start state polling every 10 seconds
   while (1) {
      sleep(10);

      if (!check(s7Client.ConnectTo(ip, rack, slot), "s7Client.ConnectTo()")) {
         check(s7Client.Disconnect(), "s7Client.Disconnect()");
         continue;
      }

      if (S7CpuStatusStop == s7Client.PlcStatus()) {
         cout << "Plc state STOP." << endl;
         string receipt = push_emergency(retry, expire, key, token);
         bool acknowledged = false;

         do {
            sleep(5); // respect API and wait 5 seconds
            cout << "Acknowledged?" << endl;
            acknowledged = poll_receipt(receipt, token);
         } while (!acknowledged && (S7CpuStatusStop == s7Client.PlcStatus()));

         if (acknowledged) {
            cout << "Acknowledged! Request RUN and re-arm watchdog." << endl;
            check(s7Client.PlcHotStart(), "s7Client.PlcColdStart()");
         } else {
            cout << "Left STOP. Cancel emergency and re-arm watchdog!" << endl;
            cancel_emergency(receipt, token);
         }
      }

      check(s7Client.Disconnect(), "s7Client.Disconnect()");
   }

   ext: if (!verbose || daemon) {
      // restore cout stream buffer
      cout.rdbuf(strm_out_buffer);
      cerr.rdbuf(strm_err_buffer);
   }

   return status;
}
