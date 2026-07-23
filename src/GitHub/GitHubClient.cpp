/*
 * Created on: 2026-07-23
 * Author(s): Toni Fey
 * License: MIT
 * Description:
 */

#include "GitHubClient.h"

String GitHubClient::getProfileData(const String User)
{
    String profileJson = receiveData(String(String(profileURL) + User).c_str());
    return profileJson;
}

String GitHubClient::getReposData(const String User)
{
    String reposJson = receiveData(String(String(profileURL) + User + "/repos").c_str());
    return reposJson;
}

String GitHubClient:: getRepoData(const String repo, const String User) {
    String repoJson = receiveData(String(String(reposURL) + User + "/" + repo).c_str());
    return repoJson;
}

/**
 * Fetch data from a given URL using HTTPS
 * @param URL The HTTPS URL to fetch data from
 * @return Response payload as a String, or empty string on failure
 */
String GitHubClient::receiveData(const char *URL)
{
    String response;
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient https;

    if (https.begin(client, URL))
    {                               // Use the secure client
        int httpCode = https.GET(); // Perform the GET request

        if (httpCode > 0)
        {
            if (httpCode == HTTP_CODE_OK)
            {
                response = https.getString();
            }
        }
        else
        {
            Serial.printf("[HTTPS] GET failed, error: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end(); // Free resources
    }
    else
    {
        Serial.println("[HTTPS] Unable to connect");
    }
    return response;
}
