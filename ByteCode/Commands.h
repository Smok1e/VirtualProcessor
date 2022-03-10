#pragma once

//------------------------------

#define TOKENS_(args) std::initializer_list <TokenType> (args)

#define COMMANDS_DEFINES_										                                                                                                                                                                                                    \
/*     name            arguments             description                                processor code                                                                                                                                                           */ \
ACD_ ( push, TOKENS_ ({TokenType::numeric}), "push <value> - push value to stack",    { push (nextStackValue ());                                                                                                                                                }) \
ACD_ ( pop,  TOKENS_ ({                  }), "pop - pop value from stack",            { pop ();                                                                                                                                                                  }) \
ACD_ ( add,  TOKENS_ ({                  }), "add - sum 2 top values",                { double rgt = popNumber (); double lft = popNumber (); push (lft + rgt);                                                                                                  }) \
ACD_ ( sub,  TOKENS_ ({                  }), "sub - subtract 2 top values",           { double rgt = popNumber (); double lft = popNumber (); push (lft - rgt);                                                                                                  }) \
ACD_ ( mul,  TOKENS_ ({                  }), "mul - multiply 2 top values",           { double rgt = popNumber (); double lft = popNumber (); push (lft * rgt);                                                                                                  }) \
ACD_ ( div,  TOKENS_ ({                  }), "div - divide 2 top values",             { double rgt = popNumber (); double lft = popNumber (); if (rgt == 0) { error ("Fatal error: Zero division"); throw processor_error ("Zero division"); } push (lft / rgt); }) \
ACD_ ( in,   TOKENS_ ({                  }), "in - push value from input",            { output ("Enter value: "); push (std::stod (input ()));                                                                                                                   }) \
ACD_ ( out,  TOKENS_ ({                  }), "out - print top value as float",        { output ("%lf\n", popNumber ());                                                                                                                                          }) \
ACD_ ( hlt,  TOKENS_ ({                  }), "hlt - halt program",                    {                                                                                                                                                                          }) \
ACD_ ( ver,  TOKENS_ ({                  }), "ver - push assembler version to stack", { push (static_cast <double> (ASSEMBLER_VERSION));                                                                                                                         }) \
ACD_ ( hlp,  TOKENS_ ({                  }), "hlp - print commands reference",        { output ("Commands reference:\n"); for (size_t i = ByteCodesBegin; i < ByteCodesAmount; i++) output ("%s\n", CommandManual (static_cast <ByteCode> (i)));                 }) \
ACD_ ( man,  TOKENS_ ({TokenType::keyword}), "man <command> - print command manual",  { output ("%s\n", CommandManual (nextInstruction ()));                                                                                                                     }) \
ACD_ ( ret,  TOKENS_ ({                  }), "ret - set program retval from stack",   { setReturnValue (popNumber ());                                                                                                                                           }) \

//------------------------------