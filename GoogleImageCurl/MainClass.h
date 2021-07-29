#pragma once
#define CURL_STATICLIB
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <fileapi.h>
#include <fstream>
#include <iostream>

using std::cout;
using std::cin;
using std::endl;

#include "curl/curl.h"
#include "nlohmann/json.hpp"



class MainClass
{

	struct memory {
		char* response;
		size_t size;
	};

private:

	std::string url;
	int nextPage;
	std::string engineIDString;
	std::string queryString;
	std::string folderName;
	std::string verifiedQuery;
	nlohmann::json jsonCredentials;

	std::fstream googleFile;

	std::string APIKey_json;
	std::string apiKey;
	std::string searchEngineId;

	int k;
	char buff;

	int contador;

public:

	MainClass(int amount, char* query);

	static size_t write_callback(void* data, size_t size, size_t nmemb, void* userp);
	static size_t write_to_file(void* data, size_t size, size_t nmemb, void* userp);

	nlohmann::json getJsonObject(std::string data);
	nlohmann::json getJsonObject(char* data);
	std::string verifyQuery(char* query);
	std::string makeGETRequest(std::string url, bool saveToFile, std::string folderName);

	void saveImages(std::string url, std::string folderName);



};