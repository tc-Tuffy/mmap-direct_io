#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#define FILE_SIZE_LENGTH (100)

int main() {

    int N=5;
    int fd = 0;

    fd = open("1.txt", O_RDWR | O_CREAT);
    if (fd<0) {
      printf("device not opened\n");
      return 0;
    }
    lseek(fd, FILE_SIZE_LENGTH, SEEK_SET);/*创建一个空文件,长度是FILE_SIZE_LENGTH*/
    write(fd, "+", 1);/*在文件的最后位置，写下一个字符*/
    // void* mmap(void* start,size_t length,int prot,int flags,int fd,off_t offset);
    //  start 为 null，由系统分配一个内存其实地址并返回，length 文件大小， prot  读写劝降   flags  共享（立即写回）还是写时复制
    auto ptr = (char *)mmap(NULL,FILE_SIZE_LENGTH,
     PROT_READ | PROT_WRITE,
     MAP_SHARED,
     fd,0); 

    if(ptr == MAP_FAILED){
     printf("Mapping Failed\n");
     return 1;
    }
    for(int i=0; i < N; i++){
     ptr[i] = 'a' + i;
    }
    msync(ptr, FILE_SIZE_LENGTH, MS_SYNC);

    printf("Initial values of the array elements :\n");
    for (int i = 0; i < N; i++ ){
     printf(" %c", ptr[i] );
    }
    printf("\n");

    pid_t child_pid = fork();

    if (child_pid == 0) { // child
     printf("\nChild update file\n");
     for (int i = 0; i < N; i++){
      ptr[i] = 'A' + i + 10;
     }
     msync(ptr, FILE_SIZE_LENGTH, MS_SYNC);
    } else { // parent
     waitpid ( child_pid, NULL, 0);
     printf("\nParent:\n");
     printf("Updated values of the array elements :\n");
     for (int i = 0; i < N; i++ ){
      printf(" %c", ptr[i] );
     }
     printf("\n");
     const char* sys = "rm 1.txt";
     system(sys);
    }

    int err = munmap(ptr, N*sizeof(int));

    if(err != 0){
     printf("UnMapping Failed\n");
     return 1;
    }
    close(fd);
    return 0;
}