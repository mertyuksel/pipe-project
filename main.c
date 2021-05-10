
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  
#include <unistd.h>    
#include <ctype.h>     
#include <sys/wait.h> 

#define RD 0
#define WR 1
#define LINELEN 40
         
int main()
{	
    char readBuffer[LINELEN+1] ;         
    char read2Buffer[LINELEN+1] ;         

    int flag = 0, countLine = 0 ; 
    char temp[LINELEN];  

    int pipe1[2];
    int pipe2[2];
    pid_t pid1;
    pid_t pid2;
    int returnval, returnval2;
    
    if (pipe(pipe1) == -1){fprintf (stderr, "Pipe 1 failed.\n");return EXIT_FAILURE;}
    if (pipe(pipe2) == -1){fprintf (stderr, "Pipe 2 failed.\n");return EXIT_FAILURE;}

    // Create child 1. 
    pid1 = fork();
    if (pid1 == -1)
    {
        fprintf (stderr, "Child 2 fork failed.\n");
        return EXIT_FAILURE; 
    }
    
    // Child 1 begins. 
    if (pid1 == (pid_t) 0)
    {
        int i, j;
        close(pipe1[RD]);

        printf("\nEnter a string (EOF to exit.(ctrl+d)): "); 
        while((fgets(temp, sizeof(temp), stdin)) != NULL) 
        { 
            flag = 0; 
            for ( i = 0; temp[i] != '\0'; i++)
            {
                if (!((temp[i] >= 49) && (temp[i] <= 57)) && temp[i] != '\n')
                {
                    printf("Entered string contains alphabetic letter.\n\n");
                    flag = 1; 
                    break;
                }
            }            
           
            // if string contains alphabetic letter, break the loop. 
            if (flag == 1)
            {       
                printf("Enter a string (EOF to exit.(ctrl+d)): "); 
                continue;
            }
            else
            {
                write(pipe1[WR], temp, LINELEN);
            }
            printf("\nEnter a string (EOF to exit.(ctrl+d)): ");       
        }

        close(pipe1[WR]);
        return EXIT_SUCCESS; // Child 1 finished.
    }
    else
    {
        // Create the child 2.
        pid2 = fork();
        if (pid2 == -1)
        {
            fprintf(stderr, "Child 2 fork failed.\n");
            return EXIT_FAILURE; 
        }

        // Child 2 begins 
        if (pid2 == (pid_t) 0)
        {
            close(pipe1[WR]);
            close(pipe2[RD]);

            while (1)
            {
                returnval2 = read(pipe1[RD], readBuffer, sizeof(readBuffer));

                if (returnval2 == -1)
                {
                    printf("\nRead failed.\n"); 
                    break;
                }
                
                // when eof is encountered, stop reading.
                if (returnval2 == 0)
                    break; 
                
                // if string length is lower than four, then write it to the pipe2.
                if (strlen(readBuffer) - 1 < 4)
                {
                    write(pipe2[WR], readBuffer, sizeof(readBuffer));
                }    
            }
        
            close(pipe1[RD]);
            close(pipe2[WR]);
            return EXIT_SUCCESS;  // Child 2 finished 
        }
        else
        {
            // Parent begins 
            close(pipe2[WR]);
            close(pipe1[RD]);
            close(pipe1[WR]);

            // Read from pipe2 and count lines. 
            while (1)
            {
                returnval = read(pipe2[RD], read2Buffer, sizeof(read2Buffer));

                // if read() fails, then stop reading 
                if (returnval == -1)
                {
                    printf("\nRead failed.\n"); 
                    break;
                }
                else if (returnval == 0)  // when eof is encountered, stop reading.
                {
                    break;
                }
                countLine++;  // count the lines.
            }

            printf("\n\nLine Count: %d\n", countLine);
            close(pipe2[RD]);
            
            // wait for the children.
            wait(NULL);
	        return 0;
        }     
    }
}

