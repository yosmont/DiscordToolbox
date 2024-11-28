#ifndef CONFIG
#define CONFIG

#include "nlohmann/json.hpp"

class Config {
  public:
    Config(void);
    void EditParam(const std::string &guildID, const std::string &feature, const std::string &param, const std::string &val);
    std::string GetParam(const std::string &guildID, const std::string &feature, const std::string &param);
  protected:
    nlohmann::json _data{};
};

#endif // !DISCORD_CONFIG
