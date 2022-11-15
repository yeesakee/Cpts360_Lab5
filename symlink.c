/************* symlink.c file **************/

#ifndef __SYMLINK_C__
#define __SYMLINK_C__

#include "mkdir_creat.c"


int symlink_file(char *old_file, char *new_file)
{
    MINODE *mip; 
  //check old_file must exist 
  //pathname: symlink old_file new_file
  int old_ino = getino(old_file);
  if(old_ino == -1)
  {
    printf("old file does not exist\n");
    return -1; 
  }
  //and new file does not exist 
  int new_ino = getino(new_file);
  if(new_ino != -1)
  {
      printf("error new file aleady exists\n");
      return -1; 
  }
  //creat new file
  creat(new_file);
  //change new file to LNK type
  // check you created file
  if(new_ino == -1)
  {
      printf("error in creat new file does not exist\n");
      return -1; 
  }
  
  mip = iget(dev, new_ino);

  //change new file LNK type
  mip->INODE.i_mode = 0xA1FF;
  
  //store old file name in new files block area
  strncpy(mip->INODE.i_block, old_file, 84);

  //set file size to length of old file name
  mip->INODE.i_size = strlen(old_file) + 1;
  
  //mark new files minode dirty 
  mip->dirty = 1; 
  // iput new files minode 
  iput(mip);

  //mark newfile parent miode dirty 

  //iput new file parent mindoe 


  return 0;
}

int my_readlink(char *pathname)
{
    return 1;
}

#endif