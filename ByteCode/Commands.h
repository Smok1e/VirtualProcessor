#pragma once

//------------------------------

#define TOKENS_(...) std::initializer_list <TokenType> (__VA_ARGS__)

#define COMMANDS_DEFINES_ \
ACD_ (push, TOKENS_ ({TokenType::Numeric | TokenType::Register}), "push <number> - push number value to stack",        { push (nextStackValue ());                                                                                                                                       }) \
ACD_ (pop,  TOKENS_ ({                                        }), "pop - pop value from stack",                        { pop ();                                                                                                                                                         }) \
ACD_ (add,  TOKENS_ ({                                        }), "add - sum 2 top stack values",                      { double rgt = popNumber ();  double lft = popNumber ();   push (lft + rgt);                                                                                      }) \
ACD_ (sub,  TOKENS_ ({                                        }), "sub - subtract 2 top stack values",                 { double rgt = popNumber ();  double lft = popNumber ();   push (lft - rgt);                                                                                      }) \
ACD_ (mul,  TOKENS_ ({                                        }), "mul - multiply 2 top stack values",                 { double rgt = popNumber ();  double lft = popNumber ();   push (lft * rgt);                                                                                      }) \
ACD_ (div,  TOKENS_ ({                                        }), "div - divide 2 top stack values",                   { double rgt = popNumber ();  double lft = popNumber ();   if (rgt == 0) { error ("Zero division"); throw processor_error ("Zero division"); }  push (lft / rgt); }) \
ACD_ (in,   TOKENS_ ({                                        }), "in - push number value from program input",         { output ("Enter value: "); push (std::stod (input ()));                                                                                                          }) \
ACD_ (out,  TOKENS_ ({                                        }), "out - pop and print number value",                  { output ("%lf\n", popNumber ());                                                                                                                                 }) \
ACD_ (hlt,  TOKENS_ ({                                        }), "hlt - halt program",                                {                                                                                                                                                                 }) \
ACD_ (ver,  TOKENS_ ({                                        }), "ver - push assembler version to stack",             { push (static_cast <double> (ASSEMBLER_VERSION));                                                                                                                }) \
ACD_ (man,  TOKENS_ ({TokenType::Keyword                      }), "man <command> - print command manual",              { output ("%s\n", CommandManual (nextInstruction ()));                                                                                                            }) \
ACD_ (hlp,  TOKENS_ ({                                        }), "hlp - print commands reference",                    { output ("Commands reference:\n"); for (size_t i = ByteCodesBegin; i < ByteCodesAmount; i++) output ("%s\n", CommandManual (static_cast <ByteCode> (i)));        }) \
ACD_ (ret,  TOKENS_ ({                                        }), "ret - set top stack value as program return value", { setReturnValue (popNumber ());                                                                                                                                  }) \

//------------------------------
