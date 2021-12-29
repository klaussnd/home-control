#include <mqttbase/helper.h>

#include <netdb.h>
#include <pwd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

std::string homeDirectory()
{
   const char* homeDir = getenv("HOME");
   if (homeDir == nullptr)
   {
      const auto pw = getpwuid(getuid());
      homeDir = pw->pw_dir;
   }
   return homeDir;
}

std::string hostname()
{
   char buffer[64];
   gethostname(buffer, sizeof(buffer));
   return buffer;
}

std::string getFqdn(const std::string& host)
{
   struct addrinfo hints = {};
   hints.ai_family = AF_UNSPEC;
   hints.ai_flags = AI_CANONNAME;

   struct addrinfo* res = nullptr;
   if (getaddrinfo(host.c_str(), nullptr, &hints, &res) == 0)
   {
      // The hostname was successfully resolved.
      std::string result = res->ai_canonname;
      freeaddrinfo(res);
      return result;
   }
   else
   {
      return host;
   }
}

std::string timeToString(std::time_t time)
{
   struct tm tm;
   gmtime_r(&time, &tm);
   char buf[32];
   strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%SZ", &tm);
   return buf;
}
