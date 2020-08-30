use lazy_static::lazy_static;
use maplit::hashmap;
use serde_json::json;
use std::collections::HashMap;
use std::str::FromStr;

use crate::character_stats::{CharacterStat, CharacterStats};
use crate::colors::Color;
use crate::game_object::GameObject;
use crate::objects::{Class, Gender, Race, Realm};
use crate::sessions::SessionOutput as Output;
use crate::text_utils::{
    capitalize, colorize, format_columns, highlight, split_lines, FormatOptions,
};

lazy_static! {
    static ref SIGN_UP_STEPS: HashMap<SignUpStep, StepImpl> = get_sign_up_steps();
}

#[derive(Clone, Debug)]
pub struct SignUpState {
    step: SignUpStep,
    pub data: SignUpData,
}

impl SignUpState {
    pub fn is_signed_up(&self) -> bool {
        self.step == SignUpStep::SignedUp
    }

    pub fn new(user_name: String) -> Self {
        new_state(
            SignUpStep::AskingUserNameConfirmation,
            SignUpData::with_user_name(user_name),
        )
    }
}

#[derive(Clone, Debug, Eq, Hash, PartialEq)]
enum SignUpStep {
    AskingClass,
    AskingExtraStats,
    AskingGender,
    AskingPassword,
    AskingPasswordConfirmation,
    AskingRace,
    AskingSignUpConfirmation,
    AskingUserNameConfirmation,
    SessionClosed,
    SignedUp,
}

#[derive(Clone, Debug)]
pub struct SignUpData {
    pub class: Option<Class>,
    pub gender: Gender,
    pub height: f32,
    pub password: String,
    pub race: Option<Race>,
    pub stats: CharacterStats,
    pub user_name: String,
    pub weight: f32,
}

impl SignUpData {
    fn new() -> Self {
        Self::with_user_name("".to_owned())
    }

    fn with_user_name(user_name: String) -> Self {
        Self {
            class: None,
            gender: Gender::Unspecified,
            height: 0.0,
            password: String::new(),
            race: None,
            stats: CharacterStats::new(),
            user_name,
            weight: 0.0,
        }
    }
}

struct StepImpl {
    enter: fn(&SignUpData, &Realm) -> Output,
    exit: fn(&SignUpData) -> Output,
    prompt: fn(&SignUpData) -> Output,
    process_input: fn(&SignUpState, &Realm, String) -> (SignUpState, Output),
}

pub fn enter_sign_up_step(state: &SignUpState, realm: &Realm) -> Output {
    match SIGN_UP_STEPS.get(&state.step) {
        Some(step) => (step.enter)(&state.data, realm),
        None => Output::None,
    }
}

pub fn exit_sign_up_step(state: &SignUpState) -> Output {
    match SIGN_UP_STEPS.get(&state.step) {
        Some(step) => (step.exit)(&state.data),
        None => Output::None,
    }
}

pub fn prompt_sign_up_step(state: &SignUpState) -> Output {
    match SIGN_UP_STEPS.get(&state.step) {
        Some(step) => (step.prompt)(&state.data),
        None => Output::None,
    }
}

pub fn process_sign_up_input(
    state: &SignUpState,
    realm: &Realm,
    input: String,
) -> (SignUpState, Output) {
    match SIGN_UP_STEPS.get(&state.step) {
        Some(step) => (step.process_input)(state, realm, input),
        None => (state.clone(), Output::None),
    }
}

fn get_sign_up_steps() -> HashMap<SignUpStep, StepImpl> {
    hashmap! {
        SignUpStep::AskingUserNameConfirmation => StepImpl {
            enter: |_, _| Output::None,
            exit: |_| Output::None,
            prompt: |data| Output::String(format!("{}, did I get that right? ", data.user_name)),
            process_input: process_asking_user_name_confirmation_input,
        },

        SignUpStep::AskingPassword => StepImpl {
            enter: |_, _| Output::JSON(json!({ "inputType": "password" })),
            exit: |_| Output::None,
            prompt: |_| Output::Str("Please choose a password: "),
            process_input: process_asking_sign_up_password_input,
        },

        SignUpStep::AskingPasswordConfirmation => StepImpl {
            enter: |_, _| Output::None,
            exit: |_| Output::JSON(json!({ "inputType": "text" })),
            prompt: |_| Output::Str("Please confirm a password: "),
            process_input: process_asking_sign_up_password_confirmation_input,
        },

        SignUpStep::AskingRace => StepImpl {
            enter: enter_race,
            exit: |_| Output::None,
            prompt: |_| Output::Str(
                "\n\
                Please select the race you would like to use, or type *info <race>* to get \
                more information about a race.\n"
            ),
            process_input: process_asking_race_input
        },

        SignUpStep::AskingClass => StepImpl {
            enter: enter_class,
            exit: |_| Output::None,
            prompt: |_| Output::Str(
                "\n\
                Please select the class you would like to use, or type *info <class>* to \
                get more information about a class.\n"
            ),
            process_input: process_asking_class_input
        },

        SignUpStep::AskingGender => StepImpl {
            enter: |_, _| Output::Str(
                "\n\
                Please select which gender you would like your character to be.\n\
                Your gender has a minor influence on the physique of your character."
            ),
            exit: |_| Output::None,
            prompt: |_| Output::Str(
                "\n\
                Please choose *male* or *female*.\n\
                \n\
                To revisit your choice of class, type *back*.\n"
            ),
            process_input: process_asking_gender_input
        },

        SignUpStep::AskingExtraStats => StepImpl {
            enter: enter_extra_stats,
            exit: |_| Output::None,
            prompt: |data| Output::String(format!(
                "\n\
                Please enter the distribution you would like to use in the following form:\n\
                \n  *{}*\n  (Replace every part with a number, for a total of 9. Suggestion: {})\n\
                \n\
                To revisit your choice of gender, type *back*. If you want more \
                information about character stats, type *info stats*. If you just want to \
                accept the suggested stats, type *accept*.\n",
                if data.class.as_ref().map(|class| class.get_name()) == Some("barbarian") {
                    "<str> <dex> <vit> <end> <fai>"
                } else {
                    "<str> <dex> <vit> <end> <int> <fai>"
                },
                format_stats_string(&get_base_stats(&data).3, &data.class)

            )),
            process_input: process_asking_extra_stats_input
        },

        SignUpStep::AskingSignUpConfirmation => StepImpl {
            enter: enter_sign_up_confirmation,
            exit: |_| Output::None,
            prompt: |_| Output::Str("\nAre you ready to create a character with these stats?\n"),
            process_input: process_asking_sign_up_confirmation_input
        },

        SignUpStep::SignedUp => StepImpl {
            enter: |data, realm| Output::String(format!(
                "\nWelcome to {}, {}.\n",
                realm.get_name(),
                data.user_name,
            )),
            exit: |_| Output::None,
            prompt: |_| Output::None,
            process_input: |state, _, _| (state.clone(), Output::None)
        }
    }
}

fn enter_race(_: &SignUpData, realm: &Realm) -> Output {
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

fn enter_class(data: &SignUpData, realm: &Realm) -> Output {
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
                        .map(|object| object.get_name().to_owned()))
                    .collect(),
                FormatOptions::Capitalized | FormatOptions::Highlighted
            ),
            None => "(cannot determine classes without race)".to_owned(),
        }
    ))
}

fn enter_extra_stats(data: &SignUpData, _: &Realm) -> Output {
    let stats = get_base_stats(data).2;
    Output::String(format!(
        "\n\
        You have selected to become a {} {} {}.\n\
        Your base character stats are:\n\
        \n  *STR: {}*, *DEX: {}*, *VIT: {}*, *END: {}*, *INT: {}*, *FTH: {}*.\n\
        \n\
        You may assign an additional 9 points freely over your various attributes.\n",
        if data.gender == Gender::Male {
            "male"
        } else {
            "female"
        },
        match &data.race {
            Some(race) => race.get_adjective(),
            None => "",
        },
        match &data.class {
            Some(class) => class.get_name(),
            None => "",
        },
        stats.get(CharacterStat::STRENGTH),
        stats.get(CharacterStat::DEXTERITY),
        stats.get(CharacterStat::VITALITY),
        stats.get(CharacterStat::ENDURANCE),
        stats.get(CharacterStat::INTELLIGENCE),
        stats.get(CharacterStat::FAITH),
    ))
}

fn enter_sign_up_confirmation(data: &SignUpData, _: &Realm) -> Output {
    Output::String(format!(
        "\n\
        You have selected to become a {} {} {}.\n\
        Your final character stats are: \n\
        \n  *STR: {}*, *DEX: {}*, *VIT: {}*, *END: {}*, *INT: {}*, *FTH: {}*.\n",
        if data.gender == Gender::Male {
            "male"
        } else {
            "female"
        },
        data.race
            .as_ref()
            .map(|race| race.get_adjective())
            .unwrap_or(""),
        data.class
            .as_ref()
            .map(|class| class.get_name())
            .unwrap_or(""),
        data.stats.get(CharacterStat::STRENGTH),
        data.stats.get(CharacterStat::DEXTERITY),
        data.stats.get(CharacterStat::VITALITY),
        data.stats.get(CharacterStat::ENDURANCE),
        data.stats.get(CharacterStat::INTELLIGENCE),
        data.stats.get(CharacterStat::FAITH),
    ))
}

fn format_stats_string(stats: &CharacterStats, class: &Option<Class>) -> String {
    if class.as_ref().map(|class| class.get_name()) == Some("barbarian") {
        format!(
            "{} {} {} {} {}",
            stats.get(CharacterStat::STRENGTH),
            stats.get(CharacterStat::DEXTERITY),
            stats.get(CharacterStat::VITALITY),
            stats.get(CharacterStat::ENDURANCE),
            stats.get(CharacterStat::FAITH)
        )
    } else {
        format!(
            "{} {} {} {} {} {}",
            stats.get(CharacterStat::STRENGTH),
            stats.get(CharacterStat::DEXTERITY),
            stats.get(CharacterStat::VITALITY),
            stats.get(CharacterStat::ENDURANCE),
            stats.get(CharacterStat::INTELLIGENCE),
            stats.get(CharacterStat::FAITH)
        )
    }
}

fn get_base_stats(data: &SignUpData) -> (f32, f32, CharacterStats, CharacterStats) {
    let (mut height, mut weight, race_stats, race_stats_suggestion) = match &data.race {
        Some(race) => (
            race.get_height(),
            race.get_weight(),
            race.get_stats(),
            race.get_stats_suggestion(),
        ),
        None => (0.0, 0.0, CharacterStats::new(), CharacterStats::new()),
    };

    let (class_name, class_stats, class_stats_suggestion) = match &data.class {
        Some(class) => (
            class.get_name(),
            class.get_stats(),
            class.get_stats_suggestion(),
        ),
        None => ("", CharacterStats::new(), CharacterStats::new()),
    };

    let mut stats = race_stats + class_stats;
    let mut stats_suggestion = race_stats_suggestion + class_stats_suggestion;

    if class_name == "knight" {
        weight += 10.0;
    } else if class_name == "warrior" || class_name == "soldier" {
        weight += 5.0;
    } else if class_name == "barbarian" {
        stats.set(CharacterStat::INTELLIGENCE, 0);
        weight += 5.0;

        stats_suggestion.set(
            CharacterStat::FAITH,
            stats_suggestion.get(CharacterStat::FAITH)
                + stats_suggestion.get(CharacterStat::INTELLIGENCE),
        );
        stats_suggestion.set(CharacterStat::INTELLIGENCE, 0);
    }

    if data.gender == Gender::Male {
        stats.inc(CharacterStat::STRENGTH);
        height += 10.0;
        weight += 10.0;
    } else {
        stats.inc(CharacterStat::DEXTERITY);
        weight -= 10.0;
    }

    (height, weight, stats, stats_suggestion)
}

fn new_state(step: SignUpStep, data: SignUpData) -> SignUpState {
    SignUpState { step, data }
}

fn process_asking_user_name_confirmation_input(
    state: &SignUpState,
    _: &Realm,
    input: String,
) -> (SignUpState, Output) {
    let answer = input.to_lowercase();
    if answer == "yes" || answer == "y" {
        (
            new_state(SignUpStep::AskingPassword, state.data.clone()),
            Output::None,
        )
    } else if answer == "no" || answer == "n" {
        (
            new_state(SignUpStep::SessionClosed, SignUpData::new()),
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
    state: &SignUpState,
    _: &Realm,
    input: String,
) -> (SignUpState, Output) {
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
                SignUpStep::AskingPasswordConfirmation,
                SignUpData {
                    password: input,
                    ..state.data.clone()
                },
            ),
            Output::None,
        )
    }
}

fn process_asking_sign_up_password_confirmation_input(
    state: &SignUpState,
    _: &Realm,
    input: String,
) -> (SignUpState, Output) {
    if state.data.password == input {
        (
            new_state(SignUpStep::AskingRace, state.data.clone()),
            Output::String(colorize("Password confirmed.\n", Color::Green)),
        )
    } else {
        (
            new_state(SignUpStep::AskingPassword, state.data.clone()),
            Output::String(colorize("Passwords don't match.\n", Color::Red)),
        )
    }
}

fn process_asking_race_input(
    state: &SignUpState,
    realm: &Realm,
    input: String,
) -> (SignUpState, Output) {
    let answer = input.to_lowercase();
    if let Some(race) = realm.get_race_by_name(&answer) {
        (
            new_state(
                SignUpStep::AskingClass,
                SignUpData {
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
    state: &SignUpState,
    realm: &Realm,
    input: String,
) -> (SignUpState, Output) {
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
                SignUpStep::AskingGender,
                SignUpData {
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
            new_state(SignUpStep::AskingRace, state.data.clone()),
            Output::None,
        )
    } else {
        (state.clone(), Output::None)
    }
}

fn process_asking_gender_input(
    state: &SignUpState,
    _: &Realm,
    input: String,
) -> (SignUpState, Output) {
    let answer = input.to_lowercase();
    if answer == "male" || answer == "m" {
        (
            new_state(
                SignUpStep::AskingExtraStats,
                SignUpData {
                    gender: Gender::Male,
                    ..state.data.clone()
                },
            ),
            Output::String(colorize("\nYou have chosen to be male.\n", Color::Green)),
        )
    } else if answer == "female" || answer == "f" {
        (
            new_state(
                SignUpStep::AskingExtraStats,
                SignUpData {
                    gender: Gender::Female,
                    ..state.data.clone()
                },
            ),
            Output::String(colorize("\nYou have chosen to be female.\n", Color::Green)),
        )
    } else if answer == "back" || answer == "b" {
        (
            new_state(SignUpStep::AskingClass, state.data.clone()),
            Output::None,
        )
    } else {
        (state.clone(), Output::None)
    }
}

fn process_asking_extra_stats_input(
    state: &SignUpState,
    _: &Realm,
    input: String,
) -> (SignUpState, Output) {
    let answer = input.to_lowercase();
    if answer == "info stats" {
        (
            state.clone(),
            Output::Str(
                "\n\
                Your character has several attributes, each of which will have a value assigned. \
                Collectively, we call these your character stats. Here is an overview:\n\
                \n\
                *Strength* (STR)\n\
                  Strength primarily determines the power of your physical attacks. When\n\
                  wielding a shield, it also gives a small defense power up.\n\
                \n\
                *Dexterity* (DEX)\n\
                  Dexterity determines the speed with which attacks can be dealt. It also \n\
                  improves your chances of evading enemy attacks, and the chance of success when\n\
                  fleeing.\n\
                \n\
                *Vitality* (VIT)\n\
                  Vitality primarily determines your max. health points (HP).\n\
                \n\
                *Endurance* (END)\n\
                  Endurance primarily determines your physical defense power.\n\
                \n\
                *Intelligence* (INT)\n\
                  Intelligence determines your max. magic points (MP). Barbarians cannot assign \n\
                  any points to this attribute.\n\
                \n\
                *Faith* (FTH)\n\
                  Faith determines the magical defense power. It also decreases the chance that\n\
                  a spell will fail when cast.\n",
            ),
        )
    } else if answer == "back" || answer == "b" {
        (
            new_state(SignUpStep::AskingGender, state.data.clone()),
            Output::None,
        )
    } else {
        let (mut height, mut weight, mut stats, stats_suggestion) = get_base_stats(&state.data);
        let is_barbarian =
            state.data.class.as_ref().map(|class| class.get_name()) == Some("barbarian");

        let user_stats = if answer == "accept suggestion" || answer == "accept" || answer == "a" {
            stats_suggestion
        } else {
            let expected_num_attributes = if is_barbarian { 5 } else { 6 };
            if answer.split(' ').count() != expected_num_attributes {
                return (state.clone(), Output::None);
            }
            let mut attributes = answer.split(' ');
            CharacterStats::from_stats(
                u16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0) as i16,
                u16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0) as i16,
                u16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0) as i16,
                u16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0) as i16,
                if is_barbarian {
                    0
                } else {
                    u16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0) as i16
                },
                u16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0) as i16,
            )
        };

        if user_stats.total() != 9 {
            return (
                state.clone(),
                Output::String(colorize(
                    "\nThe total of attributes should be 9.\n",
                    Color::Red,
                )),
            );
        }

        stats = stats + &user_stats;
        height += (user_stats.get(CharacterStat::INTELLIGENCE) as f32)
            - (user_stats.get(CharacterStat::DEXTERITY) as f32) / 2.0;
        weight += user_stats.get(CharacterStat::STRENGTH) as f32;

        (
            new_state(
                SignUpStep::AskingSignUpConfirmation,
                SignUpData {
                    height,
                    weight,
                    stats,
                    ..state.data.clone()
                },
            ),
            Output::String(colorize(
                "\nYour character stats have been recorded.\n",
                Color::Green,
            )),
        )
    }
}

fn process_asking_sign_up_confirmation_input(
    state: &SignUpState,
    realm: &Realm,
    input: String,
) -> (SignUpState, Output) {
    let answer = input.to_lowercase();
    if realm.get_player_by_name(&state.data.user_name).is_some() {
        (
            new_state(SignUpStep::SessionClosed, SignUpData::new()),
            Output::Str(
                "Uh-oh, it appears someone has claimed your character name while \
                you were creating yours. I'm terribly sorry, but it appears you \
                will have to start over.\n",
            ),
        )
    } else if answer == "yes" || answer == "y" {
        (
            new_state(SignUpStep::SignedUp, state.data.clone()),
            Output::None,
        )
    } else if answer == "no" || answer == "n" || answer == "back" || answer == "b" {
        (
            new_state(SignUpStep::AskingExtraStats, state.data.clone()),
            Output::None,
        )
    } else {
        (
            state.clone(),
            Output::Str("Please answer with yes or no.\n"),
        )
    }
}
