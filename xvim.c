#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 128 
#define TTY "/dev/tty"
#define VIM "/usr/bin/vim"

extern char **environ;
static int len;

char** append(char **arr, char *str)
/*
 *  Grows a an array (char**) one string (char*) at a time
 */
{
	static int size = 1;
	size++;
	if (arr == NULL)
		arr = (char**)malloc(size*sizeof(char*));
	else
		arr = (char**)realloc(arr, size*sizeof(char*)); 
	if (arr == NULL) 
		perror("malloc()");
	arr[size-2] = str;
	arr[size-1] = 0;
	return arr;
}

char* alloc_str(char *str)
/*
 * Alloc and ZERO pad string (realloc if needed)
 */

{
	char *ret;
	len = BUF_SIZE;
	if (str == NULL)
		ret = (char*)calloc(BUF_SIZE, sizeof(char));
	else {
		len = strlen(str)*2;
		ret = (char*)calloc(len, sizeof(char)); 
	}
	if (ret == NULL) 
		perror("malloc()");
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
		while(c=getchar()) {
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
	if(freopen(TTY, "r", stdin) == NULL)
		perror("freopen()");
	execve(VIM, args, environ);
	return 0;	
}
