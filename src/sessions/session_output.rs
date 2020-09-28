use crate::text_utils::process_highlights;

#[derive(Clone, Copy, Debug, PartialEq)]
pub struct SessionPromptInfo {
    pub hp: i16,
    pub mp: i16,
}

#[derive(Debug, PartialEq)]
pub enum SessionOutput {
    None,
    Str(&'static str),
    String(String),
    JSON(serde_json::Value),
    Prompt(SessionPromptInfo),
    Aggregate(Vec<SessionOutput>),
}

impl SessionOutput {
    pub fn combine(mut outputs: Vec<Self>) -> Self {
        if outputs.len() == 0 {
            Self::None
        } else if outputs.len() == 1 {
            outputs.remove(0)
        } else if outputs.len() == 2 {
            if outputs[0] == Self::None {
                outputs.remove(1)
            } else if outputs[1] == Self::None {
                outputs.remove(0)
            } else {
                Self::Aggregate(outputs)
            }
        } else {
            let mut filtered_outputs = outputs
                .into_iter()
                .filter(|output| *output != Self::None)
                .collect::<Vec<Self>>();

            if filtered_outputs.len() == 0 {
                Self::None
            } else if filtered_outputs.len() == 1 {
                filtered_outputs.remove(0)
            } else {
                Self::Aggregate(filtered_outputs)
            }
        }
    }

    pub fn process_highlights(self) -> Self {
        match self {
            Self::Str(string) => Self::String(process_highlights(string)),
            Self::String(string) => Self::String(process_highlights(&string)),
            Self::Aggregate(outputs) => Self::Aggregate(
                outputs
                    .into_iter()
                    .map(|output| output.process_highlights())
                    .collect(),
            ),
            other => other,
        }
    }

    pub fn with(self, other: Self) -> Self {
        if self == Self::None {
            other
        } else if other == Self::None {
            self
        } else {
            // FIXME: Prevent nested aggregates where possible
            Self::Aggregate(vec![self, other])
        }
    }
}
