#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>

#define PORT       8081
#define SERVER_IP  "127.0.0.1"
#define BUF_SIZE   1024

static ssize_t send_all(int sock, const void *buf, size_t len) {
    size_t total = 0;
    const char *p = buf;
    while (total < len) {
        ssize_t s = send(sock, p + total, len - total, 0);
        if (s <= 0) {
            perror("[client] send");
            return -1;
        }
        total += s;
    }
    return total;
}

int main(void) {
    int sock;
    struct sockaddr_in srv;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[client] socket");
        exit(1);
    }

    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port   = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &srv.sin_addr);

    if (connect(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("[client] connect");
        close(sock);
        exit(1);
    }

    const char *files[2] = { "processes.txt", "config.txt" };

    for (int i = 0; i < 2; ++i) {
        const char *fn = files[i];

        struct stat st;
        if (stat(fn, &st) < 0) {
            perror(fn);
            close(sock);
            exit(1);
        }
        uint32_t sz = htonl((uint32_t)st.st_size);

        if (send_all(sock, &sz, sizeof(sz)) != sizeof(sz)) {
            fprintf(stderr, "[client] failed to send size\n");
            close(sock);
            exit(1);
        }

        FILE *fp = fopen(fn, "rb");
        if (!fp) { perror("[client] fopen"); close(sock); exit(1); }
        char buf[BUF_SIZE];
        size_t r;
        while ((r = fread(buf, 1, BUF_SIZE, fp)) > 0) {
            if (send_all(sock, buf, r) != (ssize_t)r) {
                fprintf(stderr, "[client] failed to send data chunk\n");
                fclose(fp);
                close(sock);
                exit(1);
            }
        }
        fclose(fp);
    }

    close(sock);
    return 0;
}
