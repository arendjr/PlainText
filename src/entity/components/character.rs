use crate::entity::{Entity, EntityRef};
use crate::sessions::SignUpData;
use crate::vector3d::Vector3D;
use crate::{entity_copy_prop, entity_ref_prop};
use futures::future::AbortHandle;
use serde::{Deserialize, Serialize};
use std::time::SystemTime;

/// Component that is shared across entities representing characters (players and NPCs).
#[derive(Clone, Debug, Deserialize, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Character {
    #[serde(skip_serializing_if = "Option::is_none")]
    class: Option<EntityRef>,

    /// Action the character is currently occupied with.
    #[serde(skip)]
    current_action: CharacterAction,

    current_room: EntityRef,

    /// The direction the character is currently facing.
    #[serde(default)]
    direction: Vector3D,

    #[serde(default)]
    gender: Gender,

    gold: u32,

    #[serde(skip)]
    group: Option<EntityRef>,

    height: f32,

    /// Health points.
    hp: i16,

    #[serde(default, skip_serializing_if = "Vec::is_empty")]
    inventory: Vec<EntityRef>,

    /// Magic points.
    mp: i16,

    #[serde(skip)]
    needs_sync: bool,

    race: EntityRef,

    /// Handle that can be invoked to cancel the timer for resetting the current action.
    #[serde(skip)]
    reset_action_abort_handle: Option<AbortHandle>,

    /// The character's base weight, excluding any inventory carried.
    weight: f32,
}

impl Character {
    entity_copy_prop!(pub, class, set_class, Option<EntityRef>);

    pub fn current_action(&self) -> CharacterAction {
        self.current_action
    }

    pub fn set_action(&mut self, action: CharacterAction, reset_abort_handle: AbortHandle) {
        self.current_action = action;

        if let Some(abort_handle) = self.reset_action_abort_handle.as_mut() {
            abort_handle.abort();
        }

        self.reset_action_abort_handle = Some(reset_abort_handle);
    }

    pub fn set_indefinite_action(&mut self, action: CharacterAction) {
        self.current_action = action;
        self.reset_action_abort_handle = None;
    }

    entity_copy_prop!(pub, current_room, set_current_room, EntityRef);

    entity_ref_prop!(pub, direction, set_direction, Vector3D);
    entity_copy_prop!(pub, gender, set_gender, Gender);
    entity_copy_prop!(pub, gold, set_gold, u32);

    pub fn group(&self) -> Option<EntityRef> {
        self.group
    }

    pub fn set_group(&mut self, group: EntityRef) {
        self.group = Some(group);
    }

    pub fn unset_group(&mut self) {
        self.group = None;
    }

    entity_copy_prop!(pub, height, set_height, f32);
    entity_copy_prop!(pub, hp, set_hp, i16);
    entity_ref_prop!(pub, inventory, set_inventory, Vec<EntityRef>);

    entity_copy_prop!(pub, mp, set_mp, i16);

    pub fn needs_sync(&self) -> bool {
        self.needs_sync
    }

    pub fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;
    }

    entity_copy_prop!(pub, race, set_race, EntityRef);
    entity_copy_prop!(pub, weight, set_weight, f32);

    pub fn from_sign_up_data(sign_up_data: &SignUpData) -> Self {
        let race = sign_up_data.race.clone().unwrap();
        Self {
            class: sign_up_data.class.as_ref().map(|class| class.entity_ref()),
            current_action: CharacterAction::default(),
            current_room: race.starting_room(),
            direction: Vector3D::new(0, 0, 0),
            gender: sign_up_data.gender,
            gold: sign_up_data.gold,
            group: None,
            height: sign_up_data.height,
            hp: sign_up_data.stats.max_hp(),
            inventory: Vec::new(),
            mp: sign_up_data.stats.max_mp(),
            needs_sync: true,
            race: race.entity_ref(),
            reset_action_abort_handle: None,
            weight: sign_up_data.weight,
        }
    }

    pub fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "class" => self.set_class(Some(EntityRef::from_str(value)?)),
            "current_room" => self.set_current_room(EntityRef::from_str(value)?),
            "direction" => self.set_direction(Vector3D::from_str(value)?),
            "gender" => self.set_gender(Gender::from_str(value)?),
            "gold" => self.set_gold(value.parse().map_err(|error| format!("{:?}", error))?),
            "group" => self.set_group(EntityRef::from_str(value)?),
            "height" => self.set_height(value.parse().map_err(|error| format!("{:?}", error))?),
            "hp" => self.set_hp(value.parse().map_err(|error| format!("{:?}", error))?),
            "inventory" => self.set_inventory(EntityRef::vec_from_str(value)?),
            "mp" => self.set_mp(value.parse().map_err(|error| format!("{:?}", error))?),
            "race" => self.set_race(EntityRef::from_str(value)?),
            "weight" => self.set_weight(value.parse().map_err(|error| format!("{:?}", error))?),
            _ => return Err(format!("No property named \"{}\"", prop_name)),
        }
        Ok(())
    }

    /// Returns an optional message that should be displayed if the character attempts to
    /// perform some action while it is stunned.
    ///
    /// Note that other character actions besides just `CharacterAction::Stunned` can cause
    /// a character to be stunned. For instance, a character occupied in battle can also be
    /// considered as stunned, because this will likewise prevent them from performing
    /// certain other actions.
    pub fn stun_message(&self) -> Option<String> {
        let stunned_end_time = match self.current_action {
            CharacterAction::Fighting { end_time, .. } | CharacterAction::Stunned { end_time } => {
                Some(end_time)
            }
            _ => None,
        };

        if let Some(end_time) = stunned_end_time {
            if let Ok(remaining_duration) = end_time.duration_since(SystemTime::now()) {
                let secs = remaining_duration.as_secs_f32().ceil() as u64;
                match secs {
                    1 => Some(format!("Please wait {} second.", secs)),
                    _ => Some(format!("Please wait {} seconds.", secs)),
                }
            } else {
                Some("Please wait.".to_owned())
            }
        } else {
            None
        }
    }
}

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum CharacterAction {
    Idle,
    Walking,
    Running,
    Fighting {
        target: EntityRef,
        end_time: SystemTime,
    },
    Guarding {
        target: EntityRef,
    },
    Stunned {
        end_time: SystemTime,
    },
}

impl CharacterAction {
    /// Returns whether the character can (or will) follow party members who are in the same room.
    pub fn can_follow_others(&self) -> bool {
        match self {
            CharacterAction::Idle | CharacterAction::Walking | CharacterAction::Running => true,
            CharacterAction::Fighting { .. }
            | CharacterAction::Guarding { .. }
            | CharacterAction::Stunned { .. } => false,
        }
    }
}

impl Default for CharacterAction {
    fn default() -> Self {
        Self::Idle
    }
}

#[derive(Clone, Copy, Debug, Deserialize, PartialEq, Serialize)]
#[serde(rename_all = "snake_case")]
pub enum Gender {
    Unspecified,
    Male,
    Female,
}

impl Gender {
    pub fn from_str(gender_str: &str) -> Result<Self, String> {
        match gender_str {
            "male" => Ok(Gender::Male),
            "female" => Ok(Gender::Female),
            "unspecified" => Ok(Gender::Unspecified),
            _ => Err(format!("Unrecognized gender \"{}\"", gender_str)),
        }
    }

    pub fn to_str(self) -> &'static str {
        match self {
            Gender::Male => "male",
            Gender::Female => "female",
            Gender::Unspecified => "",
        }
    }
}

impl Default for Gender {
    fn default() -> Self {
        Self::Unspecified
    }
}

impl ToString for Gender {
    fn to_string(&self) -> String {
        self.to_str().to_owned()
    }
}
