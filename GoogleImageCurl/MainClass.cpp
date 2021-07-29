#include "MainClass.h"


MainClass::MainClass(int amount, char* query) {

    curl_global_init(CURL_GLOBAL_DEFAULT);

    url = std::string("https://www.googleapis.com/customsearch/v1?key=");
    engineIDString = std::string("&cx=");
    queryString = std::string("&q=");
    folderName = std::string(query);
    verifiedQuery = verifyQuery(query);


    googleFile.open("google-search.json", std::fstream::in);


    if (!googleFile.is_open()) {
        cout << "specify google-search.json file" << endl;
        return;
    }


    while (googleFile.get(buff)) {
        APIKey_json = APIKey_json + buff;
    }
    

    try {
        jsonCredentials = getJsonObject(APIKey_json);
    }
    catch (nlohmann::detail::parse_error error) {
        cout << "specify google-search.json file correctly" << endl;
        return;
    }


    apiKey = jsonCredentials["apiKey"];
    searchEngineId = jsonCredentials["searchEngineId"];

    
    url = url + apiKey + engineIDString + searchEngineId + queryString + verifiedQuery + "&start=";
    nextPage = 1;


    while (k < amount) {
        saveImages(url + std::to_string(nextPage), folderName);
        nextPage = nextPage + 10;
        k++;
    }
    

    curl_global_cleanup();

}


size_t MainClass::write_callback(void* data, size_t size, size_t nmemb, void* userp){

    size_t realsize = size * nmemb;
    struct memory* mem = (struct memory*)userp;


    char* ptr = (char*)realloc(mem->response, mem->size + realsize + 1);

    if (ptr == NULL)
        return 0;

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;

}

size_t MainClass::write_to_file(void* data, size_t size, size_t nmemb, void* userp) {

    size_t realsize = size * nmemb;
    
    ((std::fstream*)userp)->write((char*)data, realsize);

    return realsize;
}


nlohmann::json MainClass::getJsonObject(std::string data) {
    return getJsonObject((char*)data.c_str());
}

nlohmann::json MainClass::getJsonObject(char* data) {

    nlohmann::json jsonObject;


    try {
        jsonObject = nlohmann::json::parse(data);
    }
    catch (nlohmann::detail::parse_error erro) {
        printf("\n%s", erro.what());
    }

    return jsonObject;

}

std::string MainClass::verifyQuery(char* query) {

    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;

    char secondPart[5000];

    std::string verifiedQuery;

    while (query[i] != '\0') {

        if (query[i] == ' ') {
            j = i + 1;
            k = 0;

            while (query[j] != '\0') {
                secondPart[k++] = query[j++];
            }

            secondPart[k] = '\0';

            query[i] = '%';
            query[i + 1] = '2';
            query[i + 2] = '0';


            l = i + 3;
            j = 0;


            while (j < k) {
                query[l++] = secondPart[j++];
            }

            query[l] = '\0';

            i = i + 3;
        }

        i++;
    }
    
    verifiedQuery = std::string(query);

    free(query);

    return verifiedQuery;

}

std::string MainClass::makeGETRequest(std::string url, bool saveToFile, std::string folderName) {

    CURL* curl = curl_easy_init();
    CURLcode res;

    std::fstream file;
    std::string fullPath;

    struct memory fullRes = { 0 };
    
    if (saveToFile) {
        CreateDirectoryA(folderName.c_str(), NULL);

        fullPath = folderName + "/" + std::to_string(contador++) + ".jpeg";

        file.open(fullPath, std::fstream::out | std::fstream::binary);
    }


    if (curl) {

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

        curl_easy_setopt(curl, CURLOPT_HTTP_CONTENT_DECODING, 0L);

        if (saveToFile) {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &MainClass::write_to_file);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
        }
        else{
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &MainClass::write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&fullRes);
        }

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);

    }
    else {
        cout << "curl not ok";
    }

    file.close();

    if (saveToFile)
        return "";
    else
        return std::string(fullRes.response);

}

// saves only up to 10 images per call, returns the next query result page
void MainClass::saveImages(std::string url, std::string folderName) {

    nlohmann::json jsonData = getJsonObject(makeGETRequest(url, false, ""));

    for (const auto& item : jsonData["items"].items())
    {
        for (const auto& val : item.value()["pagemap"]["cse_image"][0].items()) {
            makeGETRequest(val.value(), true, folderName);
        }
    }

}