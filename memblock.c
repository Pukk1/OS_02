#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/memblock.h>

extern struct memblock_type physmem;

struct data{
    unsigned long long base;
    unsigned long long size;
    unsigned long long flags;
};

SYSCALL_DEFINE2(identity, int*, data_size, struct data*, data)

{
    int i = 0;
    // while(memblock.memory.regions+i != NULL){
        copy_to_user(&(data[0].base), &memblock.memory.regions[0].base, sizeof(unsigned long long));
        copy_to_user(&(data[0].size), &memblock.memory.regions[0].size, sizeof(unsigned long long));
        copy_to_user(&(data[0].flags), &memblock.memory.regions[0].flags, sizeof(unsigned long long));
        i++;
    // }
    copy_to_user(data_size, &i, sizeof(int));

    return 0;
}





#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define __NR_identity 548

struct data{
    unsigned long long base;
    unsigned long long size;
    unsigned long long flags;
};

long identity_syscall(int* data_size, struct data* data)
{
    return syscall(__NR_identity, data_size, data);
}

int main(int argc, char *argv[])
{
    long activity;
    int data_size = 0;
    struct data* dataa = malloc(sizeof(struct data)*30);
    activity = identity_syscall(&data_size, dataa);

    for (int i = 0; i < data_size; ++i)
    {
        printf("base %llu \n", dataa[i].base);
        printf("size %llu \n", dataa[i].size);
        printf("flags %llu \n", dataa[i].flags);
    }

    return 0;
}