#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

namespace
{
    void doChild()
    {
        printf("I'm child, my pid is %d.\n", getpid()); 
        fflush(stdout);

        char *args[] = {"/bin/echo", "hello", nullptr};
        execve("/bin/echo", args, nullptr);
        err(EXIT_FAILURE, "error: execve()");
    }
    
    void doParent(const pid_t child)
    {
        printf("I'm parent, my pid is %d and the pid of my child is %d.\n", getpid(), child); 
    }
}

int main()
{
    const auto r = fork();
    if(r == -1)
    {
        err(EXIT_FAILURE, "error: fork()");
    }
    if(r == 0)
    {
        doChild();
    }

    doParent(r);
    exit(EXIT_SUCCESS);
}
