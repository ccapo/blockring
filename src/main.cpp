/**
 * blockring
 *
 * Author: Chris Capobianco
 * Date: 2020-10-19
 */
#include "main.hpp"

// Signal Handler
void signalHandler(int signum) {
  void *array[10];
  size_t size;

  std::cerr << "Interrupt signal (" << signum << ") received" << std::endl;

  // Print stack trace in event of segmentation fault
  if (signum == SIGSEGV) {
    // Get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // Print out all the frames to stderr
    std::cerr << "Error: signal " << signum << std::endl;
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
  }
  exit(signum);
}

int main(int argc, char *argv[]) {
  int status = Global::Ok;

  // Trigger execution of signalHandler if we receive these interrupts
  signal(SIGHUP, signalHandler);
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  signal(SIGPIPE, signalHandler);
  signal(SIGSEGV, signalHandler);

  // Disable buffering to STDOUT
  std::cout.setf(std::ios::unitbuf);
  std::cerr.setf(std::ios::unitbuf);

  // Create Command Instance
  Command cmd;

  if (Global::Debug) std::cout << ">>> Debug Mode <<<" << std::endl;

  // Connect to DB
  cmd.connectDB();

  // Create Logger and set database filename
  if (argc == 2) {
    if (std::string(argv[1]) == "version") {
      // Print Program Header
      std::cout << "blockring v" << Global::Major << "." << Global::Minor << "." << Global::Revision << std::endl;
    } else if (std::string(argv[1]) == "help") {
      Global::usage();
    } else if (std::string(argv[1]) == "newring") {
      status = cmd.newRing();
    } else {
      std::cerr << "Unrecognized command line option: '" << argv[1] << "'" << std::endl;
      status = Global::Warn;
    }
  } else if (argc == 3) {
    if (std::string(argv[1]) == "getring") {
      status = cmd.getRing(std::string(argv[2]));
    } else if (std::string(argv[1]) == "closering") {
      status = cmd.closeRing(std::string(argv[2]));
    } else if (std::string(argv[1]) == "purgering") {
      status = cmd.purgeRing(std::string(argv[2]));
    } else if (std::string(argv[1]) == "purgeblock") {
      status = cmd.purgeBlock(std::string(argv[2]));
    } else {
      // Abort if unrecognized arguments are provided
      std::cerr << "Unrecognized command line options: '" << argv[1] << "', '" << argv[2] << "'" << std::endl;
      status = Global::Warn;
    }
  } else if (argc == 4) {
    if (std::string(argv[1]) == "newblock") {
      status = cmd.newBlock(std::string(argv[2]), std::string(argv[3]));
    } else {
      // Abort if unrecognized arguments are provided
      std::cerr << "Unrecognized command line options: '" << argv[1] << "', '" << argv[2] << "', '" << argv[3] << "'" << std::endl;
      status = Global::Warn;
    }
  } else {
    // Abort if incorrect number of arguments are provided
    std::cerr << "Incorrect number of command line options" << std::endl;
    status = Global::Warn;
  }

  // Disconnect from DB
  cmd.disconnectDB();
  
  if (status == Global::Warn) {
    Global::usage();
  }

  return status;
}