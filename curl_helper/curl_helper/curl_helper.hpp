//
//  curl_helper.hpp
//  curl_helper
//
//  Created by Ji Chen on 2023/7/12.
//

#ifndef curl_helper_hpp
#define curl_helper_hpp

namespace curl_helper {

typedef struct {
  char* buf;
  int buf_sz;
  int real_sz;
  int overflow;
} simple_http_resp_t;

/**
 @param url escaped URL
 @param p_resp response data to save. if NULL, it will be printed to stdout.
 @param connect_timeout_ms Default 300000ms.
 @param query_timeout_ms Default timeout is 0 (zero) which means it never times out during transfer.
 @retval negative: internal error, positive: http response code
 */
int simple_http_query(const char* url,
                      const char* headers, int nr_headers,
                      const char* data, int data_sz,
                      simple_http_resp_t* p_resp,
                      int connect_timeout_ms, int query_timeout_ms);

} /* namespace curl_helper */

#endif /* curl_helper_hpp */
