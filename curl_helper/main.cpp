//
//  main.cpp
//  curl_helper
//
//  Created by Ji Chen on 2023/7/12.
//

#include <iostream>
#include "curl_helper.hpp"

using std::cout;
using std::cerr;
using std::endl;

char buf[100];

int main(int argc, char* argv[]) {
  int rc = 0;
  curl_helper::simple_http_resp_t resp = {0};

  resp.buf = buf;
  resp.buf_sz = sizeof(buf);

  rc = curl_helper::simple_http_query("https://www.google.com",
                                      NULL, 0, // headers
                                      NULL, 0, // post data
                                      &resp,   // response data
                                      0, 0);   // timeouts

  cout << "status code: " << rc << endl;

  if (rc > 0) {
    cout << "response data:";
    if (resp.overflow) {
      cout << " (overflow)";
    }
    cout.operator<<(endl).write(buf, resp.real_sz).operator<<(endl);
  }

  return 0;
}
