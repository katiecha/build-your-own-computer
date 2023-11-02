#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]){
    int numOfVals = atoi(argv[1]);
    int result[numOfVals];
    int indexOfResult = 0;
    int number = 0;

    while(true){
        if(indexOfResult >= numOfVals){
            break;
        }
        if(((number%5)==0 || (number%6==0)) && (number!=0)){
            result[indexOfResult] = number;
            indexOfResult++;
        }
        number++;
    }

    for (int i=0; i<sizeof(result)/sizeof(result[0]); i++){
        printf("%d\n", result[i]);
    }
    
    return EXIT_SUCCESS;
}