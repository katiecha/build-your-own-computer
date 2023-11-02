#include <stdio.h>
#include <stdlib.h>

int f(int number){
    int solution;
    if (number == 0){ // base case
        return 2;
    } else {
        solution = 2*number + f(number-1) - 1; // recursion
        return solution;
    }
}

int main(int argc, char *argv[]){
    int num = atoi(argv[1]);
    int result = f(num);

    printf("%d\n", result);
    
    return EXIT_SUCCESS;
}