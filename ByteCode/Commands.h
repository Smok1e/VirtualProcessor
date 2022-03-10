#pragma once

//------------------------------

#define TOKENS_(args) std::initializer_list <TokenType> (args)

#define COMMANDS_DEFINES_										                       \
ACD_ ( push, TOKENS_ ({TokenType::numeric}), "push <value> - push value to stack"    ) \
ACD_ ( pop,  TOKENS_ ({                  }), "pop - pop value from stack"            ) \
ACD_ ( add,  TOKENS_ ({                  }), "add - sum 2 top values"                ) \
ACD_ ( sub,  TOKENS_ ({                  }), "sub - subtract 2 top values"           ) \
ACD_ ( mul,  TOKENS_ ({                  }), "mul - multiply 2 top values"           ) \
ACD_ ( div,  TOKENS_ ({                  }), "div - divide 2 top values"             ) \
ACD_ ( in,   TOKENS_ ({                  }), "in - push value from input"            ) \
ACD_ ( out,  TOKENS_ ({                  }), "out - print top value as float"        ) \
ACD_ ( hlt,  TOKENS_ ({                  }), "hlt - halt program"                    ) \
ACD_ ( ver,  TOKENS_ ({                  }), "ver - push assembler version to stack" ) \
ACD_ ( hlp,  TOKENS_ ({                  }), "hlp - print commands reference"        ) \
ACD_ ( man,  TOKENS_ ({TokenType::keyword}), "man <command> - print command manual"  ) \
ACD_ ( ret,  TOKENS_ ({                  }), "ret - set program retval from stack"   ) \

//------------------------------