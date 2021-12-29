#pragma once

#include <ctime>
#include <string>

std::string homeDirectory();
/// Return the hostname of the local machine
std::string hostname();
/// Return the fully qualified domain name of the given host
std::string getFqdn(const std::string& host);
/// Format a time as string in UTC like "2020-03-21 08:00:00Z"
std::string timeToString(std::time_t time);
