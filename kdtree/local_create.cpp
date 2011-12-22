#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <stdint.h>

double get_interval(struct timeval start, struct timeval end)
{

    uint64_t val = (end.tv_sec - start.tv_sec) * 1000000 +
        (end.tv_usec - start.tv_usec);
    return (double)val / 1000000;
}


int main()
{
    char buf[1024];
    struct timeval start, end;

    gettimeofday(&start, NULL);
    for(int i = 0; i < 10000; i++) {
        sprintf(buf, "testdata/%d", i);
        int fd = open(buf, O_CREAT | O_RDONLY);
        fchmod(fd, 0777);
        fsync(fd);
        close(fd);    
    }
    gettimeofday(&end, NULL);
    
    printf("%lf", 1 /  get_interval(start, end));

    return 0;
}
