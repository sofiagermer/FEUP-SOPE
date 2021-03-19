#include "signal_handling.h"

void sigHandlerSigInt(int signo) {
    //Records the receivement
    regitExecution(getpid(), "SIGNAL_RECV", "SIGINT");
    printf("PID:%d FILEPATH:%s NUMBER OF FILES FOUND:%d NUMBER OF FILES MODIFIED:%d\n",
     getpid(), pInfo->filePath, pInfo->noFilesFound, pInfo->noFilesChanged);
    kill(getpid(), SIGTSTP);
}

void sigHandlerSigIntInitial(int signo) {
    char answer[2];

    //Records the receivement
    regitExecution(getpid(), "SIGNAL_RECV", "SIGINT");
    printf("PID:%d FILEPATH:%s NUMBER OF FILES FOUND:%d NUMBER OF FILES MODIFIED:%d \n", getpid(), pInfo->filePath, pInfo->noFilesFound, pInfo->noFilesChanged);
    usleep(10000);
    
    //Asks if the program should continue and acts accordingly
    while(1) {
        printf("DO YOU WANT TO WANT THE EXECUTION TO PROCEED?(y/n)");
        scanf("%s", answer);
        
        if(strcmp(answer,"y") == 0) {
            kill(0, SIGCONT);
            registSignalSent("SIGCONT", 0);
            break;
        } else if (strcmp(answer,"n") == 0) {
            kill(0, SIGTERM);
            registSignalSent("SIGTERM", 0);
            endProgram(1);
        } else {
            printf("NOT A VALID ANSWER\n");
        }
    } 
}

void sigHandlerSigCont(int signo) {
    //Records the receivement
    regitExecution(getpid(), "SIGNAL_RECV", "SIGCONT");
}

void sigHandlerSigAlrm(int signo) {
    //Records the receivement
    regitExecution(getpid(), "SIGNAL_RECV", "SIGALRM");
    endProgram(1);
}

void sigHandlerSigTerm(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGTERM");
    endProgram(1);
}

void sigHandlerSigHup(int signo) {
    //Records the receivement
    regitExecution(getpid(), "SIGNAL_RECV", "SIGHUP");
    endProgram(1);
}

void sigHandlerSigIO(int signo) {
    //Records the receivement
    regitExecution(getpid(), "SIGNAL_RECV", "SIGIO");
    endProgram(1);
}

void sigHandlerSigPipe(int signo) {
    //Records the receivement
    regitExecution(getpid(), "SIGNAL_RECV", "SIGPIPE");
    endProgram(1);
}

void sigHandlerSigUsr1(int signo) {
    //Records the receivement
    regitExecution(getpid(), "SIGNAL_RECV", "SIGUSR1");
    endProgram(1);
}

void sigHandlerSigUsr2(int signo) {
    //Records the receivement
    regitExecution(getpid(), "SIGNAL_RECV", "SIGUSR2");
    endProgram(1);
}

void setUpSigCont() {
    struct sigaction new, old;
    sigset_t smask;

    //Blocks no signals
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerSigCont; //handler
    if (sigaction(SIGCONT, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigPipe() {
    struct sigaction new, old;
    sigset_t smask;

    //Blocks no signals
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerSigPipe;
    if (sigaction(SIGPIPE, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigUsr1() {
    struct sigaction new, old;
    sigset_t smask;

    //Blocks no signals
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerSigUsr1;
    if (sigaction(SIGUSR1, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigUsr2() {
    struct sigaction new, old;
    sigset_t smask;

    //Blocks no signals
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerSigUsr2;
    if (sigaction(SIGUSR2, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigHup() {
    struct sigaction new, old;
    sigset_t smask;

    //Blocks no signals
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerSigHup;
    if (sigaction(SIGHUP, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigIO() {
    struct sigaction new, old;
    sigset_t smask;

    //Blocks no signals
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerSigIO;
    if (sigaction(SIGIO, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigTerm() {
    struct sigaction new, old;
    sigset_t smask;

    //Blocks no signals
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerSigTerm;
    if (sigaction(SIGTERM, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigAlrm() {
    struct sigaction new, old;
    sigset_t smask;

    //Blocks no signals
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerSigAlrm;
    if (sigaction(SIGALRM, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigInt () {
    struct sigaction new, old;
    sigset_t smask;

    //Blocks no signals
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = SA_NODEFER | SA_RESTART;
    if (pInfo->isInitial)
        new.sa_handler = sigHandlerSigIntInitial; //If first process
    else
        new.sa_handler = sigHandlerSigInt; //If not
    if (sigaction(SIGINT, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigHandlers () {
    setUpSigAlrm();
    setUpSigInt();
    setUpSigTerm();
    setUpSigPipe();
    setUpSigIO();
    setUpSigUsr1();
    setUpSigHup();
    setUpSigUsr2();
    setUpSigCont();
}

