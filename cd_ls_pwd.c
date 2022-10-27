/************* cd_ls_pwd.c file **************/
int cd(char* pathname)
{
   //printf("cd: under construction READ textbook!!!!\n");
   int ino = getino(pathname);
   //return error if ino = 0
   if(ino == 0)
   {
         printf("error in cd\n");
         return -1;
   }
   MINODE *mip = iget(dev, ino);

   //verify that mip->INODE is a DIR return error if not
   //S_ISDIR 
   //use S_ISDIR to see if dir type 
   //mip = &minode[i];
   if (!S_ISDIR(mip->INODE.i_mode))
   {
         printf("error minode type not DIR\n");
         return -1;
   }

   // release old cwd
   iput(running->cwd);
   //change cwd to mip
   running->cwd = mip;
   return -1;
   // READ Chapter 11.7.3 HOW TO chdir
}

int ls_file(MINODE *mip, char *name)
{
  // READ Chapter 11.7.3 HOW TO ls
  char *t1 = "xwrxwrxwr-------";
  char *t2 = "----------------";
  char ftime[64];
  u16 mode = mip->INODE.i_mode;
  if ((S_ISREG(mode)))
        printf("%c", '-');
    if ((S_ISDIR(mode)))
        printf("%c", 'd');
    if ((S_ISLNK(mode)))
        printf("%c", 'l');
    
    for (int i=8; i >= 0; i--) {
        if (mode & (1 << i))
            printf("%c", t1[i]);
        else
            printf("%c", t2[i]);
    }
    printf("%4d ", mip->INODE.i_links_count);
    printf("%4d ", mip->INODE.i_gid);
    printf("%4d ", mip->INODE.i_uid);
    printf("%8d ", mip->INODE.i_size);

    strcpy(ftime, ctime(&mip->INODE.i_ctime));
    ftime[strlen(ftime-1)] = 0;
    printf("%s ", ftime);
    printf("%s", basename(name));
    char linkname[256];
    if ((S_ISLNK(mode))) {
        printf(" -> %s", mip->INODE.i_mode);
    }
}

int ls_dir(MINODE *mip)
{
  printf("IN LS_DIR");
  char buf[BLKSIZE], temp[256];
  DIR *dp;
  char *cp;

  get_block(dev, mip->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;
  
  while (cp < buf + BLKSIZE){
     strncpy(temp, dp->name, dp->name_len);
     temp[dp->name_len] = 0;
     MINODE *mip1 = iget(dev, dp->inode);
     ls_file(mip1, temp);
     mip1->dirty = 1;
     cp += dp->rec_len;
     dp = (DIR *)cp;
  }
  printf("\n");
  return 0;
}

int ls(char *pathname)
{
  // if not given pathname then call ls_dir on current working directory
  if (strcmp(pathname, "") == 0) {
    ls_dir(running->cwd);
  }
  else {
    printf("IN ELSE\n");
    // if INODE cannot be found
    if (getino(pathname) == -1) {
      printf("INODE does not exist\n");
      return -1;
    }
    else {
      // check if path is a directory or file
      MINODE* mip = iget(dev, getino(pathname));
      u16 mode = mip->INODE.i_mode;
      // if INODE is a directory call ls_dir
      if (S_ISDIR(mode)) {
        printf("CALLING LS_DIR\n");
        ls_dir(mip);
      }
      else {
        // otherwise call ls_file
        printf("CALLING LS_FILE\n");
        ls_file(mip, pathname);
      }
    }

  }
}

char *rpwd(MINODE *wd)
{
  printf("pwd: READ HOW TO pwd in textbook!!!!\n");
  if(wd==root)
  {
      return "0";
  }

   //from wd->INODE.i_block[0] get my_ino and parent ino
   char buf[BLKSIZE];
   get_block(dev, wd->INODE.i_block[0], buf);
   int ino;
   // getting parent ino?
   int parent_ino = getino(wd, &ino);
   MINODE* pip = iget(dev, parent_ino);
    //from pip->INODE.i_block[] get my_name string by my_ino as LOCAL
   pip = iget(dev, parent_ino);
   char name[256];
   findmyname(pip, ino, name);
   pip->dirty = 1;

  // recursive call to rpwd(pip) with parent minode
  rpwd(pip);
  printf("/%s", name);
}

char *pwd(MINODE *wd)
{
   
   if(wd==root)
   {
            printf("/\n");
   }
   else rpwd(wd);

   //pwd start
   //pwd(running->cwd);

}


