#include <dpp/dpp.h>

#include <string>
#include "nlohmann/json.hpp"
#include "dotenv.h"

#include "Config.hpp"

int main() {
  dotenv::env.load_dotenv();
  Config config{};
  dpp::cluster bot(dotenv::env["DISCORD_TOKEN"]);

  bot.on_log(dpp::utility::cout_logger());

  bot.on_slashcommand([](const dpp::slashcommand_t& event) {
    event.thinking(false, [event](const dpp::confirmation_callback_t &callback) {
      if (event.command.get_command_name() == "ping") {
        event.edit_original_response(dpp::message("Pong!"));
      } else if (event.command.get_command_name() == "config") {
        dpp::command_interaction cmd_data = event.command.get_command_interaction();
        auto featureName = cmd_data.options[0];
        auto paramName = cmd_data.options[1];
        int guild_id{(int)(event.command.guild_id)};
        if (featureName.name == "msgDeleteTrack") {
          if (paramName.name == "activate") {
            std::string val{std::get<std::string>(event.get_parameter("bool"))}
            config.EditParam(guild_id, "msgDeleteTrack", "activate", val);
          } else if (paramName.name == "msg") {
            std::string val{std::get<std::string>(event.get_parameter("string"))}
            config.EditParam(guild_id, "msgDeleteTrack", "msg", val);
          }
        }
      }
    });
  });

  bot.on_ready([&bot](const dpp::ready_t& event) {
    if (dpp::run_once<struct register_bot_commands>()) {
      bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));

      dpp::slashcommand configCli("config", "Let you customize or activate feature", bot.me.id);
      configCli.add_option(
        dpp::command_option(dpp::co_sub_command, "msgDeleteTrack", "notify deleted message")
          .add_option(dpp::command_option(dpp::co_sub_command, "activate", "true or false")
            .add_option(dpp::command_option(dpp::co_string, "bool", "param value", true)
              .add_choice(dpp::command_option_choice("true", std::string("true")))
              .add_choice(dpp::command_option_choice("false", std::string("false")))
            )
          ).add_option(dpp::command_option(dp::co_sub_command, "msg", "custom message")
            .add_option(dpp::command_option(dpp::co_string, "string", "param value", true))
          )
      );

      bot.global_command_create(configCli);
    }
  });

  bot.on_message_delete([&bot](const dpp::message_delete_t& event) {
    int guild_id{(int)(event.guild_id)};
    if ("false" == config.GetParam(guild_id, "msgDeleteTrack", "activate")) return;
    dpp::message msg(event.channel_id, config.GetParam(guild_id, "msgDeleteTrack", "msg"));
    bot.message_create(msg);
  });

  bot.start(dpp::st_wait);
}
