#ifndef METATYPEREGISTRY_H
#define METATYPEREGISTRY_H

#include <QMap>
#include <QString>
#include <QVariant>


class QScriptEngine;

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
        return QVariant::fromValue(Type::fromUserString(string));                                 \
    }                                                                                             \
    inline QString convert##Type##ToJsonString(const QVariant &variant) {                         \
        return Type::toJsonString(variant.value<Type>());                                         \
    }                                                                                             \
    inline QVariant convertJsonVariantTo##Type(const QVariant &variant) {                         \
        return QVariant::fromValue(Type::fromVariant(variant));                                   \
    }


#endif // METATYPEREGISTRY_H
