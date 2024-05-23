#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <vector>
#include <string>

void add_file_to_kqueue(int kq, const char* filename, std::vector<int>& fds) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    struct kevent change;
    EV_SET(&change, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

    if (kevent(kq, &change, 1, NULL, 0, NULL) == -1) {
        perror("kevent");
        close(fd);
        return;
    }
    fds.push_back(fd);
}

int main() {
    int kq = kqueue();
    if (kq == -1) {
        perror("kqueue");
        return 1;
    }
    // std::cout << "kq : " << kq << " >> kq create!" << std::endl;
 
    std::vector<int> fds;
    const char* filenames[] = {"example1.txt", "example2.txt", "example3.txt"};
    size_t num_files = sizeof(filenames) / sizeof(filenames[0]);

    for (size_t i = 0; i < num_files; ++i) {
        add_file_to_kqueue(kq, filenames[i], fds);
    }

    while (1) {
        struct kevent events[10];
        int nev = kevent(kq, NULL, 0, events, 10, NULL);
        if (nev == -1) {
            perror("kevent");
            return 1;
        }

        for (int i = 0; i < nev; ++i) {
            struct kevent event = events[i];
            if (event.filter == EVFILT_READ) {
                printf("Read event on fd %d\n", event.ident);

                // Read from the file descriptor
                char buffer[1024];
                ssize_t bytesRead = read(event.ident, buffer, sizeof(buffer) - 1);
                if (bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    printf("Read %zd bytes: %s\n", bytesRead, buffer);
                } else if (bytesRead == 0) {
                    // End of file reached, reset file offset to the beginning
                    printf("End of file on fd %d, resetting file offset\n", (int)event.ident);
                    lseek(event.ident, 0, SEEK_SET);
                } else {
                    perror("read");
                }
            }
        }
    }

    for (size_t i = 0; i < fds.size(); ++i) {
        close(fds[i]);
    }
    close(kq);
    return 0;
}