#include "pubnub.hpp"

#include <iostream>
#include <exception>

const std::string channel_group("family");

int main() {
  pubnub::context pn("demo", "demo");
  // This keys causes 403 response from API:
  //pubnub::context pn("pub-c-a82961b5-d8ee-462c-bd16-0cd53edbbe09", "sub-c-f5674c7e-7d52-11e3-a993-02ee2ddab7fe");
  enum pubnub_res res;

  try {
    res = pn.list_channel_group(channel_group).await();

    if (PNR_OK == res) {
      std::cout << "Success: " << res << std::endl;
      std::vector<std::string> msg = pn.get_all();

      for (std::vector<std::string>::iterator it = msg.begin(); it != msg.end(); ++it) {
        std::cout << *it << std::endl;
      }
    } else {
      std::cout << "Failed with code " << res << std::endl;
    }
  } catch (std::exception &ex) {
    std::cout << "Exception: " << ex.what() << std::endl;
  }

  return 0;
}

