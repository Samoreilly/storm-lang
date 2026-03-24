# Storm - execution (rough idea)

## Two pass
1. Loop through ast and build symbol table of initialized variables/functions
2. Loop through global statements and look for a tempest() function (the entrypoint) and call node->exec(),
   this will recursively trigger all variables, function calls etc 
