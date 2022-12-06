/************* mount_umount.c file **************/

#ifndef __MOUNTUMOUNT_C__
#define __MOUNTUMOUNT_C__

#include "mkdir_creat.c"

//display current mounted file systems
int print_mount(void)
{
    printf("Currently mounted file systems\n");
    printf("Dev  Dev Name   Mount Name\n");
    for(int i = 0; i < 8; i++)
    {
        if(mountTable[i].dev != 0 )
        {
            printf("%d\t", mountTable[i].dev, mountTable[i].name, mountTable[i].mount_name);
        }
    }
    return 0; 
}

int mount(char *filesys, char *mount_point)
{
  MINODE *mip; 
  MTABLE *mtptr; 
  // given virtual disk and mount_point a DIR pathname


  //check if file system is already mounted 
  //if already mounted reject
  for (int i = 0; i < 8; i++ )
  {
      if(mountTable[i].dev != 0)
      {
          if(!strcmp(filesys, mountTable[i].name))
          {
              printf("rejected already mounted\n");
              return -1; 
          }
      }
  }
  //if not allocate a free MOUNT table entry (dev = 0)

  
  return 1;
}

int umount(char *pathname)
{
  return 0;
}

#endif
