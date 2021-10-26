use crate::{
    entity::{
        ActorState, Behavior, Character, Entity, EntityId, EntityRef, EntityType, Respawnable,
        StatsItem,
    },
    entity_copy_prop, entity_string_prop,
};
use serde::{Deserialize, Serialize};

serializable_flags! {
    pub struct NpcFlags: u32 {
        const AlwaysUseDefiniteArticle = 0b00000001;
    }
}

#[derive(Debug, Deserialize, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Npc {
    #[serde(skip)]
    id: EntityId,

    #[serde(skip)]
    pub actor_state: ActorState,

    #[serde(default, skip_serializing_if = "Option::is_none")]
    behavior: Option<Behavior>,

    #[serde(flatten)]
    pub character: Character,

    #[serde(default, skip_serializing_if = "String::is_empty")]
    description: String,

    #[serde(default, skip_serializing_if = "NpcFlags::is_default")]
    flags: NpcFlags,

    #[serde(default, skip_serializing_if = "String::is_empty")]
    indefinite_article: String,

    name: String,

    #[serde(skip)]
    needs_sync: bool,

    #[serde(default, rename = "plural", skip_serializing_if = "String::is_empty")]
    plural_form: String,

    #[serde(default, skip_serializing_if = "Option::is_none")]
    respawnable: Option<Box<Respawnable>>,

    #[serde(flatten)]
    stats_item: StatsItem,
}

impl Npc {
    entity_copy_prop!(pub, behavior, set_behavior, Option<Behavior>);

    entity_copy_prop!(pub, flags, set_flags, NpcFlags);

    pub fn has_flags(&self, flags: NpcFlags) -> bool {
        self.flags & flags == flags
    }

    pub fn hydrate<'a>(id: EntityId, json: &str) -> Result<Box<dyn Entity + 'a>, String> {
        let mut npc =
            serde_json::from_str::<Npc>(json).map_err(|error| format!("parse error: {}", error))?;
        npc.id = id;

        Ok(Box::new(npc))
    }

    pub fn set_indefinite_article(&mut self, indefinite_article: String) {
        self.indefinite_article = indefinite_article;
    }

    pub fn set_plural_form(&mut self, plural_form: String) {
        self.plural_form = plural_form;
    }
}

impl Entity for Npc {
    entity_string_prop!(name, set_name);
    entity_string_prop!(description, set_description);

    fn as_actor_state(&self) -> Option<&ActorState> {
        Some(&self.actor_state)
    }

    fn as_actor_state_mut(&mut self) -> Option<&mut ActorState> {
        Some(&mut self.actor_state)
    }

    fn as_character(&self) -> Option<&Character> {
        Some(&self.character)
    }

    fn as_character_mut(&mut self) -> Option<&mut Character> {
        Some(&mut self.character)
    }

    fn as_npc(&self) -> Option<&Self> {
        Some(self)
    }

    fn as_npc_mut(&mut self) -> Option<&mut Self> {
        Some(self)
    }

    fn as_respawnable(&self) -> Option<&Respawnable> {
        self.respawnable.as_ref().map(Box::as_ref)
    }

    fn as_respawnable_mut(&mut self) -> Option<&mut Respawnable> {
        self.respawnable.as_mut().map(Box::as_mut)
    }

    fn as_stats_item(&self) -> Option<&StatsItem> {
        Some(&self.stats_item)
    }

    fn as_stats_item_mut(&mut self) -> Option<&mut StatsItem> {
        Some(&mut self.stats_item)
    }

    fn dehydrate(&self) -> String {
        serde_json::to_string_pretty(self).unwrap_or_else(|error| {
            panic!(
                "Failed to serialize entity {:?}: {:?}",
                self.entity_ref(),
                error
            )
        })
    }

    fn entity_ref(&self) -> EntityRef {
        EntityRef::new(EntityType::Npc, self.id)
    }

    fn id(&self) -> EntityId {
        self.id
    }

    fn indefinite_article(&self) -> &str {
        &self.indefinite_article
    }

    fn indefinite_name(&self) -> String {
        if self.has_flags(NpcFlags::AlwaysUseDefiniteArticle) {
            format!("the {}", self.name())
        } else if self.indefinite_article().is_empty() {
            self.name().to_owned()
        } else {
            format!("{} {}", self.indefinite_article(), self.name())
        }
    }

    fn needs_sync(&self) -> bool {
        self.needs_sync
            || self.character.needs_sync()
            || self
                .respawnable
                .as_ref()
                .map(|respawnable| respawnable.needs_sync())
                .unwrap_or(false)
            || self.stats_item.needs_sync()
    }

    fn plural_form(&self) -> &str {
        &self.plural_form
    }

    fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;

        if !needs_sync {
            self.character.set_needs_sync(needs_sync);
            if let Some(respawnable) = self.respawnable.as_mut() {
                respawnable.set_needs_sync(needs_sync);
            }
            self.stats_item.set_needs_sync(needs_sync);
        }
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "description" => self.set_description(value.to_owned()),
            "flags" => self.set_flags(NpcFlags::from_str(value)?),
            "indefiniteArticle" => self.set_indefinite_article(value.to_owned()),
            "name" => self.set_name(value.to_owned()),
            "pluralForm" => self.set_plural_form(value.to_owned()),
            _ => {
                return self
                    .actor_state
                    .set_property(prop_name, value)
                    .or_else(|_| self.character.set_property(prop_name, value))
                    .or_else(|_| self.stats_item.set_property(prop_name, value))
                    .or_else(|_| match self.respawnable.as_mut() {
                        Some(respawnable) => respawnable.set_property(prop_name, value),
                        None => Err(format!("No property named \"{}\"", prop_name)),
                    })
            }
        }
        Ok(())
    }

    fn to_json_value(&self) -> serde_json::Result<serde_json::Value> {
        serde_json::to_value(self)
    }
}
