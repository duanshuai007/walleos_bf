#include <cmd.h>
#include <assert.h>
#include <string.h>
#include <uspienv.h>
#include <fs/file.h>
#include <uspios.h>
#include <fs/file_struct.h>
#include <photograph.h>
#include <graphic.h>
#include <stdio.h>
#include <alloc.h>
#include "lwip/apps/net_main.h"
#include <errno.h>
#include <s5p4418_serial_stdio.h>

#include <lwip/apps/net_main.h>

void cmd_help           (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_load_picture   (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_picture        (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_newline        (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_open           (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_ls             (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_pwd            (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_exit           (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_cd             (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_cat            (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_mkdir          (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_delete         (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_write1         (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_write2         (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_mkdirs         (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_isdir          (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_size           (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_seek_open      (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_seek           (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_seek_close     (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_seek_num       (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_net_ping       (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_send_http_data (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_get_ip         (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);
void cmd_system_reset   (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult);

char *tool_parser_command(char *param, char **command);
void tool_parser_param1(char *param, TCmdParam *pCmdParam);
void tool_parser_param2(char *param, TCmdParam *pCmdParam);
void tool_parser_param4(char *param, TCmdParam *pCmdParam);

void tool_general_result(unsigned cmd_type, const char *pBuffer, unsigned nBuffLen, TCmdResult *pResult);

typedef enum ECommandIndex {
    COMMAND_INDEX_HELP = 0,
    COMMAND_INDEX_PIC,
    COMMAND_INDEX_LOAD_PIC,
    COMMAND_INDEX_NEWLINE,
    COMMAND_INDEX_OPEN,
    COMMAND_INDEX_LS,
    COMMAND_INDEX_PWD,
    COMMAND_INDEX_EXIT,
    COMMAND_INDEX_CD,
    COMMAND_INDEX_CAT,
    COMMAND_INDEX_MKDIR,
    COMMAND_INDEX_DELETE,
    COMMAND_INDEX_WRITE1,
    COMMAND_INDEX_WRITE2,
    COMMAND_INDEX_MKDIRS,
    COMMAND_INDEX_ISDIR,
    COMMAND_INDEX_SIZE,
    COMMAND_INDEX_SEEK_OPEN,   
    COMMAND_INDEX_SEEK_SET_GET_NUM,
    COMMAND_INDEX_SEEK,
    COMMAND_INDEX_SEEK_CLOSE,
    COMMAND_INDEX_NET_PING,
    COMMAND_INDEX_NET_SEND_HTTP_DATA,
    COMMAND_INDEX_GET_LOCAL_IP_ADDR,
	COMMAND_INDEX_SYSTEM_RESET,
    COMMAND_INDEX_MAX_NUM

}ECommandIndex;

TCmd cmd[] = {
    { COMMAND_INDEX_HELP                , "help"    ,cmd_help           , 0, "help"},    
    //显示当前路径下的picture,200为显示图片窗口的大小   
    { COMMAND_INDEX_PIC                 , "pic"     ,cmd_picture        , 4, "pic [picname] [size]"},
    //显示绝对路径下的picture,200为显示图片窗口的大小   
    { COMMAND_INDEX_LOAD_PIC            , "loadpic" ,cmd_load_picture   , 4, "loadpic [picname] [size]"}, 
    //输入回车换行
    { COMMAND_INDEX_NEWLINE             , "enter"   ,cmd_newline        , 0, "new line"}, 
    //打开绝对路径下的文件，1234位偏移，从该偏移处开始读取文件
    { COMMAND_INDEX_OPEN                , "open"    ,cmd_open           , 2, "open [file] [index]"}, 
    //显示文件列表
    { COMMAND_INDEX_LS                  , "ls"      ,cmd_ls             , 0, "ls"}, 
    //显示当前路径
    { COMMAND_INDEX_PWD                 , "pwd"     ,cmd_pwd            , 0, "pwd"}, 
    //退出窗口
    { COMMAND_INDEX_EXIT                , "exit"    ,cmd_exit           , 0, "exit"}, 
    //进入目录
    { COMMAND_INDEX_CD                  , "cd"      ,cmd_cd             , 1, "cd [dir]"},
    //查看当前路径下的文件，1234位偏移，从该偏移处开始读取文件
    { COMMAND_INDEX_CAT                 , "cat"     ,cmd_cat            , 2, "cat [file] [index]"},
    //在当前路径下创建文件夹
    { COMMAND_INDEX_MKDIR               , "mkdir"   ,cmd_mkdir          , 1, "mkdir [dirname]"},
    //删除当前路径下的文件，暂不支持删除文件夹
    { COMMAND_INDEX_DELETE              , "delete"  ,cmd_delete         , 1, "delete [file]"},
    //以创建文件的方式进行写文件操作
    { COMMAND_INDEX_WRITE1              , "write1"  ,cmd_write1         , 1, "write1 [file]"},
    //以追加的方式进行写文件操作
    { COMMAND_INDEX_WRITE2              , "write2"  ,cmd_write2         , 1, "write2 [file]"},
    //创建多层目录
    { COMMAND_INDEX_MKDIRS              , "mkdirs"  ,cmd_mkdirs         , 1, "mkdirs [dirname]"},
    //判断参数给出的是否是路径
    { COMMAND_INDEX_ISDIR               , "isdir"   ,cmd_isdir          , 1, "isdir [file]"},
    //获取当前目录下文件的大小    
    { COMMAND_INDEX_SIZE                , "size"    ,cmd_size           , 1, "size [file]"},
    //测试fseek函数，1位从文件头，文件位，当前位置开始偏移   
    { COMMAND_INDEX_SEEK_OPEN           , "seekopen",cmd_seek_open      , 2, "seekopen [file] [seekflag]"},
    //从偏移位置开始，显示十个字符   
    { COMMAND_INDEX_SEEK_SET_GET_NUM    , "seeknum" ,cmd_seek_num       , 2, "seeknum [file] [num]"},
    //测试fseek函数，100位偏移   
    { COMMAND_INDEX_SEEK                , "seek"    ,cmd_seek           , 2, "seek [file] [index]"},
    //关闭
    { COMMAND_INDEX_SEEK_CLOSE          , "seekclose",cmd_seek_close    , 0, "seekclose"},
    //ip地址＋ ping的次数
    { COMMAND_INDEX_NET_PING            , "ping"    ,cmd_net_ping       , 2, "ping [ip] [timers]"}, 
    //从指定的链接下载文件到本地
    { COMMAND_INDEX_NET_SEND_HTTP_DATA	, "http"	,cmd_send_http_data , 2, "http [link]"}, 
    //显示本地ip,mask,gw
    { COMMAND_INDEX_GET_LOCAL_IP_ADDR   , "ip"		,cmd_get_ip			, 0, "show ip/mask/gw"}, 
    //复位
    { COMMAND_INDEX_SYSTEM_RESET		, "reset"   ,cmd_system_reset	, 0, "reset system"},
};

CMD_PROGRESS_OUTPUT *g_progress_output_func;

void regiProgressOutputFunc(CMD_PROGRESS_OUTPUT *func)
{
    g_progress_output_func = func;
}

unsigned cmdIndex(char *pCmd)
{
    assert(pCmd != 0);

    for(int i = 0; i < COMMAND_INDEX_MAX_NUM; i++)
    {
        if(!strcmp(cmd[i].cmd, pCmd))
        {
            return i;
        }
    }

    return COMMAND_INDEX_MAX_NUM;
}

unsigned os_cmd_init(TPath **ppPath)
{
    TPath *pPath = (TPath *)malloc(sizeof(TPath));
    if(pPath == NULL)
    {
        printf("os_cmd_init pPath == NULL\r\n");
        return -ENOMEM;
    }
    
    printf("os_cmd_init: malloc success, pPath:%08x\r\n", pPath);
    unsigned result = fopenRootDir(pPath);
    printf("fopenRootDir = %s\r\n", result ? "TRUE" : "FALSE");
    if (result == TRUE)
    {
        *ppPath = pPath;
        return TRUE;
    }
    else
    {
        free(pPath);
        return FALSE;
    }
}

void os_cmd_destroy(TPath *pPath)
{
    assert(pPath != 0);
    free(pPath);

    g_progress_output_func = 0;
}

void os_cmd_execute(TPath *curPath, char *param, TCmdResult *result)
{

    assert(curPath != 0);
    assert(result != 0);
    assert(param != 0);

    char *command;
    TCmdParam cmdParam;
    char *pParamStart = tool_parser_command(param, &command);
    if (pParamStart == 0)
    {
        cmd[COMMAND_INDEX_NEWLINE].cmd_execute(curPath, &cmdParam, result);
        return;
    }
    
    memset(&cmdParam, 0, sizeof(TCmdParam));

    int cmdindex = cmdIndex(command);

    if (cmdindex == COMMAND_INDEX_MAX_NUM)
    {
        char *pBuff = "command not found!\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), result);
    }
    else if(cmd[cmdindex].nMaxParamNum == 0)
    {
        cmd[cmdindex].cmd_execute(curPath, &cmdParam, result);
    }
    else if (cmd[cmdindex].nMaxParamNum == 1)
    {
        tool_parser_param1(pParamStart, &cmdParam);

        cmd[cmdindex].cmd_execute(curPath, &cmdParam, result);
    }
    else if (cmd[cmdindex].nMaxParamNum == 2)
    {
        tool_parser_param2(pParamStart, &cmdParam);

        cmd[cmdindex].cmd_execute(curPath, &cmdParam, result);
    }
    else if (cmd[cmdindex].nMaxParamNum == 4)
    {
        tool_parser_param4(pParamStart, &cmdParam);

        cmd[cmdindex].cmd_execute(curPath, &cmdParam, result);
    }

    if (command != 0)
    {
        free(command);
        command = 0;
    }

    if (cmdParam.path != 0)
    {
        free(cmdParam.path);
        cmdParam.path = 0;
    }
}

void tool_general_result(unsigned cmd_type, const char *pBuffer, unsigned nBuffLen, TCmdResult *pResult){

    assert(pBuffer!=0);
    assert(pResult!=0);

    pResult->cmd_type = cmd_type;
    pResult->nBuffLen = nBuffLen;

    pResult->pBuffer = malloc(nBuffLen + 1);
    memcpy(pResult->pBuffer, pBuffer, nBuffLen);
    pResult->pBuffer[nBuffLen] = '\0';
}

char *tool_parser_command(char *param, char **command){

    assert(param != 0);
    assert(command != 0);

    int paramLen = strlen(param);

    int start = 0;
    int end = 0;

    while(*(param+start) == ' '&& start < paramLen){
        start++;
    }

    end = start;

    while(*(param+end) != ' ' && end < paramLen){
        end++;
    }

    if (end == start)
    {
        return 0;
    }

    *command = (char *)malloc(end - start + 1);
    memcpy(*command, param + start, end - start);

    (*command)[end - start] = '\0';

    return (char *)(param + end);
}

void tool_parser_param1(char *param, TCmdParam *pCmdParam){

    assert(param != 0);
    assert(pCmdParam != 0);

    int paramLen = strlen(param);

    int start = 0;
    int end = 0;

    while(*(param+start) == ' '&& start < paramLen){
        start++;
    }

    end = start;

    while(*(param+end) != ' ' && end < paramLen){
        end++;
    }

    if (end == start)
    {
        return;
    }

    pCmdParam->path = (char *)malloc(end - start + 1);
    memcpy(pCmdParam->path, param + start, end - start);
    pCmdParam->path[end - start] = '\0';
}

int str2int(char *str){
    assert(0 != str);  
    int ret=0,sign=1;  

    for(; *str==' ' || *str=='\t'; str++);

    if(*str == '-')
    {
        sign = -1;
    }

    if(*str == '-' || *str == '+')
    {
        str++;  
    }

    while(*str >= '0' && *str <= '9')
    {  
        ret = ret * 10 + ( *str - '0');  
        str++;  
    }  

    return (sign * ret);  
}

void tool_parser_param2(char *param, TCmdParam *pCmdParam){

    assert(param != 0);
    assert(pCmdParam != 0);

    int paramLen = strlen(param);

    int start = 0;
    int end = 0;

    while(*(param+start) == ' '&& start < paramLen){
        start++;
    }

    end = start;

    while(*(param+end) != ' ' && end < paramLen){
        end++;
    }

    if (end == start)
    {
        return;
    }

    pCmdParam->path = (char *)malloc(end - start + 1);
    memcpy(pCmdParam->path, param + start, end - start);
    pCmdParam->path[end - start] = '\0';

    if (end < paramLen)
    {
        start = end;

        while(*(param+start) == ' '&& start < paramLen){
            start++;
        }

        end = start;

        while(*(param+end) != ' ' && end < paramLen){
            end++;
        }

        if (end != start)
        {
            char *pNumber = (char *)malloc(end - start + 1);
            memcpy(pNumber, param + start, end - start);
            pNumber[end - start] = '\0';

            pCmdParam->nNumerical = str2int(pNumber);

            if (pNumber != 0)
            {
                free(pNumber);
                pNumber = 0;
            }            
        }
    }
}

void tool_parser_param4(char *param, TCmdParam *pCmdParam){

    assert(param != 0);
    assert(pCmdParam != 0);

    int paramLen = strlen(param);

    int start = 0;
    int end = 0;

    while(*(param+start) == ' '&& start < paramLen){
        start++;
    }

    end = start;

    while(*(param+end) != ' ' && end < paramLen){
        end++;
    }

    if (end == start)
    {
        return;
    }

    pCmdParam->path = (char *)malloc(end - start + 1);
    memcpy(pCmdParam->path, param + start, end - start);
    pCmdParam->path[end - start] = '\0';

    if (end < paramLen)
    {
        start = end;

        while(*(param+start) == ' '&& start < paramLen){
            start++;
        }

        end = start;

        while(*(param+end) != ' ' && end < paramLen){
            end++;
        }

        if (end != start)
        {
            char *pNumber = (char *)malloc(end - start + 1);
            memcpy(pNumber, param + start, end - start);
            pNumber[end - start] = '\0';

            pCmdParam->nNumerical = str2int(pNumber);

            if (pNumber != 0)
            {
                free(pNumber);
                pNumber = 0;
            }            
        }
    }

    if (end < paramLen)
    {
        start = end;

        while(*(param+start) == ' '&& start < paramLen){
            start++;
        }

        end = start;

        while(*(param+end) != ' ' && end < paramLen){
            end++;
        }

        if (end != start)
        {

            char *pNumber = (char *)malloc(end - start + 1);
            memcpy(pNumber, param + start, end - start);
            pNumber[end - start] = '\0';

            pCmdParam->nNumerical1 = str2int(pNumber);

            if (pNumber != 0)
            {
                free(pNumber);
                pNumber = 0;
            }            
        }
    }

    if (end < paramLen)
    {

        start = end;

        while(*(param+start) == ' '&& start < paramLen){
            start++;
        }

        end = start;

        while(*(param+end) != ' ' && end < paramLen){
            end++;
        }

        if (end != start)
        {

            char *pNumber = (char *)malloc(end - start + 1);
            memcpy(pNumber, param + start, end - start);
            pNumber[end - start] = '\0';

            pCmdParam->nNumerical2 = str2int(pNumber);

            if (pNumber != 0)
            {
                free(pNumber);
                pNumber = 0;
            }            
        }
    }        
}

void cmd_help (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    UNUSED(curPath);
    UNUSED(pParam);

    char output[4096];

    char *p = output;
    memset(p, 0, 4096);

    sprintf(p, "support the follow commands:\r\n");
    
    for (int i = 0; i < COMMAND_INDEX_MAX_NUM; ++i)
    {
        p = output + strlen(output);
        sprintf (p, "%12s param num : %d  info:%s\r\n", cmd[i].cmd, cmd[i].nMaxParamNum, cmd[i].info);
    }

    tool_general_result(CMD_TYPE_INFO, output, strlen(output), pResult);
}

void cmd_load_picture (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    char *pFilePath = pParam->path;

    if (pFilePath == 0)
    {
        char *pBuff = "need param of file name\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    unsigned shtwidth = pParam->nNumerical;
    if (shtwidth == 0)
    {
        shtwidth = 960;
    }

    unsigned shtHeight = pParam->nNumerical1;
    if (shtHeight == 0)
    {
        shtHeight = 600;
    }  

    unsigned shtother = pParam->nNumerical2;
    if (shtother == 0)
    {
        shtother = 3;
    }     

    SHEET *sht_win;
    //	create_win(&sht_win,0, 100, shtwidth, shtHeight, edit_pic, "test");
    TScreenColor COL8_C6C6C6 = COLOR16(198, 198, 198);
    create_imge_win(&sht_win, 10, 10, shtwidth, shtHeight, COL8_C6C6C6, 1, "pic");
    printf("sht_win = %08x\r\n", sht_win);
    int result = pg_load_pic(pFilePath, sht_win, shtother);

    if (result == 0)
    {
        char *pBuff = "open picture success!\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);
    }
    else if(result == 1)
    {
        char *pBuff = "picture is not exist\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);
    }
    else
    {
        char *pBuff = "the formal of picture is error\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);
    }
}

void cmd_newline (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult){
    assert(curPath != 0);
    assert(pResult != 0);
    UNUSED(pParam);

    char *pBuff = "";
    tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);
}

void cmd_open (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult){
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    if (pParam->path == 0)
    {
        char *pBuff = "need param of file name\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    // Reopen file, read it and display its contents
    FILE *stream = fopen (pParam->path, "r");
    if (NULL == stream)
    {

        char *pBuff = "Cannot open file\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    if (!fseek(stream, pParam->nNumerical, SEEK_SET))
    {
        char *pBuff = "Cannot seek the position\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        fclose(stream);

        return;
    }

    int fileSize = fsize(stream);

    if (fileSize == 0)
    {
        char *pBuff = "get file size error\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        fclose(stream);

        return;
    }

    char Buffer[100];
    unsigned nResult;

    if (pParam->nNumerical >= (unsigned int)fileSize)
    {
        char *pBuff = "seek position error\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        fclose(stream);

        return;
    }

    char *pOutput = malloc(fileSize - pParam->nNumerical);

    int currentPos = 0;
    while (!feof(stream))
    {
        nResult = fread (Buffer, 1, sizeof(Buffer), stream);

        if (nResult == FS_OPT_ERROR)
        {
            free(pOutput);
            pOutput = 0;

            char *pBuff = "read file error\r\n";
            tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

            fclose(stream);

            return;
        }

        memcpy(pOutput + currentPos, Buffer, nResult);

        currentPos += nResult;
    }

    fclose(stream);

    tool_general_result(CMD_TYPE_INFO, pOutput, fileSize - pParam->nNumerical, pResult);
}

void cmd_ls (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pResult != 0);
    UNUSED(pParam);

    // Show contents of root directory
    TDirentry Direntry;
    TFindCurrentEntry CurrentEntry;
    unsigned nEntry = findFirstDir (curPath, &CurrentEntry, &Direntry);

    char output[2048];

    char *p = output;
    memset(output, 0, 2048);

    for (unsigned i = 0; nEntry != 0; i++)
    {
        if (!(Direntry.nAttributes & (FAT_DIR_ATTR_VOLUME_ID | FAT_DIR_ATTR_HIDDEN | FAT_DIR_ATTR_SYSTEM)))
        {
            p = output + strlen(output);
            sprintf(p, "%-14s  %-24s\r\n", Direntry.cFileName, Direntry.CreatedTime);
        }

        nEntry = findNextDir (curPath, &CurrentEntry, &Direntry);
    }

    tool_general_result(CMD_TYPE_INFO, output, strlen(output), pResult);
}

void cmd_pwd (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pResult != 0);
    UNUSED(pParam);

    unsigned int i;
    char output[128];
    char *p = output;
    
    memset(output, 0, 128);

    for (i = 0; i < curPath->nSeriesNum; ++i)
    {
        p = output + strlen(output);

        if (i != 0)
        {
            sprintf(p, "%s/", curPath->aDirectory[i].cDirectoryName);
        }
        else
        {
            sprintf(p, "/");

            continue;
        }
    }

    p = output + strlen(output);
    sprintf(p, "\r\n");

    tool_general_result(CMD_TYPE_INFO, output, strlen(output), pResult);
}

void cmd_exit (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    UNUSED(curPath);
    UNUSED(pParam);

    char *pBuff = "this is a quit command!\r\n";
    tool_general_result(CMD_TYPE_QUIT, pBuff, strlen(pBuff), pResult);
}

void cmd_cd (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    TCmdParam param;

    memset(&param, 0 ,sizeof(TCmdParam));

    if (pParam->path == 0)
    {
        param.path = (char *)malloc(2);
        assert(param.path != 0);
        strcpy(param.path, "/");
    }
    else if (strlen(pParam->path) == 1 && pParam->path[0] == '-')
    {
        param.path = (char *)malloc(2);
        assert(param.path != 0);
        strcpy(param.path, "/");
    }
    else {
        param.path = (char *)malloc(strlen(pParam->path) + 1);
        assert(param.path != 0);
        strcpy(param.path, pParam->path);
        param.path[strlen(pParam->path)] = '\0';       
    }

    if(!fswitchDir(curPath, param.path)){
        char output[128];
        memset(output, 0, 128);

        sprintf (output, "%s isn't exist!\r\n", param.path);

        tool_general_result(CMD_TYPE_INFO, output, strlen(output), pResult);

        if (param.path != 0)
        {
            free(param.path);
            param.path = 0;
        }
        return;
    }

    char output[128];
    memset(output, 0, 128);

    sprintf (output, "%s\r\n", curPath->aDirectory[curPath->nSeriesNum - 1].cDirectoryName);
    tool_general_result(CMD_TYPE_INFO, output, strlen(output), pResult);

    if (param.path != 0)
    {
        free(param.path);
        param.path = 0;
    }
}

void cmd_picture (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    unsigned int i;
    char *pFileName = pParam->path;
    if (pFileName == 0)
    {
        char *pBuff = "need param of file name\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    char bmpWholePath[256];
    char *p = bmpWholePath;
    memset(bmpWholePath, 0, 256);

    for (i = 0; i < curPath->nSeriesNum; ++i)
    {
        p = bmpWholePath + strlen(bmpWholePath);

        if (i != 0)
        {
            sprintf(p, "%s/", curPath->aDirectory[i].cDirectoryName);
        }
        else
        {
            sprintf(p, "/");
            continue;
        }
    }

    p = bmpWholePath + strlen(bmpWholePath);
    sprintf(p, "%s", pFileName);

    TCmdParam wholePathParam;
    memset(&wholePathParam, 0, sizeof(TCmdParam));

    wholePathParam.path = (char *)malloc (strlen(bmpWholePath) + 1);
    strcpy(wholePathParam.path, bmpWholePath);
    wholePathParam.nNumerical = pParam->nNumerical;
    wholePathParam.nNumerical1 = pParam->nNumerical1;
    wholePathParam.nNumerical2 = pParam->nNumerical2;

    cmd_load_picture(curPath, &wholePathParam, pResult);

    free(wholePathParam.path);
}

void cmd_cat (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    if (pParam->path == 0)
    {
        char *pBuff = "need param of file name\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }
    // Reopen file, read it and display its contents
    FILE *stream = fopenCurFile (curPath, pParam->path, "r");
    if (NULL == stream)
    {
        char *pBuff = "Cannot open file\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    if (!fseek(stream, pParam->nNumerical, SEEK_SET))
    {
        char *pBuff = "Cannot seek the position\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        fclose(stream);

        return;
    }

    unsigned fileSize = fsize(stream);

    if (fileSize == 0)
    {
        char *pBuff = "get file size error\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        fclose(stream);

        return;
    }

    char Buffer[100];
    unsigned nResult;

    if (pParam->nNumerical >= fileSize)
    {
        char *pBuff = "seek position error\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        fclose(stream);

        return;
    }

    char *pOutput = malloc(fileSize - pParam->nNumerical);

    int currentPos = 0;
    while (!feof(stream))
    {
        nResult = fread (Buffer, 1, sizeof(Buffer), stream);
        if (nResult == FS_OPT_ERROR)
        {
            free(pOutput);
            pOutput = 0;

            char *pBuff = "read file error\r\n";
            tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

            fclose(stream);

            return;
        }

        memcpy(pOutput + currentPos, Buffer, nResult);

        currentPos += nResult;
    }

    fclose(stream);

    tool_general_result(CMD_TYPE_INFO, pOutput, fileSize - pParam->nNumerical, pResult);
}

void cmd_mkdir (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    if (pParam->path == 0)
    {
        char *pBuff = "need param of dir\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    unsigned result = fcreateDir(curPath, pParam->path);

    char *pBuff;
    if(FS_DIR_CREATE_SUCCESS == result)
    {
        pBuff = "create dir success\r\n";
    }
    else if(FS_DIR_CREATE_EXISTED == result)
    {
        pBuff = "the dir is aleady exist\r\n";
    }
    else
    {
        pBuff = "create dir failed\r\n";
    }

    tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);
}

void cmd_delete (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    if (pParam->path == 0)
    {
        char *pBuff = "need param of file name\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    char *pBuff;

    if(fdelete(curPath, pParam->path) == FILE_SUCCESS){

        pBuff = "file delete success!\r\n";

    }else{
        pBuff = "file delete failed!\r\n";
    }

    tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);
}

void cmd_write1 (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

   if (pParam->path == 0)
    {
        char *pBuff = "need param of file name\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }    

        // Reopen file, read it and display its contents
    FILE *stream = fopenCurFile (curPath, pParam->path, "w");
    if (NULL == stream)
    {
        char *pBuff = "Cannot open file\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    for (unsigned nLine = 1; nLine <= 5; nLine++)
    {
        char text[128];
        memset(text, 0, 128);

        sprintf (text, "create mode (Line %u)\r\n", nLine);

        if (fwrite (text, 1, strlen(text), stream) != strlen(text))
        {
             char *pBuff = "Write error!\r\n";
            tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

            fclose(stream);

            return;
        }
    }

    fclose(stream);

    char *pBuff = "Write success!\r\n";
    tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);
}

void cmd_write2 (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

   if (pParam->path == 0)
    {
        char *pBuff = "need param of file name\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }
        // Reopen file, read it and display its contents
    FILE *stream = fopenCurFile (curPath, pParam->path, "a");
    if (NULL == stream)
    {
        char *pBuff = "Cannot open file\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    for (unsigned nLine = 100; nLine <= 106; nLine++)
    {
        char text[128];
        memset(text, 0, 128);

        sprintf (text, "add mode (Line %u)\r\n", nLine);

        if (fwrite (text, 1, strlen(text), stream) != strlen(text))
        {
             char *pBuff = "Write error!\r\n";
            tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

            fclose(stream);

            return;
        }
    }

    fclose(stream);

    char *pBuff = "Write success\r\n";
    tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);
}

void cmd_mkdirs (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

   if (pParam->path == 0)
    {
        char *pBuff = "need param of dir\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    char *str;
    if (fcreateDirs(curPath, pParam->path))
    {
        str = "create directory success\r\n";
    }
    else
    {
        str = "create directory failed\r\n";
    }

    tool_general_result(CMD_TYPE_INFO, str, strlen(str), pResult);

    return;
}

void cmd_isdir (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

   if (pParam->path == 0)
    {
        char *pBuff = "need param of file name\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    char *str;

    TDirectory *pUpperDir = &(curPath->aDirectory[curPath->nSeriesNum  - 1]);

    if (isDirectory(pUpperDir, pParam->path))
    {
        str = "is a directory\r\n";


    }else{

        str = "not a directory\r\n";
    }

    tool_general_result(CMD_TYPE_INFO, str, strlen(str), pResult);
}

void cmd_size (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    if (pParam->path == 0)
    {
        char *pBuff = "need param of file name\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    // Reopen file, read it and display its contents
    FILE *stream = fopenCurFile (curPath, pParam->path, "r");
    if (NULL == stream)
    {

        char *pBuff = "Cannot open file\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    int fileSize = fsize(stream);

    char text[128];
    memset(text, 0, 128);
    sprintf (text, "size: %d\r\n", fileSize);

    tool_general_result(CMD_TYPE_INFO, text, strlen(text), pResult);

    fclose(stream);
}

unsigned g_seek_start = SEEK_SET;
FILE *g_seek_file = NULL;
unsigned g_seek_char_num = 8;

void cmd_seek_open (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    if (pParam->path == 0)
    {
        char *pBuff = "need param of file name\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    // Reopen file, read it and display its contents
    g_seek_file = fopenCurFile (curPath, pParam->path, "r");
    if (NULL == g_seek_file)
    {

        char *pBuff = "Cannot open file\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    }

    if (pParam->nNumerical > SEEK_END)
    {
        pParam->nNumerical = SEEK_SET;
    }

    g_seek_start = pParam->nNumerical;

    char *pBuff = "open file success\r\n";
    tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);  
}

void cmd_seek_num (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    UNUSED(curPath);

    if (pParam->nNumerical != 0)
    {
        g_seek_char_num = pParam->nNumerical;
    }

    char text[128];
    memset(text, 0, 128);
    sprintf (text, "current get num set %d per opt \r\n", g_seek_char_num);

    tool_general_result(CMD_TYPE_INFO, text, strlen(text), pResult);  
}

void cmd_seek (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    unsigned int i;

    if (!fseek(g_seek_file, pParam->nNumerical, g_seek_start))
    {
        char *pBuff = "Cannot seek the position\r\n";
        tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);

        return;
    } 

    unsigned char *buffer = (unsigned char *)malloc(g_seek_char_num);
    unsigned nResult;

    nResult = fread (buffer, 1, g_seek_char_num, g_seek_file);

    char *text = (char *)malloc(g_seek_char_num*5);

    char *p = text;
    memset(p, 0, 1024);
    sprintf (p, "start %d, read %d CHAR \r\n", pParam->nNumerical, nResult);

    for (i = 0; i < nResult; ++i)
    {
        p = text + strlen(text);

        sprintf(p, "%2x ", buffer[i]);  


        if ((i+1) % 16 == 0)
        {
            p = text + strlen(text);
            sprintf(p, "\r\n");
        }
    }

    p = text + strlen(text);
    sprintf(p, "\r\n");

    free(buffer);

    tool_general_result(CMD_TYPE_INFO, text, strlen(text), pResult);
}

void cmd_seek_close (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    if (g_seek_file != NULL)
    {        
        fclose(g_seek_file);
        g_seek_file = NULL;
    }

    char *pBuff = "close seek file success\r\n";
    tool_general_result(CMD_TYPE_INFO, pBuff, strlen(pBuff), pResult);
}

unsigned receiveCount = 0;
void ping_result(char *respone)
{
    receiveCount++;
    (*g_progress_output_func)(respone);
}

void cmd_net_ping (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(curPath != 0);
    assert(pParam != 0);
    assert(pResult != 0);

    char *ipAddr = pParam->path;
    int count = pParam->nNumerical;

    receiveCount = 0;

    start_ping(ipAddr, count, ping_result);

    char text[1024] = {0};
    sprintf(text, "----  %s ping statistics----  \r\n%d packets transmitted, %d packets received, %d packets lost\r\n", ipAddr, count, receiveCount, count-receiveCount);

    tool_general_result(CMD_TYPE_INFO, text, strlen(text), pResult);
}

void cmd_download_file_callback(char *buffer, int buflen, char *fileType)
{
    char filename[12] = {0};

    sprintf(filename, "%d.%s", TimerGetTicks(), fileType);

    // Reopen file, read it and display its contents
    FILE *stream = fopen (filename, "w");
    if (NULL == stream)
    {
        return;
    }

    if (fwrite (buffer, 1, buflen, stream) != (size_t)buflen)
    {

        fclose(stream);

        return;
    }

    fclose(stream);  

    char message[32] = {0};
    sprintf(message, "%s download finish\r\n", filename);
    (*g_progress_output_func)(message);
}

char *pTestUrl[20] = {
    "http://pic1.win4000.com/wallpaper/2/57e89a18522ca.jpg",
    "http://pic1.win4000.com/wallpaper/c/539fd80ec0048.jpg",//2560*1600
    "http://192.168.150.203/123.bmp",
    "http://n.sinaimg.cn/sports/2_img/uplaod/4178cfbf/20170825/GGjq-fykiuaz0757009.jpg",
    "http://pic1.win4000.com/wallpaper/e/56f113aecb8bb.jpg",
    "https://timgsa.baidu.com/timg?image&quality=80&size=b9999_10000&sec=1503910657577&di=e003d400e33d525ca307edeb1f3db6b3&imgtype=0&src=http%3A%2F%2Ff.hiphotos.baidu.com%2Fzhidao%2Fpic%2Fitem%2Fae51f3deb48f8c5481fc87e03d292df5e1fe7fd5.jpg",
    "https://b-ssl.duitang.com/uploads/item/201212/14/20121214223106_Gdn8c.thumb.700_0.jpeg",
    "http://desk.fd.zol-img.com.cn/t_s1920x1200c5/g5/M00/06/0B/ChMkJllkKLqIa3DBAAsV05DdLCAAAeWIAHAzXYACxXr695.jpg",
    "https://gss0.baidu.com/9fo3dSag_xI4khGko9WTAnF6hhy/zhidao/pic/item/a8773912b31bb0516204efe7317adab44bede0d9.jpg",
    "http://img.netbian.com/file/2017/0818/fab0d46ecce749c65205a57ef75725f9.jpg",
    "http://192.168.200.21/1.jpg",
    "http://192.168.200.21/2.jpg",
    "http://192.168.200.21/3.jpg",
    "http://192.168.200.21/4.jpg",
    "http://192.168.200.21/5.jpg",
    "http://192.168.200.21/6.jpg",
    "http://192.168.200.21/7.jpg",
    "http://192.168.200.21/8.jpg",
    "http://192.168.200.21/9.jpg",
    "http://192.168.200.21/10.jpg",
};

void cmd_send_http_data (TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    assert(pParam != 0);
    assert(pParam->path != NULL);
    UNUSED(curPath);

    if (pParam->nNumerical != 0)
    {
        SendHttpGetReq(pTestUrl[pParam->nNumerical%20], cmd_download_file_callback);
    }
    else
    {
       SendHttpGetReq(pParam->path, cmd_download_file_callback);       
    }
    
    char *message = "downloading...\r\n";

    tool_general_result(CMD_TYPE_INFO, message, strlen(message), pResult);
}

extern ip_addr_t ipaddr;
extern ip_addr_t netmask;
extern ip_addr_t gw; 

void cmd_get_ip(TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    UNUSED(curPath);
    UNUSED(pParam);

    int len = 0;
    char *p = malloc(sizeof(char)*256);
    memset(p, 0, 256);

    len += sprintf(p, "ip:   %d.%d.%d.%d\r\n", 
            ip4_addr1_16(&ipaddr), ip4_addr2_16(&ipaddr), ip4_addr3_16(&ipaddr), ip4_addr4_16(&ipaddr)); 
    len += sprintf(p + len, "mask: %d.%d.%d.%d\r\n", 
            ip4_addr1_16(&netmask), ip4_addr2_16(&netmask), ip4_addr3_16(&netmask), ip4_addr4_16(&netmask));
    len += sprintf(p + len, "gw:   %d.%d.%d.%d\r\n", 
            ip4_addr1_16(&gw), ip4_addr2_16(&gw), ip4_addr3_16(&gw), ip4_addr4_16(&gw));

    tool_general_result(CMD_TYPE_INFO, p, len, pResult);

	free(p);
}

extern void reset_cpu(void);
void cmd_system_reset(TPath *curPath, TCmdParam *pParam, TCmdResult *pResult)
{
    UNUSED(curPath);
    UNUSED(pParam);

	int len = 0;
	char *p = malloc(sizeof(char) * 128);
	memset(p, 0, 128);

	len += sprintf(p, "%s", "System reset!\r\n");

	tool_general_result(CMD_TYPE_INFO, p, len, pResult);

	reset_cpu();

	free(p);
}

