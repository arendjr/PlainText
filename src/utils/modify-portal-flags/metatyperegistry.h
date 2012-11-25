#ifndef METATYPEREGISTRY_H
#define METATYPEREGISTRY_H

#include <cstring>

#include <QMap>
#include <QScriptValue>
#include <QStringList>
#include <QVariant>

#include "conversionutil.h"
#include "foreach.h"


class MetaTypeRegistry {

    public:
        typedef QString (*TypeToUserStringFunc)(const QVariant &);
        typedef QVariant (*UserStringToTypeFunc)(const QString &);

        typedef QString (*TypeToJsonStringFunc)(const QVariant &);
        typedef QVariant (*JsonVariantToTypeFunc)(const QVariant &);

        struct UserStringConverters {
            TypeToUserStringFunc typeToUserStringConverter;
            UserStringToTypeFunc userStringToTypeConverter;
        };

        struct JsonConverters {
            TypeToJsonStringFunc typeToJsonStringConverter;
            JsonVariantToTypeFunc jsonVariantToTypeConverter;
        };

        static void registerMetaTypes(QScriptEngine *engine);

        static UserStringConverters userStringConverters(const char *typeName);

        static JsonConverters jsonConverters(const char *typeName);

    private:
        static QMap<QString, UserStringConverters> s_userStringConvertersMap;
        static QMap<QString, JsonConverters> s_jsonConvertersMap;
};


#define PT_DECLARE_METATYPE(Type)                                                                 \
    Q_DECLARE_METATYPE(Type)                                                                      \

#define PT_DECLARE_SERIALIZABLE_METATYPE(Type)                                                    \
    PT_DECLARE_METATYPE(Type)                                                                     \
    inline QString convert##Type##ToUserString(const QVariant &variant) {                         \
        return Type::toUserString(variant.value<Type>());                                         \
    }                                                                                             \
    inline QVariant convertUserStringTo##Type(const QString &string) {                            \
        Type value;                                                                               \
        Type::fromUserString(string, value);                                                      \
        return QVariant::fromValue(value);                                                        \
    }                                                                                             \
    inline QString convert##Type##ToJsonString(const QVariant &variant) {                         \
        return Type::toJsonString(variant.value<Type>());                                         \
    }                                                                                             \
    inline QVariant convertJsonVariantTo##Type(const QVariant &variant) {                         \
        Type value;                                                                               \
        Type::fromVariant(variant, value);                                                        \
        return QVariant::fromValue(value);                                                        \
    }

#define PT_ENUM_VALUE(Item) Item,

#define PT_ENUM_STRING(Item) #Item,

#define PT_DEFINE_ENUM(Type, ...)                                                                 \
    class Type {                                                                                  \
        public:                                                                                   \
            enum Values : unsigned int {                                                          \
                FOR_EACH(PT_ENUM_VALUE, __VA_ARGS__)                                              \
                NumValues                                                                         \
            } value;                                                                              \
            Type() = default;                                                                     \
            Type(Values value) : value(value) {}                                                  \
            const char *toCString() const {                                                       \
                static const char *strings[] = { FOR_EACH(PT_ENUM_STRING, __VA_ARGS__) "" };      \
                return strings[value];                                                            \
            }                                                                                     \
            QString toString() const {                                                            \
                return toCString();                                                               \
            }                                                                                     \
            static Type fromString(const QString &string) {                                       \
                static const char *strings[] = { FOR_EACH(PT_ENUM_STRING, __VA_ARGS__) "" };      \
                const char *data = string.toAscii().constData();                                  \
                for (int i = 0; strings[i][0] != '\0'; i++) {                                     \
                    if (strcmp(data, strings[i]) == 0) {                                          \
                        return (Values) i;                                                        \
                    }                                                                             \
                }                                                                                 \
                return (Values) 0;                                                                \
            }                                                                                     \
            int intValue() const {                                                                \
                return value;                                                                     \
            }                                                                                     \
            Type &operator=(Values newValue) {                                                    \
                value = newValue;                                                                 \
                return *this;                                                                     \
            }                                                                                     \
            Type &operator=(const QString &string) {                                              \
                *this = fromString(string);                                                       \
                return *this;                                                                     \
            }                                                                                     \
            bool operator==(Type other) const {                                                   \
                return value == other.value;                                                      \
            }                                                                                     \
            bool operator==(Values other) const {                                                 \
                return value == other;                                                            \
            }                                                                                     \
            bool operator!=(Type other) const {                                                   \
                return value != other.value;                                                      \
            }                                                                                     \
            bool operator!=(Values other) const {                                                 \
                return value != other;                                                            \
            }                                                                                     \
            static QScriptValue toScriptValue(QScriptEngine *engine, const Type &type) {          \
                Q_UNUSED(engine)                                                                  \
                return QScriptValue(type.toString());                                             \
            }                                                                                     \
            static void fromScriptValue(const QScriptValue &object, Type &type) {                 \
                type = object.toString();                                                         \
            }                                                                                     \
    };                                                                                            \
    PT_DECLARE_METATYPE(Type)                                                                     \
    inline QString convert##Type##ToUserString(const QVariant &variant) {                         \
        return variant.value<Type>().toString();                                                  \
    }                                                                                             \
    inline QVariant convertUserStringTo##Type(const QString &string) {                            \
        return QVariant::fromValue(Type::fromString(string));                                     \
    }                                                                                             \
    inline QString convert##Type##ToJsonString(const QVariant &variant) {                         \
        return ConversionUtil::jsString(variant.value<Type>().toString());                        \
    }                                                                                             \
    inline QVariant convertJsonVariantTo##Type(const QVariant &variant) {                         \
        return QVariant::fromValue(Type::fromString(variant.toString()));                         \
    }

#define PT_FLAG_VALUE(Item, Num) Item = 1 << Num,

#define PT_DEFINE_FLAGS(Type, ...)                                                                \
    class Type {                                                                                  \
        public:                                                                                   \
            enum Flags : unsigned int {                                                           \
                NoFlags = 0,                                                                      \
                FOR_EACH_COUNTED(PT_FLAG_VALUE, __VA_ARGS__)                                      \
                NumFlags = COUNT(__VA_ARGS__)                                                     \
            } value;                                                                              \
            Type() = default;                                                                     \
            Type(Flags value) : value(value) {}                                                   \
            Type(unsigned int value) : value((Flags) value) {}                                    \
            QString toString() const {                                                            \
                static const char *strings[] = { FOR_EACH(PT_ENUM_STRING, __VA_ARGS__) "" };      \
                QStringList stringList;                                                           \
                for (int i = 0; i < (int) NumFlags; i++) {                                        \
                    if (value & 1 << i) {                                                         \
                        stringList.append(strings[i]);                                            \
                    }                                                                             \
                }                                                                                 \
                return stringList.join("|");                                                      \
            }                                                                                     \
            static Type fromString(const QString &string) {                                       \
                static const char *strings[] = { FOR_EACH(PT_ENUM_STRING, __VA_ARGS__) "" };      \
                QStringList stringList = string.split('|');                                       \
                unsigned int flags = 0;                                                           \
                for (int i = 0; i < (int) NumFlags; i++) {                                        \
                    if (stringList.contains(strings[i])) {                                        \
                        flags |= 1 << i;                                                          \
                    }                                                                             \
                }                                                                                 \
                return (Flags) flags;                                                             \
            }                                                                                     \
            int intValue() const {                                                                \
                return value;                                                                     \
            }                                                                                     \
            Type &operator=(Flags newValue) {                                                     \
                value = newValue;                                                                 \
                return *this;                                                                     \
            }                                                                                     \
            Type &operator=(const QString &string) {                                              \
                *this = fromString(string);                                                       \
                return *this;                                                                     \
            }                                                                                     \
            Flags operator|(Type other) const {                                                   \
                return (Flags) ((unsigned int) value | (unsigned int) other.value);               \
            }                                                                                     \
            Flags operator|(Flags other) const {                                                  \
                return (Flags) ((unsigned int) value | (unsigned int) other);                     \
            }                                                                                     \
            Flags operator|(unsigned int other) const {                                           \
                return (Flags) ((unsigned int) value | other);                                    \
            }                                                                                     \
            Type &operator|=(Type other) {                                                        \
                value = (Flags) ((unsigned int) value | (unsigned int) other.value);              \
                return *this;                                                                     \
            }                                                                                     \
            Type &operator|=(Flags other) {                                                       \
                value = (Flags) ((unsigned int) value | (unsigned int) other);                    \
                return *this;                                                                     \
            }                                                                                     \
            Type &operator|=(unsigned int other) {                                                \
                value = (Flags) ((unsigned int) value | other);                                   \
                return *this;                                                                     \
            }                                                                                     \
            Flags operator&(Type other) const {                                                   \
                return (Flags) ((unsigned int) value & (unsigned int) other.value);               \
            }                                                                                     \
            Flags operator&(Flags other) const {                                                  \
                return (Flags) ((unsigned int) value & (unsigned int) other);                     \
            }                                                                                     \
            Flags operator&(unsigned int other) const {                                           \
                return (Flags) ((unsigned int) value & other);                                    \
            }                                                                                     \
            Type &operator&=(Type other) {                                                        \
                value = (Flags) ((unsigned int) value & (unsigned int) other.value);              \
                return *this;                                                                     \
            }                                                                                     \
            Type &operator&=(Flags other) {                                                       \
                value = (Flags) ((unsigned int) value & (unsigned int) other);                    \
                return *this;                                                                     \
            }                                                                                     \
            Type &operator&=(unsigned int other) {                                                \
                value = (Flags) ((unsigned int) value & other);                                   \
                return *this;                                                                     \
            }                                                                                     \
            Type &operator^=(Type other) {                                                        \
                value = (Flags) ((unsigned int) value ^ (unsigned int) other.value);              \
                return *this;                                                                     \
            }                                                                                     \
            Type &operator^=(Flags other) {                                                       \
                value = (Flags) ((unsigned int) value ^ (unsigned int) other);                    \
                return *this;                                                                     \
            }                                                                                     \
            Type &operator^=(unsigned int other) {                                                \
                value = (Flags) ((unsigned int) value ^ other);                                   \
                return *this;                                                                     \
            }                                                                                     \
            bool operator==(Type other) const {                                                   \
                return value == other.value;                                                      \
            }                                                                                     \
            bool operator==(Flags other) const {                                                  \
                return value == other;                                                            \
            }                                                                                     \
            bool operator==(unsigned int other) const {                                           \
                return (unsigned int) value == other;                                             \
            }                                                                                     \
            bool operator!=(Type other) const {                                                   \
                return value != other.value;                                                      \
            }                                                                                     \
            bool operator!=(Flags other) const {                                                  \
                return value != other;                                                            \
            }                                                                                     \
            bool operator!=(unsigned int other) const {                                           \
                return (unsigned int) value != other;                                             \
            }                                                                                     \
            static QScriptValue toScriptValue(QScriptEngine *engine, const Type &type) {          \
                Q_UNUSED(engine)                                                                  \
                return QScriptValue(type.toString());                                             \
            }                                                                                     \
            static void fromScriptValue(const QScriptValue &object, Type &type) {                 \
                type = object.toString();                                                         \
            }                                                                                     \
    };                                                                                            \
    PT_DECLARE_METATYPE(Type)                                                                     \
    inline QString convert##Type##ToUserString(const QVariant &variant) {                         \
        return variant.value<Type>().toString();                                                  \
    }                                                                                             \
    inline QVariant convertUserStringTo##Type(const QString &string) {                            \
        return QVariant::fromValue(Type::fromString(string));                                     \
    }                                                                                             \
    inline QString convert##Type##ToJsonString(const QVariant &variant) {                         \
        return ConversionUtil::jsString(variant.value<Type>().toString());                        \
    }                                                                                             \
    inline QVariant convertJsonVariantTo##Type(const QVariant &variant) {                         \
        return QVariant::fromValue(Type::fromString(variant.toString()));                         \
    }

#endif // METATYPEREGISTRY_H
