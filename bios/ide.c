#include "ide.h"

#define TIMEOUT 900000


static ide_setup(int dev, unsigned int sect, int nr )
{

   WRITE_HD(SECC, nr );
   asm("nop");
   WRITE_HD(SEC, (sect & 0xff) );
   asm("nop");
   WRITE_HD(CYL, (sect >>=8 & 0xff));
   asm("nop");
   WRITE_HD(CYH, (sect >>=8 & 0xff ));
   asm("nop");
   WRITE_HD(HED, ((sect >>8)& 0x0f| 0x40| (dev & 1) <<4) );
   asm("nop");
}

ide_setmulti(int dev,int nr)
{
int out;

out=TIMEOUT;
while((READ_HD( COM) & 0x80) == 0x80) {
                                        out --;
                                        if(!out) break;
                                        }

if(!out) {
                printf("\n\rIDE_setmulti %d: TIMEOUT: %x",__LINE__,READ_HD(COM));
                return (-1);
         }
 

 WRITE_HD(SECC,nr);
 WRITE_HD(HED,0xe0 | dev << 4);
 WRITE_HD(COM,HD_SETMUL_COM);


}
 

 ide_init(int dev)
{


while((READ_HD( COM) & 0x80) == 0x80) ;
ide_setup(dev,0,0);
 WRITE_HD(COM,HD_DIAG_COM);

while((READ_HD( COM) & 0x80) == 0x80) ;
}

ide_readmulti (int dev, unsigned int sect , unsigned short * buf,int nr)
{
 
 int a;
 int out;
 
 out=TIMEOUT;
 while((READ_HD( COM) & 0x80) == 0x80) {
					out --;
					if(!out) break;
					}
 
if(!out) { 
		printf("\n\rIDE_READ %d: TIMEOUT: %x",__LINE__,READ_HD(COM));
		return (-1);
         }
  ide_setup(dev, sect, nr);
  WRITE_HD(COM, HD_READM_COM);

//		printf("\n\rIDE_READ %d: TIMEOUT: %x",__LINE__,READ_HD(COM));
 out=TIMEOUT;
 while((READ_HD( COM) & 0x08) == 0) {
					out--;
					if(!out) break;
					}
 
if(!out) { 
		printf("\n\rIDE_READ %d: TIMEOUT: %x",__LINE__,READ_HD(COM));
	          if((READ_HD(COM) &1 ) == 1) 
			 printf("\n\r Error %x ",READ_HD( ERR) );
		return (-1);
         }
for(a=0;a<(32*nr);a++)
{
*buf++=READ_HDDATA(DATA);
*buf++=READ_HDDATA(DATA);
*buf++=READ_HDDATA(DATA);
*buf++=READ_HDDATA(DATA);
/* 4*/
*buf++=READ_HDDATA(DATA);
*buf++=READ_HDDATA(DATA);
*buf++=READ_HDDATA(DATA);
*buf++=READ_HDDATA(DATA);
}
}


ide_writemulti(int dev, unsigned int sect , unsigned short * buf,int nr)
{
 int a,b; 
 int out;

 out=TIMEOUT;
 while((READ_HD( COM) & 0x80) == 0x80) {
					out--;
					if(!out) break;
					}
 
if(!out) { 
		printf("\n\rIDE_WRITE %d: TIMEOUT: %x",__LINE__,READ_HD(COM));
		return (-1);
		}
 ide_setup(dev,sect,nr );
 WRITE_HD(COM,HD_WRITEM_COM);
//		printf("\n\rIDE_WRITE %d: %d TIMEOUT: %x",__LINE__,out,READ_HD(COM));

 out=TIMEOUT;
 while((READ_HD( COM) & 0x08) == 0) { 
					out--;
					if(!out) break;
 					}
if(!out) { 
		printf("\n\rIDE_WRITE %d: %d TIMEOUT: %x",__LINE__,out,READ_HD(COM));
                 printf("\n\r Error %x \n\r",READ_HD( ERR) );
		return (-1);
		}
for(a=0;a<32*nr;a++)
{
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
/*4 */
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
}

}
  

ide_read( int dev, unsigned int sect , unsigned short * buf)
{
 int a;

 while((READ_HD( COM) & 0x80) == 0x80) ;
  ide_setup(dev, sect, 1);
  WRITE_HD(COM, HD_READ_COM);
 while((READ_HD( COM) & 0x08) == 0);

for(a=0;a<256;a++)
*buf++=READ_HDDATA(DATA);
}


ide_write(int dev, unsigned int sect , unsigned short * buf)
{
 int a,b; 
 int out;
 out=1000;
 while((READ_HD( COM) & 0x80) == 0x80) {
					out--;
					if(!out) break;
					}
 
if(!out) { 
		printf("\n\rIDE_WRITE: TIMEOUT: %x",READ_HD(COM));
		return (-1);
		}
 ide_setup(dev,sect,1 );
 WRITE_HD(COM,HD_WRITE_COM);
 while((READ_HD( COM) & 0x08) == 0);

for(a=0;a<32;a++)
{
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
/*4 */
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
WRITE_HDDATA(DATA,*buf++);
}

}

ide_ident( int dev, unsigned int sect , unsigned short * buf)
{
 int a;
 int out;

 while((READ_HD( COM) & 0x80) == 0x80) ;
  ide_setup(dev, sect, 1);
  WRITE_HD(COM, HD_IDENT_COM);
 out=TIMEOUT;
 while((READ_HD( COM) & 0x08) == 0) {
					out --;
					if (!out ) break;
					}
 if(!out)
  	{
		printf("\n\r %d: TIMEOUT: %x",__LINE__,READ_HD(COM));
		 printf("\n\r Error %x ",READ_HD( ERR) );
		return (-1);
         }

for(a=0;a<256;a++)
*buf++=READ_HDDATA(DATA);
}


		
  
#define COUNT 8


#ifdef TESTING
idemain()
{
 unsigned int a,b,c;
 char buffer[4096];
 unsigned int buf1[1024];
 unsigned char buf2[1024];
 unsigned long cap;
 char dev=0;

for(b=0;b<1024;b++) buf1[b]=b; 

//ide_init(0);
 
 printf("\n\r%cStart Testing ",0x0c);
 printf("\n\rRead / Write\n\r ");
 printf("\n\r Error %x ",READ_HD( ERR) );
 printf("\n\r COM %x ",READ_HD( COM) );
 printf("\n\r DATA %x ",READ_HDDATA( DATA ) );
 ide_ident(dev,0,(unsigned short *)&buf2[0]);
printf("\n\rCAP: %x:%x ",buf2[(49*2)],buf2[(49*2)+1]);
printf("\n\rsec: %x %x ",buf2[(57*2)],buf2[(57*2)+1]);
printf("\n\rsec: %x %x ",buf2[(58*2)],buf2[(58*2)+1]);
cap = (buf2[(58*2)+1] << 24) | (buf2[(58*2)] << 16) | ( buf2[(57*2)+1] <<8 ) |buf2[(57*2)];
printf("\n\r Capacity %x %d sectors",cap,cap);
     dump(&buf2[0]);
 ide_setmulti(dev, COUNT);


#if 0
 for(a=0;a<10000;a+=COUNT)
 {
if( !(a & 0x3ff))
 printf("\r\nWrite: %d",a);
  ide_writemulti(dev,a,(unsigned short *) &buf1[0],COUNT );
//  ide_writemulti(dev,10000000-a,(unsigned short *) &buf1[0],COUNT );
 }

#endif

 for(a=0;a<10000;a+=COUNT)
 {
#ifdef TEST
for(b=0;b<1024;b++) buf2[b]=0;
#endif

if( !(a & 0x3ff))
 printf("\rRead : %d",a);
  ide_readmulti(dev,a,(unsigned short *) &buf2[0],COUNT );
//  ide_readmulti(dev,10000000-a,(unsigned short *) &buf2[0],COUNT );
#ifdef TEST
 for(b=0;b<1024;b++) 
  if(buf1[b] != buf2[b] )
  {
    printf("\r\n Error @: %d",a);
  }
#endif

 }
return ;

 while(1)
   {
     int cnt;
     
     cnt=hex32();
     ide_readmulti(dev,cnt,(unsigned short *) &buf2[0],COUNT );
     dump(&buf2[0]);

   }

} 



dump(char *ptr)
{

  int a,b,c;
  unsigned char ch;
  char *ptr1;
  
  ptr1=ptr;
  for(a=0;a<256;a++)
    {
      printf("%03x :",a);
      for(b=0;b<16;b++)
	printf("%02x ",( *(ptr1+b) &255));
      printf("  ");
      for(b=0;b<16;b++)
	{
	  ch=*(ptr1+b);
	  if((ch < 0x20) || (ch > 0x7f)) ch='?';
	  printf("%c",ch);
	}
      printf("\n\r");
      ptr1+=16;
    }
}

#endif 


#if 0     
hexo(int a)
{
/*
  int a;
  a=chbi()& 0x7f;
*/
  printf("%c",a);
  if(a > '9') a-=0x37;
    else a-=0x30;
 return a;
}

hex32()
{
 int a,b,c;
 b=0;
 for(a=0;a<8;a++)
  {
    b=b<<4; 
     c=_chbi();
     if(c == '\n') return b;
     c=hexo(c);
    b|=(c &0xf);
   }
 return b;
}

#endif
