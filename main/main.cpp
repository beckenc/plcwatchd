#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctime>
#include "snap7.h"
#include "pushover.h"
#include "log.hpp"

using namespace std;

static TS7Client s7Client;

/** @brief check the snap7 library function result and printout the related error message
 * @param result error code
 * @param function Name of the function the result is from
 */
static bool check(int result, const char * function) {
   if (result != 0) {
      if (result < 0) {
         tcerr() << function << ": library error (-1)" << endl;
      } else {
         tcerr() << function << ": " << CliErrorText(result) << endl;
      }
   }
   return result == 0;
}

/** @brief Cleanup Snap7 connection and exit
 * @param s Received signal
 */
static void signal_handler(int s) {
   tcout() << "SIG " << s << " (" << strsignal(s) << ") received!" << endl;
   if (s7Client.Connected()) {
      tcout() << "Disconnect from PLC" << endl;
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

   tcout() << "daemonize" << endl;

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
   i = open("/dev/null", O_RDWR);   // stdin
   if (dup(i) < 0) {                // stdout
   }
   if (dup(i) < 0) {                // stderr
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
         << "  -p   polling rate of the PLC state, default 10s" << endl
         << "  -k   key - pushover.net user key" << endl
         << "  -t   token - pushover.net appliacation token" << endl
         << "  -c   retry - pushover.net retry parameter, default 60s" << endl
         << "  -e   expire - pushover.net expire parameter, default 600" << endl
         << "  -u   user - pushover.net device list e.g. dev1,dev2, default all devices" << endl
         << "  -i   ip - address of the plc" << endl
         << "  -r   rack - rack of the plc, default 0" << endl
         << "  -s   slot - slot of the plc, default 2" << endl
         << "  -l   logfile" << endl;
}

int main(int argc, char *argv[]) {

   int rack = 0;
   int slot = 2;
   int pollingRate = 10; //seconds
   const char* retry = "60";
   const char* expire = "600";
   char* ip = NULL;
   char* key = NULL;
   char* token = NULL;
   char* device = NULL;
   const char* logfile = "/dev/null";
   int option = 0;
   bool daemon = false;
   bool verbose = false;

   // catch signals to close established Snap7 client connection ...
   signal(SIGABRT, &signal_handler);
   signal(SIGTERM, &signal_handler);
   signal(SIGINT, &signal_handler);

   if (argc <= 1) {
      usage();
      return EXIT_FAILURE;
   }

   while ((option = getopt(argc, argv, "dvi:r:s:p:u:k:t:c:e:l:")) != -1) {
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
      case 'p':
         pollingRate = atoi(optarg);
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
      case 'l':
         logfile = optarg;
         break;
      case 'u':
         device = optarg;
         break;
      default:
         usage();
         return EXIT_FAILURE;
      }
   }

   // mandatory parameters available?
   if (rack == -1 || slot == -1 || !ip || !key || !token) {
      usage();
      return EXIT_FAILURE;
   }

   // daemonize process and run forever
   if (daemon) {
      daemonize();
   }

   if (!verbose || daemon) {
      // redirect tcout / cerr to logfile
      [[maybe_unused]] auto f_stdout = freopen(logfile, "a", stdout);
      [[maybe_unused]] auto f_stderr = freopen(logfile, "a", stderr);
   }

   tcout() << "Start state polling every " << pollingRate << " seconds" << endl;

   // start state polling every 'pollingRate' seconds
   while (1) {
      static bool notify_connect_error = true;
      static bool notify_connect_success = true;
      static bool notify_run = true;
      sleep(pollingRate);

      if (!check(s7Client.ConnectTo(ip, rack, slot), "s7Client.ConnectTo()")) {
         if(notify_connect_error) {
            tcout() << "S7 connection failed!" << endl;
            (void)push_emergency("Homeautomation system disconnected", "S7 connection failed", "1", retry, expire, key, token, device);
            notify_connect_error = false;
         }
         notify_connect_success = true;
         check(s7Client.Disconnect(), "s7Client.Disconnect()");
         continue;
      }
      // connection established
      if(notify_connect_success) {
         tcout() << "S7 connection established!" << endl;
         (void)push_emergency("Homeautomation system connected", "S7 connection established", "0", retry, expire, key, token, device);
         notify_connect_success = false;
      }
      notify_connect_error = true;

      if (S7CpuStatusStop == s7Client.PlcStatus()) {
         tcout() << "Plc state STOP." << endl;
         string receipt = push_emergency("Homeautomation system crashed", "Acknowlige to requst STARTUP", "2", retry, expire, key, token, device);
         bool acknowledged = false;

         if(receipt.empty()) {
            tcout() << "Error during pushing... retry" << endl;
            continue;
         }

         do {
            sleep(5); // respect API and wait 5 seconds
            tcout() << "Acknowledged?" << endl;
            acknowledged = poll_receipt(receipt, token);
         } while (!acknowledged && (S7CpuStatusStop == s7Client.PlcStatus()));

         if (acknowledged) {
            tcout() << "Acknowledged! Request RUN and re-arm watchdog." << endl;
            check(s7Client.PlcHotStart(), "s7Client.PlcColdStart()");
         } else {
            tcout() << "Left STOP. Cancel emergency and re-arm watchdog!" << endl;
            cancel_emergency(receipt, token);
         }
         notify_run = true;
      } else if(S7CpuStatusRun == s7Client.PlcStatus()) {
         if(notify_run) {
            tcout() << "Plc state RUN." << endl;
            (void)push_emergency("Homeautomation system alive", "PLC state RUN", "-1", retry, expire, key, token, device);
            notify_run = false;
         }
      }

      check(s7Client.Disconnect(), "s7Client.Disconnect()");
   }

   return EXIT_SUCCESS;
}
