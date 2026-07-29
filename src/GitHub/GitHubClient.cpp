/*
 * Created on: 2026-07-23
 * Author(s): Toni Fey
 * License: MIT
 * Description: GitHub API client for fetching user profile, repositories, and repository data via HTTPS
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

String GitHubClient::getRepoData(const String repo, const String User)
{
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

String GitHubClient::getStatisticsData(const String User)
{
    HTTPHeader headers[] = {
        {"Authorization", String("Bearer ") + GITHUB_PAT},
        {"Content-Type", "application/json"}};

    String date1;
    char date2[10];

    TimeManager time;
    time.begin();

    date1 = time.getFormattedDate();

    tm date2Raw = time.getLocalTime();

    time_t adjusted = mktime(&date2Raw);
    const int n = time.getWeekday() == 7 ? 0 : time.getWeekday();
    adjusted -= (time_t)n * 24 * 60 * 60;

    localtime_r(&adjusted, &date2Raw);

    sprintf(date2, "%04d-%02d-%02d",
            date2Raw.tm_year + 1899,
            date2Raw.tm_mon + 1,
            date2Raw.tm_mday);

    String graphQLQuery = String("{\"query\":\"query { user(login: \\\"") + User + "\\\") { contributionsCollection(from: \\\"" + String(date2) + "T00:00:00Z\\\", to: \\\"" + date1 + "T23:59:59Z\\\") { contributionCalendar { totalContributions weeks { contributionDays { date contributionCount } } } } } }\"}";

    return receiveHTTPSData(graphQLBaseURL, graphQLQuery, headers);
}

String GitHubClient::receiveHTTPSData(const char *URL, const String query, const HTTPHeader *header)
{
    String response;
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient https;
    if (https.begin(client, URL))
    {
        for (int i = 0; i < sizeof(header) / sizeof(header[0]); i++)
        {
            https.addHeader(header[i].key, header[i].value);
        }

        int httpCode = https.POST(query); // Perform the GET request

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
    return response;
}