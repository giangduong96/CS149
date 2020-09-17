#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
int main(int argc, const char * argv[]) {
    double total = 0;   // holds the sum of numbers
    double avgVal;  // holds the average
    int count = 0; // counts how many numbers in the file
    int pfds[argc][2]; // used to store 2 ends of the pipe
    double buf[argc][2]; //holds the 2D array

    pid_t p;

    for (int i = 0; i < argc-1; i++){
        pipe(pfds[i]);
        p = fork();

        //if(pipe(pfds) < 0 ) {
        //  fprintf(stderr, "pipe error");}
        if (p < 0) { // fork fail; exit
            fprintf(stderr, "fork failed\n)");
            exit(1);
        } else if (p == 0) {
            //check the child process is created properly
            printf("\nChild PID %d. From Parent ID:%d\n", getpid(), getppid());
            // hold value from each child process
            int countFromChild = 0; //
            double totalFromChild, varFromChild;
            // pointer to the file need to be opened
            FILE *fptr;
            // pass the array file name into the function
            fptr = fopen(argv[i+1], "r");
            //if the file pointer points to wrong or nothing exit with error code 1
            if (fptr == NULL) {
                printf("Error: File pointer is Null from file %d\n", i);
                return 1;
            }
                // otherwise say
            else printf("Open is success files %s! \n", argv[i+1]);
            // scan each line in the file into the function and compute the average
            while (!feof(fptr)) {
                if (fscanf(fptr, "%lf", &varFromChild) == 1) {
                    totalFromChild += varFromChild; //printf("ABC%lf \n", totalFromChild);
                    countFromChild++;
                }
            }
            //printf("Total from Child %lf ", totalFromChild);
            //  if the file cannot be close, exit with the code 1
            if (fclose(fptr) != 0) {
                printf("Closing file error!\n");
                exit(1);
            } //else
                //fclose(fptr);
            // prevent the file isnt have number
            if (countFromChild == 0) { printf("Divide by 0. Exit !!!\n");exit(1);}
            printf("Sum %s is: %lf. Count is: %d \n", argv[i+1] ,totalFromChild, countFromChild);
            buf[i][0]= totalFromChild;
            buf[i][1]= countFromChild;
            printf("Total to parent %lf. Count to parent: %lf\n", buf[i][0],buf[i][1]);
            //close reading from Parent
            close(pfds[i][0]);
            // write to the parent by using buffer
            write(pfds[i][1], buf[i], sizeof(buf));
            // close write to the parent
            close(pfds[i][1]);
            //exit child process
            exit(0);
        }
    }
    if (p>0){
        for (int i = 0; i < argc-1; i++) {
            close(pfds[i][1]); // close write to children
            read(pfds[i][0], buf[i], sizeof(buf));
            close(pfds[i][0]); // close write from children
            printf("Total from Child %lf. Count from Child: %lf\n", buf[i][0], buf[i][1]);
            total += buf[i][0];
            count += buf[i][1];
            for (int i = 0; i < argc - 1; i++) {
                wait(NULL);
            }
        }
    }
    //calculate the avg value
    avgVal = total / (count);
    printf("\nAverage of numbers from file is : %lf \n", avgVal);
    printf("Total of numbers from file is : %lf\n",  total);
    printf("Count numbers from files is :  %d\n", count);

    return 0;
}

