#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <sys/wait.h>
#include  <time.h>
#include  <unistd.h>

/*
 * Collaborated with Ibukunoluwa Esan
 */

enum BankAccountTurn {BankAccount = 0, Turn =  1};

void ClientProcess(int[]);

int main(int argc, char * argv[]) {
  int ShmID;
  int *ShmPTR;
  pid_t pid;
  int status;

  time_t t;
  srandom((unsigned) time( & t));
  ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
  if (ShmID < 0) {
    printf("*** shmget error (server) ***\n");
    exit(1);
  }
  ShmPTR = (int * ) shmat(ShmID, NULL, 0);
  if ( * ShmPTR == -1) {
    printf("*** shmat error (server) ***\n");
    exit(1);
  }  
  ShmPTR[BankAccount] = 0;
  ShmPTR[Turn] = 0;
  pid = fork();
  if (pid < 0) {
    exit(1);
  } else if (pid == 0) {
    ClientProcess(ShmPTR);
    exit(0);
  }
  
  // parent process
  int account_num;
  int std_balance;
  int i;

  for (i = 0; i < 25; ++i) {
    sleep(random() % 6);
    account_num = ShmPTR[BankAccount];
    while (ShmPTR[Turn] != 0) {}

    if (account_num <= 100) {
      std_balance = random() % 101;

      if (std_balance % 2 == 0) {
        account_num += std_balance;
        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", std_balance, account_num);
      } else {
        printf("Dear old Dad: Doesn't have any money to give\n");
      }
    } else {
      printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account_num);
    }
    ShmPTR[BankAccount] = account_num;
    ShmPTR[Turn] = 1;
  }

  wait( & status);
  printf("Server has detected the completion of its child...\n");
  shmdt((void * ) ShmPTR);
  printf("Server has detached its shared memory...\n");
  shmctl(ShmID, IPC_RMID, NULL);
  printf("Server has removed its shared memory...\n");
  printf("Server exits...\n");
  exit(0);
}

void ClientProcess(int SharedMem[]) {
  int account_num;
  int std_balance;
  int j;

  for (j = 0; j < 25; ++j) {
    sleep(random() % 6);
    account_num = SharedMem[BankAccount];
    while (SharedMem[Turn] != 1) {}
    std_balance = random() % 51;
    printf("Poor Student needs $%d\n", std_balance);

    if (std_balance <= account_num) {
      account_num -= std_balance;
      printf("Poor Student: Withdraws $%d / Balance = $%d\n", std_balance, account_num);
    } else {
      printf("Poor Student: Not Enough Cash ($%d)\n", account_num );
    }   
    SharedMem[BankAccount] = account_num;
    SharedMem[Turn] = 0;
  }
}