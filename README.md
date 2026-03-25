```       __                        
  _______/  |_  ___________  _____  
 /  ___/\   __\/  _ \_  __ \/     \ 
 \___ \  |  | (  <_> )  | \/  Y Y  \
/____  > |__|  \____/|__|  |__|_|  /
     \/                          \/ 
```


Storm is a procedural C-styled languaage, it's being developed solely for learning purposes but feel free to make a pull request if your interested in helping out!

### Features
---

- Procedures aka functions
- Procedure calls / recursion
- Various integrated types - int, double, string, bool, char, void (built in data-structures soon)
- Multiple files / folder support
- Narrowing / widening conversion
- Storm library coming soon

### Example
```

proc int fib(num: int) {
    if(num < 2) {
        return num;
    }

    return fib(n - 1) + fib(n - 2);
}

proc void main(void) {

    range(i = 0..50) {
        echo(i);
    }

    fib_result: int = fib(45);
    echo(fib_result);
}

```

### Details
- Compiler currently produces assembly, im using NASM and gcc to build the elf64 file.
