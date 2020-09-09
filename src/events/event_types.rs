macro_rules! replace_expr {
    ($_t:tt $sub:expr) => {
        $sub
    };
}

macro_rules! event_types {
    ($( $EventType:ident$( = $value:literal)? ),+) => {
        #[derive(Clone, Copy)]
        pub enum EventType {
            $($EventType$( = $value)?,)+
        }

        impl EventType {
            pub const NUM_EVENT_TYPES: usize = 0 $(+ replace_expr!($EventType 1))+;

            pub const ALL: [EventType; Self::NUM_EVENT_TYPES] = [$(EventType::$EventType),+];

            pub fn deserialize(string: &str) -> Option<EventType> {
                match string {
                    $(stringify!($EventType) => Some(EventType::$EventType)),+,
                    _ => None
                }
            }

            pub fn serialize(&self) -> &'static str {
                match self {
                    $(EventType::$EventType => stringify!($EventType)),+
                }
            }
        }
    }
}

event_types! {
    Area = 0,
    Flood,
    MovementSound,
    MovementVisual,
    Sound,
    Speech,
    Visual
}
