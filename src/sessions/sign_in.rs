use super::{
    sign_up::{
        enter_sign_up_step, exit_sign_up_step, process_sign_up_input, prompt_sign_up_step,
        SignUpData, SignUpState,
    },
    ProcessSignUpInputResult, SessionOutput as Output,
};
use crate::{
    game_object::GameObject,
    objects::{Player, Realm},
    text_utils::is_letter,
};
use lazy_static::lazy_static;
use maplit::hashmap;
use serde_json::json;
use std::collections::HashMap;

#[derive(Clone, Debug, PartialEq)]
pub struct SignInState {
    step: SignInStep,
    data: SignInData,
}

impl SignInState {
    pub fn get_sign_in_user_name(&self) -> Option<&str> {
        if self.step == SignInStep::SignedIn {
            if let Some(player) = &self.data.player {
                Some(player.name())
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
    process_input: fn(&SignInState, &Realm, &str, String) -> ProcessInputResult,
}

pub struct ProcessInputResult {
    pub new_state: SignInState,
    pub output: Output,
    pub log_messages: Vec<String>,
}

impl ProcessInputResult {
    fn with_state(new_state: SignInState) -> Self {
        Self {
            new_state,
            output: Output::None,
            log_messages: Vec::new(),
        }
    }
}

pub fn process_input(
    state: &SignInState,
    realm: &Realm,
    source: &str,
    input: String,
) -> ProcessInputResult {
    lazy_static! {
        static ref SIGN_IN_STEPS: HashMap<SignInStep, StepImpl> = get_sign_in_steps();
    }

    match SIGN_IN_STEPS.get(&state.step) {
        Some(step) => {
            let ProcessInputResult {
                new_state,
                output,
                log_messages,
            } = (step.process_input)(state, realm, source, input);
            if let Some(new_step) = SIGN_IN_STEPS.get(&new_state.step) {
                let prompt_output = (new_step.prompt)(&new_state.data);
                let outputs = if new_state == *state {
                    vec![output, prompt_output]
                } else {
                    let exit_output = (step.exit)(&new_state.data);
                    let enter_output = (new_step.enter)(&new_state.data, realm);
                    vec![output, exit_output, enter_output, prompt_output]
                };

                ProcessInputResult {
                    new_state,
                    output: Output::combine(outputs).process_highlights(),
                    log_messages,
                }
            } else {
                let exit_output = (step.exit)(&new_state.data);
                ProcessInputResult {
                    new_state,
                    output: Output::combine(vec![output, exit_output]).process_highlights(),
                    log_messages,
                }
            }
        }
        None => ProcessInputResult::with_state(state.clone()),
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
            enter: |_, _| Output::Json(json!({ "inputType": "password" })),
            exit: |_| Output::Json(json!({ "inputType": "text" })),
            prompt: |_| Output::Str("Please enter your password: "),
            process_input: process_asking_password_input,
        },

        SignInStep::AskingSignUpData => StepImpl {
            enter: |data, realm| enter_sign_up_step(data.sign_up_state.as_ref().unwrap(), realm),
            exit: |data| exit_sign_up_step(data.sign_up_state.as_ref().unwrap()),
            prompt: |data| prompt_sign_up_step(data.sign_up_state.as_ref().unwrap()),
            process_input: |state, realm, _, input| {
                let ProcessSignUpInputResult {
                    new_state: new_sign_up_state,
                    output,
                    log_messages
                } = process_sign_up_input(state.data.sign_up_state.as_ref().unwrap(), realm, input);
                ProcessInputResult {
                    new_state: new_state(
                        if new_sign_up_state.is_session_closed() {
                            SignInStep::SessionClosed
                        } else if new_sign_up_state.is_signed_up() {
                            SignInStep::SignedIn
                        } else {
                            SignInStep::AskingSignUpData
                        },
                        SignInData {
                            player: state.data.player.clone(),
                            sign_up_state: Some(new_sign_up_state)
                        }
                    ),
                    output,
                    log_messages
                }
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
    _: &str,
    input: String,
) -> ProcessInputResult {
    lazy_static! {
        static ref RESERVED_NAMES: Vec<String> = vec![
            "Admin".to_owned(),
            "Administrator".to_owned(),
            "Root".to_owned()
        ];
    }
    let user_name = validate_user_name(input.to_lowercase());
    if user_name.len() < 3 {
        ProcessInputResult {
            new_state: state.clone(),
            output: if user_name.is_empty() {
                Output::None
            } else {
                Output::Str("I'm sorry, but your name should consist of at least 3 letters.\n")
            },
            log_messages: vec![],
        }
    } else if let Some(player) = realm.player_by_name(&user_name) {
        ProcessInputResult::with_state(new_state(
            SignInStep::AskingPassword,
            SignInData {
                player: Some(player.clone()),
                ..state.data.clone()
            },
        ))
    } else if RESERVED_NAMES.contains(&user_name) {
        ProcessInputResult {
            new_state: state.clone(),
            output: Output::Str("Yeah right, like I believe that...\n"),
            log_messages: vec![],
        }
    } else {
        ProcessInputResult::with_state(new_state(
            SignInStep::AskingSignUpData,
            SignInData {
                sign_up_state: Some(SignUpState::new(user_name)),
                ..state.data.clone()
            },
        ))
    }
}

fn process_asking_password_input(
    state: &SignInState,
    _: &Realm,
    _: &str,
    input: String,
) -> ProcessInputResult {
    if let Some(player) = &state.data.player {
        let name = player.name();
        if player.matches_password(&input) {
            let log_messages = vec![format!("Authentication success for player \"{}\"", name)];

            if player.session_id().is_some() {
                ProcessInputResult {
                    new_state: new_state(SignInStep::SessionClosed, state.data.clone()),
                    output: Output::Str(
                        "Cannot sign you in because you're already signed in from another \
                        location.\n",
                    ),
                    log_messages,
                }
            } else {
                ProcessInputResult {
                    new_state: new_state(SignInStep::SignedIn, state.data.clone()),
                    output: Output::String(format!(
                        "Welcome back, {}. Type *help* if you're feeling lost.\n",
                        name
                    )),
                    log_messages,
                }
            }
        } else {
            ProcessInputResult {
                new_state: state.clone(),
                output: Output::Str("Password incorrect.\n"),
                log_messages: vec![format!("Authentication failed for player \"{}\"", name)],
            }
        }
    } else {
        ProcessInputResult::with_state(state.clone())
    }
}

fn validate_user_name(input: String) -> String {
    let mut user_name = String::new();
    let mut num_special_chars = 0;
    for character in input.chars() {
        if is_letter(character) {
            user_name.push(if user_name.is_empty() {
                character.to_ascii_uppercase()
            } else {
                character
            });
        } else if !user_name.is_empty()
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
