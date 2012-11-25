#ifndef CONVERSIONUTIL_H
#define CONVERSIONUTIL_H

#include <QMetaProperty>
#include <QString>
#include <QVariant>

#include "constants.h"


class ConversionUtil {

    public:
        static QVariant fromVariant(QVariant::Type type, int userType, const QVariant &variant);

        static QString toJsonString(const QVariant &variant, Options options = NoOptions);

        static QString toUserString(const QVariant &variant);

        static QString jsString(QString string);
};

#endif // CONVERSIONUTIL_H
