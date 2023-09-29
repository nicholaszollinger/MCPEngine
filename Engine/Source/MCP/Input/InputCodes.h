#pragma once
// InputCodes.h

// This is going to house all of the possible inputs in enumerations.

enum class MCPKey : int
{
    // Letters
    kInvalid,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    // Special Keys
    Escape,
    Space,
    Enter,
};

enum class MCPMouseButton : int
{
    Invalid,
    Left,
    Middle,
    Right,
};