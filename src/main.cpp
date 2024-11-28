#include <dpp/dpp.h>

#include <string>
#include "nlohmann/json.hpp"
#include "dotenv.h"

#include "Config.hpp"

#include <signal.h>
#include <future>

std::promise<void> end{};

void signal_handler(int sig)
{
  end.set_value();
}

#include <iostream>

int main() {
  dotenv::env.load_dotenv();
  Config config{};
  dpp::cluster bot(dotenv::env["DISCORD_TOKEN"]);

  bot.on_log(dpp::utility::cout_logger());

  bot.on_slashcommand([&config](const dpp::slashcommand_t& event) {
    event.thinking(false, [event, &config](const dpp::confirmation_callback_t &callback) {
      if (event.command.get_command_name() == "ping") {
        event.edit_original_response(dpp::message("Pong!"));
      } else if (event.command.get_command_name() == "config") {
        /* Fetch a parameter value from the command parameters */
	std::string guild{event.command.guild_id.str()};
	std::string feature{std::get<std::string>(event.get_parameter("feature"))};
	std::string param{std::get<std::string>(event.get_parameter("param"))};
	std::string value{std::get<std::string>(event.get_parameter("val"))};
	 
	/* Reply to the command. There is an overloaded version of this
	 * call that accepts a dpp::message so you can send embeds.
	 */
	//event.edit_original_response(dpp::message(std::string("guild: ") + guild + ", feat: " + feature + ", param: " + param));
	event.edit_original_response(dpp::message(feature + " -> " + param + " == " + value));
	config.EditParam(guild, feature, param, value);
      }
    });
  });

  bot.on_ready([&bot](const dpp::ready_t& event) {
    //if (dpp::run_once<struct register_bot_commands>()) {
      bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));

      dpp::slashcommand configCli("config", "Send a random adorable animal photo", bot.me.id);
      configCli.add_option(
        dpp::command_option(dpp::co_string, "feature", "feature to configure", true)
	  .add_choice(dpp::command_option_choice("Message deleted tracker", std::string("msgdeletetrack")))
      ).add_option(
	dpp::command_option(dpp::co_string, "param", "parameter to edit", true)
	  .add_choice(dpp::command_option_choice("Activate", std::string("activate")))
	  .add_choice(dpp::command_option_choice("Message", std::string("msg")))
      ).add_option(
	dpp::command_option(dpp::co_string, "val", "new value", true)
      );
      bot.global_command_create(configCli);
    //}
  });

  bot.on_message_delete([&bot, &config](const dpp::message_delete_t& event) {
    std::string guild{event.guild_id.str()};
    if ("false" == config.GetParam(guild, "msgdeletetrack", "activate")) return;
    dpp::message msg(event.channel_id, config.GetParam(guild, "msgdeletetrack", "msg"));
    bot.message_create(msg);
  });

  bot.start(dpp::st_return);
  signal(SIGTERM, &signal_handler);
  signal(SIGINT, &signal_handler);
  end.get_future().wait();
  bot.shutdown();
}
