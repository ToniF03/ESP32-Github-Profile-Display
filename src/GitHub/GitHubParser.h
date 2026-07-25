/*
 * Created on: 2026-07-23
 * Author(s): Toni Fey
 * License: MIT
 * Description: Parser for GitHub API responses to extract profile, stats, and repository data
 */

#pragma once

#include <ArduinoJson.h>      // JSON parsing for API responses

#include "../models/GitHubProfile.h"
#include "../models/GitHubStats.h"
#include "../models/GitHubRepo.h"
#include "GitHubClient.h"

class GitHubParser
{
public:
    explicit GitHubParser(const String User);
    GitHubProfile getProfile();
    GitHubProfile getProfile(const String User);
    GitHubStats getStatistics(const uint8_t currentWeekday);
    GitHubRepo getRepo(const String repoName);
    GitHubRepo getRepo(const String repoName, const String User);
    GitHubRepo* getRepos(int amount);
    GitHubRepo* getRepos(const String User, const int amount);

private:
    GitHubClient client;
    String _user;
};