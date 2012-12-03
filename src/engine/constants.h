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
    NoOptions = 0x0000,
    Capitalized = 0x0001,
    DefiniteArticles = 0x0002,
    Copy = 0x0004 | 0x0008,
    DontSave = 0x0008,
    SkipId = 0x0010,
    IncludeTypeInfo = 0x0020,
    IfNotLast = 0x0040,
    DontRegister = 0x0080,
    DontServe = 0x0100,
    AutoDelete = 0x0200,
    NeverDelete = 0x0400
};

#endif // CONSTANTS_H
