#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/pid.h>
#include <linux/sched/task.h>
#include <linux/fdtable.h>
#include <asm/processor.h>

struct data{
    unsigned int uid;
    int files_size;
};

SYSCALL_DEFINE5(identity, 
        int, pid,
        struct data*, addr,
        char*, com_path,
        unsigned long*, inodes,
        char*, remote_path)
{
  struct pid *pid_struct = find_get_pid(pid);
  struct task_struct *proc_struct = get_pid_task(pid_struct, PIDTYPE_PID);
  char* buf1 = (char*)kmalloc(sizeof(char)*100, GFP_KERNEL);
  char* buf2 = (char*)kmalloc(sizeof(char)*100, GFP_KERNEL);

  copy_to_user(&addr->uid, &get_task_cred(proc_struct)->uid.val, sizeof(unsigned int));
  char* comm_path = dentry_path_raw(proc_struct->mm->exe_file->f_path.dentry, buf1, 100*sizeof(char));
  copy_to_user(&com_path[0], comm_path, 100*sizeof(char));

  struct fdtable* files_table = files_fdtable(proc_struct->files);
  int i = 0;
  while(files_table->fd[i] != NULL) { 
        struct file* fd = files_table->fd[i];
        struct path files_path = fd->f_path;
        char* cwd = d_path(&files_path, buf2, 100*sizeof(char));
        unsigned long inode = fd->f_inode->i_ino;
        copy_to_user(&inodes[i], &inode, sizeof(unsigned long));
        copy_to_user(&remote_path[i*100], cwd, 100*sizeof(char));

        i++;
    }

    copy_to_user(&addr->files_size, &i, sizeof(int));

    kfree(buf1);
    kfree(buf2);

  return 0;
}







#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define __NR_identity 548

struct data{
    unsigned int uid;
    int files_size;
};

long identity_syscall(struct data* addr, char* com_path, unsigned long* inodes, char* remote_path)
{
    return syscall(__NR_identity, 1692, addr, com_path, inodes, remote_path);
}

int main(int argc, char *argv[])
{
    long activity;
    struct data* addr = malloc(sizeof(struct data));
    char* com_path = malloc(sizeof(char)*100);
    unsigned long* inodes = malloc(sizeof(unsigned long)*30);
    char* remote_path = malloc(sizeof(char)*100*30);
    activity = identity_syscall(addr, com_path, inodes, remote_path);

    if(activity < 0)
    {
        perror("Sorry, Jasper. Your system call appears to have failed.\n");
    }

    else
    {
        printf("Congratulations, Jasper! Your system call is functional. Run the command dmesg in the terminal and find out!\n");

        printf("User: %u \n", addr->uid);
        printf("Exe command: %s \n", com_path);


            printf("%d \n", addr->files_size);

        for(int i =0; i<addr->files_size; i++){
            printf("Open file with fd inode: %lu path: %s \n", inodes[i], &remote_path[100*i]);
        }
    }


    return 0;
}
