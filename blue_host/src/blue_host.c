/*
 ============================================================================
 Name        : blue_host.c
 Author      : linux
 Version     :
 Copyright   : 1.0.0
 Description : blue_host in C, Ansi-style
 ============================================================================
 */

#include "blue_host.h"
#include "serial/serial.h"
#include <stdio.h>
#include <stdarg.h>  //va_start(), va_end()
#include <unistd.h> //pipe()
#include <string.h>

/*
*my_debug: 调试函数
*/
int debug_inf(const char *format, ...)
{
#ifdef DEBUG_INF
	va_list arglist;

	va_start(arglist, format);

	vprintf(format, arglist);

	va_end(arglist);
#endif
}

/*
*error_inf: 打印错误信息
*/
void error_inf(const char *str)
{
#ifdef ERROR_INF
	perror(str);
#endif
}

/*
*打印串口属性
*/
void print_serial_property(struct serial_property *sp)
{
	printf("portname : %s\n", sp->portname);
	printf("speed    : %d\n", sp->speed);
	printf("databits : %d\n", sp->databits);
	printf("stopbits : %d\n", sp->stopbits);
	printf("parity   : %d\n", sp->parity);
}

/*
*打印错误参数信息
*/
void parameter_err(char const *argv1, char const *argv2)
{
	printf("%s: invalid option -- '%s'\n", argv1, argv2);
	printf("Try '%s -?' or '%s --help' for more information.\n", argv1, argv1);
}

/*
*串口属性设置
*/
int serial_set(int fd, serial_property *sp)
{
	if(set_speed(fd, sp->speed) < 0)
	{
		debug_inf("set_speed error\n");
		return -1;
	}
	debug_inf("set_speed success\n");

	if(set_parity(fd, sp->databits, sp->stopbits, sp->parity) == -1)
	{
		debug_inf("set_parity error\n");
		return -1;
	}
	debug_inf("set_parity success\n");

	return 0;
}

/*
*创建无名管道
*/
int pipe_create(int pipefd[2])
{
	return pipe(pipefd);
}

/*
*任务选项
*/
void task_options(char *buf)
{
	char opt;

st:
	printf("Command action\n");
	printf("\ta\t蓝牙免提(HFP)\n");
	printf("\tb\t蓝牙立体声(A2DP)\n");
	printf("\tc\t蓝牙触摸控制(BTC)\n");
	printf("\td\t电话本和通话记录\n");
	printf("\te\t其他\n");
	printf("\tm\t查看选项\n");
cont:
	printf("Command (m for help): ");
	if(scanf("%c", &opt) > 0)
	{
		while(getchar() != '\n');

		if(opt == 'm')
		{
			goto st;
		}
		else if(opt == 'a') //蓝牙免提(HFP)
		{
			hfp(buf);
		}
		else if(opt == 'b') //蓝牙立体声(A2DP)
		{
			a2dp(buf);
		}
		else if(opt == 'c') //蓝牙触摸控制(BTC)
		{
			btc(buf);
		}
		else if(opt == 'd') //电话本和通话记录
		{
			phone(buf);
		}
		else if(opt == 'e') //其他
		{
			other(buf);
		}
		else if(opt == '\n') //其他
		{
			goto cont;
		}
		else
		{
			printf("Input error, please continue\n");
		}
	}
	else
	{
		perror("scanf error");
		return;
	}
}

int hfp(char *buf)
{
	printf("===> In HFP\n");
	int a;
	FILE *fd;
	int i = 0;
	int n = 0;
	char buff[128];
	cmd_inf p[CMD_TYPE][TYPE_NO];

	if((fd = fopen("../cmd_inf.txt", "r")) == NULL)
	{
		perror("open error");
		return -1;
	}

	for(i = 0; i < CMD_TYPE; i++)
	{
		for(n = 0; n < TYPE_NO; n++)
		{
			if(fgets(buff, 64, fd) > 0)
			{
				sscanf(buff, "%d%d%s%s", &p[i][n].id, &p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
				//debug_inf("buff |%s\n", buff);
			}
		}
	}

	fclose(fd);

	printf("Command action\n");
	i = HFP_TYPE;
	printf("\tID\tCMD\t说明\n");
	printf("-----------------------------------------------------------\n");
	for(n = 0; n < HFP_NO; n++)
	{
		printf("\t%d\t%s\t%s\n", p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
	}
	printf("\t0\t查看选项\n");

	while(1)
	{
		i = HFP_TYPE;
		printf("Command ID(0 for help): ");
		scanf("%d", &a);
		while(getchar() != '\n');

		if(a == 0)
		{
			printf("Command action\n");
			printf("\tID\tCMD\t说明\n");
			printf("-----------------------------------------------------------\n");
			for(n = 0; n < HFP_NO; n++)
			{
				printf("\t%d\t%s\t%s\n", p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
			}
			printf("\t0\t查看选项\n");
		}
		else
		{
			if(a < (HFP_NO + 1) && a > 0)
			{
				memset(buf, 0, CMD_SIZE);
				strcpy(buf, p[i][a - 1].cmd);
				if(a == 10)
				{
					memset(buff, 0, 128);
					printf("please input number: ");
					scanf("%s", buff);
					while(getchar() != '\n');
					strncat(buf, buff, strlen(buff));
				}
				debug_inf("Leave HFP, In Home\n");
				break;
			}
			else
			{
				printf("Input error, please continue\n");
				continue;
			}

		}
	}

	return 0;
}

int a2dp(char *buf)
{
	printf("===> In A2DP\n");
	int a;
	FILE *fd;
	int i = 0;
	int n = 0;
	char buff[128];
	cmd_inf p[CMD_TYPE][TYPE_NO];

	if((fd = fopen("../cmd_inf.txt", "r")) == NULL)
	{
		perror("open error");
		return -1;
	}

	for(i = 0; i < CMD_TYPE; i++)
	{
		for(n = 0; n < TYPE_NO; n++)
		{
			if(fgets(buff, 64, fd) > 0)
			{
				sscanf(buff, "%d%d%s%s", &p[i][n].id, &p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
				//debug_inf("buff |%s\n", buff);
			}
		}
	}

	fclose(fd);

	printf("Command action\n");
	i = A2DP_TYPE;
	printf("\tID\tCMD\t说明\n");
	printf("-----------------------------------------------------------\n");
	for(n = 0; n < A2DP_NO; n++)
	{
		printf("\t%d\t%s\t%s\n", p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
	}
	printf("\t0\t查看选项\n");

	while(1)
	{
		i = A2DP_TYPE;
		printf("Command ID(0 for help): ");
		scanf("%d", &a);
		while(getchar() != '\n');

		if(a == 0)
		{
			printf("Command action\n");
			printf("\tID\tCMD\t说明\n");
			printf("-----------------------------------------------------------\n");
			for(n = 0; n < A2DP_NO; n++)
			{
				printf("\t%d\t%s\t%s\n", p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
			}
			printf("\t0\t查看选项\n");
		}
		else
		{
			if(a < (A2DP_NO + 1) && a > 0)
			{
				memset(buf, 0, CMD_SIZE);
				strcpy(buf, p[i][a - 1].cmd);
				debug_inf("Leave A2DP, In Home\n");
				break;
			}
			else
			{
				printf("Input error, please continue\n");
			}

		}
	}

	return 0;
}

int phone(char *buf)
{
	printf("===> In PHONE\n");
	int a;
	FILE *fd;
	int i = 0;
	int n = 0;
	char buff[128];
	cmd_inf p[CMD_TYPE][TYPE_NO];

	if((fd = fopen("../cmd_inf.txt", "r")) == NULL)
	{
		perror("open error");
		return -1;
	}

	for(i = 0; i < CMD_TYPE; i++)
	{
		for(n = 0; n < TYPE_NO; n++)
		{
			if(fgets(buff, 64, fd) > 0)
			{
				sscanf(buff, "%d%d%s%s", &p[i][n].id, &p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
				//debug_inf("buff |%s\n", buff);
			}
		}
	}

	fclose(fd);

	printf("Command action\n");
	i = PHONE_TYPE;
	printf("\tID\tCMD\t说明\n");
	for(n = 0; n < PHONE_NO; n++)
	{
		printf("\t%d\t%s\t%s\n", p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
	}
	printf("\t0\t查看选项\n");

	while(1)
	{
		i = PHONE_TYPE;
		printf("Command ID(0 for help): ");
		scanf("%d", &a);
		while(getchar() != '\n');

		if(a == 0)
		{
			printf("Command action\n");
			printf("\tID\tCMD\t说明\n");
			for(n = 0; n < PHONE_NO; n++)
			{
				printf("\t%d\t%s\t%s\n", p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
			}
			printf("\t0\t查看选项\n");
		}
		else
		{
			if(a < (PHONE_NO + 1) && a > 0)
			{
				memset(buf, 0, CMD_SIZE);
				strcpy(buf, p[i][a - 1].cmd);
				if(a == 1)
				{
				i_1:
					memset(buff, 0, 128);
					printf("please input target(m for help): ");
					scanf("%s", buff);
					while(getchar() != '\n');
					if(buff[0] == 'm')
					{
						printf("\t0\tSIM\n");
						printf("\t1\tPhone\n");
						printf("\t2\t呼出\n");
						printf("\t3\t未接\n");
						printf("\t4\t呼入\n");
						goto i_1;
					}
					else
					{
						strncat(buf, buff, 1);
						strncat(buf, ",", 1);

						memset(buff, 0, 128);
						printf("please input offset: ");
						scanf("%s", buff);
						while(getchar() != '\n');
						strncat(buf, buff, strlen(buff));
						strncat(buf, ",", 1);

						memset(buff, 0, 128);
						printf("please input count: ");
						scanf("%s", buff);
						while(getchar() != '\n');
						strncat(buf, buff, strlen(buff));
					}
					//strncat(buf, buff, strlen(buff));
				}
				debug_inf("Leave PHONE, In Home\n");
				break;
			}
			else
			{
				printf("Input error, please continue\n");
			}

		}
	}

	return 0;
}

int btc(char *buf)
{
	printf("暂未实现，请选择其他选项\n");
}

int other(char *buf)
{
	printf("===> In OTHER\n");
	int a;
	FILE *fd;
	int i = 0;
	int n = 0;
	char s[10];
	char buff[128];
	cmd_inf p[CMD_TYPE][TYPE_NO];

	if((fd = fopen("../cmd_inf.txt", "r")) == NULL)
	{
		perror("open error");
		return -1;
	}

	for(i = 0; i < CMD_TYPE; i++)
	{
		for(n = 0; n < TYPE_NO; n++)
		{
			if(fgets(buff, 64, fd) > 0)
			{
				sscanf(buff, "%d%d%s%s", &p[i][n].id, &p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
				//debug_inf("buff |%s\n", buff);
			}
		}
	}

	fclose(fd);

	printf("Command action\n");
	i = OTHER_TYPE;
	printf("\tID\tCMD\t说明\n");
	for(n = 0; n < OTHER_NO; n++)
	{
		printf("\t%d\t%s\t%s\n", p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
	}
	printf("\t0\t查看选项\n");

	while(1)
	{
		i = OTHER_TYPE;
		printf("Command ID(0 for help): ");
		scanf("%d", &a);
		while(getchar() != '\n');

		if(a == 0)
		{
			printf("Command action\n");
			printf("\tID\tCMD\t说明\n");
			for(n = 0; n < OTHER_NO; n++)
			{
				printf("\t%d\t%s\t%s\n", p[i][n].flag, p[i][n].cmd, p[i][n].cmd_inf);
			}
			printf("\t0\t查看选项\n");
		}
		else
		{
			if(a < (OTHER_NO + 1) && a > 0)
			{
				memset(buf, 0, CMD_SIZE);
				strcpy(buf, p[i][a - 1].cmd);

				if(a == 1)
				{
					memset(buff, 0, 128);
					printf("please input Name(m = null): ");
					scanf("%s", buff);
					while(getchar() != '\n');
					if(buff[0] != 'm')
					{
						strncat(buf, buff, strlen(buff));
					}
				}
				else if(a == 2)
				{
					memset(buff, 0, 128);
					printf("please input PIN(m = null): ");
					scanf("%s", buff);
					while(getchar() != '\n');
					if(buff[0] != 'm')
					{
						strncat(buf, buff, strlen(buff));
					}
				}
				else if(a == 11)
				{
					memset(buff, 0, 128);
					printf("please input Vol(m = null): ");
					scanf("%s", buff);
					while(getchar() != '\n');
					if(buff[0] != 'm')
					{
						strncat(buf, buff, strlen(buff));
					}
				}
				debug_inf("Leave OTHER, In Home\n");
				break;
			}
			else
			{
				printf("Input error, please continue\n");
			}

		}
	}

	return 0;
}

/*
*电话呼入
*/
void phone_in(char *buf, char *cmdbuf)
{
	char a;
	printf("Call Me: number [%s]\n", buf + 2);
	printf("Command action\n");
	printf("\ta\t接听电话\n"); //CE
	printf("\tb\t拒绝接听\n"); //CF
	printf("\tc\t挂断电话\n"); //CG
	printf("\td\t断开蓝牙免提\n"); //CD
	printf("\te\t连接蓝牙免提\n"); //CC
	printf("\tm\t查看选项\n");

	while(1)
	{
		printf("(Call In)Command (m for help): ");
		memset(cmdbuf, 0, CMD_SIZE);
		scanf("%c", &a);
		while(getchar() != '\n');

		if('m' == a)
		{
			printf("Command action\n");
			printf("\ta\t接听电话\n"); //CE
			printf("\tb\t拒绝接听\n"); //CF
			printf("\tc\t挂断电话\n"); //CG
			printf("\td\t断开蓝牙免提\n"); //CD
			printf("\te\t连接蓝牙免提\n"); //CC
			printf("\tm\t查看选项\n");
			continue;
		}

		else if('a' == a)
		{
			strcpy(cmdbuf, "CE");
			break;
		}
		else if('b' == a)
		{
			strcpy(cmdbuf, "CF");
			break;
		}
		else if('c' == a)
		{
			strcpy(cmdbuf, "CG");
			break;
		}
		else if('d' == a)
		{
			strcpy(cmdbuf, "CD");
			break;
		}
		else if('e' == a)
		{
			strcpy(cmdbuf, "CC");
			break;
		}
	}
}

/*
*电话呼出
*/
void phone_out(char *buf, char *cmdbuf)
{
	char a;
	printf("Call Out: number [%s]\n", buf + 2);

	printf("Command action\n");
	printf("\ta\t挂断电话\n"); //CG
	printf("\tb\t断开蓝牙免提\n"); //CD
	printf("\tc\t连接蓝牙免提\n"); //CC
	printf("\tm\t查看选项\n");

	while(1)
	{
		printf("(Call Out)Command (m for help): ");
		memset(cmdbuf, 0, CMD_SIZE);
		scanf("%c", &a);
		while(getchar() != '\n');

		if('m' == a)
		{
			printf("Command action\n");
			printf("\ta\t挂断电话\n"); //CG
			printf("\tb\t断开蓝牙免提\n"); //CD
			printf("\tc\t连接蓝牙免提\n"); //CC
			printf("\tm\t查看选项\n");
			continue;
		}
		else if('a' == a)
		{
			strcpy(cmdbuf, "CG");
			break;
		}
		else if('b' == a)
		{
			strcpy(cmdbuf, "CD");
			break;
		}
		else if('c' == a)
		{
			strcpy(cmdbuf, "CC");
			break;
		}
	}
}

/*
*当前通话
*/
void call(char *buf, char *cmdbuf)
{
	char a;
	printf("Call: number [%s]\n", buf + 2);

	printf("Command action\n");
	printf("\ta\t挂断电话\n"); //CG
	printf("\tb\t断开蓝牙免提\n"); //CD
	printf("\tc\t连接蓝牙免提\n"); //CC
	printf("\tm\t查看选项\n");

	while(1)
	{
		printf("(Call)Command (m for help): ");
		memset(cmdbuf, 0, CMD_SIZE);
		scanf("%c", &a);
		while(getchar() != '\n');

		if('m' == a)
		{
			printf("Command action\n");
			printf("\ta\t挂断电话\n"); //CG
			printf("\tb\t断开蓝牙免提\n"); //CD
			printf("\tc\t连接蓝牙免提\n"); //CC
			printf("\tm\t查看选项\n");
			continue;
		}
		else if('a' == a)
		{
			strcpy(cmdbuf, "CG");
			break;
		}
		else if('b' == a)
		{
			strcpy(cmdbuf, "CD");
			break;
		}
		else if('c' == a)
		{
			strcpy(cmdbuf, "CC");
			break;
		}
	}
}
