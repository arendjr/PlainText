#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <qglobal.h>


enum Color {
    White = 0,
    Silver,
    Gray,
    Black,
    Red,
    Maroon,
    Yellow,
    Olive,
    Lime,
    Green,
    Aqua,
    Teal,
    Blue,
    Navy,
    Fuchsia,
    Purple
};

enum Option {
    NoOptions = 0x00,
    Capitalized = 0x01,
    DefiniteArticles = 0x02,
    Copy = 0x04,
    DontSave = 0x08,
    SkipId = 010,
    IncludeTypeInfo = 0x20
};
Q_DECLARE_FLAGS(Options, Option)
Q_DECLARE_OPERATORS_FOR_FLAGS(Options)

#endif // CONSTANTS_H
