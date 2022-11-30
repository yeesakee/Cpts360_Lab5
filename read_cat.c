int read_file() {
    int fd = 0;
    int nbytes = 0;
    printf("Input a fd (file descriptor): ");
    scanf("%d", fd);
    if (!is_valid_fd(fd)) {
        printf("Error: Invalid fd\n");
        return -1;
    }
    printf("Input number of bytes to read: ");
    scanf("%d", nbytes);

    if (running->fd[fd]->mode != 0 || running->fd[fd]->mode != 2) {
        printf("Error: fd not open for R/RW");
        return -1;
    }

    char buf[BLKSIZE];
    return my_read(fd, buf, nbytes);
}
int my_read(int fd, char* buf, int nbytes) {
    MINODE *mip = running->fd[fd]->minodePtr;
    // byte offset in file to READ
    int offset = running->fd[fd]->offset;
    // bytes available in file
    int avil = mip->INODE.i_size - offset;
    int lbk, start, blk, count = 0;
    int ibuf[BLKSIZE];
    int jbuf[BLKSIZE];
    while (nbytes && avil) {
        // logical block
        lbk = offset / BLKSIZE;
        // start byte in block
        start = offset % BLKSIZE;
        // convert logical block number lbk to physical block number
        // page# 348
        // direct block
        if (lbk < 12) {
            // read block < 12
            blk = mip->INODE.i_block[lbk];
        }
        // indirect block
        else if (lbk >= 12 && lbk < 256 + 12) {
            // read block 12
            get_block(mip->dev, mip->INODE.i_block[12], ibuf);
            blk = ibuf[lbk-12];
        }
        // double indirect blocks
        else {
            // read block 13
            get_block(mip->dev, mip->INODE.i_block[13], jbuf);
            lbk = lbk - 256 - 12;
            // divide by length to get the block
            blk = jbuf[lbk / 256];
            get_block(mip->dev, blk, ibuf);
            blk = ibuf[lbk%256];
        }

        char* kbuf[BLKSIZE];
        get_block(mip->dev, blk, kbuf);
        char *cp = kbuf + start;
        int remain = BLKSIZE - start;
        if (nbytes > remain) {
            memcpy(buf, cp, remain);
            cp += remain;
            buf += remain;
            running->fd[fd]->offset += remain;
            count += remain;
            avil -= remain;
            nbytes -= remain;
            remain -= remain;
        }
        else {
            memcpy(buf, cp, nbytes);
            cp += nbytes;
            buf += nbytes;
            running->fd[fd]->offset += nbytes;
            count += nbytes;
            avil -= nbytes;
            nbytes -= nbytes;
            remain -= nbytes;
        }
        return count;
    }
}
int my_cat(char* pathname) {
    char mybuf[1024], temp[1024];
    int n;
    strncpy(temp, pathname, 1024);
    int fd = open_file(temp, 0);
    while (n = my_read(fd, mybuf[1024], 1024)) {
        mybuf[n] = 0;
        char* c = mybuf;
        while (c != "\0") {
            if (c == "\n") {
                printf("\n");
            }
            else {
                printf("%c", *c);
            }
            c++;
        }
    }
    close(fd);
}