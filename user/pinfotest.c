#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "pinfo.h"

int
main(void)
{
  struct pinfo info[10];

  printf("Negative max: %d\n", getpinfo(info, -1));
  printf("Zero max: %d\n", getpinfo(info, 0));
  printf("Invalid pointer: %d\n", getpinfo((struct pinfo *)-1, 1));


  int child = fork();

  if (child == 0) {
    struct pinfo childinfo[10];
    int n = getpinfo(childinfo, 10);

    for (int i = 0; i < n; i++) {
      if (childinfo[i].pid == getpid()) {
        printf("Child initial stats: Level %d, Runtime %d, Schedules %d\n",
               childinfo[i].level,
               childinfo[i].runticks,
               childinfo[i].nsched);
      }
    }

    exit(0);
  } else if (child > 0) {
    wait(0);
  } else {
    printf("Fork failed\n");
  }


  int start = uptime();

  // Use CPU time to reach level 3.
  while (uptime() - start < 30) {
  }

  // Record the current process information.
  int count = getpinfo(info, 10);

  for (int i = 0; i < count; i++) {
    if (info[i].pid == getpid()) {
      printf("Before sleep: Level %d, Runtime %d\n",
             info[i].level, info[i].runticks);
    }
  }

  // Sleep long enough for a global priority boost to occur.
  pause(100);

  count = getpinfo(info, 10);

  for (int i = 0; i < count; i++) {
    if (info[i].pid == getpid()) {
      printf("After sleep: Level %d, Runtime %d\n",
             info[i].level, info[i].runticks);
    }
  }

  exit(0);
}
