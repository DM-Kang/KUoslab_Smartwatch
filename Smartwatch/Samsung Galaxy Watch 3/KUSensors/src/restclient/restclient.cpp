/*
 * restclient.cpp
 *
 *  Created on: May 3, 2021
 *      Author: vincentkang
 */


#include "restclient/restclient.h"

#include <curl/curl.h>
#if __cplusplus >= 201402L
#include <memory>
#endif

//#include "restclient-cpp/version.h"
#include "restclient/connection.h"

/**
 * @brief global init function. Call this before you start any threads.
 */
int RestClient::init() {
	CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
	if (res == CURLE_OK) {
		return 0;
	} else {
		return 1;
	}
}

/**
 * @brief global disable function. Call this before you terminate your
 * program.
 */
void RestClient::disable() {
	curl_global_cleanup();
}

/**
 * @brief HTTP GET method
 *
 * @param url to query
 *
 * @return response struct
 */
RestClient::Response RestClient::get(const std::string& url) {
#if __cplusplus >= 201402L
	auto conn = std::make_unique<RestClient::Connection>("");
	return conn->get(url);
#else
	RestClient::Response ret;
	RestClient::Connection *conn = new RestClient::Connection("");
	ret = conn->get(url);
	delete conn;
	return ret;
#endif
}

/**
 * @brief HTTP POST method
 *
 * @param url to query
 * @param ctype content type as string
 * @param data HTTP POST body
 *
 * @return response struct
 */
RestClient::Response RestClient::post(const std::string& url,
		const std::string& ctype,
		const std::string& data) {
#if __cplusplus >= 201402L
auto conn = std::make_unique<RestClient::Connection>("");
conn->AppendHeader("Content-Type", ctype);
return conn->post(url, data);
#else
RestClient::Response ret;
RestClient::Connection *conn = new RestClient::Connection("");
conn->AppendHeader("Content-Type", ctype);
ret = conn->post(url, data);
delete conn;
return ret;
#endif
}

/**
 * @brief HTTP PUT method
 *
 * @param url to query
 * @param ctype content type as string
 * @param data HTTP PUT body
 *
 * @return response struct
 */
RestClient::Response RestClient::put(const std::string& url,
		const std::string& ctype,
		const std::string& data) {
#if __cplusplus >= 201402L
auto conn = std::make_unique<RestClient::Connection>("");
conn->AppendHeader("Content-Type", ctype);
return conn->put(url, data);
#else
RestClient::Response ret;
RestClient::Connection *conn = new RestClient::Connection("");
conn->AppendHeader("Content-Type", ctype);
ret = conn->put(url, data);
delete conn;
return ret;
#endif
}

/**
 * @brief HTTP PATCH method
 *
 * @param url to query
 * @param ctype content type as string
 * @param data HTTP PATCH body
 *
 * @return response struct
 */
RestClient::Response RestClient::patch(const std::string& url,
		const std::string& ctype,
		const std::string& data) {
	RestClient::Response ret;
	RestClient::Connection *conn = new RestClient::Connection("");
	conn->AppendHeader("Content-Type", ctype);
	ret = conn->patch(url, data);
	delete conn;
	return ret;
}

/**
 * @brief HTTP DELETE method
 *
 * @param url to query
 *
 * @return response struct
 */
RestClient::Response RestClient::del(const std::string& url) {
#if __cplusplus >= 201402L
	auto conn = std::make_unique<RestClient::Connection>("");
	return conn->del(url);
#else
	RestClient::Response ret;
	RestClient::Connection *conn = new RestClient::Connection("");
	ret = conn->del(url);
	delete conn;
	return ret;
#endif
}

/**
 * @brief HTTP HEAD method
 *
 * @param url to query
 *
 * @return response struct
 */
RestClient::Response RestClient::head(const std::string& url) {
#if __cplusplus >= 201402L
	auto conn = std::make_unique<RestClient::Connection>("");
	return conn->head(url);
#else
	RestClient::Response ret;
	RestClient::Connection *conn = new RestClient::Connection("");
	ret = conn->head(url);
	delete conn;
	return ret;
#endif
}

/**
 * @brief HTTP OPTIONS method
 *
 * @param url to query
 *
 * @return response struct
 */
RestClient::Response RestClient::options(const std::string& url) {
	RestClient::Response ret;
	RestClient::Connection *conn = new RestClient::Connection("");
	ret = conn->options(url);
	delete conn;
	return ret;
}

