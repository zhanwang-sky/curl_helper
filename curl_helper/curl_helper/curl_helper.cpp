//
//  curl_helper.cpp
//  curl_helper
//
//  Created by Ji Chen on 2023/7/12.
//

#include <cstring>
#include <curl/curl.h>
#include "curl_helper.hpp"

using namespace curl_helper;

static std::size_t simple_http_write_callback(char* ptr,
                                              size_t size, size_t nmemb,
                                              void* userdata) {
  simple_http_resp_t* p_resp = (simple_http_resp_t*) userdata;
  std::size_t realsize = size * nmemb;
  std::size_t cpsize = (realsize > (p_resp->buf_sz - p_resp->real_sz))
                       ? (p_resp->buf_sz - p_resp->real_sz) : realsize;
  memcpy(p_resp->buf + p_resp->real_sz, ptr, cpsize);
  p_resp->real_sz += cpsize;
  if (cpsize < realsize) {
    p_resp->overflow = 1;
  }
  return realsize;
}

int curl_helper::simple_http_query(const char* url,
                                   const char* headers, int nr_headers,
                                   const char* data, int data_sz,
                                   simple_http_resp_t* p_resp,
                                   int connect_timeout_ms, int query_timeout_ms) {
  CURL* curl = NULL;
  struct curl_slist* slist = NULL;
  const char* p_header = headers;
  long response_code = 0;
  int ret = 0;

  if (!(curl = curl_easy_init())) {
    ret = -1;
    goto exit;
  }

  for (int i = 0; i != nr_headers && p_header && *p_header; ++i) {
    struct curl_slist* tmp_slist = curl_slist_append(slist, p_header);
    if (!tmp_slist) {
      ret = -2;
      goto exit;
    }
    slist = tmp_slist;
    p_header += strlen(p_header) + 1;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  if (slist) {
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
  }
  if (data && data_sz > 0) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_sz);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
  }
  if (p_resp) {
    p_resp->real_sz = 0;
    p_resp->overflow = 0;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, simple_http_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) p_resp);
  }
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, connect_timeout_ms);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, query_timeout_ms);
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

  if (curl_easy_perform(curl) != CURLE_OK) {
    ret = -3;
    goto exit;
  }

  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

  ret = (int) response_code;

exit:
  if (slist) {
    curl_slist_free_all(slist);
  }

  if (curl) {
    curl_easy_cleanup(curl);
  }

  return ret;
}
