#pragma once

//------------------------------

#define TOKENS_(args) std::initializer_list <TokenType> (args)

#define COMMANDS_DEFINES_ \ 
ACD_ ( push  , TOKENS_ ({ TokenType::numeric,            }), push - push value to stack                                                                          , { push (nextStackValue ());                                                                                                                                                                                }) \ 
ACD_ ( pop   , TOKENS_ ({ TokenType::,                   }), pop - pop value from stack                                                                          , { pop ();                                                                                                                                                                                                  }) \ 
ACD_ ( add   , TOKENS_ ({ TokenType::,                   }), add - sum 2 top values                                                                              , { double rgt = popNumber (); double lft = popNumber ();  push (lft + rgt);                                                                                                                                 }) \ 
ACD_ ( man   , TOKENS_ ({ TokenType::keyword, TokenType::, TokenType::test,  }), man <command> - print command manual                                                                , { output ("%s\n", CommandManual (nextInstruction ()));                                                                                                                                                     }) \ 

//------------------------------
