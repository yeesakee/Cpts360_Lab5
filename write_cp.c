/************* write_cp.c file **************/
#ifndef __WRITECP_C__
#define __WRITECP_C__

#include "read_cat.c"
#include "link_unlink.c"

int write_file()
{
    int fd = 0;
    int nbytes = 0;
    char wbuf[BLKSIZE] = { 0 };
    printf("Input a fd (file descriptor): \n");
    scanf("%d", &fd);
    printf("echo fd entered = %d\n", fd);
    if (!is_valid_fd(fd)) {
        printf("Error: Invalid fd\n");
        return -1;
    }
    printf("Enter what you want to write: \n");
    scanf("%s", &wbuf);
    printf("Echo text = %s\n", wbuf);

    //printf("running fd mode =  %d\n", running->fd[fd]->mode);

    if (running->fd[fd]->mode != 1 && running->fd[fd]->mode != 2) {
        printf("Error: fd not open for R/RW\n");
        return -1;
    }

    //char buf[BLKSIZE];
    nbytes = strlen(wbuf);
    return my_write(fd, wbuf, nbytes);
    //return 1; // Eventually: return the results of my_write
}

int my_write(int fd, char *buf, int nbytes)
{
    char ibuf[BLKSIZE];
   // char writebuf[BLKSIZE] = { 0 };
    char dbuf[BLKSIZE];
    int blk, startByte, lblk, dblk; 
    int numBytes = 0;
    int count = 0; 
    OFT *oftp = running->fd[fd];
    MINODE *mip = oftp->minodePtr; 
    INODE *ip = &mip->INODE; 
    printf("**************************************\n");
    printf("Enter my_write\n");
    printf("File %d size %d offset %d\n", fd, mip->INODE.i_size, oftp->offset);

    printf("Writing to ino = %d\n", mip->ino);
   // int numBytes = nbytes;

    while(nbytes > 0)
    {
        //compute logical blk
        lblk = oftp->offset / BLKSIZE;
        //compute start byte
        startByte = oftp->offset % BLKSIZE;

        //convert lbk to physical block number
        
        //write direct data blocks
        if(lblk < 12)
        {
            if(ip->i_block[lblk] == 0)
            {
                ip->i_block[lblk] = balloc(mip->dev); //allocate a block
            }
            blk = ip->i_block[lblk];  //blk shoud be a disk block now
            printf("mip->dev: %d\n", mip->dev);
        }
        //indirect blocks
        else if(lblk >= 12 && lblk < 256 +12)
        {
            //char ibuf[256]; 
            if(ip->i_block[12] == 0)
            {
                //allocate a block 
                ip->i_block[12] = balloc(mip->dev);
                //zero out block on disk
                int block12 = ip->i_block[12];

                if(block12 == 0)
                {
                    return 0; 
                }
                //get iblock 12 into an int buf
                get_block(mip->dev, ip->i_block[12], ibuf);
                int *pointer = (int *)ibuf;
                for(int i = 0; i < (BLKSIZE / sizeof(int)); i++)
                {
                    pointer[i] = 0; 
                }

                put_block(mip->dev, ip->i_block[12], ibuf);
                mip->INODE.i_blocks++;
            }
            int ibuf[BLKSIZE / sizeof(int)];
            get_block(mip->dev, ip->i_block[12], (char *)ibuf);
            blk = ibuf[lblk - 12];

            if(blk == 0)
            {
                //allocate a disk block 
                //record it in Iblock[12]
                blk = ibuf[lblk - 12] = balloc(mip->dev);
                ip->i_blocks++;
                put_block(mip->dev, ip->i_block[12], (char *)ibuf);
            }
        }
        else
        //double indirect blocks
        {
            lblk = lblk - (BLKSIZE/sizeof(int)) - 12; 
            if(mip->INODE.i_block[13] == 0)
            {
                int block13 = ip->i_block[13] = balloc(mip->dev);
                if(block13 == 0)
                {
                    return 0; 
                }
                get_block(mip->dev, ip->i_block[13], (char *)ibuf);
                int *point = (int *)ibuf;
                for(int i = 0; i < (BLKSIZE / sizeof(int)); i++)
                {
                    point[i] = 0; 
                }
                put_block(mip->dev, ip->i_block[13], ibuf);
                ip->i_blocks++;
            }
            int doublebuf[BLKSIZE/sizeof(int)];
            get_block(mip->dev, ip->i_block[13], (char *)doublebuf);
            dblk = doublebuf[lblk/(BLKSIZE / sizeof(int))];

            if(dblk == 0)
            {
                dblk = doublebuf[lblk/(BLKSIZE / sizeof(int))] = balloc(mip->dev);
                if(dblk == 0)
                {
                    return 0; 
                }
                get_block(mip->dev, dblk, dbuf);
                int *point = (int *)dbuf;
                for(int i = 0; i < (BLKSIZE / sizeof(int)); i++)
                {
                    point[i] = 0; 
                }
                put_block(mip->dev, dblk, dbuf);
                ip->i_blocks++;
                put_block(mip->dev, mip->INODE.i_block[13], (char *)doublebuf);
            }

            memset(doublebuf, 0, BLKSIZE / sizeof(int));
            get_block(mip->dev, dblk, (char *)doublebuf);
            if(doublebuf[lblk % (BLKSIZE / sizeof(int))] == 0)
            {
                blk = doublebuf[lblk % (BLKSIZE / sizeof(int))] = balloc(mip->dev);
                if(blk == 0)
                {
                    return 0; 
                }
                ip->i_blocks++;
                put_block(mip->dev, dblk, (char *)doublebuf);
            }
        }

        char writebuf[BLKSIZE];
        get_block(mip->dev, blk, writebuf);
        char *cp = writebuf + startByte;
        int remain = BLKSIZE - startByte;
        char *temp_buf = buf;

        if(nbytes > remain)
        {
            memcpy(temp_buf, cp, remain);
            cp += remain;
            buf += remain;
            oftp->offset += remain;
            numBytes += remain;
            nbytes -= remain;
            remain = 0; 
        }
        else
        {
            memcpy(temp_buf, cp, nbytes);
            cp += nbytes; 
            buf += nbytes;
            oftp->offset += nbytes;
            numBytes += nbytes;
            remain -= nbytes;
            nbytes = 0; 
        }

        if(oftp->offset > mip->INODE.i_size)
        {
            mip->INODE.i_size = oftp->offset;
        }
        put_block(mip->dev, blk, writebuf);
    }
    mip->dirty = 1; 
    printf("**************************************\n");
    printf("wrote %d char into file fd = %d\n", numBytes, fd);
    printf("End my_write\n");
    printf("**************************************\n");
    return numBytes;
}

int my_cp(char *src_file, char *dest_file)
{
    printf("enter cp\n");
    printf("src file name: %s dest file name: %s\n", src_file, dest_file);
    int n = 0; 
    char buf[BLKSIZE];

     if(src_file[0] == '/')
    {
        dev = root->dev;
    }
    else
    {
        dev = running->cwd->dev;
    }

    //open source file for read
    int fd_src = open_file(src_file, 0);

     if(dest_file[0] == '/')
    {
        dev = root->dev;
    }
    else
    {
        dev = running->cwd->dev;
    }
    char dest_copy[100];
    strcpy(dest_copy, dest_file);

    int ino = getino(dest_copy);
    //cant find dest file need to creat it
    if(ino == 0)
    {
        printf("need to creat dest file\n");
        creat_file(dest_file);
    }
    //open destination file for read write
    int fd_dest = open_file(dest_file, 2);

    if(fd_src == -1 || fd_dest == -1)
    {
        if(fd_dest == -1)
        {
            close_file(fd_dest);
        }
        if(fd_src == -1)
        {
            close_file(fd_src);
        }
        return -1;
    }

    memset(buf, '\0', BLKSIZE);
    while(n = my_read(fd_src, buf, BLKSIZE))
    {
        buf[n] = 0;
        my_write(fd_dest, buf, n);
        memset(buf, '\0', BLKSIZE);
    }

    close_file(fd_src);
    close_file(fd_dest);


    return 0;
}

#endif

