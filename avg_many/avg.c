// Created by Giang Duong on 2/17/20.
// CS 149. HW1. 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// library for print string error
int main(int argc, const char * argv[]) {
   //int MAX_LENGTH = 50; //set the max line the program can take
    double var; // variable for holds the value
    double total = 0;   // holds the sum of numbers
    double avgVal;  // holds the average
    int count = 0; // counts how many numbers in the file

    FILE *fptr;             // pointer to the file need to be opened
    // fptr = fopen(argv[1], "r"); // pass name of the text file by argument
    if (argc != 2) {
        printf(" Wrong input. Used: %s filename\n", argv[0]);
        exit(1);
    } else {
        fptr = fopen(argv[1], "r"); // pass the array file name into the function
            //if the file pointer points to wrong or nothing exit with error code 1
            if (fptr == NULL) {
                printf("Error: File pointer is Null\n");
                return 1;
            }
            // otherwise say
            else printf("Open is success! \n");
            printf("Number from files:");
            // scan each line in the file into the function and compute the average
            while (fscanf(fptr, "%lf", &var) != EOF) {
                printf("%lf ", var);
                total += var;
                count++;
            }
            avgVal = total / count;

            printf("\nAverage of numbers from file is : %lf\n", avgVal);
        }
    //  if the file cannot be close, exit with the code 1
        if (fclose(fptr) != 0) {
            printf("Closing file error!\n");
            exit(1);
        } else
            fclose(fptr);
        return 0;
    }
