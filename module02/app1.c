#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE_PATH "/dev/ram_array"
#define RAM_CLEAR_BUFFER _IO('r', 1)
#define RAM_GET_SIZE _IOR('r', 2, int)
#define RAM_SET_CURSOR _IOW('r', 3, int)
#define RAM_COUNT_VOWELS _IOR('r', 4, int)

void clear_buffer(int fd) {
    ioctl(fd, RAM_CLEAR_BUFFER);
    printf("Buffer cleared.\n");
}

void get_size(int fd) {
    int size;
    ioctl(fd, RAM_GET_SIZE, &size);
    printf("Buffer size: %d bytes\n", size);
}

void set_cursor(int fd, int pos) {
    ioctl(fd, RAM_SET_CURSOR, &pos);
    printf("Cursor set to %d\n", pos);
}

void count_vowels(int fd) {
    int vowel_count;
    ioctl(fd, RAM_COUNT_VOWELS, &vowel_count);
    printf("Vowel count in buffer: %d\n", vowel_count);
}

void write_data(int fd) {
    char buffer[100];
    printf("Enter data to write: ");
    fgets(buffer, sizeof(buffer), stdin);
    write(fd, buffer, strlen(buffer));
}

void read_data(int fd) {
    char buffer[100];
    int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Read: %s\n", buffer);
    } else {
        printf("No data read.\n");
    }
}

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        perror("Failed to open device");
        return 1;
    }
    
    int choice, pos;
    while (1) {
        printf("\nOptions:\n");
        printf("1. Write\n");
        printf("2. Read\n");
        printf("3. Seek\n");
        printf("4. Clear Buffer (ioctl)\n");
        printf("5. Get Buffer Size (ioctl)\n");
        printf("6. Set Cursor (ioctl)\n");
        printf("7. Count Vowels (ioctl)\n");
        printf("8. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                write_data(fd);
                break;
            case 2:
                read_data(fd);
                break;
            case 3:
                printf("Enter seek position: ");
                scanf("%d", &pos);
                lseek(fd, pos, SEEK_SET);
                break;
            case 4:
                clear_buffer(fd);
                break;
            case 5:
                get_size(fd);
                break;
            case 6:
                printf("Enter cursor position: ");
                scanf("%d", &pos);
                set_cursor(fd, pos);
                break;
            case 7:
                count_vowels(fd);
                break;
            case 8:
                close(fd);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}

