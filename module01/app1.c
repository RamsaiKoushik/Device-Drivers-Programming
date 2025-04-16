#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE_PATH "/dev/ram_array"

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    char command;
    char buffer[128];
    int offset;

    while (1) {
        printf("Enter command (r=read, w=write, s=seek, q=quit): ");
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
                printf("Enter seek position: ");
                scanf("%d", &offset);
                if (lseek(fd, offset, SEEK_SET) == -1) {
                    perror("Seek error");
                }
                break;

            case 'q':
                close(fd);
                return 0;

            default:
                printf("Invalid command!\n");
        }
    }
}

