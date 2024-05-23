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

    // Register the event
    if (kevent(kq, &change, 1, NULL, 0, NULL) == -1) {
        perror("kevent");
        return 1;
    }

    while (1) {
        struct kevent event;
        int nev = kevent(kq, NULL, 0, &event, 1, NULL);
        if (nev == -1) {
            perror("kevent");
            return 1;
        } else if (nev > 0) {
            if (event.filter == EVFILT_READ) {
                printf("Read event on fd %d\n", fd);

                // Read from the file descriptor
                char buffer[1024];
                ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
                if (bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    printf("Read %zd bytes: %s\n", bytesRead, buffer);
                } else if (bytesRead == 0) {
                    // End of file
                    printf("End of file on fd %d\n", fd);
                    break;
                } else {
                    perror("read");
                    break;
                }
            }
        }
    }

    close(fd);
    close(kq);
    return 0;
}
