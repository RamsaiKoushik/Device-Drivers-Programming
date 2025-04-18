#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

#define DEVICE_PATH "/dev/ram_array7"
#define RAM_CLEAR_BUFFER _IO('R', 2)
#define RAM_GET_SIZE _IOR('R', 1, int)
#define RAM_COUNT_VOWELS _IOR('R', 3, int)

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
    //ioctl(fd, RAM_SET_CURSOR, &pos);
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
    int num_bytes;
    printf("Enter number of bytes to read: ");
    scanf("%d", &num_bytes);
    getchar(); // Clear newline
    
    if (num_bytes <= 0 || num_bytes > 100) {
        printf("Invalid read size. Must be between 1 and 100.\n");
        return;
    }
    
    char buffer[100];
    int bytes_read = read(fd, buffer, num_bytes);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Read: %s\n", buffer);
    } else {
        printf("No data read.\n");
    }
}

int main() {
    //int fd = open(DEVICE_PATH, O_RDWR);
    //if (fd == -1) {
        //perror("Failed to open device");
        //return 1;
    //}
    //
    //
    int fd;

    // Retry loop for open()
    while ((fd = open(DEVICE_PATH, O_RDWR)) == -1) {
        if (errno == EBUSY) {
            printf("Device busy, retrying...\n");
            usleep(100000); // Sleep for 100ms before retry
        } else {
            perror("Failed to open device");
            return 1;
        }
    }

    printf("Device opened successfully with fd = %d\n", fd);
    
    int choice, pos;
    while (1) {
        printf("\nOptions:\n");
        printf("1. Write\n");
        printf("2. Read\n");
        printf("3. Seek\n");
        printf("4. Clear Buffer (ioctl)\n");
        printf("5. Get Buffer Size (ioctl)\n");
        //printf("6. Set Cursor (ioctl)\n");
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
            //case 6:
                //printf("Enter cursor position: ");
                //scanf("%d", &pos);
                //set_cursor(fd, pos);
                //break;
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

