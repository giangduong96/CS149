// C program to demonstrate use of fork() and pipe()
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>


int main(void) {
 
    int pfds[2];
 
    double buf[2];
 
 
    pipe(pfds);
 
 
    if (!fork()) {
 
        printf(" CHILD: writing to pipe\n");
	buf[0] = 10.12;
	buf[1] = 23.45;
 
        write(pfds[1], buf, sizeof(buf));
 
        printf(" CHILD: exiting\n");
    } else {
 
        printf("PARENT: reading from pipe\n");
 
        read(pfds[0], buf, sizeof(buf));
 
	double sum = buf[0] + buf[1];

        printf("PARENT: read \"%f\"\n" , buf[0]);
        printf("PARENT: read \"%f\"\n" , buf[1]);
	printf("PARENT: sum \"%f\"\n" , sum);
 
        wait(NULL);
    }
 
    return 0;
}





// Homework HW2 extend avg from HW1. Use fork() pipe() and wait() for calculating
// this program use fork() to start a process for each of the files so that we can compute sums and counts in parallel.
// To communicate between processes you will use pipe(). You need to wait for all the processes to finish using wait().
// Giang Duong  Mar 4th, 2020
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
int main(int argc, const char * argv[]) {
    double total = 0;   // holds the sum of numbers of each file
    int count = 0; // counts how many numbers in the file
    double avgVal;  // holds the average of the numbers
    double buf[argc][2]; // 2-D array to hold the buf [0] is for total, [1] is for count
    int pfds[argc][2]; // 2-D array used to store 2 ends of the pipe for each child process to commute with parent
    // use for loop to make pipe for each child process
    for (int i = 0; i < argc; i++){
        pipe(pfds[i]);
        // if pipe has error then stop
        if(pipe(pfds[i]) < 0 ) {
            fprintf(stderr, "pipe error");}

    }
    // create process ID
    pid_t p;
    // for loop for each process with parent
    for (int i = 0; i < argc-1; i++){
        // use fork for each file
        p = fork();
        // fork fail; exit
        if (p < 0) {
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
            } else
                fclose(fptr);
            // prevent the file isnt have number
            if (countFromChild == 0) { printf("Divide by 0. Exit !!!\n");exit(1);}
            printf("Sum %s is: %lf. Count is: %d \n", argv[i+1] ,totalFromChild, countFromChild);
            buf[i][0]= totalFromChild;
            buf[i][1]= countFromChild;
            printf("Total to parent %lf. Count to parent: %lf\n", buf[i][0],buf[i][1]);
            //close reading from Parent
            close(pfds[i][0]);
            // write to the parent by using buffer
            write(pfds[i][1], buf, sizeof(buf));
            // close write to the parent
            close(pfds[i][1]);
            //exit child process
            exit(0);
            //int status;
            //pid_t wpid = waitpid(pid, &status, 0); // wait for child to finish before exiting
            //return wpid == pid && WIFEXITED(status) ? WEXITSTATUS(status) : -1;

        }
        else
            // child process
        {
            // parent process  //printf("PID : %lu", pid = getpid());
            //create the communication by pipe to receive the value from each child process by buffer
            // close write to children
            close(pfds[i][1]);
            //read from children
            read(pfds[i][0], buf, sizeof(buf));
            // close write from children
            close(pfds[i][0]);
            printf("Total from Child %lf. Count from Child: %lf\n", buf[i][0], buf[i][1]);
            total += buf[i][0];
            count += buf[i][1];
            // wait for child process
            for (int i = 0; i < argc; i++) {
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
// end



