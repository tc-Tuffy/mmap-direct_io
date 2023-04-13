#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("testfile.txt", O_RDWR); // 打开文件
    if (fd == -1) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        std::cerr << "Failed to get file status" << std::endl;
        close(fd);
        return 1;
    }

    char *addr = (char *)mmap(nullptr, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // 映射文件到内存中

    if (addr == MAP_FAILED) {
        std::cerr << "Failed to mmap file" << std::endl;
        close(fd);
        return 1;
    }

    // 从内存中读取数据
    std::cout << "Contents: " << addr << std::endl;

    // 修改内存中的数据
    addr[0] = 'H';
    addr[1] = 'e';
    addr[2] = 'l';
    addr[3] = 'l';
    addr[4] = 'o';

    // 将修改后的数据同步回磁盘
    if (msync(addr, sb.st_size, MS_SYNC) == -1) {
        std::cerr << "Failed to msync file" << std::endl;
        munmap(addr, sb.st_size);
        close(fd);
        return 1;
    }

    munmap(addr, sb.st_size); // 解除内存映射
    close(fd); // 关闭文件

    return 0;
}
