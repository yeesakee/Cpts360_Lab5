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
    int count = 0; 
    OFT *oftp = running->fd[fd];
    MINODE *mip = oftp->mptr; 
    INODE *ip = &mip->INODE; 


    while(nbytes > 0)
    {
        //compute logical blk
        int lbk = oftp->offset / BLKSIZE;
        //compute start byte
        int start = oftp->offset % BLKSIZE;

        //write direct data blocks
        if(lbk < 12)
        {
            if(ip->i_block[lbk] == 0)
            {
                ip->i_block[lbk] = balloc(mip->dev);
            }
            int blk = ip->i_block[lbk];
        }
        //indirect blocks
        else if(lbk >= 12 && lbk < 256 +12)
        {
            if(ip->i_block[12] == 0)
            {
                //allocate a block 
                //zero out block on disk
                int block12 = ip->i_block[12] = balloc(mip->dev);

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
            int ibuf[BLKSIZE / sizeof(int)] = { 0 };
            get_block(mip->dev, ip->i_block[12], (char *)ibuf);
            blk = ibuf[lbk - 12];

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

        }
    }
    return 1; // Eventually: return the number of bytes written
}

int my_cp(char *pathname)
{
    return 1;
}

