#include <stdio.h>

int sum (int one, int two) {
    return one + two;
}
int fibonaci (int n) {
    if(n <= 1) {
        return n;
    }
    return fibonaci(n - 1) + fibonaci(n - 2);
}



void main () {
    printf("%i\n", fibonaci(10));
    int x = sum(1, 2);
    printf("%i\n", x);
}