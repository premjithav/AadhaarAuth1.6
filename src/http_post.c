/*  http_post_simple - do a POST to an HTTP URL and return the values

    Copyright GPL 2003 by Mike Chirico <mchirico@users.sourceforge.net>
    Updated: Sun Jun 13 13:53:14 EDT 2004

    A few things to note with this example:
      .. note the \r\n .. it needs a carrage return line feed

          "POST /test.php HTTP/1.0\r\n"
          "Host: souptonuts.sourceforge.net\r\n"

      souptonuts.sourceforge.net ip address is 66.35.250.209
      but use the dns name "souptonuts.sourceforge.net"

      A note on Content-length

         "Content-length: 36\r\n\r\n"
        ->"mode=login&user=test&password=test\r\n"<-- 36 characters

      36 is the length of the string "mode=login&user=test&password=test\r\n"
      If the string is changed, then length must be adjusted.

     Reference:
      http://souptonuts.sourceforge.net/chirico/test.php
      http://souptonuts.sourceforge.net/code/test.php.html

     Download:
      http://prdownloads.sourceforge.net/cpearls/spider.tar.gz?download

*/
#include "aadhaar.h"
#include "uid_auth.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>

#define SA      struct sockaddr
#define MAXLINE 8096
#define MAXSUB  8096


#define LISTENQ         1024

extern int h_errno;

ssize_t process_http(int sockfd, char *host, char *page, unsigned char *poststr)
{
	char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
	ssize_t n;
	snprintf(sendline, MAXSUB,
		 "POST %s HTTP/1.1\r\n"
		 "Host: %s\r\n"
		 "Content-type: text/xml\r\n"
		 "Content-length: %d\r\n\r\n"
		 "%s", page, host, strlen(poststr), poststr);
	printf("\n\nTotal length :%d\n",strlen(poststr));
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\nHTTP REQUEST\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	printf("%s\n",sendline);
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\nHTTP REQUEST-END\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	write(sockfd, sendline, strlen(sendline));
	
	printf("\n\n\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\nHTTP RESPONSE\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

	while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = '\0';
		printf("%s", recvline);
	}
	printf("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\nHTTP RESPONSE-END\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	return n;

}
int main(void)
{
	int sockfd;
	struct sockaddr_in servaddr;

	char **pptr;
	//********** You can change. Puy any values here *******
	//unsigned char *pid_xml;/*FIXME*/
	char *hname = "auth.uidai.gov.in";
	char *page = "/1.6/public/9/9/MKzX8dnY5qyuO4z8neQPDqrSCMAU5pCS32obnzl83xwtFdi45gwK6QA";
	unsigned char *poststr;
	//char *template_data ="Rk1SACAyMAAAAADkAAgAyQFnAMUAxQEAAAARIQBqAGsgPgCIAG0fRwC2AG2dSQBVAIUjPABuALShMgCxAL0jMAByAM6lPgCmAN2kQQBwAN8qNAB1AN8mPADJAOcgOQA8AOorNABoAOomOQC+AO2fMQDFAPqlSgCvAP8lRQB8AQuhPABwAQ4fMgB7ASqcRADAAS4iNwCkATMeMwCFATYeNwBLATYwMQBWATcoMQCkATecMQBEATwyMgBJAUciQQCkAU8cNQB9AVQWNgCEAVUVRACoAVgYOgBBAV69NgCsAWeYNwAA";
	//*******************************************************

	char str[50];
	struct hostent *hptr;
	/************************************************************************/
	FILE *fileptr;
	unsigned char *template_data;
	long filelen;

	fileptr = fopen("bio.bin", "rb");  // Open the file in binary mode
	fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(fileptr);             // Get the current byte offset in the file
	rewind(fileptr);                      // Jump back to the beginning of the file
	
	template_data = (char *)malloc((filelen)*sizeof(char)); // Enough memory for file + \0
	fread(template_data, filelen, 1, fileptr); // Read in the entire file
	fclose(fileptr); // Close the file
	/************************************************************************/
	//poststr = authxml_demographic_details("999999990019","Shivshankar Choudhury");
	printf("File length %d,str %d\n",filelen);
	poststr = biometric_proto_details("999999990026", template_data, filelen);
	//poststr = authxml_biometric_with_fdc("999999990019","", template_data);
	//poststr =authxml_biometric("999999990019",template_data);	

	if ((hptr = gethostbyname(hname)) == NULL) {
		fprintf(stderr, " gethostbyname error for host: %s: %s",
			hname, hstrerror(h_errno));
		exit(1);
	}
	printf("hostname: %s\n", hptr->h_name);
	if (hptr->h_addrtype == AF_INET
	    && (pptr = hptr->h_addr_list) != NULL) {
		printf("address: %s\n",
		       inet_ntop(hptr->h_addrtype, *pptr, str,
				 sizeof(str)));
	} else {
		fprintf(stderr, "Error call inet_ntop \n");
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(80);
	inet_pton(AF_INET, str, &servaddr.sin_addr);

	connect(sockfd, (SA *) & servaddr, sizeof(servaddr));
	process_http(sockfd, hname, page, poststr);
	close(sockfd);
	exit(0);

}

