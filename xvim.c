/******************************************************************************

Copyright (c) 2011 Courtney Ludwin (cludwin@andbot.com)

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 128 
#define TTY "/dev/tty"
#define VIM "/usr/bin/vim"

extern char **environ;
static int len;

static void* ordie(void *p, char *msg) { if (p != NULL) return p; perror(msg); exit(1); }

static char** append(char **arr, char *str)
/*
 *  Grows a an array (char**) one string (char*) at a time
 */
{
	static int size = 1;
	size++;
	arr = (char**)ordie(realloc(arr, size*sizeof(char*)), "realloc"); 
	arr[size-2] = str;
	arr[size-1] = 0;
	return arr;
}

static char* alloc_str(char *str)
/*
 * Alloc and ZERO pad string (realloc if needed)
 */

{
	char *ret;
	len = BUF_SIZE;
	if (str != NULL)
		len = strlen(str)*2;
	ret = (char*)ordie(calloc(len, sizeof(char)), "calloc"); 
	if (str == NULL)
		return ret;
	ret = strcpy(ret, str);
	free(str);
	return ret;
}

int main(int argc, char *argv[])
/*
 * This program provides an xargs-like wrapper around vim 
 */
{
	int i, c;
	char **args = NULL;
	char *buf   = NULL;
	for (i=0; i<argc; i++) 
		args = append(args, argv[i]);
	if (!isatty(fileno(stdin))) {
		i=0;
		buf = alloc_str(NULL);
		while((c=getchar())) {
			if (c <=0 || c == EOF) break;
			if (c == '\n') {
				if (i>0) {
					args = append(args, buf);
					buf  = alloc_str(NULL);
					i    = 0;
				}
				continue;
			}
			if (i>=len-1)
				buf = alloc_str(buf);	
			buf[i++] = (char)c;
		}
		if (i>0)
			args = append(args, buf);
	}
	ordie(freopen(TTY, "r", stdin), "freopen");
	execve(VIM, args, environ);
	return 0;	
}
