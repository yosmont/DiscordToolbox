#include "Config.hpp"
#include <fstream>

const nlohmann::json DEFAULT_CONFIG{
  {"msgDeleteTrack", {
    {"activate", "false"},
    {"msg", "a message as been deleted"}
  }}
};

Config::Config(void)
{
  std::ifstream f("config.json");
  if (f.is_open()) _data = nlohmann::json::parse(f);
}

void Config::EditParam(const std::string &guildID, const std::string &feature, const std::string &param, const std::string &val)
{
  _data[guildID][feature] = val;
  std::ofstream f("config.json");
  if (f.is_open()) f << _data.dump();
}

std::string Config::GetParam(const std::string &guildID, const std::string &feature, const std::string &param)
{
  nlohmann::json tmp = DEFAULT_CONFIG;
  if (_data.contains(guildID)) tmp.merge_patch(_data[guildID]);
  return tmp[feature][param];
}
