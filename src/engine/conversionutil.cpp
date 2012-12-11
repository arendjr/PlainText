#include "conversionutil.h"

#include <QDateTime>
#include <QDebug>
#include <QStringList>

#include "metatyperegistry.h"


QVariant ConversionUtil::fromVariant(QVariant::Type type, int userType, const QVariant &variant) {

    switch (type) {
        case QVariant::Bool:
        case QVariant::Int:
        case QVariant::Double:
        case QVariant::String:
            return variant;
        case QVariant::StringList:
            return variant.toStringList();
        case QVariant::DateTime:
            return QDateTime::fromMSecsSinceEpoch(variant.toLongLong());
        case QVariant::Map: {
            QVariantMap variantMap;
            QVariantMap map = variant.toMap();
            for (const QString &key : map.keys()) {
                QVariantList valueList = map[key].toList();
                if (valueList.length() == 3) {
                    QVariant::Type valueType = (QVariant::Type) valueList[0].toInt();
                    int valueUserType = valueList[1].toInt();
                    QVariant value = valueList[2];
                    variantMap[key] = fromVariant(valueType, valueUserType, value);
                } else {
                    qWarning() << "Invalid map format in key" << key;
                }
            }
            return variantMap;
        }
        case QVariant::UserType: {
            MetaTypeRegistry::JsonConverters converters =
                    MetaTypeRegistry::jsonConverters(QMetaType::typeName(userType));
            if (converters.jsonVariantToTypeConverter) {
                return converters.jsonVariantToTypeConverter(variant);
            } else {
                const char *typeName = QMetaType::typeName(userType);
                if (typeName) {
                    qDebug() << "User type not serializable: " << typeName;
                } else {
                    qDebug() << "Unknown user type: " << userType;
                }
                return QVariant();
            }
        }
        default:
            qDebug() << "Unknown type: " << type;
            return QVariant();
    }
}

QString ConversionUtil::toJsonString(const QVariant &variant, Options options) {

    switch (variant.type()) {
        case QVariant::Bool:
            return variant.toBool() ? "true" : "false";
        case QVariant::Int:
            return QString::number(variant.toInt());
        case QVariant::UInt:
            return QString::number(variant.toUInt());
        case QVariant::Double:
            return QString::number(variant.toDouble());
        case QVariant::String:
            return variant.toString().isEmpty() ? QString() : jsString(variant.toString());
        case QVariant::List: {
            QStringList stringList;
            for (const QVariant &item : variant.toList()) {
                stringList.append(ConversionUtil::toJsonString(item));
            }
            if (stringList.isEmpty()) {
                return QString();
            } else {
                return "[ " + stringList.join(", ") + " ]";
            }
        }
        case QVariant::StringList: {
            QStringList stringList;
            for (const QString &string : variant.toStringList()) {
                stringList.append(jsString(string));
            }
            if (stringList.isEmpty()) {
                return QString();
            } else {
                return "[ " + stringList.join(", ") + " ]";
            }
        }
        case QVariant::DateTime:
            return QString::number(variant.toDateTime().toMSecsSinceEpoch());
        case QVariant::Map: {
            QStringList stringList;
            QVariantMap map = variant.toMap();
            for (const QString &key : map.keys()) {
                QVariant value = map[key];
                QString jsonString = toJsonString(value, options);
                if (!jsonString.isEmpty()) {
                    if (options & IncludeTypeInfo) {
                        stringList.append(QString("%1: [ %2, %3, %4 ]")
                                          .arg(jsString(key), QString::number(value.type()),
                                               QString::number(value.userType()), jsonString));
                    } else {
                        stringList.append(QString("%1: %2").arg(jsString(key), jsonString));
                    }
                }
            }
            return stringList.isEmpty() ? QString() : "{ " + stringList.join(", ") + " }";
        }
        case QVariant::UserType: {
            MetaTypeRegistry::JsonConverters converters =
                    MetaTypeRegistry::jsonConverters(QMetaType::typeName(variant.userType()));
            if (converters.typeToJsonStringConverter) {
                return converters.typeToJsonStringConverter(variant);
            } else {
                const char *typeName = QMetaType::typeName(variant.userType());
                if (typeName) {
                    qDebug() << "User type not serializable: " << typeName;
                } else {
                    qDebug() << "Unknown user type: " << variant.userType();
                }
                return QString();
            }
        }
        default:
            qDebug() << "Unknown type: " << variant.type();
            return QString();
    }
}

QString ConversionUtil::toUserString(const QVariant &variant) {

    if (!variant.isValid()) {
        return "(not set)";
    }

    switch (variant.type()) {
        case QVariant::Bool:
            return variant.toBool() ? "true" : "false";
        case QVariant::Int:
            return QString::number(variant.toInt());
        case QVariant::Double:
            return QString::number(variant.toDouble());
        case QVariant::String: {
            QString string = variant.toString();
            return string.isEmpty() ? "(empty string)" : string;
        }
        case QVariant::StringList:
            return variant.toStringList().join("\n");
        case QVariant::DateTime:
            return variant.toDateTime().toString();
        case QVariant::Map: {
            QStringList stringList;
            QVariantMap map = variant.toMap();
            for (const QString &key : map.keys()) {
                stringList << QString("%1: %2").arg(jsString(key), toUserString(map[key]));
            }
            return stringList.isEmpty() ? "(empty)" : stringList.join("\n");
        }
        case QVariant::UserType: {
            MetaTypeRegistry::UserStringConverters converters =
                    MetaTypeRegistry::userStringConverters(QMetaType::typeName(variant.userType()));
            if (converters.typeToUserStringConverter) {
                return converters.typeToUserStringConverter(variant);
            }
        }   // fall-through
        default:
            return "(unknown type)";
    }
}

QString ConversionUtil::jsString(QString string) {

    return "\"" + string.replace('\\', "\\\\")
                        .replace('"', "\\\"")
                        .replace('\n', "\\n") + "\"";
}
