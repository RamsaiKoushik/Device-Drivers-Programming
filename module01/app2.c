#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE_PATH "/dev/ram_array"
#define ARRAY_SIZE 1024

void scan_device(int fd) {
    lseek(fd, 0, SEEK_SET);
    char buffer[ARRAY_SIZE + 1];
    int bytesRead = read(fd, buffer, ARRAY_SIZE);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        printf("Scanned Data: %s\n", buffer);
    } else {
        printf("Scan failed or no data to read.\n");
    }
}

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    char command;
    char buffer[128];
    int offset, whence;

    while (1) {
        printf("\nEnter command (r=read, w=write, s=seek, x=scan, q=quit): ");
        scanf(" %c", &command);

        switch (command) {
            case 'r':
                printf("Enter number of bytes to read: ");
                scanf("%d", &offset);
                if (read(fd, buffer, offset) > 0) {
                    buffer[offset] = '\0';
                    printf("Read Data: %s\n", buffer);
                }
                break;

            case 'w':
                printf("Enter data to write: ");
                scanf("%s", buffer);
                write(fd, buffer, strlen(buffer));
                break;

            case 's':
                printf("Enter seek offset: ");
                scanf("%d", &offset);
                printf("Select seek mode (0=SEEK_SET, 1=SEEK_CUR, 2=SEEK_END): ");
                scanf("%d", &whence);
                if (lseek(fd, offset, whence) == -1) {
                    perror("Seek error");
                }
                break;

            case 'x':
                scan_device(fd);
                break;

            case 'q':
                close(fd);
                return 0;

            default:
                printf("Invalid command!\n");
        }
    }
}

