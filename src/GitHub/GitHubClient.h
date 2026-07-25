/*
 * Created on: 2026-07-23
 * Author(s): Toni Fey
 * License: MIT
 * Description: GitHub API client for ESP32 to fetch user profile, repository, and contribution heatmap data
 */

#include <Arduino.h>
#include <HTTPClient.h>       // HTTP client for API calls
#include <WiFiClientSecure.h> // Secure HTTPS client

#include "../models/HTTPHeader.h"
#include "../models/deviceInformation.h"
#include "../time/TimeManager.h"
#include "resources/credentials.h"

#pragma once

class GitHubClient
{
public:
    void init(const String username);
    String getProfileData(const String User);
    String getStatisticsData(const String User);
    String getReposData(const String User);
    String getRepoData(const String repo, const String User);

private:
    char *profileURL = "https://api.github.com/users/";
    char *reposURL = "https://api.github.com/repos/";
    char *graphQLBaseURL = "https://api.github.com/graphql";
    String receiveData(const char *URL);
    String receiveHTTPSData(const char *URL, const String query, const HTTPHeader *header);
};