/*
 * blue_host.h
 *
 *  Created on: Apr 22, 2014
 *      Author: linux
 */

#ifndef BLUE_HOST_H_
#define BLUE_HOST_H_

#include <stdio.h>
#include <stdarg.h>  //va_start(), va_end()

#define DEBUG_INF //print debug
#define ERROR_INF //print error  information

#define SIZE 		128
#define CMD_SIZE 	32

#define CMD_TYPE	5
#define TYPE_NO		15

#define HFP_NO		12
#define A2DP_NO		8
#define BTC_NO		15
#define PHONE_NO	2
#define OTHER_NO	11

#define HFP_TYPE	0
#define A2DP_TYPE	1
#define BTC_TYPE	2
#define PHONE_TYPE 	3
#define OTHER_TYPE	4

//char *app_name = "blue_host"; //Applications name
/*
*my_debug: 调试函数
*/
int debug_inf(const char *format, ...);

/*
*error_inf: 打印错误信息
*/
void error_inf(const char *str);

/*
*串口属性结构体
*/
typedef struct serial_property
{
	char portname[32];
	int speed;
	int databits;
	int stopbits;
	int parity;
}serial_property;

/*
*AT命令及信息
*/
typedef struct cmd_inf
{
	int id;
	int flag;
	char cmd[3];
	char cmd_inf[64];
}cmd_inf;

/*
*打印串口属性
*/
void print_serial_property(struct serial_property *sp);

/*
*打印错误参数信息
*/
void parameter_err(char const *argv1, char const *argv2);

/*
*串口属性设置
*/
int serial_set(int fd, serial_property *sp);

/*
*从CMD文件中获得AT命令信息
*/
//int at_get(const char *pathname, cmd_inf *buff);

/*
*创建无名管道
*/
int pipe_create(int pipefd[2]);

void task_options(char *cmdbuf);
int hfp(char *buf);
int a2dp(char *buf);
int btc(char *buf);
int phone(char *buf);
int other(char *buf);

/*
*电话呼入
*/
void phone_in(char *buf, char *cmdbuf);

/*
*电话呼出
*/
void phone_out(char *buf, char *cmdbuf);

/*
*当前通话
*/
void call(char *buf, char *cmdbuf);

#endif /* BLUE_HOST_H_ */
