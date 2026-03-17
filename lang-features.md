# Storm


--- Storm Language Features ---

- Procedures
- Loops (for, while, range);
- Conditional statements
- Function calls
- Variable types - int, double, bool, string, user-defined types
- Multiple files support


--- Procedures

```

proc int sum_two_numbers(int: var_one, int: var_two) {
    return var_one + var_two;
}

proc void main() {
    echo(sum_two_numbers(1, 2));
 
}


```
--- For loops

```
1. Declare int i = 0 in loop and loop until i < 10
        
         init      comparison      update

    for(int i = 0; i < 10 && i != 8; i++) {
    
    }

2. Outside variable declaration

    int i = 0;

    for(i < 10 && i != 8; i++) {

    }

3. Inside variable declaration, i is an integer using Implicit Typing

    range(i = 0..10) {

    }    
```

--- While loops

- Allows for declaration and conditional statements.
- No updates for example: i++, i *= 2

```
   
1. Outside variable declaration

    int i = 0;
    
    while(i < 10) {
        i++;
    }

}


```


















