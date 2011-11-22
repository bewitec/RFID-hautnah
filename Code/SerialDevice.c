// copyright - 2011 INKA - HTW Berlin


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>




static int numTrys = 3;


static struct termios origOptions;


static int OpenSerialDevice(const char *dev);
static char *ReturnChar(char *str);
static char *WriteCommand(int fileDescriptor, const char *commandString);
static void CloseSerialDevice(int fileDescriptor);


static int OpenSerialDevice(const char *dev)
{
    int				fileDescriptor = -1;
    int				handshake;
    struct termios	options;
    
    fileDescriptor = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fileDescriptor == -1){
        close(fileDescriptor);
    }
    
    if (ioctl(fileDescriptor, TIOCEXCL) == -1){
        close(fileDescriptor);
    }

    if (tcgetattr(fileDescriptor, &origOptions) == -1){
        close(fileDescriptor);
    }
    
    options = origOptions;
    
    cfmakeraw(&options);
    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 10; 
    cfsetspeed(&options, 115200);		// Set 115200 baud 
			

    
    if (tcsetattr(fileDescriptor, TCSANOW, &options) == -1){
        close(fileDescriptor);
    }
    
    if (ioctl(fileDescriptor, TIOCSDTR) == -1){
        close(fileDescriptor);
    }
    
    if (ioctl(fileDescriptor, TIOCCDTR) == -1) {
        close(fileDescriptor);
    }
    
    handshake = TIOCM_DTR | TIOCM_RTS | TIOCM_CTS | TIOCM_DSR;
    if (ioctl(fileDescriptor, TIOCMSET, &handshake) == -1){
        close(fileDescriptor);
    }
    
    if (ioctl(fileDescriptor, TIOCMGET, &handshake) == -1){
        close(fileDescriptor);
    }
    

    return fileDescriptor;
    return -1;
}


static char *ReturnChar(char *str)
{
    static char     buf[2048];
    char            *ptr = buf;
    *ptr = '\0';
    while (*str){
		if (isprint(*str)){
			*ptr++ = *str++;
		}
		else {
			switch(*str){
				case ' ':
					*ptr++ = *str;
					break;
                    
				case '\r':
					*ptr++ = '\\';
					*ptr++ = 'r';
					break;
            }
			str++;
		}
		*ptr = '\0';
	}
    return buf;

    
    
}

static char *WriteCommand(int fileDescriptor, const char *commandString){
    char		buffer[256];	// Input buffer
    char		*bufPtr;		// Current char in buffer
    ssize_t		numBytes;		// Number of bytes read or written
    int			tries;			// Number of tries so far

    
    for (tries = 1; tries <= numTrys; tries++)
    {
        numBytes = write(fileDescriptor, commandString, strlen(commandString));
        
		if (numBytes == -1){
			printf("Error writing to metraTec reader \n");
			continue;
		}
        
		if (numBytes < strlen(commandString)){
            continue;
		}
        struct timeval tv;
        fd_set readfds;
        tv.tv_sec = 0;
        tv.tv_usec = 200000;
        FD_ZERO(&readfds);
        FD_SET(fileDescriptor, &readfds);
        
            bufPtr = buffer;
            do
            {
                select(fileDescriptor+1, &readfds, NULL, NULL, &tv);
                if (FD_ISSET(fileDescriptor, &readfds)){
                numBytes = read(fileDescriptor, bufPtr, &buffer[sizeof(buffer)] - bufPtr - 1);
                if (numBytes == -1){
                    break;
                }
                else if (numBytes > 0)
                {
                    bufPtr += numBytes;
                }
                else {
                    printf("Nothing read.\n");
                    break;
                }
                }
            } while (numBytes > 0 && FD_ISSET(fileDescriptor, &readfds));
        *bufPtr = '\0';
        
        //printf("Read %s\n", ReturnChar(buffer));
        break;
    }
    return ReturnChar(buffer);
}


static void CloseSerialDevice(int fileDescriptor)
{
    if (tcdrain(fileDescriptor) == -1)
    {
        printf("Error waiting for drain\n");
    }
    
    if (tcsetattr(fileDescriptor, TCSANOW, &origOptions) == -1)
    {
        printf("Error resetting tty attributes\n");
    }

    close(fileDescriptor);
}

