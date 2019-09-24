#ifndef _CMD_H_
#define _CMD_H_

#include <fs/file_struct.h>

typedef struct TCmdParam{
    char *path;
    unsigned int nNumerical;
    unsigned int nNumerical1;
    unsigned int nNumerical2;
}TCmdParam;

typedef struct TCmdResult{
    unsigned int cmd_type;
    #define CMD_TYPE_INFO 1
    #define CMD_TYPE_QUIT 2
    #define CMD_TYPE_BMP 3
    char *pBuffer;
    unsigned int nBuffLen;
}TCmdResult;


typedef void CMD_EXECUTE (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);

typedef void CMD_PROGRESS_OUTPUT (char *message);

#define COMMANDLEN 16

typedef struct TCmd {
    unsigned     int nIndex;
    char         cmd[COMMANDLEN];
    CMD_EXECUTE  *cmd_execute;   
    unsigned     int nMaxParamNum;
    char         info[256];
}TCmd;

unsigned os_cmd_init(TPath **ppPath);

void os_cmd_execute(TPath *curPath, char *param, TCmdResult *result);

void regiProgressOutputFunc(CMD_PROGRESS_OUTPUT *func);

void os_cmd_destroy(TPath *pPath);

#endif