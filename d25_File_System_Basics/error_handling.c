#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>


typedef enum FSError {
    FS_ERROR_NONE = 0,
    FS_ERROR_IO,
    FS_ERROR_NOMEM,
    FS_ERROR_NOSPACE,
    FS_ERROR_NOTFOUND,
    FS_ERROR_PERMISSION,
    FS_ERROR_INVALID
} FSError;


typedef struct FSErrorInfo {
    FSError code;
    char msg[256];
    int system_errno;
} FSErrorInfo;


typedef struct FSContext {
    FSErrorInfo last_error;
} FSContext;


void fs_set_error(FSContext *ctx, FSError code, const char *msg) {
    if (!ctx) return;
    
    ctx->last_error.code = code;
    ctx->last_error.system_errno = errno;
    strncpy(ctx->last_error.msg, msg, 255);
    ctx->last_error.msg[255] = '\0';
}


const char *fs_error_string(FSContext *ctx) {
    if (!ctx) return "Invalid context";

    static char error_buffer[512];
    snprintf(error_buffer, sizeof(error_buffer),
            "Error: %s (code: %d, error: %d - %s)",
            ctx->last_error.msg,
            ctx->last_error.code,
            ctx->last_error.system_errno,
            strerror(ctx->last_error.system_errno));
    
    return error_buffer;    
}
