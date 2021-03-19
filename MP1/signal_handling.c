#include "signal_handling.h"

void sigHandlerSigInt(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGINT");
    printf("\nPID:%d FILEPATH:%s NUMBER OF FILES FOUND:%d NUMBER OF FILES MODIFIED:%d\n",
     getpid(), pInfo->filePath, pInfo->noFilesFound, pInfo->noFilesChanged);
    wait(NULL);
}

void sigHandlerSigIntInitial(int signo) {
    char answer[2];

    regitExecution(getpid(), "SIGNAL_RECV", "SIGINT");
    printf("\nPID:%d FILEPATH:%s NUMBER OF FILES FOUND:%d NUMBER OF FILES MODIFIED:%d \n", getpid(), pInfo->filePath, pInfo->noFilesFound, pInfo->noFilesChanged);
    sleep(1);
    printf("\nDO YOU WANT TO WANT THE EXECUTION TO PROCEED?(y/n)");
    scanf("%s", answer);

    while(1) {
        if(strcmp(answer,"y") == 0) {
            for (unsigned int i = 0; i < pInfo->noChildren; i++) {
                kill(pInfo->childrenPIDs[i], SIGALRM);
                registSignalSent("SIGALRM", pInfo->childrenPIDs[i]);
            }
            break;
        } else if (strcmp(answer,"n") == 0) {
            for (unsigned int i = 0; i < pInfo->noChildren; i++) {
                kill(pInfo->childrenPIDs[i], SIGTERM);
                registSignalSent("SIGTERM", pInfo->childrenPIDs[i]);
            }
            endProgram(1);
        } else 
            printf("NOT A VALID ANSWER\n");
    } 
}

void sigHandlerSigAlrm(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGALRM");
    if (pInfo->isParent) {
        for (unsigned int i = 0; i < pInfo->noChildren; i++) {
            kill(pInfo->childrenPIDs[i], SIGALRM);   
            registSignalSent("SIGALRM", pInfo->childrenPIDs[i]);
        }
    }
}

void sigHandlerSigAlrmInit(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGALRM");
    endProgram(1);
}

void sigHandlerSigTermInit(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGTERM");
    endProgram(1);
}

void sigHandlerSigTerm(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGTERM");
    if (pInfo->isParent) {
        for (unsigned int i = 0; i < pInfo->noChildren; i++) {
            kill(pInfo->childrenPIDs[i], SIGTERM);   
            registSignalSent("SIGTERM", pInfo->childrenPIDs[i]);
        }
    }
    endProgram(1);
}

void sigHandlerSigHup(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGHUP");
    endProgram(1);
}

void sigHandlerSigIO(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGIO");
    endProgram(1);
}

void sigHandlerSigPipe(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGPIPE");
    endProgram(1);
}

void sigHandlerSigSegv(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGSEGV");
    endProgram(1);
}

void sigHandlerSigUsr1(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGUSR1");
    endProgram(1);
}

void sigHandlerSigUsr2(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGUSR2");
    endProgram(1);
}

void setUpSigPipe() {
    struct sigaction new, old;
    sigset_t smask;
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

void setUpSigSegv() {
    struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerSigSegv;
    if (sigaction(SIGSEGV, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigIO() {
    struct sigaction new, old;
    sigset_t smask;
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
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (pInfo->isInitial) 
        new.sa_handler = sigHandlerSigTermInit;
    else
        new.sa_handler = sigHandlerSigTerm;
    if (sigaction(SIGTERM, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigAlrm() {
    struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (pInfo->isInitial)
        new.sa_handler = sigHandlerSigAlrmInit;
    else
        new.sa_handler = sigHandlerSigAlrm;
    if (sigaction(SIGALRM, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}

void setUpSigHandlers () {
    setUpSigAlrm();
    setUpSigInt();
    setUpSigTerm();
    setUpSigSegv();
    setUpSigPipe();
    setUpSigIO();
    setUpSigUsr1();
    setUpSigHup();
    setUpSigUsr2();
}

void setUpSigInt () {
    struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        endProgram(1);
    }
    new.sa_mask = smask;
    new.sa_flags = SA_NODEFER | SA_RESTART;
    if (pInfo->isInitial)
        new.sa_handler = sigHandlerSigIntInitial;
    else
        new.sa_handler = sigHandlerSigInt;
    if (sigaction(SIGINT, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        endProgram(1);
    }
}
