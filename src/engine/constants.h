#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QFlags>


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
    NoOptions = 0x0000,
    Capitalized = 0x0001,
    DefiniteArticles = 0x0002,
    Copy = 0x0004,
    DontSave = 0x0008,
    SkipId = 0x0010,
    IncludeTypeInfo = 0x0020,
    IfNotLast = 0x0040,
    DontRegister = 0x0080
};
Q_DECLARE_FLAGS(Options, Option)
Q_DECLARE_OPERATORS_FOR_FLAGS(Options)

#endif // CONSTANTS_H
