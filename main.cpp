#include "include/kproto/ipc.hpp"
#include <iostream>

int main(int argc, char** argv)
{

  auto msg = kiq::platform_request{"kiq", "1234", "logicp", "hello", "{\"key\": \"value\"}"};

  std::cout << msg.to_string() << std::endl;
  return 0;
}

