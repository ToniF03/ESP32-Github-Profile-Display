/*
 * Created on: 2026-07-23
 * Author(s): Toni Fey
 * License: MIT
 * Description: Parses GitHub profile and repository data from the GitHub API for display.
 */

#include "GitHubParser.h"

GitHubParser::GitHubParser(const String User)
{
    _user = User;
}

GitHubProfile *GitHubParser::getProfile()
{
    return getProfile(_user);
}

GitHubProfile *GitHubParser::getProfile(const String User)
{
    GitHubProfile *profile = new GitHubProfile;
    JsonDocument doc;
    String profileJson = client.getProfileData(User);
    DeserializationError error = deserializeJson(doc, profileJson);
    if (error)
    {
        Serial.print("Error occured while fetching the profile: ");
        Serial.println(error.c_str());
        return nullptr;
    }

    profile->followers = doc["followers"].as<int>();    
    profile->following = doc["following"].as<int>();
    profile->publicGists = doc["public_gists"].as<int>();
    profile->publicRepos = doc["public_repos"].as<int>();
    profile->bio = doc["bio"].as<String>();
    profile->blog = doc["blog"].as<String>();
    profile->company = doc["company"].as<String>();
    profile->email = doc["email"].as<String>();
    profile->name = doc["name"].as<String>();
    profile->twitterUsername = doc["twitter_username"].as<String>();

    return profile;
}

GitHubRepo *GitHubParser::getRepos(const int amount)
{
    return getRepos(_user, amount);
}

GitHubRepo *GitHubParser::getRepos(const String User, const int amount)
{
    GitHubRepo *repos = new GitHubRepo[amount];
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, client.getReposData(User));

    if (error)
    {
        Serial.print("Error occured while fetching repos: ");
        Serial.println(error.c_str());
        return nullptr;
    }

    for (int i = 0; i < amount; i++)
    {
        GitHubRepo repo;
        repo.description = doc[i]["description"].as<String>();
        repo.language = doc[i]["language"].as<String>();
        repo.license = doc[i]["license"].as<String>();
        repo.name = doc[i]["name"].as<String>();
        repo.stargazers = doc[i]["stargazers_count"].as<int>();
        repo.watchers = doc[i]["watchers_count"].as<int>();
        repos[i] = repo;
    }

    return repos;
}

GitHubRepo *GitHubParser::getRepo(const String repoName)
{
    return getRepo(repoName, _user);
}

GitHubRepo *GitHubParser::getRepo(const String repoName, const String User)
{
    GitHubRepo *repo = new GitHubRepo;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, client.getReposData(User));

    if (error)
    {
        Serial.print("Error occured while fetching repos: ");
        Serial.println(error.c_str());
        return nullptr;
    }

    repo->description = doc["description"].as<String>();
    repo->language = doc["language"].as<String>();
    repo->license = doc["license"].as<String>();
    repo->name = doc["name"].as<String>();
    repo->stargazers = doc["stargazers_count"].as<int>();
    repo->watchers = doc["watchers_count"].as<int>();

    return repo;
}

GitHubStats *GitHubParser::getStatistics(const uint8_t currentWeekday)
{
    JsonDocument doc;
    String statsJson = client.getStatisticsData(_user);
    DeserializationError error = deserializeJson(doc, statsJson);
    GitHubStats *stats = new GitHubStats;

    if (error)
    {
        Serial.print("Error occured while fetching profile statistics: ");
        Serial.println(error.c_str());
    }

    // Initialize statistics variables
    int streak = 0; // Temporary streak counter

    stats->contributions = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["totalContributions"].as<int>();
    for (int i = 0; i <= 371; i++)
    {
        stats->commits[i] = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"][i / 7]["contributionDays"][i % 7]["contributionCount"].as<int>();

        // Calculate longest streak
        if (stats->commits[i] > 0)
        {
            streak++;
            if (streak > stats->longestStreak)
                stats->longestStreak = streak;
        }
        else
        {
            streak = 0;
        }

        // Calculate max contributions in a day
        if (stats->commits[i] > stats->maxContributions)
        {
            stats->maxContributions = stats->commits[i];
        }
    }

    // Calculate average contributions per day, rounded to 2 decimal places
    stats->averageContributions = (float)stats->contributions / (365 + currentWeekday);
    stats->averageContributions = roundf(stats->averageContributions * 100) / 100;

    // Calculate current active streak (consecutive days from today backwards)
    for (int i = 371 - (7 - currentWeekday); i >= 0; i--)
    {
        if (stats->commits[i] > 0)
            stats->currentStreak++;
        else
            break;
    }

    return stats;
}