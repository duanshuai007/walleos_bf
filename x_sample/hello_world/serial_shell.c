//#include <main.h>
#include <types.h>
#include <readline.h>
#include <string.h>
#include <sizes.h>
#include <alloc.h>

#include <fs/file_struct.h>
#include <cmd.h>

#include <s5p4418_serial_stdio.h>
#include <s5p4418_tick_delay.h>
#include <synchronize.h>

TPath *pPath = 0;
void result_output(char *message)
{
	console_print(0, message);
}

enum parse_state_t {
	PS_WHITESPACE,
	PS_TOKEN,
	PS_STRING,
	PS_ESCAPE
};

struct command_t {
	const char * name;
	void (*func)(int argc, char ** argv);
	const char * help;
};

void help(int argc, char ** argv);
void clear(int argc, char ** argv);
void hello(int argc, char ** argv);

struct command_t command_lit[] = {
//	{"help", help, " help -- Command help"},
	{"clear", clear," clear -- Clear the screen"},
//	{"hello", hello, " hello -- Say hello to system"}
};

void parse_args(char *argstr, char *argc_p, char **argv, char **resid)
{
	unsigned char argc = 0;
	char c;
	enum parse_state_t s_state = PS_WHITESPACE;
    enum parse_state_t l_state = PS_WHITESPACE;
	enum parse_state_t n_state = PS_WHITESPACE;

	while ((c = *argstr) != 0)
	{
		if (c == ';' && l_state != PS_STRING && l_state != PS_ESCAPE)
			break;

		if (l_state == PS_ESCAPE)
		{
			n_state = s_state;
		}
		else if (l_state == PS_STRING)
		{
			if (c == '"')
		 	{
				n_state = PS_WHITESPACE;
				*argstr = 0;
			}
		 	else
			{
				n_state = PS_STRING;
			}
		}
	 	else if ((c == ' ') || (c == '\t'))
		{
			*argstr = 0;
			n_state = PS_WHITESPACE;
		}
	 	else if (c == '"')
		{
			n_state = PS_STRING;
			*argstr++ = 0;
			argv[argc++] = argstr;
		}
	 	else if (c == '\\')
		{
			s_state = l_state;
			n_state = PS_ESCAPE;
		}
	 	else
		{
			if (l_state == PS_WHITESPACE)
			{
				argv[argc++] = argstr;
			}
			n_state = PS_TOKEN;
		}

		l_state = n_state;
		argstr++;
	}

	argv[argc] = NULL;
	if (argc_p != NULL)
		*argc_p = argc;

	if (*argstr == ';')
	{
		*argstr++ = '\0';
	}
	*resid = argstr;
}

void exec_command(char * intput)
{
	char argc, *argv[20], *resid;
	unsigned int i;
	struct command_t * cmd = 0;

	char *buf = (char *)malloc(strlen(intput) + 1);
	memcpy(buf, intput, strlen(intput) + 1);

	char *temp_buf = buf;

	while(temp_buf && *temp_buf)
 	{
		memset(argv, 0, sizeof(argv));
		parse_args(temp_buf, &argc, argv, &resid);
		//printf("%s, %d, %s, %s\r\n", temp_buf, argc, argv, resid);
		if(argc > 0)
		{
			if(strcmp("*", argv[0]) == 0)
				break;

			for(i = 0; i < ARRAY_SIZE(command_lit); i++)
			{
				cmd = &command_lit[i];
				if(strncmp(cmd->name,argv[0],strlen(argv[0])) == 0)
					break;
				else
					cmd = 0;
			}
			if(cmd == 0)
			{
				if (intput!=NULL && strlen(intput) > 0)
				{					
					TCmdResult result;
                    memset(&result, 0, sizeof(TCmdResult));

					os_cmd_execute(pPath, intput, &result);
					console_print(0, result.pBuffer);

					if (result.pBuffer != 0)
					{
					    free(result.pBuffer);
			            result.pBuffer = 0;
			            result.nBuffLen = 0;
					}
				}				
		   	}
			else
			{
				cmd->func(argc,argv);
			}
		}
		//modify by duan
		//temp_buf = resid;
		temp_buf = NULL;
	}

	if (buf!=NULL)
	{
		free(buf);
		buf = NULL;
	}
}

/*****************************************************************************/
void help(int argc, char ** argv)
{
    UNUSED(argv);

	unsigned int i;

	switch(argc)
	{
	case 1:
		for(i = 0; i < ARRAY_SIZE(command_lit); i++)
		{
			console_print(0, command_lit[i].help);
			console_print(0, "\r\n");
		}
		break;
	default:
		console_print(0, " Invalid 'help' command: too many arguments\r\n");
		console_print(0, " Usage:\r\n");
		console_print(0, "     help\r\n");
		break;
	}
}

extern s32_t cx, cy;
void clear(int argc, char ** argv)
{
    UNUSED(argv);

	switch(argc)
	{
	case 1:
		console_print(0, "\033[2J");
		cx = 0;
		cy = 0;
		break;

	default:
		console_print(0, " Invalid 'clear' command: too many arguments\r\n");
		console_print(0, " Usage:\r\n");
		console_print(0, "     clear\r\n");
		break;
	}
}

void hello(int argc, char ** argv)
{
    UNUSED(argv);

	switch(argc)
	{
	case 1:
		console_print(0, " world\r\n");
		break;

	default:
		console_print(0, " Invalid 'hello' command: too many arguments\r\n");
		console_print(0, " Usage:\r\n");
		console_print(0, "     hello\r\n");
		break;
	}
}

int tester_serial_shell(void)
{
	char * p = NULL;

    if(os_cmd_init(&pPath))
	{
		regiProgressOutputFunc(result_output);
	}
    //else
    //{
    //    return 1;
    //}

//	console_print(0, "\033[2J");
	console_print(0, "s5p4418 shell start...\r\n");

	while(1)
	{
	    p = readline(0, "--> ");
        if(p)
        {
            exec_command(p);
            free(p);
        }
    }

	return 0;
}

