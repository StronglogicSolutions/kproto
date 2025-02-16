#include "include/kproto/ipc.hpp"

int main(int argc, char** argv)
{

  auto msg = kiq::platform_request{"kiq", "1234", "logicp", "hello", "{\"key\": \"value\"}"};
  return 0;
}

