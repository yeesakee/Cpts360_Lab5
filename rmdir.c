

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
    findname(pmip, ino, name);
    rm_child(pmip, name);
    iput(pmip);
    bdalloc(mip->dev, mip->INODE.i_block[0]);
    idalloc(mip->dev, mip->ino);
    iput(mip);
}