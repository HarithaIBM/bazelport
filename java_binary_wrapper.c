/*
 * java_binary_wrapper.c
 * 
 * Wrapper for Java on z/OS that disables EBCDIC/ASCII auto-conversion
 * on stdout/stderr BEFORE launching the JVM.
 * 
 * Uses z/OS specific fcntl F_CONTROL_CVT command
 * 
 * Compile: xlc -D_OPEN_SYS_FILE_EXT -o java_binary_wrapper java_binary_wrapper.c
 */

#define _OPEN_SYS_FILE_EXT 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/* z/OS function to disable auto-conversion on a file descriptor */
/* Uses structures and constants already defined in fcntl.h */
int __disableautocvt(int fd) {
    struct f_cnvrt req = {SETCVTOFF, 0, 0};
    return fcntl(fd, F_CONTROL_CVT, &req);
}

int main(int argc, char **argv) {
    int rc;
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <java-path> <java-args...>\n", argv[0]);
        fprintf(stderr, "Disables auto-conversion on stdout/stderr before launching Java\n");
        return 1;
    }
    
    fprintf(stderr, "[wrapper] Disabling auto-conversion on file descriptors\n");
    
    /* Disable auto-conversion on stdout */
    rc = __disableautocvt(STDOUT_FILENO);
    if (rc != 0) {
        fprintf(stderr, "[wrapper] WARNING: __disableautocvt(stdout) failed, rc=%d\n", rc);
        perror("__disableautocvt stdout");
    } else {
        fprintf(stderr, "[wrapper] SUCCESS: stdout auto-conversion disabled\n");
    }
    
    /* Disable auto-conversion on stderr */
    rc = __disableautocvt(STDERR_FILENO);
    if (rc != 0) {
        fprintf(stderr, "[wrapper] WARNING: __disableautocvt(stderr) failed, rc=%d\n", rc);
        perror("__disableautocvt stderr");
    } else {
        fprintf(stderr, "[wrapper] SUCCESS: stderr auto-conversion disabled\n");
    }
    
    /* Also set environment variables */
    setenv("_BPXK_AUTOCVT", "OFF", 1);
    setenv("_EDC_AUTOCVT", "OFF", 1);
    
    fprintf(stderr, "[wrapper] Executing: %s\n", argv[1]);
    fflush(stderr);
    
    /* Now exec Java - it inherits the FDs with conversion disabled */
    execv(argv[1], &argv[1]);
    
    /* If we get here, exec failed */
    fprintf(stderr, "[wrapper] ERROR: Failed to execute %s\n", argv[1]);
    perror("execv");
    return 1;
}
