#include "signal_handling.h"

void sigHandlerSigInt(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGINT");
    printf("\nPID:%d FILEPATH:%s NUMBER OF FILES FOUND:%d NUMBER OF FILES MODIFIED:%d\n",
     getpid(), pInfo->filePath, pInfo->noFilesFound, pInfo->noFilesChanged);
    pInfo->sleep = true;
    while(pInfo->sleep) sleep(1);
}

void sigHandlerSigIntInitial(int signo) {
    char answer[2];

    regitExecution(getpid(), "SIGNAL_RECV", "SIGINT");
    printf("\nPID:%d FILEPATH:%s NUMBER OF FILES FOUND:%d NUMBER OF FILES MODIFIED:%d \n", getpid(), pInfo->filePath, pInfo->noFilesFound, pInfo->noFilesChanged);
    sleep(0.5);
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
        } else {
            printf("NOT A VALID ANSWER\n");
        }
    } 
}

void setUpSigHandlers () {
    setUpSigAlrm();
    setUpOtherSigs();
    setUpSigInt();
    setUpSigTerm();
}

void setUpSigInt () {
    struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        exit(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (pInfo->isInitial)
        new.sa_handler = sigHandlerSigIntInitial;
    else
        new.sa_handler = sigHandlerSigInt;
    if (sigaction(SIGINT, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        exit(1);
    }
}

void sigHandlerSigAlrm(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGALRM");
    pInfo->sleep = false;
    if (pInfo->isParent) {
        for (unsigned int i = 0; i < pInfo->noChildren; i++) {
            kill(pInfo->childrenPIDs[i], SIGALRM);   
            registSignalSent("SIGALRM", pInfo->childrenPIDs[i]);
        }
    }
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

void setUpSigTerm() {
    struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        exit(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerSigTerm;
    if (sigaction(SIGTERM, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        exit(1);
    }
}

void setUpSigAlrm() {
    struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        exit(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (pInfo->isInitial)
        new.sa_handler = sigHandlerSigAlrm;
    else
        new.sa_handler = sigHandlerSigAlrm;
    if (sigaction(SIGALRM, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        exit(1);
    }
}

void setUpOtherSigs() {
    /*struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        exit(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerOtherSigs;
    if (sigaction(SIGIO, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        exit(1);
    }
    if (sigaction(SIGPIPE, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        exit(1);
    }*/

}