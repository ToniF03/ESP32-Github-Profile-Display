/*
 * Created on: 2026-07-23
 * Author(s): Toni Fey
 * License: MIT
 * Description:
 */

#include "GitHubParser.h"

GitHubParser::GitHubParser(const String User)
{
    _user = User;
}

GitHubProfile GitHubParser::getProfile()
{
    getProfile(_user);
}

GitHubProfile GitHubParser::getProfile(const String User)
{
    GitHubProfile profile;
    JsonDocument doc;
    String profileJson = client.getProfileData(User);
    DeserializationError error = deserializeJson(doc, profileJson);

    if (error)
    {
        Serial.print("Error occured while fetching the profile: ");
        Serial.println(error.c_str());
        return;
    }

    profile.followers = doc["followers"].as<int>();
    profile.following = doc["following"].as<int>();
    profile.publicGists = doc["public_gists"].as<int>();
    profile.publicRepos = doc["public_repos"].as<int>();
    profile.bio = doc["bio"].as<String>();
    profile.blog = doc["blog"].as<String>();
    profile.company = doc["company"].as<String>();
    profile.email = doc["email"].as<String>();
    profile.name = doc["name"].as<String>();
    profile.twitterUsername = doc["twitter_username"].as<String>();

    return profile;
}

GitHubRepo *GitHubParser::getRepos(const int amount)
{
    return getRepos(_user, amount);
}

GitHubRepo *GitHubParser::getRepos(const String User, const int amount)
{
    GitHubRepo repos[amount];
    JsonDocument doc;
    String reposJson = client.getReposData(User);
    DeserializationError error = deserializeJson(doc, reposJson);

    if (error)
    {
        Serial.print("Error occured while fetching repos: ");
        Serial.println(error.c_str());
        return;
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

GitHubRepo GitHubParser::getRepo(const String repoName) {
    return getRepo(repoName, _user);
}

GitHubRepo GitHubParser::getRepo(const String repoName, const String User)
{
    GitHubRepo repo;
    JsonDocument doc;
    String repoJson = client.getReposData(User);
    DeserializationError error = deserializeJson(doc, repoJson);

    if (error)
    {
        Serial.print("Error occured while fetching repos: ");
        Serial.println(error.c_str());
        return;
    }

    repo.description = doc["description"].as<String>();
    repo.language = doc["language"].as<String>();
    repo.license = doc["license"].as<String>();
    repo.name = doc["name"].as<String>();
    repo.stargazers = doc["stargazers_count"].as<int>();
    repo.watchers = doc["watchers_count"].as<int>();

    return repo;
}

GitHubStats GitHubParser::getHeatmap() {
    
}