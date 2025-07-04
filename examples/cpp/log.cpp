#include <iomanip>
#include <iostream>
#include <thread>

#include "rby1-sdk/log.h"
#include "rby1-sdk/model.h"
#include "rby1-sdk/robot.h"

using namespace rb;
using namespace std::chrono_literals;

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <server address>" << std::endl;
    return 1;
  }

  std::string address{argv[1]};
  std::cout << "Initializing robot at address: " << address << std::endl;

  auto robot = Robot<y1_model::A>::Create(address);

  std::cout << "Attempting to connect to the robot..." << std::endl;
  if (!robot->Connect()) {
    std::cerr << "Error: Unable to establish connection to the robot at " << address << std::endl;
    return 1;
  }
  std::cout << "Successfully connected to the robot." << std::endl;

  std::cout << "Get 5 last logs" << std::endl;
  const auto& logs = robot->GetLastLog(5);
  for (const auto& log : logs) {
    std::cout << log << std::endl;
  }

  std::cout << "Starting log streaming ..." << std::endl;
  robot->StartLogStream(
      [](const std::vector<Log>& logs) {
        for (const auto& log : logs) {
          std::cout << log << std::endl;
        }
      },
      1.0);

  std::this_thread::sleep_for(10s);

  robot->StopLogStream();

  return 0;
}