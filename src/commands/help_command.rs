use crate::entity::EntityRef;
use crate::entity::Realm;
use crate::player_output::PlayerOutput;
use crate::utils::{format_columns, highlight, process_highlights, split_lines};

use super::command_registry::CommandRegistry;
use super::CommandHelpers;

/// Shows in-game help.
pub fn help(
    realm: &mut Realm,
    player_ref: EntityRef,
    helpers: CommandHelpers,
) -> Result<Vec<PlayerOutput>, String> {
    let processor = helpers.command_line_processor;
    processor.take_word(); // alias

    let command_name = processor.take_word().ok_or_else(|| {
        process_highlights(
            "Type *help commands* to see a list of all commands.\n\
            Type *help <command>* to see help about a particular command.",
        )
    })?;

    let mut output = Vec::<PlayerOutput>::new();
    let is_admin = realm
        .player(player_ref)
        .map(|player| player.is_admin())
        .unwrap_or(false);

    if command_name == "commands" {
        let command_names = helpers.command_registry.command_names();

        push_output_string!(
            output,
            player_ref,
            process_highlights(&format!(
                "\n\
                Here is a list of all the commands you can use:\n\
                \n\
                {}\
                \n\
                Type *help <command>* to see help about a particular command.\n",
                format_columns(
                    command_names
                        .iter()
                        .filter(|name| !name.starts_with("api-"))
                        .map(|name| highlight(name))
                        .collect()
                )
            ))
        );

        if is_admin {
            push_output_string!(
                output,
                player_ref,
                process_highlights(
                    "\nTo see all the admin commands you can use, type *help admin-commands*.\n\n"
                )
            );
        }
    } else if let Ok(command) = helpers.command_registry.lookup(&command_name) {
        push_output_string!(
            output,
            player_ref,
            process_highlights(&format!(
                "\n  *{}*\n  {}\n\n",
                command_name,
                split_lines(command.description(), 77).join("\n  ")
            ))
        );
    } else if is_admin {
        if let Ok(admin_command) = helpers.admin_command_registry.lookup(&command_name) {
            push_output_string!(
                output,
                player_ref,
                process_highlights(&format!(
                    "\n  *{}*\n  {}\n\n",
                    command_name,
                    split_lines(admin_command.description(), 77).join("\n  ")
                ))
            );
        } else if let Some(mut admin_help) =
            show_admin_help(&command_name, helpers.admin_command_registry)
        {
            admin_help.push('\n');
            push_output_string!(output, player_ref, admin_help);
        }
    }

    if output.is_empty() {
        push_output_string!(
            output,
            player_ref,
            process_highlights(&format!(
                "The command \"{}\" is not recognized.\n\
                Type *help commands* to see a list of all commands.\n",
                command_name
            ))
        );
    }

    Ok(output)
}

fn show_admin_help(command_name: &str, admin_registry: &CommandRegistry) -> Option<String> {
    match command_name {
        "admin-commands" => Some(process_highlights(&format!(
            "\n\
            Here is a list of all the commands you can use as an admin:\n\
            \n\
            *Remember: with great power comes great responsibility!*\n\
            \n\
            {}\
            \n\
            Type *help <command>* to see help about a particular command.\n\
            Type *help admin-tips* to see some general tips for admins.\n",
            format_columns(
                admin_registry
                    .command_names()
                    .iter()
                    .map(|name| highlight(name))
                    .collect()
            )
        ))),
        "admin-tips" => Some(process_highlights(
            "\n\
            *Admin Tips*\n\
            \n\
            Now that you are an admin, you can actively modify the game world. It should go \
            without saying that with great power, comes great responsibility. Especially because \
            many modifications are not reversible. Now, trusting you will do the right thing, \
            here as some tips for you:\n\
            \n\
            When referring to an entity, you can use *#<id>* rather than referring to them by \
            name. For example, *set-prop #35 description This is the entity with ID 35.*.\n\
            \n\
            Similarly, you can always use the word *room* to refer to the current room you are \
            in. But, to make editing of rooms even easier, you can use *@<property>* as a \
            shortcut for *get-* or *set-prop room <property>*. Thus, you can simply type: \
            *@id* to get the ID of the current room. Or, to set the description: *@description \
            As you stand just outside the South Gate, ...*\n\
            \n\
            Not listed in the admin commands overview, but very useful: *edit-prop* is available \
            for more convenient editing of properties. Its usage is the same as for *get-prop*, \
            but it will pop up an interactive dialog for editing the property. Do note that
            this command is only available if you use the web interface (it's not supported when \
            using telnet).\n",
        )),
        _ => None,
    }
}
