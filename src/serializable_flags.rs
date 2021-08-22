use serde::de::{Deserialize, Deserializer, Visitor};

pub struct Flag<T>(pub T);

impl<'de, T> Deserialize<'de> for Flag<T>
where
    T: Default + Visitor<'de, Value = T>,
{
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        deserializer.deserialize_str(T::default()).map(Flag)
    }
}

#[macro_export]
macro_rules! serializable_flags {
    ($vis:vis struct $BitFlags:ident: $T:ty {$(
        const $Flag:ident = $value:expr;
    )+}) => {
        bitflags::bitflags! {
            #[derive(Default)]
            pub struct $BitFlags: $T {
                const None = 0;
                $(
                    const $Flag = $value;
                )+
            }
        }

        impl $BitFlags {
            pub fn from_str(flags_str: &str) -> Result<Self, String> {
                let mut bits = 0;
                for flag in flags_str.split('|') {
                    if !flag.is_empty() {
                        match flag {
                            $(
                                stringify!($Flag) => bits |= $value,
                            )+
                            unknown_flag => return Err(format!("Dropped unknown flag: \"{}\"", unknown_flag))
                        }
                    }
                }
                Ok($BitFlags { bits })
            }
        }

        impl<'de> serde::de::Visitor<'de> for $BitFlags {
            type Value = Self;

            fn expecting(&self, formatter: &mut core::fmt::Formatter) -> core::fmt::Result {
                write!(formatter, "string with pipe-separated flags")
            }

            fn visit_str<E>(self, value: &str) -> Result<Self::Value, E>
            where
                E: serde::de::Error,
            {
                Self::from_str(value).map_err(serde::de::Error::custom)
            }
        }

        impl<'de> serde::Deserialize<'de> for $BitFlags {
            fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
            where
                D: serde::Deserializer<'de>
            {
                deserializer.deserialize_str($BitFlags { bits: 0 })
            }
        }

        impl serde::Serialize for $BitFlags {
            fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
            where S: serde::Serializer
            {
                let mut value = String::new();
                $(
                    if self.bits & $value == self.bits {
                        if !value.is_empty() {
                            value.push('|');
                        }
                        value += stringify!($Flag);
                    }
                )+
                serializer.serialize_str(&value)
            }
        }
    };
}
