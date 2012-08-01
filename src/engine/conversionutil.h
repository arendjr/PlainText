#ifndef CONVERSIONUTIL_H
#define CONVERSIONUTIL_H

#include <QMetaProperty>
#include <QString>
#include <QVariant>


class ConversionUtil {

    public:
        static QVariant fromVariant(QVariant::Type type, int userType, const QVariant &variant);

        static QString toJSON(const QVariant &variant);

        static QString toUserString(const QVariant &variant);

        static QString jsString(QString string);
};

#endif // CONVERSIONUTIL_H
