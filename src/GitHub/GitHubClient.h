/*
 * Created on: 2026-07-23
 * Author(s): Toni Fey
 * License: MIT
 * Description:
 */

#include <Arduino.h>
#include <HTTPClient.h>       // HTTP client for API calls
#include <WiFiClientSecure.h> // Secure HTTPS client

#pragma once

class GitHubClient
{
public:
    void init(const String username);
    String getProfileData(const String User);
    String getHeatmapData();
    String getReposData(const String User);
    String getRepoData(const String repo, const String User);

private:
    char *profileURL = "https://api.github.com/users/";
    char *reposURL = "https://api.github.com/users/";
    String receiveData(const char *URL);
};