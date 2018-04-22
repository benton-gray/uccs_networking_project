	/* This page contains the client program. The following one contains the
 * server program. Once the server has been compiled and started, clients
 * anywhere on the Internet can send commands (file names) to the server.
 * The server responds by opening and returning the entire file requested.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#define SERVER_PORT 2567		/* arbitrary, but client and server must agree */
#define BUF_SIZE 4096			/* block transfer size */

int main(int argc, char **argv)
{
  int c, s, bytes;
  char buf[BUF_SIZE];			/* buffer for incoming file */
  struct hostent *h;			/* info about server */
  struct sockaddr_in channel;		/* holds IP address */
  FILE *fp;

  if (argc != 3) fatal("Usage: client server-name file-name");
  h = gethostbyname(argv[1]);		/* look up host's IP address */
  if (!h) fatal("gethostbyname failed");

  s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s < 0) fatal("socket");
  memset(&channel, 0, sizeof(channel));
  channel.sin_family= AF_INET;
  memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
  channel.sin_port= htons(SERVER_PORT);
  c = connect(s, (struct sockaddr *) &channel, sizeof(channel));
  if (c < 0) fatal("connect failed");

  /* Connection is now established. Send file name including 0 byte at end. */
  write(s, argv[2], strlen(argv[2])+1);

  /* Open file or overwrite file sitting in local directory if transmission is not zero */
  fp = fopen(argv[2], "w");  
  fclose(fp);
  int filedesc = open(argv[2],  O_WRONLY | O_APPEND);

  /* Go get the file and write it to standard output. */
  while (1) {
        bytes = read(s, buf, BUF_SIZE);	/* read from socket */
        if (bytes <= 0) exit(0); 	/* check for end of file */
 	write(filedesc, buf, bytes);	/* write to file name specified  */
        
  }
}

fatal(char *string)
{
  printf("%s\n", string);
  exit(1);
}
