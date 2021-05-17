#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>

struct termios orig_termios;
void errorHandler(const char *s) {
  perror(s);
  exit(1);
}
void disableRawMode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1){
		errorHandler("tcsetattr");
	}
    
}
void enableRawMode() {
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) errorHandler("tcgetattr");
	atexit(disableRawMode);
	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ICANON);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) errorHandler("tcsetattr");
}

int main(){
	enableRawMode();
	char c;
	char str[5000] = "";

	static struct flock lock;
	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_WRLCK;
	lock.l_start = (off_t)0;
    lock.l_whence = SEEK_SET;
    lock.l_len = (off_t)0;
    lock.l_pid = getpid();

	int fd; 
	fd = open("file.txt", O_RDWR | O_APPEND | O_CREAT, 0644);
	if(fd == -1) {
      printf("Unable to open the file\n");
      exit(0);
    }
    int ch;
    printf("Welcome to Terminal based Text Editor! This text editor allows saving text to file and reading from file. The file implements advisory lock. Choose mode:\n1. Read\n2. Write");
    scanf("\n%d", &ch);

    int ret = fcntl(fd, F_SETLK, &lock);
    if(ret == -1) {
    	printf("Warning: Lock cannot be acquired. Unexpected output may be encountered due to race conditions.\n");
    }
    if(ch == 1){
			if(ret = read(fd, &str, 5000)<0){
				printf("Cannot read from file.\n %d", ret);
			}	
			printf("%s", str);
	}
		
	if(ch == 2){
		printf("To exit text editor from write mode, enter ~ key.\n");
		while (read(STDIN_FILENO, &c, 1) == 1 && c != '~'){

			if(write(fd, &c, 1)<0){
				printf("Cannot write to file.\n");
			}
		}
	}
	lock.l_type = F_UNLCK;
	ret = fcntl(fd, F_SETLK, &lock);
    if(ret == -1) {
    	printf("Lock cannot be released.\n");
    	exit(0);
    }
	close(fd);
	return 0;
}
