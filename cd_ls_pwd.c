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
  printf("ls_file: to be done: READ textbook!!!!\n");
  // READ Chapter 11.7.3 HOW TO ls
  printf("%4d", mip->INODE.i_block[0]);
}

int ls_dir(MINODE *mip)
{
  printf("ls_dir: list CWD's file names; YOU FINISH IT as ls -l\n");

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
	
     printf("%s  ", temp);

     cp += dp->rec_len;
     dp = (DIR *)cp;
  }
  printf("\n");
}

int ls(char *pathname)
{
  printf("ls: list CWD only! YOU FINISH IT for ls pathname\n");
  ls_dir(running->cwd);
}

void *rpwd(MINODE *wd)
{
  if(wd == root)
  {
      return;
  }
  int myino;
  char myname[256];
  char buf[BLKSIZE];

  // from wd->INODE.i_block[0] get my_ino and parent ino
  // use findino()

  //takes block number and loads it into buf
  get_block(dev, wd->INODE.i_block[0], buf);

  //get parent_ino using getino with the myino you got from findino
  int parent_ino = findino(wd, &myino);

  MINODE *pip = iget(dev, parent_ino);

  // get name use find my name()
  findmyname(pip, myino, myname);

  // recursive call to rpwd(pip) with parent minode
  rpwd(pip);
  //release minode
  iput(pip);
  printf("/%s", myname);
  return; 
}

char *pwd(MINODE *wd)
{
   if(wd == root)
   {
      printf("/\n");
   }
   else 
   {
      rpwd(wd);
      printf("\n");
   }
}


