#include"kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);

int lpipe_first_data(int lpipe[2],int *dst)
{
    if(read(lpipe[RD],dst,sizeof(int)) == sizeof(int)){
        printf("prime %d\n",*dst);
        return 0;
    }

    return -1;
}

void transmit_data(int lpipe[2],int rpipe[2],int first)
{
    int data;
    //read data from left pipe
    while(read(lpipe[RD],&data,sizeof(int)) == sizeof(int) ){
        //将无法整除的数据传递入右管道
        if(data % first){
            write(rpipe[WR],&data,sizeof(int));
        }
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}

void primers(int lpipe[2])
{
    close(lpipe[WR]);

    int first;
    if(lpipe_first_data(lpipe,&first) == -1)
        exit(1);
    int p[2];
    pipe(p);
    transmit_data(lpipe,p,first);

    if(fork() == 0){
        primers(p);
    }else{
        close(p[RD]);
        wait(0);
    }
    exit(0);
}


int main(int argc,char*argv[])
{
    int p[2];
    pipe(p);

    for(int i = 2 ; i <= 35 ; i++){
        write(p[WR],&i,INT_LEN);
    }

    if(fork() == 0){
        primers(p);
    }else{
        close(p[WR]);
        close(p[RD]);
        wait(0);
    }
    exit(0);
}