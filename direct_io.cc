#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#define BLOCK_SIZE 4096

int main() {
    int fd = open("testfile.txt", O_RDWR | O_DIRECT | O_SYNC); // 打开文件，指定 Direct I/O 标志

    if (fd == -1) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    char *buf = nullptr;
    posix_memalign((void **)&buf, BLOCK_SIZE, BLOCK_SIZE); // 分配内存，要求按 BLOCK_SIZE 对齐

    // 从文件中读取数据
    ssize_t nread = pread(fd, buf, BLOCK_SIZE, 0);

    if (nread == -1) {
        std::cerr << "Failed to read from file" << std::endl;
        close(fd);
        return 1;
    }

    std::cout << "Read " << nread << " bytes from file" << std::endl;
    std::cout << "Contents: " << buf << std::endl;

    // 将数据写入文件
    ssize_t nwritten = pwrite(fd, buf, nread, 0);

    if (nwritten == -1) {
        std::cerr << "Failed to write to file" << std::endl;
        close(fd);
        return 1;
    }

    std::cout << "Wrote " << nwritten << " bytes to file" << std::endl;

    free(buf); // 释放内存
    close(fd); // 关闭文件

    return 0;
}
