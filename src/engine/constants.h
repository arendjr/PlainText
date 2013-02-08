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
    NoOptions = 0,
    Capitalized = (1 << 0),
    DefiniteArticles = (1 << 1),
    Copy = (1 << 2) | (1 << 3),
    DontSave = (1 << 3),
    SkipId = (1 << 4),
    IncludeTypeInfo = (1 << 5),
    IfNotLast = (1 << 6),
    DontRegister = (1 << 7),
    DontServe = (1 << 8),
    AutoDelete = (1 << 9),
    EndOfLife = (1 << 9), // same as AutoDelete
    NeverDelete = (1 << 10),
    Highlighted = (1 << 11),
    AutomaticNameForms = (1 << 12)
};

#endif // CONSTANTS_H
