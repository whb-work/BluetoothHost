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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SIZE 512
#define CMD_SIZE 32

void task_options(char *cmdbuf);
void hfp(char *buf);
void a2dp(char *buf);
void phone(char *buf);
void other(char *buf);

int main(int argc, char const *argv[])
{
	int fd_help = -1;
	int fd = -1;
	char rbuf[SIZE] = {'\0'}; //read buf
	char wbuf[SIZE] = {'\0'}; //write buf
//	char cmdbuf[SIZE] = {"\0"};
	int rcount = -1;
//	int n = -1;
//	int m = 100;
//	struct timeval tv = {0, 100};
	pid_t pid;
	struct serial_property *sp;

	sp = malloc(sizeof(struct serial_property));

	if(argc == 1)
	{
		strcpy(sp->portname, "/dev/ttyUSB0");
		sp->speed    = 115200;
		sp->databits = 8;
		sp->stopbits = 1;
		sp->parity   = 'n';
	}
	else if(argc == 2)
	{
		if((strcmp(argv[1], "-v")) == 0 || \
			(strcmp(argv[1], "--version")) == 0)
		{

			printf("version: 1.0.0\n");
			return 0;
		}
		else if((strcmp(argv[1], "-?")) == 0 || \
			(strcmp(argv[1], "--help")) == 0)
		{
			if((fd_help = open("../blue_host_help.txt", O_RDONLY)) < 0)
			{
				perror("open error");
				return -1;
			}

			while(read(fd_help, rbuf, SIZE) > 0)
			{
				printf("%s", rbuf);
				memset(rbuf, 0, SIZE);
			}
			printf("\n");

			close(fd_help);

			return 0;
		}
		else
		{
			parameter_err(argv[1]);

			return 0;
		}
	}
	else if(argc == 7)
	{
		if((strcmp(argv[1], "-l")) == 0 || \
			(strcmp(argv[1], "--list")) == 0)
		{
			sprintf(sp->portname, "/dev/tty%s", argv[2]);
			sp->speed    = atoi(argv[3]);
			sp->databits = atoi(argv[4]);
			sp->stopbits = atoi(argv[5]);
			sp->parity   = *argv[6];
		}
		else
		{
			parameter_err(argv[1]);

			return 0;
		}

	}
	else
	{
		printf("%s: invalid option !\n", app_name);
		printf("Try '%s -?' or '%s --help' for more information.\n", app_name, app_name);

		return 0;
	}
	print_serial_property(sp);


	if((fd = serial_open(sp->portname)) == -1)
	{
		debug_inf("serial_open error\n");
		return -1;
	}
	debug_inf("serial_open success\n");

	if(set_speed(fd, sp->speed) < 0)
	{
		debug_inf("set_speed error\n");
		goto err;
	}
	debug_inf("set_speed success\n");

	//int set_parity(int fd, int databits, int stopbits, int parity)
	if(set_parity(fd, sp->databits, sp->stopbits, sp->parity) == -1)
	{
		debug_inf("set_parity error\n");
		goto err;
	}
	debug_inf("set_parity success\n");

	printf("Connecting to %s, speed %d\n\n", sp->portname, sp->speed);

	if((pid = fork()) < 0)
	{
		perror("fork error");
		goto err;
	}
	else if(pid == 0) //child process
	{
		int cmdlean = 1;

		printf("Bluetooth Host controller\n\n");

		while(1)
		{
			memset(wbuf, 0, SIZE);
			task_options(wbuf);
			printf("|| wbuf: %s\n", wbuf);

			cmdlean = strlen(wbuf);
			wbuf[cmdlean] = '\r';
			wbuf[cmdlean + 1] = '\n';
			printf("CMD: %s\n", wbuf);

			if(write(fd, wbuf, cmdlean + 2) < 0)
			{
				perror("write error");
				return -1;
			}

			sleep(1);
		}
	}
	else //parent process
	{
		while(1)
		{
			memset(rbuf, 0, SIZE);
			if((rcount = serial_read(fd, rbuf, SIZE)) < 0)
			{
				perror("serial_read error");
				return -1;
			}
			else if(rcount == 0)
			{
				continue;
			}
			rbuf[rcount + 1] = '\0';
			printf("%s", rbuf);
			printf("---------------------------------------\n");
		}
	}

err:
	if(serial_close(fd) < 0)
	{
		debug_inf("serial_close error\n");
		return -1;
	}
	debug_inf("serial_close success\n");

//#endif

	return 0;
}

void task_options(char *buf)
{
	char opt;
	char cmdbuf[CMD_SIZE];

	buf[0] = 'A';
	buf[1] = 'T';
	buf[2] = '#';

	//printf("Bluetooth Host controller\n\n");

	while(1)
	{
		printf("Command (m for help): ");
		if(scanf("%c", &opt) > 0)
		{
			while(getchar() != '\n');

			if(opt == 'm')
			{
				printf("Command action\n");
				printf("\ta\t蓝牙免提(HFP)\n");
				printf("\tb\t蓝牙立体声(A2DP)\n");
				printf("\tc\t蓝牙触摸控制(BTC)\n");
				printf("\td\t电话本和通话记录\n");
				printf("\te\t其他\n");
				printf("\tm\t查看选项\n");
			}
			else if(opt == 'a') //蓝牙免提(HFP)
			{
				hfp(cmdbuf);
				strcpy(buf + 3, cmdbuf);
				break;
			}
			else if(opt == 'b') //蓝牙立体声(A2DP)
			{
				hfp(cmdbuf);
				strcpy(buf + 3, cmdbuf);
				break;
			}
			else if(opt == 'c') //蓝牙触摸控制(BTC)
			{
				hfp(cmdbuf);
				strcpy(buf + 3, cmdbuf);
				break;
			}
			else if(opt == 'd') //电话本和通话记录
			{
				hfp(cmdbuf);
				strcpy(buf + 3, cmdbuf);
				break;
			}
			else if(opt == 'e') //其他
			{
				hfp(cmdbuf);
				strcpy(buf + 3, cmdbuf);
				break;
			}
			else
			{
				printf("Input error, please continue\n");
			}
		}
	}
}

void hfp(char *buf)
{
	printf("===> In HFP\n");

	while(1)
	{
		printf("Command (m for help): ");
		memset(buf, 0, CMD_SIZE);
		scanf("%s", buf);
		if(buf[0] == 'm')
		{
			printf("Command action\n");
			printf("\tm\t查看选项\n");
			printf("\tHFP Command: CA, CB, CC ... \n");
		}
		else
		{
			debug_inf("Leave HFP, In Home\n");
			break;
		}
	}
}

void a2dp(char *buf)
{

}

void phone(char *buf)
{

}

void other(char *buf)
{

}

