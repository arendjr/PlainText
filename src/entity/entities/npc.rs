use crate::{
    entity::{Character, Entity, EntityId, EntityRef, EntityType},
    entity_copy_prop, entity_string_prop,
};
use serde::{Deserialize, Serialize};

serializable_flags! {
    pub struct NpcFlags: u32 {
        const AlwaysUseDefiniteArticle = 0b00000001;
    }
}

#[derive(Clone, Debug, Deserialize, PartialEq, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Npc {
    #[serde(skip)]
    id: EntityId,
    character: Character,
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
}

impl Npc {
    entity_copy_prop!(pub, flags, set_flags, NpcFlags);

    pub fn has_flags(&self, flags: NpcFlags) -> bool {
        self.flags & flags == flags
    }

    pub fn hydrate(id: EntityId, json: &str) -> Result<Box<dyn Entity>, String> {
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

    fn as_entity(&self) -> Option<&dyn Entity> {
        Some(self)
    }

    fn as_entity_mut(&mut self) -> Option<&mut dyn Entity> {
        Some(self)
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
        self.needs_sync || self.character.needs_sync()
    }

    fn plural_form(&self) -> &str {
        &self.plural_form
    }

    fn set_needs_sync(&mut self, needs_sync: bool) {
        self.needs_sync = needs_sync;

        if !needs_sync {
            self.character.set_needs_sync(needs_sync);
        }
    }

    fn set_property(&mut self, prop_name: &str, value: &str) -> Result<(), String> {
        match prop_name {
            "description" => self.set_description(value.to_owned()),
            "flags" => self.set_flags(NpcFlags::from_str(value)?),
            "indefiniteArticle" => self.set_indefinite_article(value.to_owned()),
            "name" => self.set_name(value.to_owned()),
            "pluralForm" => self.set_plural_form(value.to_owned()),
            _ => return self.character.set_property(prop_name, value),
        }
        Ok(())
    }

    fn to_json_value(&self) -> serde_json::Result<serde_json::Value> {
        serde_json::to_value(self)
    }
}
