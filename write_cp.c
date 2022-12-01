/************* write_cp.c file **************/

//#include "read_cat.c"
#include "link_unlink.c"

int write_file(int fd, char *buf)
{

    return 1; // Eventually: return the results of my_write
}

int my_write(int fd, char buf[], int nbytes)
{
    char ibuf[BLKSIZE];
    char kbuf[BLKSIZE];
    char dbuf[BLKSIZE];
    int blk, remain; 
    int count = 0; 
    OFT *oftp = running->fd[fd];
    MINODE *mip = oftp->minodePtr; 
    INODE *ip = &mip->INODE; 

    printf("Writing to ino = %d\n", mip->ino);
    int numBytes = nbytes;

    while(nbytes > 0)
    {
        //compute logical blk
        int lbk = oftp->offset / BLKSIZE;
        //compute start byte
        int startByte = oftp->offset % BLKSIZE;

        //convert lbk to physical block number
        
        //write direct data blocks
        if(lbk < 12)
        {
            if(mip->INODE.i_block[lbk] == 0)
            {
                mip->INODE.i_block[lbk] = balloc(mip->dev); //allocate a block
            }
            blk = mip->INODE.i_block[lbk];  //blk shoud be a disk block now
            printf("mip->dev: %d\n", mip->dev);
        }
        //indirect blocks
        else if(lbk >= 12 && lbk < 256 +12)
        {
            //char ibuf[256]; 
            if(ip->i_block[12] == 0)
            {
                //allocate a block 
                mip->INODE.i_block[12] = balloc(mip->dev);
                //zero out block on disk
                int block12 = mip->INODE.i_block[12];

                if(block12 == 0)
                {
                    return 0; 
                }
                //get iblock 12 into an int buf
                get_block(mip->dev, mip->INODE.i_block[12], (char *)ibuf);
                int *pointer = (int *)ibuf;
                for(int i = 0; i < 256; i++)
                {
                    if(ibuf[i] == 0)
                    {
                        ibuf[i] = balloc(mip->dev);
                        blk = ibuf[i];
                        put_block(mip->dev, mip->INODE.i_block[12], (char *)ibuf);
                        break;
                    }
                   // pointer[i] = 0; 
                }

                put_block(mip->dev, ip->i_block[12], ibuf);
                mip->INODE.i_blocks++;
            }
            int ibuf[BLKSIZE / sizeof(int)] = { 0 };
            get_block(mip->dev, ip->i_block[12], (char *)ibuf);
            int blk = ibuf[lbk - 12];

            if(blk == 0)
            {
                //allocate a disk block 
                //record it in Iblock[12]
                blk = ibuf[lbk -12] = balloc(mip->dev);
                ip->i_blocks++;
                put_block(mip->dev, ip->i_block[12], (char *)ibuf);
            }
        }
        else
        //double indirect blocks
        {
            get_block(mip->dev, mip->INODE.i_block[13], (char *)ibuf);

            for(int i = 0; i <256; i++)
            {
                if(ibuf[i])
                {
                    get_block(mip->dev, ibuf, (char *)dbuf);
                    for(int j = 0; j < 256; j++)
                    {
                        if(dbuf[j] == 0)
                        {
                            dbuf[j] = balloc(mip->dev);
                            blk = dbuf[j];
                            put_block(mip->dev, ibuf[i], (char *)dbuf);
                            break;
                        }
                    }
                }
            }
        }
        get_block(mip->dev, blk, kbuf);
        char *cp = kbuf + startByte; 
        remain = BLKSIZE - startByte;

        while(remain > 0)
        {
            *cp++ = *buf++; 
            oftp->offset++; 
            count++; 
            remain--; 
            nbytes--; 
            if(oftp->offset > mip->INODE.i_size)
                mip->INODE.i_size++; 
            if(nbytes <= 0) 
                break; 
        }
        put_block(mip->dev, blk, kbuf);
    }
    mip->dirty = -1; 
    printf("wrote %d char into fd= %d\n", nbytes, fd);
    printf("size = %d offset = %d\n", mip->INODE.i_size, oftp->offset);
    return nbytes; // Eventually: return the number of bytes written
}

int my_cp(char *src_file, char *dest_file)
{
    return 1;
}

