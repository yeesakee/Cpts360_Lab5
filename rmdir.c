int rm_child(MINODE* pmip, char* name) {
  char buf[BLKSIZE], temp[BLKSIZE];
  DIR *dp, *prevdp;
  char *cp;

  u16 mode = pmip->INODE.i_mode;
  INODE *ip = &pmip->INODE;

  get_block(dev, pmip->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;
  while (cp < buf + BLKSIZE)
  {
     strncpy(temp, dp->name, dp->name_len);
     temp[dp->name_len] = 0;
     if (!strcmp(temp, name)) {
        // if child is first and only entry in data block
        if (cp == buf && cp + dp->rec_len == buf + BLKSIZE) {
            bdalloc(pmip->dev, ip->i_block[0]);
            ip->i_size -= BLKSIZE;
            put_block(pmip->dev, ip->i_block[0], buf);
        }
        else if (cp + dp->rec_len == buf + BLKSIZE) {
            prevdp->rec_len += dp->rec_len;
            put_block(pmip->dev, ip->i_block[0], buf);
        }
        else {
            // store the last directory information
            DIR *lastdp = (DIR*)buf;
            char *lastcp = buf;

            // loop until we get to the last directory
            while (lastcp + lastdp->rec_len != buf + BLKSIZE) {
                lastcp += lastdp->rec_len;
                lastdp = (DIR*)lastcp;
            }
            
            // updating size
            lastdp->rec_len += dp->rec_len;

            // start of block
            char *start = cp + dp->rec_len;
            char *end = buf + BLKSIZE;

            memmove(cp, start, end - start);
            put_block(pmip->dev, ip->i_block[0], buf);
        }
        pmip->dirty = 1;
        iput(pmip);
        return 0;
     }
     
     cp += dp->rec_len;
     prevdp = dp;
     dp = (DIR *)cp;
  }
  return -1;
}

int my_rmdir(char* pathname) {
    int ino = getino(pathname);
    if (ino == -1) {
        printf("error: ino doesn't exist \n");
        return -1;
    }
    MINODE *mip = iget(dev, ino);
    if (!S_ISDIR(mip->INODE.i_mode)) {
        printf("error minode type not DIR \n");
        return -1;
    }
    if (mip->refCount > 2) {
        printf("error current directory is busy \n");
        return -1;
    }
    if (mip->INODE.i_links_count > 2) {
        printf("error current directory not empty \n");
        return -1;
    }
    char buf[BLKSIZE], name[BLKSIZE];
    get_block(dev, mip->INODE.i_block[0], buf);
    DIR *dp = (DIR *) buf;
    char *cp = buf;
    int count = 0;
    while (cp < buf + BLKSIZE) {
        cp += dp->rec_len;
        dp = (DIR *)cp;
        count++;
        if (count > 2) {
            printf("error current directory is not empty \n");
            return -1;
        }
    }


    int pino = findino();
    MINODE *pmip = iget(mip->dev, pino);
    strcpy(pathname, name);
    char* child = basename(name);
    rm_child(pmip, child);
    iput(pmip);
    bdalloc(mip->dev, mip->INODE.i_block[0]);
    idalloc(mip->dev, mip->ino);
    iput(mip);
}