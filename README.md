# STORM
Status is beta.

## Execution Pipeline

Source Code → Token Extraction → LParsing -> Semantic Analysis → x86_64 Assembly Emission → ELF64 Object Generation → Linked Linux Binary

### Linux x86_64 ELF64 Support

Architecture highlights:
Memory for local variables is managed via the stack frame using base pointer offsets.
The first six procedure arguments are stored in registers RDI, RSI, RDX, RCX, R8, and R9 for performance.
Return results are passed back through the RAX register.
Linking with the C library provides access to input plus output functions like printf.
Assembly output is formatted for the NASM assembler to produce ELF64 compatible object files.

## Syntax Preview

```storm
proc int add_numbers(a: int, b: int) {
    return a + b;
}

proc void main() {
    result: int = add_numbers(50, 50);
    echo(result);
}
```

```storm
storm User {
    id: int;
    active: int;
};
```
