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
      /*} else if (event.command.get_command_name() == "config") {
      	std::cout << "0" << std::endl;
        dpp::command_interaction cmd_data = event.command.get_command_interaction();
      	std::cout << "1" << std::endl;
	std::string guild_id{event.command.guild_id.str()};
      	std::cout << "2 - " << guild_id << std::endl;
	std::cout << "3 - " << cmd_data.options.size() << std::endl;
        //std::string featureName{cmd_data.options[0].name};
        //std::string featureName{cmd_data.get_value<std::string>(0)};
        std::string featureName{event.command.get_context_message().content};
      	std::cout << "4 - " << featureName << std::endl;
	return;
        std::string paramName{cmd_data.options[1].name};
      	std::cout << "5 - " << paramName << std::endl;
        std::string val{std::get<std::string>(event.get_parameter("val"))};
      	std::cout << "6 - " << val << std::endl;
        //config.EditParam(guild_id, featureName, paramName, val);
      	std::cout << "7" << std::endl;
	*/
      } else if (event.command.get_command_name() == "config") {
        /* Fetch a parameter value from the command parameters */
	std::string feature = std::get<std::string>(event.get_parameter("feature"));
	std::string param = std::get<std::string>(event.get_parameter("param"));
	 
	/* Reply to the command. There is an overloaded version of this
	 * call that accepts a dpp::message so you can send embeds.
	 */
	event.edit_original_response(dpp::message(std::string("guild: ") + event.command.guild_id.str() + "feat: " + feature + ", param: " + param));
      }
    });
  });

  bot.on_ready([&bot](const dpp::ready_t& event) {
    //if (dpp::run_once<struct register_bot_commands>()) {
      bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));

      dpp::slashcommand newcommand("config", "Send a random adorable animal photo", bot.me.id);
      newcommand.add_option(
        dpp::command_option(dpp::co_string, "feature", "feature to configure", true)
	  .add_choice(dpp::command_option_choice("Message deleted tracker", std::string("msgdeletetrack")))
      ).add_option(
	dpp::command_option(dpp::co_string, "param", "parameter to edit", true)
	  .add_choice(dpp::command_option_choice("Activate", std::string("activate")))
	  .add_choice(dpp::command_option_choice("Message", std::string("msg")))
      );
      bot.global_command_create(newcommand);

      /*dpp::slashcommand configCli("config", "Let you customize or activate feature", bot.me.id);
      configCli.add_option(
        dpp::command_option(dpp::co_sub_command, "msgDeleteTrack", "notify deleted message")
          .add_option(dpp::command_option(dpp::co_sub_command, "activate", "true or false")
            .add_option(dpp::command_option(dpp::co_string, "val", "param value", true)
              .add_choice(dpp::command_option_choice("true", std::string("true")))
              .add_choice(dpp::command_option_choice("false", std::string("false")))
            )
          ).add_option(dpp::command_option(dpp::co_sub_command, "msg", "custom message")
            .add_option(dpp::command_option(dpp::co_string, "val", "param value", true))
          )
      );

      bot.global_command_create(configCli);*/
    //}
  });

  bot.on_message_delete([&bot, &config](const dpp::message_delete_t& event) {
    int guild_id{(int)(event.guild_id)};
    if ("false" == config.GetParam(guild_id, "msgdeletetrack", "activate")) return;
    dpp::message msg(event.channel_id, config.GetParam(guild_id, "msgdeletetrack", "msg"));
    bot.message_create(msg);
  });

  bot.start(dpp::st_return);
  signal(SIGTERM, &signal_handler);
  signal(SIGINT, &signal_handler);
  end.get_future().wait();
  bot.shutdown();
}
