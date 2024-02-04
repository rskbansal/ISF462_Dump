#include <stdio.h> // for printf()
#include <stdlib.h> // for exit()
#include <unistd.h> // for fork() & getpid()
#include <sys/wait.h> // for waitpid()

int main() {

    // create a child process
    pid_t pid = fork();

    // determine if the process is the parent or child
    switch (pid) {

        case -1:
            // handling error while creating the child process
            perror("fork");
            return 1;


        case 0:
            // this is the child process
            printf("I am the child process\nMy PID is : %d\nMy parent's PID is : %d\n\n", getpid(), getppid());

            // exit the child process
            _exit(0);


        default:
            // this is the parent process
            printf("I am the parent process\nMy PID is : %d\nMy child's PID is : %d\n\n", getpid(), pid);
            int status;

            // wait for the child process to finish
            waitpid(pid, &status, 0);
            printf("I am the parent process\nMy PID is : %d\n", getpid());
            printf("The child has now exited successfully\nMy child's PID was : %d\n\n", pid);

            // exit the parent process
            exit(0);
    }
    return 0;
}
