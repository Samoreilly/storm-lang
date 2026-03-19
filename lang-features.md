# Storm - procedural language


--- Storm Language Features ---

- Procedures (proc)
- Loops (for, while, range)
- Conditional statements
- Function calls
- Variable types - int, double, bool, string, user-defined types
- Pass by reference/copy
- Multiple files support
- Storm - containerizes multiple data objects



--- Procedures

```

proc int sum_two_numbers(int: var_one, int: var_two) {
    return var_one + var_two;
}

//procedure called and printed
proc void main() {
    echo(sum_two_numbers(1, 2));
}


```
--- For loops

```
1. Declare int i = 0 in loop and loop until i < 10
        
         init      comparison      update

    for(i: int; i < 10 && i != 8; i++) {
    
    }

2. Outside variable declaration

    i: int = 0;

    for(i < 10 && i != 8; i++) {

    }
```

--- Range loops

```
1. Inside variable declaration, i is an integer using Implicit Typing

    range(i = 0..10) {

    }    

2. Outside variable declaration
    
    i: int = 5;
   
    range(i..10) {

    }
```

--- While loops

- Allows for declaration and conditional statements.
- No updates for example: i++, i *= 2

```
1. Outside variable declaration

    i: int = 0;
    
    while(i < 10) {
        i++;
    }

}

```

--- If statement

```
1. Same as most other languages

    if(condition) {
        body      
    }
```

--- Print
```
    echo(...);
```



# Built in data-structures (wont be a priority for a while)
