/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Data model for a GitHub profile
 */

#pragma once

#include <Arduino.h>

struct GitHubProfile
{
    String username;
    String name;
    String bio;
    String blog;
    String twitterUsername;
    String email;
    String company;
    String* followerList;
    String* followingList;
    String* repos;
    String* starredRepos;
    int publicRepos;
    int publicGists;
    int followers;
    int following;
    int stars;
    int starred;
};