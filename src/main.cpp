#include <dpp/dpp.h>

#include <string>
#include "nlohmann/json.hpp"
#include "dotenv.h"

//TODO: make a get server config value or default value system, if the server hasn't set a value -> get the default one 
//  -> use merge/patch to merge a server config to default in a temporary json (?)

int main() {
  dotenv::env.load_env();
  nlohmann::json config{}; //TODO: load config file, add a default config 
  dpp::cluster bot(dotenv::env["DISCORD_TOKEN"]);

  bot.on_log(dpp::utility::cout_logger());

  bot.on_slashcommand([](const dpp::slashcommand_t& event) {
    if (event.command.get_command_name() == "ping") {
      event.reply("Pong!");
    } else if (event.command.get_command_name() == "config") {
      //TODO: arg1 = command, arg2 = name, arg2 = value
    }
  });

  bot.on_ready([&bot](const dpp::ready_t& event) {
    if (dpp::run_once<struct register_bot_commands>()) {
      bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));
    }
  });

  bot.on_message_delete([&bot](const dpp::message_delete_t& event) {
    //if (false == (config["msgDeleteTrack"]["activate"].template get<bool>())) return;
    (void)event.channel_id; //from channel
    (void)event.guild_id; //from server
    (void)event.id; //message id, message is already deleted when this function is called

    //dpp::message msg(event.channel_id, config["msgDeleteTrack"]["msg"].template get<std::string>());
    dpp::message msg(event.channel_id, "a message as been deleted");
    bot.message_create(msg);
  });

  bot.start(dpp::st_wait);
}
