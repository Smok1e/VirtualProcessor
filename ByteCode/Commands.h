#pragma once
#pragma warning (disable: 6031)

//------------------------------

#define TOKENS_(...) std::initializer_list <TokenType> (__VA_ARGS__)

//------------------------------

#define PUSH_IMPL__ push (*nextAddress (args));

//------------------------------

#define POP_IMPL__                                                           \
stack_value_t value = popValue ();                                           \
                                                                             \
if      (args.is_address                      ) *nextAddress (args) = value; \
else if (args.arg1_type == TokenType::Register) regSet (nextByte (), value);

//------------------------------

#define JUMP_IF__(condition__)                   \
stack_value_t addr = nextStackValue ();			 \
stack_value_t rgt = popValue ();                 \
stack_value_t lft = popValue ();	             \
if (condition__) { jump (addr); }

//------------------------------

#define RGB_EXTRACT__(channel__)                                                        \
ACD_ (ext##channel__, TOKENS_ ({}),                                                     \
"ext" #channel__ " - extract " #channel__ " channel value from rgb color",              \
{                                                                                       \
    double channel = txu::Color (static_cast <COLORREF> (popValue ())).##channel__;     \
    pushNumber (channel);                                                               \
})

//------------------------------

#define COMMANDS_DEFINES_                                                                                                                                                                         \
ACD_ (push, TOKENS_ ({TokenType::Numeric | TokenType::Register, TokenType::None | TokenType::Numeric | TokenType::Register}),                                                                     \
"push <number/register> - push number value to stack",                                                                                                                                            \
{                                                                                                                                                                                                 \
    PUSH_IMPL__                                                                                                                                                                                   \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (pop,  TOKENS_ ({TokenType::None | TokenType::Numeric | TokenType::Register, TokenType::None | TokenType::Numeric | TokenType::Register}),                                                   \
"pop <optional: register> - pop value from stack",                                                                                                                                                \
{                                                                                                                                                                                                 \
    POP_IMPL__                                                                                                                                                                                    \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (add, TOKENS_ ({}),                                                                                                                                                                          \
"add - sum 2 top stack values",                                                                                                                                                                   \
{                                                                                                                                                                                                 \
    double rgt = popNumber ();                                                                                                                                                                    \
    double lft = popNumber ();                                                                                                                                                                    \
    pushNumber (lft + rgt);                                                                                                                                                                       \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (sub, TOKENS_ ({}),                                                                                                                                                                          \
"sub - subtract 2 top stack values",                                                                                                                                                              \
{                                                                                                                                                                                                 \
    double rgt = popNumber ();                                                                                                                                                                    \
    double lft = popNumber ();                                                                                                                                                                    \
    pushNumber (lft - rgt);                                                                                                                                                                       \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (mul, TOKENS_ ({}),                                                                                                                                                                          \
"mul - multiply 2 top stack values",                                                                                                                                                              \
{                                                                                                                                                                                                 \
    double rgt = popNumber ();                                                                                                                                                                    \
    double lft = popNumber ();                                                                                                                                                                    \
    pushNumber (lft * rgt);                                                                                                                                                                       \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (div, TOKENS_ ({}),                                                                                                                                                                          \
"div - divide 2 top stack values",                                                                                                                                                                \
{                                                                                                                                                                                                 \
    double rgt = popNumber ();                                                                                                                                                                    \
    double lft = popNumber ();                                                                                                                                                                    \
    if (rgt == 0)                                                                                                                                                                                 \
    {                                                                                                                                                                                             \
        error ("Zero division");                                                                                                                                                                  \
        throw processor_error ("Zero division");                                                                                                                                                  \
    }                                                                                                                                                                                             \
    pushNumber (lft / rgt);                                                                                                                                                                       \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (rmd, TOKENS_ ({}),                                                                                                                                                                          \
"rmd - division remainder",                                                                                                                                                                       \
{                                                                                                                                                                                                 \
    int rgt = static_cast <int> (floor (popNumber ()));                                                                                                                                           \
    int lft = static_cast <int> (floor (popNumber ()));                                                                                                                                           \
    pushNumber (lft % rgt);                                                                                                                                                                       \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (in, TOKENS_ ({}),                                                                                                                                                                           \
"in - push number value from program input",                                                                                                                                                      \
{                                                                                                                                                                                                 \
    output ("Enter value: ");                                                                                                                                                                     \
    pushNumber (std::stod (input ()));                                                                                                                                                            \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (out, TOKENS_ ({}),                                                                                                                                                                          \
"out - pop and print number value",                                                                                                                                                               \
{                                                                                                                                                                                                 \
    output ("%lf\n", popNumber ());                                                                                                                                                               \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (hlt, TOKENS_ ({}),                                                                                                                                                                          \
"hlt - halt program",                                                                                                                                                                             \
{                                                                                                                                                                                                 \
                                                                                                                                                                                                  \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (ver, TOKENS_ ({}),                                                                                                                                                                          \
"ver - push assembler version to stack",                                                                                                                                                          \
{                                                                                                                                                                                                 \
    pushNumber (static_cast <double> (ASSEMBLER_VERSION));                                                                                                                                        \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (man, TOKENS_ ({TokenType::Keyword}),                                                                                                                                                        \
"man <command> - print command manual",                                                                                                                                                           \
{                                                                                                                                                                                                 \
    output ("%s\n", CommandManual (nextInstruction ()));                                                                                                                                          \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (hlp, TOKENS_ ({}),                                                                                                                                                                          \
"hlp - print commands reference",                                                                                                                                                                 \
{                                                                                                                                                                                                 \
    output ("Commands reference:\n");                                                                                                                                                             \
    for (size_t i = ByteCodesBegin; i < ByteCodesAmount; i++)                                                                                                                                     \
        output ("%s\n", CommandManual (static_cast <ByteCode> (i)));                                                                                                                              \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (dup, TOKENS_ ({}),                                                                                                                                                                          \
"dup - duplicate top stack value",                                                                                                                                                                \
{                                                                                                                                                                                                 \
    push (top ());                                                                                                                                                                                \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (dreg, TOKENS_ ({}),                                                                                                                                                                         \
"dreg - dump registers",                                                                                                                                                                          \
{                                                                                                                                                                                                 \
    output ("Registers dump:\n");                                                                                                                                                                 \
    for (size_t i = 0; i < REGISTERS_COUNT; i++)                                                                                                                                                  \
        output ("  %s: 0x%0*X (%lf)\n", StrRegisterIndex (i), sizeof (stack_value_t)*2, regGetStackValue (i), (i == NUMBERS_MODIFIER_REGISTER)? (double) regGetStackValue (i): regGetNumber (i)); \
                                                                                                                                                                                                  \
    output ("\n");                                                                                                                                                                                \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (dstk, TOKENS_ ({}),                                                                                                                                                                         \
"dstk - dump stack",                                                                                                                                                                              \
{                                                                                                                                                                                                 \
    output ("Stack dump:\n");                                                                                                                                                                     \
    for (size_t i = 0; i < m_stack.size (); i++)                                                                                                                                                  \
        output ("  0x%0*X [%lf]\n", sizeof (stack_value_t)*2, m_stack[i], static_cast <double> (m_stack[i]) / NUMBERS_MODIFIER);                                                                  \
                                                                                                                                                                                                  \
    output ("\n");                                                                                                                                                                                \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (sqr, TOKENS_ ({}),                                                                                                                                                                          \
"sqr - square top stack value",                                                                                                                                                                   \
{                                                                                                                                                                                                 \
    double number = popNumber ();                                                                                                                                                                 \
    pushNumber (number*number);                                                                                                                                                                   \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (sqrt, TOKENS_ ({}),                                                                                                                                                                         \
"sqrt - square root from top stack value",                                                                                                                                                        \
{                                                                                                                                                                                                 \
    double number = popNumber ();                                                                                                                                                                 \
    pushNumber (static_cast <double> (sqrt (number)));                                                                                                                                            \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (sin, TOKENS_ ({}),                                                                                                                                                                          \
"sin - sin function of top stack value as angle",                                                                                                                                                 \
{                                                                                                                                                                                                 \
    pushNumber (sin (popNumber ()));                                                                                                                                                              \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (cos, TOKENS_ ({}),                                                                                                                                                                          \
"cos - cos function of top stack value as angle",                                                                                                                                                 \
{                                                                                                                                                                                                 \
    pushNumber (cos (popNumber ()));                                                                                                                                                              \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (test, TOKENS_ ({TokenType::None | TokenType::Numeric}),                                                                                                                                     \
"test <optional: number> - print debug test marker",                                                                                                                                              \
{                                                                                                                                                                                                 \
    if (args.arg1_type == TokenType::Numeric)                                                                                                                                                     \
        output ("Test marker: %.0lf\n", static_cast <double> (nextStackValue ()) / NUMBERS_MODIFIER);                                                                                             \
                                                                                                                                                                                                  \
    else output ("Test marker #%d\n", ++m_test_marker);                                                                                                                                           \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (jmp, TOKENS_ ({TokenType::LabelRef}),                                                                                                                                                       \
"jmp <label> - jump to a label point",                                                                                                                                                            \
{                                                                                                                                                                                                 \
    jump (nextStackValue ());                                                                                                                                                                     \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (je, TOKENS_ ({TokenType::LabelRef}),                                                                                                                                                        \
"je <label> - jump to a label point if lft == rgt",                                                                                                                                               \
{                                                                                                                                                                                                 \
    JUMP_IF__ (lft == rgt)                                                                                                                                                                        \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (jne, TOKENS_ ({TokenType::LabelRef}),                                                                                                                                                       \
"jne <label> - jump to a label point if lft != rgt",                                                                                                                                              \
{                                                                                                                                                                                                 \
    JUMP_IF__ (lft != rgt)                                                                                                                                                                        \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (ja, TOKENS_ ({TokenType::LabelRef}),                                                                                                                                                        \
"jb <label> - jump to a label point if lft > rgt",                                                                                                                                                \
{                                                                                                                                                                                                 \
    JUMP_IF__ (lft > rgt)                                                                                                                                                                         \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (jb, TOKENS_ ({TokenType::LabelRef}),                                                                                                                                                        \
"ja <label> - jump to a label point if lft < rgt",                                                                                                                                                \
{                                                                                                                                                                                                 \
    JUMP_IF__ (lft <  rgt)                                                                                                                                                                        \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (jae, TOKENS_ ({TokenType::LabelRef}),                                                                                                                                                       \
"jae <label> - jump to a label point if lft >= rgt",                                                                                                                                              \
{                                                                                                                                                                                                 \
    JUMP_IF__ (lft >= rgt)                                                                                                                                                                        \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (jbe, TOKENS_ ({TokenType::LabelRef}),                                                                                                                                                       \
"jbe <label> - jump to a label point if lft <= rgt",                                                                                                                                              \
{                                                                                                                                                                                                 \
    JUMP_IF__ (lft <= rgt)                                                                                                                                                                        \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (jt, TOKENS_ ({TokenType::LabelRef}),                                                                                                                                                        \
"jt <label> - jump if current time cecond is even",                                                                                                                                               \
{                                                                                                                                                                                                 \
    stack_value_t addr = nextStackValue ();                                                                                                                                                       \
    if ((time (0) % 2) == 0)                                                                                                                                                                      \
        jump (addr);                                                                                                                                                                              \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (call, TOKENS_ ({TokenType::LabelRef}),                                                                                                                                                      \
"call <label> - call a function",                                                                                                                                                                 \
{                                                                                                                                                                                                 \
    stack_value_t addr = nextStackValue ();                                                                                                                                                       \
    pushCall (static_cast <stack_value_t> (m_next_data_index));                                                                                                                                   \
    jump (addr);                                                                                                                                                                                  \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (ret, TOKENS_ ({}),                                                                                                                                                                          \
"ret - return to previous location after call",                                                                                                                                                   \
{                                                                                                                                                                                                 \
    size_t addr = static_cast <size_t> (popCall ());                                                                                                                                              \
    jump (addr);                                                                                                                                                                                  \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (chout, TOKENS_ ({}),                                                                                                                                                                        \
"chout - print top stack value as character",                                                                                                                                                     \
{                                                                                                                                                                                                 \
    output ("%c", static_cast <char> (floor (popNumber ())));                                                                                                                                     \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (valsz, TOKENS_ ({}),                                                                                                                                                                        \
"valsz - push single stack value size in bytes to stack",                                                                                                                                         \
{                                                                                                                                                                                                 \
    pushNumber (sizeof (stack_value_t));                                                                                                                                                          \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (mov, TOKENS_ ({TokenType::Register, TokenType::Register}),                                                                                                                                  \
"cpy <register, register> - copy value from register 1 to register 2",                                                                                                                            \
{                                                                                                                                                                                                 \
    stack_value_t val = regGetStackValue (nextByte ());                                                                                                                                           \
    regSet (nextByte (), val);                                                                                                                                                                    \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (sleep, TOKENS_ (),                                                                                                                                                                          \
"sleep - freeze program for N msec",                                                                                                                                                              \
{                                                                                                                                                                                                 \
    double msec = popNumber ();                                                                                                                                                                   \
    std::this_thread::sleep_for (std::chrono::duration <double, std::milli> (msec));                                                                                                              \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (wnd, TOKENS_ ({}),                                                                                                                                                                          \
"wnd - open a display window with two top stack values as width and height",                                                                                                                      \
{                                                                                                                                                                                                 \
    double size_y = popNumber ();                                                                                                                                                                 \
    double size_x = popNumber ();                                                                                                                                                                 \
    openWindow (size_x, size_y);                                                                                                                                                                  \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (display, TOKENS_ ({TokenType::Numeric | TokenType::Register}),                                                                                                                              \
"display <numeric/register> - display memory data in opened window, starting from address given in arguments",                                                                                    \
{                                                                                                                                                                                                 \
    display (static_cast <double> (*nextAddress (args)) / NUMBERS_MODIFIER);                                                                                                                      \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (floor, TOKENS_ ({}),                                                                                                                                                                        \
"floor - floor",                                                                                                                                                                                  \
{                                                                                                                                                                                                 \
    pushNumber (floor (popNumber ()));                                                                                                                                                            \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (pause, TOKENS_ ({}),                                                                                                                                                                        \
"pause - wait for any key input",                                                                                                                                                                 \
{                                                                                                                                                                                                 \
    output ("[Press any key to continue]\n");                                                                                                                                                     \
    _getch ();                                                                                                                                                                                    \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (rgb, TOKENS_ ({}),                                                                                                                                                                          \
"rgb - mix 3 top stack values to rgb color",                                                                                                                                                      \
{                                                                                                                                                                                                 \
    double b = popNumber ();                                                                                                                                                                      \
    double g = popNumber ();                                                                                                                                                                      \
    double r = popNumber ();                                                                                                                                                                      \
    push (static_cast <stack_value_t> (RGB (r, g, b)));                                                                                                                                           \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
RGB_EXTRACT__ (r)                                                                                                                                                                                 \
RGB_EXTRACT__ (g)                                                                                                                                                                                 \
RGB_EXTRACT__ (b)                                                                                                                                                                                 \
                                                                                                                                                                                                  \
ACD_ (hsv, TOKENS_ ({}),                                                                                                                                                                          \
"hsv - mix 3 top stack values to hsv color",                                                                                                                                                      \
{                                                                                                                                                                                                 \
    double v = popNumber ();                                                                                                                                                                      \
    double s = popNumber ();                                                                                                                                                                      \
    double h = popNumber ();                                                                                                                                                                      \
    push (static_cast <stack_value_t> (txRGB2HSL (RGB (h, s, v))));                                                                                                                               \
})                                                                                                                                                                                                \
                                                                                                                                                                                                  \
ACD_ (brk, TOKENS_ ({}),                                                                                                                                                                          \
"brk - debug break",                                                                                                                                                                              \
{                                                                                                                                                                                                 \
    DebugBreak ();                                                                                                                                                                                \
})                                                                                                                                                                                                

//------------------------------