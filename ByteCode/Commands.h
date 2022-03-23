#pragma once

//------------------------------

/*

#define TEST_																										\
TokenType arg1 = nextArgType (), arg2 = nextArgType ();																\
                                                                                                                    \
stack_value_t lft = 0; 																								\
if      (arg1 == TokenType::Numeric ) { lft = nextStackValue ();	    	   }									\
else if (arg1 == TokenType::Register) { lft = regGetStackValue (nextByte ());  }									\
                                                                                                                    \
stack_value_t rgt = 0;																								\
if      (arg2 == TokenType::Numeric ) { rgt = nextStackValue ();	    	   }    								\
else if (arg2 == TokenType::Register) { rgt = regGetStackValue (nextByte ());  }                                    \
                                                                                                                    \
push (static_cast <stack_value_t> (lft + rgt));                                                                     \

*/

//------------------------------

#define PUSH_IMPL__                                                                   \
byte_type type = nextByteType ();                                                     \
                                                                                      \
stack_value_t lft = 0;                                                                \
if      (type.arg1_type == TokenType::Numeric ) lft = nextStackValue ();              \
else if (type.arg1_type == TokenType::Register) lft = regGetStackValue (nextByte ()); \
                                                                                      \
stack_value_t rgt = 0;                                                                \
if      (type.arg2_type == TokenType::Numeric ) rgt = nextStackValue ();              \
else if (type.arg2_type == TokenType::Register) rgt = regGetStackValue (nextByte ()); \
                                                                                      \
push (static_cast <stack_value_t> (lft + rgt));

//------------------------------

#define POP_IMPL__                                 \
byte_type     type  = nextByteType ();             \
stack_value_t value = popValue     ();             \
                                                   \
if (type.arg1_type == TokenType::Register)         \
    regSet (nextByte (), value);

//------------------------------

#define TOKENS_(...) std::initializer_list <TokenType> (__VA_ARGS__)

#define COMMANDS_DEFINES_ \
ACD_ (push, TOKENS_ ({TokenType::Numeric | TokenType::Register, TokenType::None | TokenType::Numeric | TokenType::Register}), "push <number/register> - push number value to stack", { PUSH_IMPL__ }) \
ACD_ (pop,  TOKENS_ ({TokenType::None    | TokenType::Register                                                            }), "pop <optional: register> - pop value from stack",     { POP_IMPL__  }) \
ACD_ (add,  TOKENS_ ({                                                                                                    }), "add - sum 2 top stack values",                        { double rgt = popNumber ();  double lft = popNumber ();   push (lft + rgt);                                                                                                                                         }) \
ACD_ (sub,  TOKENS_ ({                                                                                                    }), "sub - subtract 2 top stack values",                   { double rgt = popNumber ();  double lft = popNumber ();   push (lft - rgt);                                                                                                                                         }) \
ACD_ (mul,  TOKENS_ ({                                                                                                    }), "mul - multiply 2 top stack values",                   { double rgt = popNumber ();  double lft = popNumber ();   push (lft * rgt);                                                                                                                                         }) \
ACD_ (div,  TOKENS_ ({                                                                                                    }), "div - divide 2 top stack values",                     { double rgt = popNumber ();  double lft = popNumber ();   if (rgt == 0) { error ("Zero division"); throw processor_error ("Zero division"); }  push (lft / rgt);                                                    }) \
ACD_ (in,   TOKENS_ ({                                                                                                    }), "in - push number value from program input",           { output ("Enter value: "); push (std::stod (input ()));                                                                                                                                                             }) \
ACD_ (out,  TOKENS_ ({                                                                                                    }), "out - pop and print number value",                    { output ("%lf\n", popNumber ());                                                                                                                                                                                    }) \
ACD_ (hlt,  TOKENS_ ({                                                                                                    }), "hlt - halt program",                                  {                                                                                                                                                                                                                    }) \
ACD_ (ver,  TOKENS_ ({                                                                                                    }), "ver - push assembler version to stack",               { push (static_cast <double> (ASSEMBLER_VERSION));                                                                                                                                                                   }) \
ACD_ (man,  TOKENS_ ({TokenType::Keyword                                                                                  }), "man <command> - print command manual",                { output ("%s\n", CommandManual (nextInstruction ()));                                                                                                                                                               }) \
ACD_ (hlp,  TOKENS_ ({                                                                                                    }), "hlp - print commands reference",                      { output ("Commands reference:\n"); for (size_t i = ByteCodesBegin; i < ByteCodesAmount; i++) output ("%s\n", CommandManual (static_cast <ByteCode> (i)));                                                           }) \
ACD_ (ret,  TOKENS_ ({                                                                                                    }), "ret - set top stack value as program return value",   { setReturnValue (popNumber ());                                                                                                                                                                                     }) \
ACD_ (dup,  TOKENS_ ({                                                                                                    }), "dup - duplicate top stack value",                     { push (top ());                                                                                                                                                                                                     }) \
ACD_ (dreg, TOKENS_ ({                                                                                                    }), "dreg - dump registers",                               { output ("Registers dump:\n"); for (size_t i = 0; i < REGISTERS_COUNT; i++) output ("  %s: 0x%0*X (%lf)\n", StrRegisterIndex (i), sizeof (stack_value_t)*2, regGetStackValue (i), regGetNumber (i)); output ("\n"); }) \
ACD_ (dstk, TOKENS_ ({                                                                                                    }), "dstk - dump stack",                                   { output ("Stack dump:\n"); for (size_t i = 0; i < m_stack.size (); i++) output ("  0x%0*X [%lf]\n", sizeof (stack_value_t)*2, m_stack[i], static_cast <double> (m_stack[i]) / NUMBERS_MODIFIER);     output ("\n"); }) \
ACD_ (sqr,  TOKENS_ ({                                                                                                    }), "sqr - square top stack value",                        { double number = popNumber (); push (number*number);                                                                                                                                                                }) \
ACD_ (sqrt, TOKENS_ ({                                                                                                    }), "sqrt - square root from top stack value",             { double number = popNumber (); push (static_cast <double> (sqrt (number)));                                                                                                                                         }) \
ACD_ (sin,  TOKENS_ ({                                                                                                    }), "sin - sin function of top stack value as angle",      { push (sin (popNumber ()));                                                                                                                                                                                         }) \
ACD_ (cos,  TOKENS_ ({                                                                                                    }), "cos - cos function of top stack value as angle",      { push (cos (popNumber ()));                                                                                                                                                                                         }) 

//------------------------------
