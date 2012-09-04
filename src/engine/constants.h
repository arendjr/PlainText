#ifndef CONSTANTS_H
#define CONSTANTS_H

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

enum Options {
    NoOptions = 0x00,
    Capitalized = 0x01,
    DefiniteArticles = 0x02,
    Copy = 0x04,
    DontSave = 0x08,
    IncludeId = 010,
    DontIncludeTypeInfo = 0x20
};

#endif // CONSTANTS_H
