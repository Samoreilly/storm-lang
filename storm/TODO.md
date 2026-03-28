# TODO list
1. Implement multi file support
2. Add a debug mode for prints to keep output clean
3. Implement common data-structures such as array, stack, queue 


## Semantic Analysis
1. Check for unique parameter names in proc | DONE
2. Check if called proc returns the same type as variable being initialized to | DONE
3. Check if the number of arguments in a proc call matches number of parameters
4. Copy propagation, remove unnecessary variable assignments | SEMI DONE
   - Implemented for proc calls
   - 
5. Remove unused variables | SEMI DONE
6. Tail call optimization

## Lexical Analysis
1. Remove regex find as its slow | DONE



# Parsing
1. Constant folding, arithemtic handled during parsing | DONE



# Later on
1. Move to a register/stack operations



## Multiple files

## Take in arguments for mulitple files or a folder
1. If folder - loop through folder for files with .storm extension and parse | DONE
2. If files - only parse said files | DONE

After: Merge global vectors from each file into one in MainNode | DONE

