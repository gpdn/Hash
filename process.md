
HASH INTERPRETER
================

- [Lexer](#lexer)
- [Compiler](#compiler)
- [Bytecode Store](#bytecode-store)
    - [Constant Pool](#constant-pool)
- [Virtual Machine](#virtual-machine)
- [Additional Modules](#additional-modules)
    - [Debug](#debug)

# Lexer
# Compiler

# Bytecode Store
    
An array containing a list of bytecode instructions represented as `uint_8`.
Each bytecode occupies 1 byte if it is a simple instruction without any value.
Otherwise the index of the value for the instruction is stored right after the instruction itself.

This index can be used to retrieve the actual value from the __constant pool__.

## Constant Pool

Array used to store a list of values.

# Virtual Machine
# Additional Modules
## Debug
