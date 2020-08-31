use lazy_static::lazy_static;
use maplit::hashmap;
use serde_json::json;
use std::collections::HashMap;

use crate::game_object::GameObject;
use crate::logs::{log_session_event, LogSender};
use crate::objects::{Player, Realm};
use crate::sessions::SessionOutput as Output;
use crate::text_utils::is_letter;

use super::sign_up::{
    enter_sign_up_step, exit_sign_up_step, process_sign_up_input, prompt_sign_up_step, SignUpData,
    SignUpState,
};

#[derive(Clone, Debug, PartialEq)]
pub struct SignInState {
    step: SignInStep,
    data: SignInData,
}

impl SignInState {
    pub fn get_sign_in_user_name(&self) -> Option<&str> {
        if self.step == SignInStep::SignedIn {
            if let Some(player) = &self.data.player {
                Some(player.get_name())
            } else if let Some(sign_up_state) = &self.data.sign_up_state {
                Some(&sign_up_state.data.user_name)
            } else {
                None
            }
        } else {
            None
        }
    }

    pub fn get_sign_up_data(&self) -> Option<&SignUpData> {
        if self.step == SignInStep::SignedIn {
            self.data.sign_up_state.as_ref().map(|state| &state.data)
        } else {
            None
        }
    }

    pub fn is_session_closed(&self) -> bool {
        self.step == SignInStep::SessionClosed
    }

    pub fn new() -> Self {
        new_state(
            SignInStep::AskingUserName,
            SignInData {
                player: None,
                sign_up_state: None,
            },
        )
    }
}

#[derive(Clone, Debug, Eq, Hash, PartialEq)]
enum SignInStep {
    AskingPassword,
    AskingSignUpData,
    AskingUserName,
    SessionClosed,
    SignedIn,
}

#[derive(Clone, Debug, PartialEq)]
struct SignInData {
    player: Option<Player>,
    sign_up_state: Option<SignUpState>,
}

struct StepImpl {
    enter: fn(&SignInData, &Realm) -> Output,
    exit: fn(&SignInData) -> Output,
    prompt: fn(&SignInData) -> Output,
    process_input: fn(&SignInState, &Realm, &LogSender, &String, String) -> (SignInState, Output),
}

pub fn process_input(
    state: &SignInState,
    realm: &Realm,
    log_tx: &LogSender,
    source: &String,
    input: String,
) -> (SignInState, Output) {
    lazy_static! {
        static ref SIGN_IN_STEPS: HashMap<SignInStep, StepImpl> = get_sign_in_steps();
    }

    match SIGN_IN_STEPS.get(&state.step) {
        Some(step) => {
            let (new_state, output) = (step.process_input)(state, realm, log_tx, source, input);
            if let Some(new_step) = SIGN_IN_STEPS.get(&new_state.step) {
                let prompt_output = (new_step.prompt)(&new_state.data);
                let outputs = if new_state == *state {
                    vec![output, prompt_output]
                } else {
                    let exit_output = (step.exit)(&new_state.data);
                    let enter_output = (new_step.enter)(&new_state.data, realm);
                    vec![output, exit_output, enter_output, prompt_output]
                };

                (new_state, Output::combine(outputs).process_highlights())
            } else {
                (new_state, output)
            }
        }
        None => (state.clone(), Output::None),
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

        SignInStep::AskingPassword => StepImpl {
            enter: |_, _| Output::JSON(json!({ "inputType": "password" })),
            exit: |_| Output::JSON(json!({ "inputType": "text" })),
            prompt: |_| Output::Str("Please enter your password: "),
            process_input: process_asking_password_input,
        },


        SignInStep::AskingSignUpData => StepImpl {
            enter: |data, realm| enter_sign_up_step(&data.sign_up_state.as_ref().unwrap(), realm),
            exit: |data| exit_sign_up_step(&data.sign_up_state.as_ref().unwrap()),
            prompt: |data| prompt_sign_up_step(&data.sign_up_state.as_ref().unwrap()),
            process_input: |state, realm, _, _, input| {
                let (sign_up_state, output) = process_sign_up_input(
                    &state.data.sign_up_state.as_ref().unwrap(),
                    realm,
                    input
                );
                (
                    new_state(
                        if sign_up_state.is_session_closed() {
                            SignInStep::SessionClosed
                        } else if sign_up_state.is_signed_up() {
                            SignInStep::SignedIn
                        } else {
                            SignInStep::AskingSignUpData
                        },
                        SignInData { sign_up_state: Some(sign_up_state), ..state.data.clone() }
                    ),
                    output
                )
            }
        }
    }
}

fn new_state(step: SignInStep, data: SignInData) -> SignInState {
    SignInState { step, data }
}

fn process_asking_user_name_input(
    state: &SignInState,
    realm: &Realm,
    _: &LogSender,
    _: &String,
    input: String,
) -> (SignInState, Output) {
    lazy_static! {
        static ref RESERVED_NAMES: Vec<String> = vec![
            "Admin".to_owned(),
            "Administrator".to_owned(),
            "Root".to_owned()
        ];
    }
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
                SignInStep::AskingSignUpData,
                SignInData {
                    sign_up_state: Some(SignUpState::new(user_name)),
                    ..state.data.clone()
                },
            ),
            Output::None,
        )
    }
}

fn process_asking_password_input(
    state: &SignInState,
    _: &Realm,
    log_tx: &LogSender,
    source: &String,
    input: String,
) -> (SignInState, Output) {
    if let Some(player) = &state.data.player {
        let name = player.get_name();
        if player.matches_password(&input) {
            log_session_event(
                log_tx,
                source.clone(),
                format!("Authentication success for player \"{}\"", name),
            );

            if player.get_session_id().is_some() {
                (
                    new_state(SignInStep::SessionClosed, state.data.clone()),
                    Output::Str(
                        "Cannot sign you in because you're already signed in from another \
                        location.\n",
                    ),
                )
            } else {
                (
                    new_state(SignInStep::SignedIn, state.data.clone()),
                    Output::String(format!(
                        "Welcome back, {}. Type *help* if you're feeling lost.\n",
                        name
                    )),
                )
            }
        } else {
            log_session_event(
                log_tx,
                source.clone(),
                format!("Authentication failed for player \"{}\"", name),
            );

            (state.clone(), Output::Str("Password incorrect.\n"))
        }
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
