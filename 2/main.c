#include <sys/wait.h> // for waitpid()
#include <unistd.h> // for fork() & getpid()
#include <stdlib.h> // for exit()
#include <stdio.h> // for printf()
#define READ 0
#define WRITE 1

int main() { 
    // create file descriptors
    // fd1 - parent to child
    // fd2 - child to parent
    // READ = 0
    // WRITE = 1
    int fd1[2], fd2[2];

    // create pipes
    int stat1 = pipe(fd1);
    int stat2 = pipe(fd2);
    if(stat1 == -1 || stat2 == -1) {
        perror("pipe");
    } 

    // create a child process
    pid_t pid = fork();

    // determine if the process is the parent or child
    if(pid == -1) {
        perror("fork");
        exit(1);

    } else if(pid == 0) {
        
        // this is the child process
        printf("Child PID: %d\n", getpid());

        // reading from the pipe
        int num;
        read(fd1[READ], &num, sizeof(num));

        printf("\nI am the child process\nMy PID is : %d\n", getpid());

        // closing unused ends
        close(fd1[WRITE]);
        close(fd2[READ]);

        printf("I recieved %d from the parent\n", num);

        // taking user input
        printf("Enter an integer for the parent process : ");
        int a;
        scanf("%d", &a);

        // writing to the pipe
        write(fd2[WRITE], &a, sizeof(a));

        // closing unused ends
        close(fd2[WRITE]);
        close(fd2[READ]);

        exit(0);

    } else {
        
        // this is the parent process
        printf("Parent PID: %d\n", getpid());
        // waiting for the child process to output its PID
        sleep(1);

        printf("\nI am the parent process\nMy PID is : %d\n", getpid());

        // closing unused ends
        close(fd1[READ]);
        close(fd2[WRITE]);

        // taking user input
        printf("Enter an integer for the child process : ");
        int n;
        scanf("%d", &n);

        // writing to the pipe
        write(fd1[WRITE], &n, sizeof(n));

        // reading from the pipe
        int num;
        read(fd2[READ], &num, sizeof(num));

        printf("\nI am the parent process\nMy PID is : %d\n", getpid());
        printf("I recieved %d from the child\n", num);
        int status;
        waitpid(pid, &status, 0);

        // closing unused ends
        close(fd2[READ]);
        close(fd2[WRITE]);
        
        exit(0);
    }
    return 0;
}