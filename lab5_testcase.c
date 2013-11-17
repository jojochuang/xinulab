#include <xinu.h>



/*#ifdef NFRAMES
        #undef NFRAMES
        #define NFRAMES 25
#endif
*/

extern int get_faults();
#define PAGESIZE  4096
unsigned long cr0val;
unsigned char* TEST5_PTR;
unsigned char* TEST6_PTR;
unsigned int TEST5_PAGE = 5020;
unsigned int TEST6_PAGE = 5025;
int TEST5_pagefault = 0;
int TEST6_pagefault = 0;


int TEST = -1;

void out_of_seq_calls(pid32 parent, bsd_t store){
        if(deallocate_bs(store) != SYSERR){
                kprintf("%s : deallocate_bs should have failed but went through\r\n", __FUNCTION__);
                send(parent, SYSERR);
                receive();
        }
        kprintf("%s : Sending OK to parent\r\n");
        send(parent, OK);
        receive();
}

void return_memory_value( pid32 parent, uint32 addr){
        // TODO: the memory access should not cause page fault
        kprintf("%s : Sending memory value [%d] to parent\r\n", *((int32 *)(addr)));
        send(parent, *((int32 *)(addr)));
        receive();
}
void xmmap_and_read(pid32 parent, bsd_t store, int num_pages, int start_page, uint32 addr){
        int val = -1;
        bsd_t temp_store = -1;

        kprintf(" %s : GRADING : Mapping and trying to read again\r\n", __FUNCTION__);
        while(1){
                temp_store = allocate_bs(100);
                if(temp_store == store)
                        break;
                if(temp_store == SYSERR){
                        kprintf("%s : GRADING : Test 8 Failed could not allocate backing store\r\n", __FUNCTION__);
                        send(parent, SYSERR);
                        receive();
                }
                deallocate_bs(temp_store);
        }

        if(open_bs(store) == SYSERR){
                kprintf("GRADING : Test 8-open Failed\r\n");
                send(parent, SYSERR);
                receive();
        }
        if(xmmap(start_page, num_pages, MAP_SHARED, store) == SYSERR){
                kprintf("GRADING : Test 8-xmmap Failed\r\n");
                close_bs(store);
                deallocate_bs(store);
                send(parent, SYSERR);
                receive();
        }

        kprintf("%s : GRADING : Test 8 Accessing the memory should be 17290: %d\r\n", __FUNCTION__, *((int32 *) addr));
        val = *((int32 *)(addr));
        
        if(xmunmap(start_page) == SYSERR){
                kprintf("%s : GRADING : Test 8-xmunmap failed, sending SYSERR\r\n", __FUNCTION__);
                close_bs(store);
                deallocate_bs(store);
                send(parent, SYSERR);
                receive();
        }
        if(close_bs(store) == SYSERR){
                kprintf("%s : GRADING : Test 8-close failed, sending SYSERR\r\n", __FUNCTION__);
                deallocate_bs(store);
                send(parent, SYSERR);
                receive();
        }
        if(deallocate_bs(store) == SYSERR){
                kprintf("%s : GRADING : Test 8-deallocate failed, sending SYSERR\r\n", __FUNCTION__);
                send(parent, SYSERR);
                receive();
        }    

        kprintf("%s : GRADING : Test 8 sending [%d] to parent\r\n", __FUNCTION__,  val);
        send(parent, val);
        receive();

}

void invalid_memory_access(void){


        int *ptr = (int *)(5000*4096);
        kprintf("%s : GRADING : Trying to access invalid memory [%u] : %d\r\n", __FUNCTION__, ptr, *ptr);
        while(1);
}

void xmmap_and_wait_for_kill(pid32 parent){
        bsd_t store ;
        store = allocate_bs(10);
        if(store == SYSERR){
                kprintf("%s : GRADING : Test 11 Allocate failed\r\n", __FUNCTION__);
                send(parent, SYSERR);
                receive();
        }
        if(open_bs(store) == SYSERR){
                kprintf("%s : GRADING : Test 11 Open failed\r\n", __FUNCTION__);
                send(parent, SYSERR);
                receive();
        }
        if(xmmap(5000, 10, MAP_SHARED, store) == SYSERR){
                kprintf("%s : GRADING : Test 11 Xmmap failed\r\n", __FUNCTION__);
                send(parent, SYSERR);
                receive();
        }
        kprintf("%s : GRADING : Test 11 Sending store id [%d] to parent\r\n", __FUNCTION__, store);
        send(parent, store);
        recvclr();
        receive();
}

int main(int argc, char **argv){

        bsd_t store_id[8] = {0};
        bsd_t local_store;
        pid32 pid;
        int i=0;
        int j=0;
        int flag = 0;
        int *ptr;
        umsg32 message;

        psinit();

        goto test6;
test0:
        /**
         * Make sure paging is enabled
         *
         * */
        asm("pushl %eax");
        asm("movl %cr0, %eax");
        asm("movl %eax, cr0val");
        asm("popl %eax");

        if( cr0val & ( 1<<31 ) == 1 ){
          kprintf("GRADING : Test 0-1 Failed: paging not enabled\r\n");
        }else{
          kprintf("GRADING : Test 0-1 Successful\r\n");
        }

        /**
         * Make sure access to the lower 16MB don't trigger page fault
         *
         * */
        unsigned long* addr_ptr;
        unsigned int pg;

        unsigned int nfaults = get_faults();

        if( nfaults == 0 ){
          kprintf("GRADING : Test 0-2 Successful\r\n");
        }else{
          kprintf("GRADING : Test 0-2 Failed: page fault already occured\r\n");
        }

        for(pg=0;pg<4096;pg++){
          addr_ptr = pg * PAGESIZE + pg;

          unsigned long v = *addr_ptr;
        }


        nfaults = get_faults();

        if( nfaults == 0 ){
          kprintf("GRADING : Test 0-3 Successful\r\n");
        }else{
          kprintf("GRADING : Test 0-3 Failed: access to the lower 16 mb should not trigger page faults!\r\n");
        }

        /** 
         * Make sure srpolicy() system call is implemented
         *
         * */
        if( FIFO == 3 ){
          kprintf("GRADING : Test 0-4 Successful\r\n");
        }else{
          kprintf("GRADING : Test 0-4 Failed: FIFO is not defined as 3!\r\n");
        }

        if( srpolicy( FIFO ) == SYSERR ){
          kprintf("GRADING : Test 0-5 Failed: can not install FIFO replacement policy!\r\n");
        }else{
          kprintf("GRADING : Test 0-5 Successful\r\n");
        }

test1:
        /*
         * Test case 1 : 
         * Make sure allocate fails beyond 8 mappings
         */
        for(i=0; i< 8; i++){
                allocate_bs(10);
        }
        if(allocate_bs(10) == SYSERR){
                kprintf("GRADING : Test 1 Successfull\r\n");
        }
        else{
                kprintf("GRADING : Test 1 Failed\r\n");
        }
        for(i=0; i< 8; i++){
                deallocate_bs(i);
                store_id[i] = -1;
        }
test2:
        /*
         * Test case 2 :
         * Make sure out of order open and close fail
         */
        flag = 0;
        if(open_bs(6) != SYSERR){
                kprintf("GRADING : Test 2-1 Failed\r\n");
                flag++;
        }
        if(close_bs(1) != SYSERR){
                kprintf("GRADING : Test 2-2 Failed\r\n");
                flag++;
        }
        if(deallocate_bs(4) != SYSERR){
                kprintf("GRADING : Test 2-3 Failed\r\n");
                flag++;
        }
        if(flag == 0){
                kprintf("GRADING  : Test 2 Successfull\r\n");
        }
test3:


        /*
         * Test case 3 : 
         * Make sure I can only call allocate/open/close/deallocate on a backing store
         * ie. Make sure all the calls are allowed only if they happen in a single process
         */
        /*store_id[0] = allocate_bs(10);
        recvclr();
        pid  = create(out_of_seq_calls, 2048, 30, "Out of Sequence Calls", 2, currpid, store_id[0]);
        resume(pid);
        if(receive() == OK){
                kprintf("GRADING : Test 3 succesfull\r\n");
        }
        else{
                kprintf("GRADING : Test 3 Failed\r\n");
        }
        //kill(pid);
        open_bs(store_id[0]);
        close_bs(store_id[0]);
        deallocate_bs(store_id[0]);
        */
test4:
        /*
         * Test case 4 :
         * Make sure xmmap works only on a store already allocated
         */
        flag = 0;
        if(xmmap(5000, 200, MAP_PRIVATE, 2) != SYSERR){
                kprintf("GRADING : Test 4-1 Failed\r\n");
                flag++;
        }
        if(xmunmap(5000) != SYSERR){
                kprintf("GRADING : Test 4-2 Failed\r\n");
                flag++;
        }
        if(flag == 0){
                kprintf("GRADING : Test 4 Successfull\r\n");
        }
        store_id[0] = -1;
test5:
        /*
         * Test case 5 : 
         * Make sure you can access mapped address/write
         */
        store_id[0] = allocate_bs(100);
        if(store_id[0] == SYSERR){
                kprintf("GRADING : Test 5-allocate Failed\r\n");
                goto test6;
        }
        if(open_bs(store_id[0]) == SYSERR){
                kprintf("GRADING : Test 5-open Failed\r\n");
                goto test6;
        }
        if(xmmap(TEST5_PAGE, 100, MAP_SHARED, store_id[0])== SYSERR){
                kprintf("GRADING : Test 5-xmmap Failed\r\n");
                close_bs(store_id[0]);
                deallocate_bs(store_id[0]);
                goto test6;
        }

        ptr = (int *)(TEST5_PAGE*4096 + 10);
        TEST5_PTR = (unsigned char*) ptr;
        kprintf("GRADING : Test 5 Accessing the memory : 0x%0X\r\n", (int32 *) ptr);
        TEST=5;
        *ptr = 1729;
        // verify a page fault occurs at here
        //kprintf("GRADING : Test 5 Accessing the memory after write : %d\r\n", *((int32 *) ptr));

        if( TEST5_pagefault == 0 ){
          kprintf("GRADING : Test 5 failed b/c page fault did not occur\r\n");
        }

        // verify the next access does not trigger a page fault
        TEST5_pagefault = 0;
        ptr = (int *)(TEST5_PAGE*4096 + 201);
        *ptr = 1999;
        if( TEST5_pagefault == 1 ){
          kprintf("GRADING : Test 5 failed b/c page fault should not occur but it did\r\n");
        }
        TEST=51;

        if(xmunmap(TEST5_PAGE) == SYSERR){
                kprintf("GRADING : Test 5-xmunmap failed\r\n");
                close_bs(store_id[0]);
                deallocate_bs(store_id[0]);
                goto test6;
        }
        if(close_bs(store_id[0]) == SYSERR){
                kprintf("GRADING : Test 5-close Failed\r\n");
                deallocate_bs(store_id[0]);
                goto test6;
        }
        if(deallocate_bs(store_id[0]) == SYSERR){
                kprintf("GRADING : Test 5-deallocate Failed\r\n");
                goto test6;
        }    
        store_id[0] = -1;
        kprintf("GRADING : Test 5 Successfull\r\n");

test6:
        TEST=6;
        /*
         * Test case 6 : 
         * Make sure another process can read the memory I have written
         * ie. Check all tables are global
         */
        flag = 0;
        store_id[1] = allocate_bs(100);
        if(store_id[1] == SYSERR){
                kprintf("GRADING : Test 6-allocate Failed\r\n");
                goto test7;
        }
        if(open_bs(store_id[1]) == SYSERR){
                kprintf("GRADING : Test 6-open Failed\r\n");
                goto test7;
        }
        if(xmmap(TEST6_PAGE, 100, MAP_SHARED, store_id[1])== SYSERR){
                kprintf("GRADING : Test 6-xmmap Failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                goto test7;
        }

        ptr = (int *)(TEST6_PAGE*4096+150);
        TEST6_PTR = (unsigned char*) ptr;
        //kprintf("GRADING : Test 6 Accessing the memory : %d\r\n", *((int32 *) ptr));
        *ptr = 1729;
        if( TEST6_pagefault == 0 ){
          kprintf("GRADING : Test 6 failed b/c page fault did not occur\r\n");
        }

        kprintf("GRADING : Spawning another process to access the memory\r\n");
        recvclr();
        TEST = 61;
        pid = create(return_memory_value, 2048, 30, "Return Memory Value", 2, currpid, TEST6_PAGE*4096+150);
        resume(pid);
        message = receive();
        if(message != 1729){
                kprintf("GRADING : Test 6 Failed while another process tried to read\r\n");
                flag++;
        }
        else{
                kprintf("GRADING  : Test 6 received the message : %d from return_memory_value\r\n", message);
        }
        kprintf("GRADING : Test 6 Accessing the memory after write : %d\r\n", *((int32 *) ptr));

        TEST = 62;
        if(xmunmap(TEST6_PAGE) == SYSERR){
                kprintf("GRADING : Test 6-xmunmap failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                goto test7;
        }
        if(close_bs(store_id[1]) == SYSERR){
                kprintf("GRADING : Test 6-close Failed\r\n");
                deallocate_bs(store_id[1]);
                goto test7;
        }
        if(deallocate_bs(store_id[1]) == SYSERR){
                kprintf("GRADING : Test 6-deallocate Failed\r\n");
                goto test7;
        }    
        if(flag == 0){
                kprintf("GRADING : Test 6 Successfull\r\n");
        }

test7:
        TEST = 7;
        /*
         * Test case 7 : 
         * Make sure xmmap/write/xunmap/xmmap/read works
         */
        flag = 0;
        store_id[2] = -1;
        store_id[3] = -1;
        store_id[2] = allocate_bs(100);
        if(store_id[2] == SYSERR){
                kprintf("GRADING : Test 7-allocate Failed\r\n");
                goto test8;
        }
        if(open_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 7-open Failed\r\n");
                goto test8;
        }
        if(xmmap(6000, 100, MAP_SHARED, store_id[2])== SYSERR){
                kprintf("GRADING : Test 7-xmmap Failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test8;
        }

        ptr = (int *)(6080*4096);
        kprintf("GRADING : Test 7 Accessing the memory : %d\r\n", *((int32 *) ptr));
        *ptr = 17290;
        kprintf("GRADING : Test 7 Accessing the memory after write : %d\r\n", *((int32 *) ptr));

        if(xmunmap(6000) == SYSERR){
                kprintf("GRADING : Test 7-xmunmap failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test8;
        }
        if(close_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 7-close Failed\r\n");
                deallocate_bs(store_id[2]);
                goto test8;
        }
        kprintf("GRADING : Mapping and trying to read again\r\n");
        if(open_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 7-againopen Failed\r\n");
                goto test8;
        }
        if(xmmap(6000, 100, MAP_SHARED, store_id[2])== SYSERR){
                kprintf("GRADING : Test 7-againxmmap Failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test8;
        }

        ptr =(int *)( 6080*4096);
        kprintf("GRADING : Test 7 Accessing the memory should be 17290: %d\r\n", *((int32 *) ptr));
        if(*((int32 *)(ptr)) != 17290){
                kprintf("GRADING : Test 7 failed, memory value not persistent\r\n");
                flag++;
        }
        if(xmunmap(6000) == SYSERR){
                kprintf("GRADING : Test 7-againxmunmap failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test8;
        }
        if(close_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 7-againclose Failed\r\n");
                deallocate_bs(store_id[2]);
                goto test8;
        }

        if(deallocate_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 7-deallocate Failed\r\n");
                goto test8;
        }    
        if(flag == 0){
                kprintf("GRADING : Test 7 Successfull\r\n");
        }
test8:

        /*
         * Test case 8 : 
         * Make sure xmmap/write/xunmap and xmmap/read/xunmap in another process works
         * with the same backing store id
         */
        flag = 0;
        store_id[2] = -1;
        store_id[3] = -1;
        store_id[2] = allocate_bs(100);
        if(store_id[2] == SYSERR){
                kprintf("GRADING : Test 8-allocate Failed\r\n");
                goto test9;
        }
        if(open_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 8-open Failed\r\n");
                goto test9;
        }
        if(xmmap(7000, 100, MAP_SHARED, store_id[2])== SYSERR){
                kprintf("GRADING : Test 8-xmmap Failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test9;
        }

        ptr = (int *)(7050*4096);
        kprintf("GRADING : Test 8 Accessing the memory : %d\r\n", *((int32 *) ptr));
        *ptr = 17290;
        kprintf("GRADING : Test 8 Accessing the memory after write : %d\r\n", *((int32 *) ptr));

        if(xmunmap(7000) == SYSERR){
                kprintf("GRADING : Test 8-xmunmap failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test9;
        }
        if(close_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 8-close Failed\r\n");
                deallocate_bs(store_id[2]);
                goto test9;
        }
        if(deallocate_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 8-deallocate Failed\r\n");
                goto test9;
        }    
        recvclr();
        kprintf("GRADING : Test 8 Creating another process to map the range and return value\r\n");
        pid = create(xmmap_and_read, 2048, 30, "xmmap and read", 5, currpid, store_id[2], 100, 7000, 7050*4096);
        resume(pid);
        message = receive();
        if(message != 17290){
                kprintf("GRADING : Test 8 Failed , the other process did not read the correct value\r\n");
                flag++;
        }
        else{
                kprintf("GRADING : Test 8 received the value [%d] from xmmap_and_read\r\n",message);
        }
        //kill(pid);
        if(flag == 0){
                kprintf("GRADING : Test 8 Successfull\r\n");
        }

test9:
        /*
         * Test case 9
         * Make sure xmmap across intersecting ranges fails, xmmap on the same backing store twice fails
         */
        store_id[0] = allocate_bs(100);
        store_id[1] = allocate_bs(100);
        if(store_id[0] == SYSERR || store_id[1] == SYSERR){
                kprintf("GRADING : Test 9 Could not allocate backing stores\r\n");
                goto test10;
        }
        if(open_bs(store_id[0]) == SYSERR || open_bs(store_id[1]) == SYSERR){
                kprintf("GRADING : Test 9 Could not open backing stores\r\n");
                close_bs(store_id[0]);
                close_bs(store_id[1]);
                deallocate_bs(store_id[0]);
                deallocate_bs(store_id[1]);
                goto test10;
        }
        if(xmmap(10000, 100, MAP_SHARED, store_id[0]) == SYSERR){
                kprintf("GRADING : Test 9 Could not xmmap backing stores\r\n");
                close_bs(store_id[0]);
                close_bs(store_id[1]);
                deallocate_bs(store_id[0]);
                deallocate_bs(store_id[1]);
                goto test10;
        }
        if(xmmap(10050, 100, MAP_SHARED, store_id[1]) != SYSERR){
                kprintf("GRADING : Test 9 Mapping should have failed but succeeded\r\n");
                xmunmap(10050);
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                xmunmap(10000);
                close_bs(store_id[0]);
                deallocate_bs(store_id[0]);
                goto test10;
        }
        else{
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                xmunmap(10000);
                close_bs(store_id[0]);
                deallocate_bs(store_id[0]);
                kprintf("GRADING : Test 9 Successfull\r\n");
        }


test10:
        /*
         * Test 10
         * Make sure invalid memory access kills() the process
         */
        recvclr();
        pid = create(invalid_memory_access, 2048, 40, "Invalid Memory Access", 0);
        resume(pid);
        receive();
        if(kill(pid) != SYSERR){
                kprintf("GRADING : Test 10 Failed  The process [%d] should have been already killed\r\n");
        }
        else{
                kprintf("GRADING : Test 10 Successfull\r\n");
        }
test11:
        /*
         * Test case 11
         * Make sure kill() cleans up mappings and deallocates
         */
        /*recvclr();
        pid = create(xmmap_and_wait_for_kill, 2048, 30, "XMMAP and wait for kill", 1, currpid);
        resume(pid);
        local_store= receive();
        if(local_store == SYSERR){
                kprintf("GRADING : Test 11 Failed the process returned SYSERR\r\n");
                goto test12;
        }
        kill(pid);
        i=0;
        for(i=0; i< 8; i++){
                store_id[i] = allocate_bs(10);
                if(store_id[i] == SYSERR){
                        kprintf("GRADING : Test 11 allocation failed before 8 iterations i=[%d]\r\n", i);
                        break;
                }
                printf("GRADING : Test 11 :The store ID is %d\r\n", store_id[i]);
                if(store_id[i] == local_store){
                        break;
                }
        }
        if(i < 8 && store_id[i] == local_store ){
                kprintf("GRADING : Test 11 Successfull, killing the process deallocated the backing store\r\n");
                for(j=0; j < 8; j++){
                        open_bs(store_id[j]);
                        close_bs(store_id[j]);
                        deallocate_bs(store_id[j]);
                }
        }
        else{
                kprintf("GRADING : Test 11 Failed, we could not allocate the store again in 8 iterations\r\n");
                for(j=0; j < 8; j++){
                        open_bs(store_id[j]);
                        close_bs(store_id[j]);
                        deallocate_bs(store_id[j]);
                }
        }

        return OK;
        */

test12:
        /*
         * Test case 12
         * Stress test xmmap, check with max 200 pages
         * and access each page
         */
        TEST=12;
        for(i=0; i< 8; i++){
                close_bs(i);
                deallocate_bs(i);
                store_id[i] = -1;
        }

        for(i=1; i< 8; i++){
                store_id[i] = allocate_bs(200);
                if(store_id[i] == SYSERR){
                        kprintf("GRADING : Test 12 : Allocation failed in iteration [%d]\r\n", i);
                        goto test12_cleanup;
                }
                if(open_bs(store_id[i]) == SYSERR){
                        kprintf("GRADING : Test 12 : Open failed in iteration [%d]\r\n", i);
                        goto test12_cleanup;
                }
                if(xmmap(5000+i*1000, 200, MAP_SHARED, store_id[i]) == SYSERR){
                        kprintf("GRADING : Test 12 : Xmmap failed in iteration [%d]\r\n", i);
                        goto test12_cleanup;
                }
                for(j=0; j < 200; j+=2){
                        kprintf("\rTest 12 : page [%d] addr [%u] val [%d]\r", 
                                        5000+i*1000+(j),
                                        (5000+i*1000)*4096+j*4096,
                                        *((int32 *)((5000+i*1000)*4096+j*4096)));
                }
                kprintf("\r\n");
        }
test12_cleanup:
        kprintf("\n\r GRADING : Test 12 : Stress Testing done, now going ahead and cleaning things\r\n");
        for(i=1; i<8; i++){
                xmunmap(5000+i*1000);
                close_bs(store_id[i]);
                deallocate_bs(store_id[i]);
                store_id[i] = -1;
        }
        kprintf("\n\r GRADING : Test 12 : Successful\r\n");
test13:
        /*
         * Test case 13
         * Test Replacement policy
         */
        TEST=13;
        kprintf("Running Test 13 with NFRAMES : %d\r\n", NFRAMES);
        local_store = allocate_bs(100);
        if(local_store == SYSERR){
                kprintf("GRADING : Test 13 : could not allocate backing store\r\n");
                return OK;
        }
        if(open_bs(local_store) == SYSERR){
                kprintf("GRADING : Test 13 : could not open backing store\r\n");
                return OK;
        }
        if(xmmap(5000, 100, MAP_SHARED, local_store) == SYSERR){
                kprintf("GRADING : Test 13 : could not xmmap backing store\r\n");
                return OK;
        }
        for(i=0; i< 100; i++){
                kprintf("\rIteration [%3d]", i);
                ptr = (int32 *)((5000+i)*4096+8);
                *ptr = 10;
        }
        kprintf("\r\n");
        if(xmunmap(5000) == SYSERR){
                kprintf("GRADING : Test 13 : could not xmunmap backing store\r\n");
                return OK;
        }
        if(close_bs(local_store) == SYSERR){
                kprintf("GRADING : Test 13 : could not close backing store\r\n");
                return OK;
        }
        if(deallocate_bs(local_store) == SYSERR){
                kprintf("GRADING : Test 13 : could not deallocate backing store\r\n");
                return OK;
        }
        kprintf("\n\r GRADING : Test 13 : Successful\r\n");


        return OK;
}

