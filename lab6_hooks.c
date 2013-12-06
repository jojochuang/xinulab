
#include "xinu.h"
extern int TEST;
extern unsigned int TEST5_PAGE;
extern unsigned char* TEST5_PTR;
extern unsigned char* TEST6_PTR;
extern int TEST5_pagefault;
extern int TEST6_pagefault;

void hook_ptable_create(unsigned int pagenum){
  /*switch( TEST ){
    case 5:
        kprintf("GRADING : Test 5 creates a new page table at frame %d as expected\r\n", pagenum );
    case 6:
        kprintf("GRADING : Test 6 creates a new page table at frame %d as expected\r\n", pagenum );

      break;
  }*/
}
void hook_ptable_delete(unsigned int pagenum){
/*
  switch( TEST ){
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        kprintf("GRADING : page table %d is unexpectedly deleted in test %d \r\n", pagenum, TEST );
        break;
    case 51:
        kprintf("GRADING : page table %d is deleted in test %d xmunmap() \r\n", pagenum, TEST );
        break;
    case 6:
        kprintf("GRADING : page table %d is unexpectedly deleted in test %d \r\n", pagenum, TEST );
        break;
    case 61:
        break;
    case 62:
        kprintf("GRADING : page table %d is deleted in test %d xmunmap() \r\n", pagenum, TEST );
        break;
  }
  */
}
void hook_pfault(char *addr){
/*
  switch( TEST ){
    case 5:
      if( addr == TEST5_PTR ){
        kprintf("GRADING : Test 5 cause page fault at address: 0x%0X as expected\r\n",  addr);
        TEST5_pagefault = 1;
      }else{
        TEST5_pagefault = 1;
        kprintf("GRADING : Test 5 cause page fault at unexpected address: expect 0x%0X but instead 0x%0X\r\n", TEST5_PTR, addr);
      }
      break;
    case 6:
      if( addr == TEST6_PTR ){
        kprintf("GRADING : Test 6 cause page fault at address: 0x%0X as expected\r\n",  addr);
        TEST6_pagefault = 1;
      }else{
        TEST6_pagefault = 1;
        kprintf("GRADING : Test 6 cause page fault at unexpected address: expect 0x%0X but instead 0x%0X\r\n", TEST6_PTR, addr);
      }
      break;
    case 61:
      kprintf("GRADING : Test 6 failed because the new process accessing the memory location should not trigger a page fault.\r\n");
      break;
    case 12:
      break;
    case 13:
      break;
    default:
      kprintf("page fault at an unknown test!\r\n");
      break;
  }
  */
}
extern int fifo_swap;
extern int ourpolicy_swap;
void hook_pswap_out(unsigned int pagenum, int framenb){
  switch( TEST ){
    /*case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        kprintf("GRADING : page %d is unexpectedly swapped out in test %d \r\n", pagenum, TEST );
        break;
        */
    case 10:
        /** FIFO policy
         * */
        kprintf("GRADING : page %d is swapped out in test %d \r\n", pagenum, TEST );
        fifo_swap++;
        break;
    case 11:
        /** OURPOLICY
         *  it should have fewer page-outs than FIFO
         * */
        kprintf("GRADING : page %d is swapped out in test %d \r\n", pagenum, TEST );
        ourpolicy_swap++;
        break;
    /*case 12:
        kprintf("GRADING : page %d is swapped out in test %d \r\n", pagenum, TEST );
        
        break;
    case 13:
        kprintf("GRADING : page %d is swapped out in test %d \r\n", pagenum, TEST );
        break;*/

  }

}

