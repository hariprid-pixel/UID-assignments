#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT       8081
#define BACKLOG    1
#define BUF_SIZE   1024

static ssize_t recv_all(int sock, void *buf, size_t len) {
    size_t total = 0;
    char *p = buf;
    while (total < len) {
        ssize_t r = read(sock, p + total, len - total);
        if (r < 0) {
            perror("[recv_all] read");
            return -1;
        }
        if (r == 0) {
            fprintf(stderr, "[recv_all] peer closed connection\n");
            return total;
        }
        total += r;
    }
    return total;
}

void fcfs_schedule(const char *fname) {
    FILE *fp = fopen(fname, "r");
    if (!fp) { perror("[fcfs] fopen"); return; }
    printf("FCFS Scheduling on %s:\n", fname);
    char pid[32];
    int arrival, burst, time = 0;
    while (fscanf(fp, "%31s %d %d", pid, &arrival, &burst) == 3) {
        if (time < arrival) time = arrival;
        printf("  %s: start=%d, end=%d\n", pid, time, time + burst);
        time += burst;
    }
    fclose(fp);
}

int main(void) {
    int server_fd, client_fd, opt = 1;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[server] socket");
        exit(1);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[server] setsockopt");
        close(server_fd);
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[server] bind");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("[server] listen");
        close(server_fd);
        exit(1);
    }

    if ((client_fd = accept(server_fd, (struct sockaddr *)&addr, &addrlen)) < 0) {
        perror("[server] accept");
        close(server_fd);
        exit(1);
    }

    const char *out_files[2] = {
        "received_processes.txt",
        "received_config.txt"
    };

    for (int i = 0; i < 2; ++i) {
        FILE *fp = fopen(out_files[i], "wb");
        if (!fp) { perror("[server] fopen"); close(client_fd); close(server_fd); exit(1); }

        uint32_t net_sz;
        if (recv_all(client_fd, &net_sz, sizeof(net_sz)) != sizeof(net_sz)) {
            fprintf(stderr, "[server] failed to read size header\n");
            exit(1);
        }
        uint32_t sz = ntohl(net_sz);

        char buf[BUF_SIZE];
        uint32_t remaining = sz;
        while (remaining) {
            size_t chunk = remaining < BUF_SIZE ? remaining : BUF_SIZE;
            if (recv_all(client_fd, buf, chunk) != (ssize_t)chunk) {
                fprintf(stderr, "[server] failed to read data chunk\n");
                exit(1);
            }
            fwrite(buf, 1, chunk, fp);
            remaining -= chunk;
        }
        fclose(fp);
    }

    FILE *cfg = fopen("received_config.txt", "r");
    if (cfg) {
        char algo[32];
        if (fscanf(cfg, "%31s", algo) == 1 && strcmp(algo, "FCFS") == 0) {
            fcfs_schedule("received_processes.txt");
        } else {
            
        }
        fclose(cfg);
    } else {
        perror("[server] fopen config");
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
