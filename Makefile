#GCC compiler

CC=gcc
CFLAGS= -D__XMLSEC_FUNCTION__=__FUNCTION__ -DXMLSEC_NO_XSLT=1 -DXMLSEC_NO_XKMS=1 -I/usr/include/libxml2 -DXMLSEC_CRYPTO_DYNAMIC_LOADING=1 -DXMLSEC_CRYPTO=\"openssl\" -DUNIX_SOCKETS -DXML_SECURITY #-DDEBUG
LDFLAGS= -lcrypto -I/usr/include/libxml2 -lxml2 -I/usr/local/include/xmlsec1 -lxmlsec1 -lprotobuf-c
all:
	$(CC) $(CFLAGS) src/http_post.c src/aadhaar.c src/uid_auth.c src/auth_1.6.pb-c.c -o AuthClient $(LDFLAGS)
clean:
	rm -f *.o *~ src/*~ AuthClient
