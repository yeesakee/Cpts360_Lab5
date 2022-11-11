int tst_bit(char *buf, int bit) // in Chapter 11.3.1
{
    // int i; 
    // int j;
    // i = bit / 8;
    // j = bit % 8; // 8 = number of bits in a byte
    // if(buf[i] & (1 << j))
    // {
    //     return 1; 
    // }
    // return 0;
    return buf[bit/8] & (1 << (bit % 8));
}

int set_bit(char *buf, int bit) // in Chapter 11.3.1
{
    // int i;
    // int j;
    // i = bit /8;
    // j = bit % 8;
    // buf[i] |= (1 << j);
    buf[bit/8] |= (1 << (bit % 8));
}

int clr_bit(char *buf, int bit) //clear bit in char 
{
    buf[bit/8] &= ~(1 << (bit % 8));
}

int decFreeInodes(int dev)
{
  // dec free inodes count in SUPER and GD
  char buf[BLKSIZE];
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count--;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count--;
  put_block(dev, 2, buf);
}

int ialloc(int dev)  // allocate an inode number from inode_bitmap
{
  int  i;
  char buf[BLKSIZE];

  //use imap, ninodes in mount table of dev
  //MTABLE *mp = (MTABLE *)get_mtable(dev);

  // read inode_bitmap block
  get_block(dev, imap, buf);

  for (i=0; i < ninodes; i++){ // use ninodes from SUPER block
    if (tst_bit(buf, i)==0){
        set_bit(buf, i);
	put_block(dev, imap, buf);

	decFreeInodes(dev);

	printf("allocated ino = %d\n", i+1); // bits count from 0; ino from 1
        return i+1;
    }
  }
  return 0;
}

// WRITE YOUR OWN balloc(dev) function, which allocates a FREE disk block number
int balloc(int dev)
{
  int  i;
  char buf[BLKSIZE];

  // read inode_bitmap block
  get_block(dev, bmap, buf);

  for (i=0; i < nblocks; i++) // use ninodes from SUPER block
  {
      if(tst_bit(buf, i)==0) 
      {
          set_bit(buf, i);
          put_block(dev, bmap, buf);
          decFreeInodes(dev);
          return i +1;

      }
  }
  return 0; 
}

// Finish iput(MINODE *mip) code in util.c (in Chapter 11.7.2 3.3)

//int bdalloc(dev, bno) deallocate a  block number

int incFreeInodes(int dev)
{
  char buf[BLKSIZE];

  // inc free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count++;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, buf);
}

//for remove dir
//int idalloc(dev, ino) deallocate an inode (number)
int idalloc(int dev, int ino)
{
  int i;  
  char buf[BLKSIZE];

  if (ino > ninodes){
    printf("inumber %d out of range\n", ino);
    return -1;
  }

  // get inode bitmap block
  get_block(dev, imap, buf);
  clr_bit(buf, ino-1);

  // write buf back
  put_block(dev, imap, buf);

  // update free inode count in SUPER and GD
  incFreeInodes(dev);
}

//WRITE YOUR bdalloc() function which deallocates a block number
//int bdalloc(dev, bno) deallocate a  block number
int bdalloc(int dev, int bno)
{
  int i;  
  char buf[BLKSIZE];

  // get inode bitmap block
  get_block(dev, bmap, buf);
  clr_bit(buf, bno-1);

  // write buf back
  put_block(dev, bmap, buf);

  // update free inode count in SUPER and GD
  incFreeInodes(dev);
}