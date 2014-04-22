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

char *app_name = "blue_host"; //Applications name

/*
*my_debug: 调试函数
*/
void debug_inf(const char *format, ...)
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


struct serial_property
{
	char portname[32];
	int speed;
	int databits;
	int stopbits;
	int parity;
};

void print_serial_property(struct serial_property *sp)
{
	printf("portname : %s\n", sp->portname);
	printf("speed    : %d\n", sp->speed);
	printf("databits : %d\n", sp->databits);
	printf("stopbits : %d\n", sp->stopbits);
	printf("parity   : %d\n", sp->parity);
}

void parameter_err(char const *argv)
{
	printf("%s: invalid option -- '%s'\n", app_name, argv);
	printf("Try '%s -?' or '%s --help' for more information.\n", app_name, app_name);
}

#endif /* BLUE_HOST_H_ */
