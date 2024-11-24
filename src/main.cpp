	#include <dpp/dpp.h>
	 
	const std::string BOT_TOKEN = "add your token here";
	 
	int main() {
	    dpp::cluster bot(BOT_TOKEN);
	 
	    bot.on_log(dpp::utility::cout_logger());
	 
	    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
	        if (event.command.get_command_name() == "ping") {
	            event.reply("Pong!");
	        }
	    });
	 
	    bot.on_ready([&bot](const dpp::ready_t& event) {
	        if (dpp::run_once<struct register_bot_commands>()) {
	            bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));
	        }
	    });

      bot.on_message_delete([&bot](const dpp::message_delete_t& event) {
          (void)event.channel_id; //from channel
          (void)event.guild_id; //from server
          (void)event.id; //message id, message is already deleted when this function is called

          dpp::message msg(event.channel_id, "a message as been deleted from here");
          bot.message_create(msg);
      });
	 
	    bot.start(dpp::st_wait);
	}
