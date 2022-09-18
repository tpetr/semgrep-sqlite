#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1

#include <bridge_ml.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 64
#define ALLOC_SIZE 4096

#define SEMGREP_ARGS(args...) (char* []){ "semgrep-core", args, NULL }


static void semgrep(sqlite3_context *context, char** args, int fileno) {
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) {
        dup2(fd[1], fileno);
        close(fd[1]);
        close(fd[0]);

        char* ret = bridge_ml_semgrep_analyze(args, NULL, NULL);
        if (ret != NULL) {
            int len = strlen(ret);
            write(fileno, ret, len);
            free(ret);
            exit(1);
        }
        exit(0);
    }
    close(fd[1]);
    char* result;
    if ((result = malloc(ALLOC_SIZE)) == 0) {
        sqlite3_result_error_nomem(context);
        return;
    }
    char buf[BUF_SIZE];
    ssize_t result_size = ALLOC_SIZE;
    ssize_t total = 0;
    ssize_t count;
    while ((count = read(fd[0], buf, BUF_SIZE)) > 0) {
        if (total + BUF_SIZE > result_size) {
            result = realloc(result, result_size * 2);
            if (result == 0) {
                sqlite3_result_error_nomem(context);
                return;
            }
            result_size = result_size * 2;
        }
        strcpy(&result[total], buf);
        total += count;
    }
    close(fd[0]);
    int exitcode;
    waitpid(pid, &exitcode, 0);
    if (exitcode == 0) {
        sqlite3_result_text(context, result, (int)total, 0);
    } else {
        sqlite3_result_error(context, result, (int)total);
    }
    free(result);
}

static void semgrep_version(sqlite3_context *context, int argc, sqlite3_value **argv) {
    semgrep(context, SEMGREP_ARGS("-version"), fileno(stderr));
}

static void semgrep_help(sqlite3_context *context, int argc, sqlite3_value **argv) {
    semgrep(context, SEMGREP_ARGS("-help"), fileno(stdout));
}

static void semgrep_scan(sqlite3_context *context, int argc, sqlite3_value **argv) {


    char filename[] = "/tmp/semgrep-sqlite.XXXXXXXX";

    int fd = mkstemp(filename);
    char* content = sqlite3_value_text(argv[2]);
    write(fd, content, strlen(content));
    close(fd);

    semgrep(context, SEMGREP_ARGS("-json_nodots", "-lang", sqlite3_value_text(argv[0]), "-e", sqlite3_value_text(argv[1]), filename), fileno(stdout));
    unlink(filename);
}

int sqlite3_semgrep_init(
  sqlite3 *db, 
  char **pzErrMsg, 
  const sqlite3_api_routines *pApi
){
    SQLITE_EXTENSION_INIT2(pApi);

    bridge_ml_startup();

    int rc;

    rc = sqlite3_create_function(db, "semgrep", 3, SQLITE_UTF8, 0, semgrep_scan, 0, 0);
    if (rc == 0)
        sqlite3_create_function(db, "semgrep_version", 0, SQLITE_UTF8, 0, semgrep_version, 0, 0);
    if (rc == 0)
        sqlite3_create_function(db, "semgrep_help", 0, SQLITE_UTF8, 0, semgrep_help, 0, 0);

    return rc;
}