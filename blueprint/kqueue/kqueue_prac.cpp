#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
    int kq = kqueue();
    if (kq == -1) {
        perror("kqueue");
        return 1;
    }

    int fd = open("example.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct kevent change;
    EV_SET(&change, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

    while (1) {
        struct kevent event;
        int nev = kevent(kq, &change, 1, &event, 1, NULL);
        if (nev == -1) {
            perror("kevent");
            return 1;
        } else if (nev > 0) {
            if (event.filter == EVFILT_READ) {
                printf("Read event on fd %d\n", fd);
            }
        }
    }

    close(fd);
    close(kq);
    return 0;
}
