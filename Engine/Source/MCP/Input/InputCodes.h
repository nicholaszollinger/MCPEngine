#pragma once
// InputCodes.h

// This is going to house all of the possible inputs in enumerations.

enum class MCPKey : int
{
    // Letters
    kNull = '\0',
    A = 'a',
    B = 'b',
    C = 'c',
    D = 'd',
    E = 'e',
    F = 'f',
    G = 'g',
    H = 'h',
    I = 'i',
    J = 'j',
    K = 'k',
    L = 'l',
    M = 'm',
    N = 'n',
    O = 'o',
    P = 'p',
    Q = 'q',
    R = 'r',
    S = 's',
    T = 't',
    U = 'u',
    V = 'v',
    W = 'w',
    X = 'x',
    Y = 'y',
    Z = 'z',

    // Numbers
    Num0 = '0',
    Num1 = '1',
    Num2 = '2',
    Num3 = '3',
    Num4 = '4',
    Num5 = '5',
    Num6 = '6',
    Num7 = '7',
    Num8 = '8',
    Num9 = '9',

    // Symbols
    Exclamation = '!',
    At = '@',
    Pound = '#',
    Dollar = '$',
    Percent = '%',
    CaretUp = '^',
    And = '&',
    Star = '*',
    LeftParen = '(',
    RightParen = ')',

    // Special Keys
    Escape = 27,
    Space = 32,
    Enter = 13,
    Backspace = 8,
};

namespace mcp
{
    [[nodiscard]] bool IsAlpha(const MCPKey key);
    [[nodiscard]] bool IsNumeric(const MCPKey key);
    [[nodiscard]] bool IsAlphaNumeric(const MCPKey key);
}

enum class MCPMouseButton : int
{
    Invalid,
    Left,
    Middle,
    Right,
};