#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QFlags>


enum Option {
    NoOptions = 0x0000,
    SkipId = 0x0010,
    IncludeTypeInfo = 0x0020
};
Q_DECLARE_FLAGS(Options, Option)
Q_DECLARE_OPERATORS_FOR_FLAGS(Options)

#endif // CONSTANTS_H
