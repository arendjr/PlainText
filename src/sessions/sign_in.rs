use lazy_static::lazy_static;
use maplit::hashmap;
use serde_json::json;
use std::collections::HashMap;

use crate::colors::Color;
use crate::game_object::GameObject;
use crate::objects::{Class, Player, Race, Realm};
use crate::sessions::SessionOutput as Output;
use crate::text_utils::{
    capitalize, colorize, format_columns, highlight, is_letter, split_lines, FormatOptions,
};

lazy_static! {
    static ref RESERVED_NAMES: Vec<String> = vec![
        "Admin".to_owned(),
        "Administrator".to_owned(),
        "Root".to_owned()
    ];
    static ref SIGN_IN_STEPS: HashMap<SignInStep, StepImpl> = get_sign_in_steps();
}

#[derive(Clone, Debug)]
pub struct SignInState {
    step: SignInStep,
    data: SignInData,
}

impl SignInState {
    pub fn new() -> Self {
        new_state(
            SignInStep::AskingUserName,
            SignInData {
                class: None,
                password: String::new(),
                player: None,
                race: None,
                user_name: String::new(),
            },
        )
    }
}

#[derive(Clone, Debug, Eq, Hash, PartialEq)]
enum SignInStep {
    AskingClass,
    AskingGender,
    AskingPassword,
    AskingRace,
    AskingSignUpPassword,
    AskingUserName,
    AskingUserNameConfirmation,
    AskingSignUpPasswordConfirmation,
    SessionClosed,
    SignedIn,
}

#[derive(Clone, Debug)]
struct SignInData {
    class: Option<Class>,
    password: String,
    player: Option<Player>,
    race: Option<Race>,
    user_name: String,
}

struct StepImpl {
    enter: fn(&SignInData, &Realm) -> Output,
    exit: fn(&SignInData) -> Output,
    prompt: fn(&SignInData) -> Output,
    process_input: fn(&SignInState, &Realm, String) -> (SignInState, Output),
}

pub fn process_input(
    state: &SignInState,
    realm: &Realm,
    input: String,
) -> (SignInState, Vec<Output>) {
    match SIGN_IN_STEPS.get(&state.step) {
        Some(step) => {
            let (new_state, output) = (step.process_input)(state, realm, input);
            match SIGN_IN_STEPS.get(&new_state.step) {
                Some(new_step) => {
                    let prompt_output = (new_step.prompt)(&new_state.data);
                    if new_state.step == state.step {
                        (new_state, vec![output, prompt_output])
                    } else {
                        let exit_output = (step.exit)(&new_state.data);
                        let enter_output = (new_step.enter)(&new_state.data, realm);
                        (
                            new_state,
                            vec![output, exit_output, enter_output, prompt_output],
                        )
                    }
                }
                None => (
                    new_state,
                    vec![Output::Str("New state has no implementation")],
                ),
            }
        }
        None => (state.clone(), vec![]),
    }
}

fn get_sign_in_steps() -> HashMap<SignInStep, StepImpl> {
    hashmap! {
        SignInStep::AskingUserName => StepImpl {
            enter: |_, _| Output::None,
            exit: |_| Output::None,
            prompt: |_| Output::Str("What is your name? "),
            process_input: process_asking_user_name_input,
        },

        SignInStep::AskingUserNameConfirmation => StepImpl {
            enter: |_, _| Output::None,
            exit: |_| Output::None,
            prompt: |data| Output::String(format!("{}, did I get that right? ", data.user_name)),
            process_input: process_asking_user_name_confirmation_input,
        },

        SignInStep::AskingSignUpPassword => StepImpl {
            enter: |_, _| Output::JSON(json!({ "inputType": "password" })),
            exit: |_| Output::None,
            prompt: |_| Output::Str("Please choose a password: "),
            process_input: process_asking_sign_up_password_input,
        },

        SignInStep::AskingSignUpPasswordConfirmation => StepImpl {
            enter: |_, _| Output::None,
            exit: |_| Output::JSON(json!({ "inputType": "text" })),
            prompt: |_| Output::Str("Please confirm a password: "),
            process_input: process_asking_sign_up_password_confirmation_input,
        },

        SignInStep::AskingRace => StepImpl {
            enter: enter_race,
            exit: |_| Output::None,
            prompt: |_| Output::Str(
                "\n\
                Please select the race you would like to use, or type *info <race>* to get \
                more information about a race.\n"
            ),
            process_input: process_asking_race_input
        },

        SignInStep::AskingClass => StepImpl {
            enter: enter_class,
            exit: |_| Output::None,
            prompt: |_| Output::Str(
                "\n\
                Please select the class you would like to use, or type *info <class>* to \
                get more information about a class.\n"
            ),
            process_input: process_asking_class_input
        }
    }
}

fn enter_race(_: &SignInData, realm: &Realm) -> Output {
    Output::String(format!(
        "\n\
        Please select which race you would like your character to be.\n\
        \n\
        Your race determines some attributes of the physique of your character, \
        as well as where in the {} you will start your journey.\n\
        \n\
        These are the major races in the {}:\n\
        \n\
        {}",
        realm.get_name(),
        realm.get_name(),
        format_columns(
            realm.race_names(),
            FormatOptions::Capitalized | FormatOptions::Highlighted
        )
    ))
}

fn enter_class(data: &SignInData, realm: &Realm) -> Output {
    Output::String(format!(
        "\n\
        Please select which class you would like your character to be specialized \
        in.\n\
        Your class determines additional attributes of the physique of your \
        character, and also can influence your choice to be good or evil.\n\
        \n\
        Note that the available classes are dependent on your choice of race. To \
        revisit your choice of race, type *back*.\n\
        \n\
        These are the classes you may choose from:\n\
        \n\
        {}",
        match &data.race {
            Some(race) => format_columns(
                race.get_classes()
                    .iter()
                    .filter_map(|object_ref| realm
                        .get(*object_ref)
                        .and_then(|object| Some(object.get_name())))
                    .collect(),
                FormatOptions::Capitalized | FormatOptions::Highlighted
            ),
            None => "(cannot determine classes without race)".to_owned(),
        }
    ))
}

fn new_state(step: SignInStep, data: SignInData) -> SignInState {
    SignInState { step, data }
}

fn process_asking_user_name_input(
    state: &SignInState,
    realm: &Realm,
    input: String,
) -> (SignInState, Output) {
    let user_name = validate_user_name(input.to_lowercase());
    if user_name.len() < 3 {
        (
            state.clone(),
            if user_name.len() == 0 {
                Output::None
            } else {
                Output::Str("I'm sorry, but your name should consist of at least 3 letters.\n")
            },
        )
    } else if let Some(player) = realm.get_player_by_name(&user_name) {
        (
            new_state(
                SignInStep::AskingPassword,
                SignInData {
                    player: Some(player),
                    ..state.data.clone()
                },
            ),
            Output::None,
        )
    } else if RESERVED_NAMES.contains(&user_name) {
        (
            state.clone(),
            Output::Str("Yeah right, like I believe that...\n"),
        )
    } else {
        (
            new_state(
                SignInStep::AskingUserNameConfirmation,
                SignInData {
                    user_name,
                    ..state.data.clone()
                },
            ),
            Output::None,
        )
    }
}

fn process_asking_user_name_confirmation_input(
    state: &SignInState,
    _: &Realm,
    input: String,
) -> (SignInState, Output) {
    let answer = input.to_lowercase();
    if answer == "yes" || answer == "y" {
        (
            new_state(SignInStep::AskingSignUpPassword, state.data.clone()),
            Output::None,
        )
    } else if answer == "no" || answer == "n" {
        (
            new_state(SignInStep::SessionClosed, state.data.clone()),
            Output::Str("Okay, bye."),
        )
    } else {
        (
            state.clone(),
            Output::Str("Please answer with yes or no.\n"),
        )
    }
}

fn process_asking_sign_up_password_input(
    state: &SignInState,
    _: &Realm,
    input: String,
) -> (SignInState, Output) {
    if input.len() < 6 {
        (
            state.clone(),
            Output::String(colorize(
                "Please choose a password of at least 6 characters.\n",
                Color::Red,
            )),
        )
    } else if input.to_lowercase() == state.data.user_name.to_lowercase() {
        (
            state.clone(),
            Output::String(colorize(
                "Your password and your username may not be the same.\n",
                Color::Red,
            )),
        )
    } else if input == "123456" || input == "654321" {
        (
            state.clone(),
            Output::String(colorize(
                "Sorry, that password is too simple.\n",
                Color::Red,
            )),
        )
    } else {
        (
            new_state(
                SignInStep::AskingSignUpPasswordConfirmation,
                SignInData {
                    password: input,
                    ..state.data.clone()
                },
            ),
            Output::None,
        )
    }
}

fn process_asking_sign_up_password_confirmation_input(
    state: &SignInState,
    _: &Realm,
    input: String,
) -> (SignInState, Output) {
    if state.data.password == input {
        (
            new_state(SignInStep::AskingRace, state.data.clone()),
            Output::String(colorize("Password confirmed.\n", Color::Green)),
        )
    } else {
        (
            new_state(SignInStep::AskingSignUpPassword, state.data.clone()),
            Output::String(colorize("Passwords don't match.\n", Color::Red)),
        )
    }
}

fn process_asking_race_input(
    state: &SignInState,
    realm: &Realm,
    input: String,
) -> (SignInState, Output) {
    let answer = input.to_lowercase();
    if let Some(race) = realm.get_race_by_name(&answer) {
        (
            new_state(
                SignInStep::AskingClass,
                SignInData {
                    race: Some(race),
                    ..state.data.clone()
                },
            ),
            Output::String(colorize(
                &format!("\nYou have chosen to become a {}.\n", answer),
                Color::Green,
            )),
        )
    } else if answer.starts_with("info ") {
        let race_name = answer[5..].trim();
        (
            state.clone(),
            if let Some(race) = realm.get_race_by_name(&race_name) {
                Output::String(format!(
                    "\n{}\n  {}\n",
                    highlight(&capitalize(&race_name)),
                    split_lines(&race.get_description(), 78).join("\n  ")
                ))
            } else if race_name.starts_with("<") && race_name.ends_with(">") {
                Output::Str(
                    "\nSorry, you are supposed to replace <race> with the name of an \
                    actual race. For example: *info human*.\n",
                )
            } else {
                Output::String(format!(
                    "\nI don't know anything about the \"{}\" race.\n",
                    race_name
                ))
            },
        )
    } else {
        (state.clone(), Output::None)
    }
}

fn process_asking_class_input(
    state: &SignInState,
    realm: &Realm,
    input: String,
) -> (SignInState, Output) {
    let answer = input.to_lowercase();
    let classes = match &state.data.race {
        Some(race) => race
            .get_classes()
            .iter()
            .filter_map(|object_ref| realm.get_class(object_ref.get_id()))
            .collect(),
        None => Vec::new(),
    };

    if let Some(class) = classes.iter().find(|class| class.get_name() == answer) {
        (
            new_state(
                SignInStep::AskingGender,
                SignInData {
                    class: Some(class.clone()),
                    ..state.data.clone()
                },
            ),
            Output::String(colorize(
                &format!("\nYou have chosen to become a {}.\n", answer),
                Color::Green,
            )),
        )
    } else if answer.starts_with("info ") {
        let class_name = answer[5..].trim();
        (
            state.clone(),
            if let Some(class) = classes.iter().find(|class| class.get_name() == class_name) {
                Output::String(format!(
                    "\n{}\n  {}\n",
                    highlight(&capitalize(&class_name)),
                    split_lines(&class.get_description(), 78).join("\n  ")
                ))
            } else if class_name.starts_with("<") && class_name.ends_with(">") {
                Output::Str(
                    "\nSorry, you are supposed to replace <class> with the name of an \
                    actual class. For example: *info knight*.\n",
                )
            } else {
                Output::String(format!(
                    "\nI don't know anything about the \"{}\" class.\n",
                    class_name
                ))
            },
        )
    } else if answer == "back" || answer == "b" {
        (
            new_state(SignInStep::AskingRace, state.data.clone()),
            Output::None,
        )
    } else {
        (state.clone(), Output::None)
    }
}

fn validate_user_name(input: String) -> String {
    let mut user_name = String::new();
    let mut num_special_chars = 0;
    for character in input.chars() {
        if is_letter(character) {
            user_name.push(if user_name.len() == 0 {
                character.to_ascii_uppercase()
            } else {
                character
            });
        } else if user_name.len() > 0
            && num_special_chars == 0
            && (character == '\'' || character == '-')
        {
            user_name.push(character);
            num_special_chars += 1;
        }
        if user_name.len() >= 12 {
            break;
        }
    }
    if user_name.len() > 1 && !is_letter(user_name.chars().rev().next().unwrap_or_default()) {
        user_name = user_name.chars().take(user_name.len() - 1).collect();
    }
    user_name
}
