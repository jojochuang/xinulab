#include <xinu.h>

        int flag = 0;

/** 
 * set NFRAMES smaller to test page replacement policy 
 *
 * */
#ifdef NFRAMES
        #undef NFRAMES
        #define NFRAMES 25
#endif

#define OP_READ 0
#define OP_WRITE 1

bsd_t store_id[8] = {0};
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
/* test1 */
void test1proc(pid32 parent){
    send(parent, OK);
}
/* test2 */
void test2proc(pid32 parent){
    int *ptr;
    store_id[0] = allocate_bs(100);
    if(store_id[0] == SYSERR){
            kprintf("GRADING : Test 2-allocate Failed\r\n");
            send( parent, SYSERR);
            flag++;
    }else{
        kprintf("test2: allocate_bs() ok\r\n");
    }
    if(open_bs(store_id[0]) == SYSERR){
            kprintf("GRADING : Test 2-open Failed\r\n");
            send( parent, SYSERR);
            flag++;
    }else{
        kprintf("test2: open_bs() ok\r\n");
    }
    //if(xmmap(5000, 100, MAP_PRIVATE, store_id[0])== SYSERR){
    if(xmmap(5000, 100, MAP_SHARED, store_id[0])== SYSERR){
            kprintf("GRADING : Test 2-xmmap Failed\r\n");
            close_bs(store_id[0]);
            deallocate_bs(store_id[0]);
            send( parent, SYSERR);
            flag++;
    }else{
        kprintf("test2: open_bs() ok\r\n");
    }

    ptr = (int *)(5000*4096);
    kprintf("GRADING : Test 2 Accessing the memory : %d\r\n", *((int32 *) ptr));
    *ptr = 1729;
    kprintf("GRADING : Test 2 Accessing the memory after write : %d\r\n", *((int32 *) ptr));
xmunmap(5000);
    /*if(xmunmap(5000) == SYSERR){
            kprintf("GRADING : Test 2-xmunmap failed\r\n");
            close_bs(store_id[0]);
            deallocate_bs(store_id[0]);
            send( parent, SYSERR);
            flag++;
    }*/
    close_bs(store_id[0]);
    /*if(close_bs(store_id[0]) == SYSERR){
            kprintf("GRADING : Test 2-close Failed\r\n");
            deallocate_bs(store_id[0]);
            send( parent, SYSERR);
            flag++;
    }*/
    if(deallocate_bs(store_id[0]) == SYSERR){
            kprintf("GRADING : Test 2-deallocate Failed\r\n");
            send( parent, SYSERR);
            flag++;
    }    
    store_id[0] = -1;
    if( flag==0){
        kprintf("GRADING : Test 2 Successful\r\n");
        send( parent,OK );
    }
}
/* test3 */
void return_memory_value( pid32 parent, uint32 addr){
/* FIXME: address not mapped by this process 
 * The result depends on the meaning of SHARE mode?
 * */
        kprintf("GRADING : Test 3: Sending memory value [%d] to parent\r\n", *((int32 *)(addr)));
        int v = *((int32 *)(addr));

        while(1);
        //send(parent, *((int32 *)(addr)));
        //receive();
}
/* test4 */
// make sure the address returned by vgetmem() is above 4096th page.
void vgetmem_test( pid32 parent  ){
    int nsize = 100;
    char *p = vgetmem( 100 );
    
    /* expect page fault here */
    *p = 88;


    send(parent, p );
    /*if( p >= 4096*4096 ){
        send( parent, OK );
    }else{
        send( parent, SYSERR );
    }*/
}
/* test5 */
void privatemap(pid32 parent, int operation){
        int pageno = 5000;
        store_id[1] = allocate_bs(200);
        send( parent, store_id[1] );
        if(store_id[1] == SYSERR){
                kprintf("GRADING : Test 5-allocate Failed\r\n");
                send(parent, SYSERR);
                return;
        }
        if(open_bs(store_id[1]) == SYSERR){
                kprintf("GRADING : Test 5-open Failed\r\n");
                send(parent, SYSERR);
                return;
        }
        if(xmmap(pageno, 100, MAP_PRIVATE, store_id[1])== SYSERR){
                kprintf("GRADING : Test 5-xmmap Failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                send(parent, SYSERR);
                return;
        }


        int* ptr = (int *)(pageno*4096);
        if( operation == OP_READ ){
            // perform read operation
            int v = *ptr;

        }else{ // OP_WRITE
            // perform write operation
            *ptr = 520;


        }

        xmunmap(pageno);
        /*if(xmunmap(pageno) == SYSERR){
                kprintf("GRADING : Test 5-xmunmap failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                send(parent, SYSERR);
                return;
        }*/
        close_bs(store_id[1]);
        /*if(close_bs(store_id[1]) == SYSERR){
                kprintf("GRADING : Test 5-close Failed\r\n");
                deallocate_bs(store_id[1]);
                send(parent, SYSERR);
                return;
        }*/
        deallocate_bs(store_id[1]);
        store_id[1] = -1;
        kprintf("GRADING : Test 5 Successful\r\n");
        send(parent, OK);

}
/* test8 */
void invalid_memory_access(void){


        int *ptr = (int *)(5000*4096);
        kprintf("%s : GRADING : Test 8 Trying to access invalid memory [%u] : %d\r\n", __FUNCTION__, ptr, *ptr);
        while(1);
}
/* test9 */
void xmmap_and_wait_for_kill(pid32 parent){
        bsd_t store ;
        store = allocate_bs(10);
        if(store == SYSERR){
                kprintf("%s : GRADING : Test 9 Allocate failed\r\n", __FUNCTION__);
                send(parent, SYSERR);
                receive();
        }
        if(open_bs(store) == SYSERR){
                kprintf("%s : GRADING : Test 9 Open failed\r\n", __FUNCTION__);
                send(parent, SYSERR);
                receive();
        }
        if(xmmap(5000, 10, MAP_SHARED, store) == SYSERR){
                kprintf("%s : GRADING : Test 9 Xmmap failed\r\n", __FUNCTION__);
                send(parent, SYSERR);
                receive();
        }
        kprintf("%s : GRADING : Test 9 Sending store id [%d] to parent\r\n", __FUNCTION__, store);
        send(parent, store);
        recvclr();
        receive();
}

/* test10 */
void policytest(pid32 parent){
        bsd_t local_store;
        int i;
        int* ptr;
        kprintf("Running Test 10 with NFRAMES : %d\r\n", NFRAMES);
        local_store = allocate_bs(100);
        if(local_store == SYSERR){
                kprintf("GRADING : Test 10 : could not allocate backing store\r\n");
                send(parent, SYSERR);
                return;
        }
        if(open_bs(local_store) == SYSERR){
                kprintf("GRADING : Test 10 : could not open backing store\r\n");
                send(parent, SYSERR);
                return;
        }
        if(xmmap(5000, 100, MAP_SHARED, local_store) == SYSERR){
                kprintf("GRADING : Test 10 : could not xmmap backing store\r\n");
                send(parent, SYSERR);
                return;
        }
        for(i=0; i< 95; i++){
                kprintf("\rIteration [%3d]", i);
                ptr = (int32 *)((5000+i)*4096+8);
                *ptr = 10;
        }
        kprintf("\r\n");
        xmunmap(5000);
        /*if(xmunmap(5000) == SYSERR){
                kprintf("GRADING : Test 10 : could not xmunmap backing store\r\n");
                send(parent, SYSERR);
                return;
        }*/
        if(close_bs(local_store) == SYSERR){
                kprintf("GRADING : Test 10 : could not close backing store\r\n");
                send(parent, SYSERR);
                return;
        }
        if(deallocate_bs(local_store) == SYSERR){
                kprintf("GRADING : Test 10 : could not deallocate backing store\r\n");
                send(parent, SYSERR);
                return;
        }

        kprintf("GRADING : Test 10 : Successful\r\n");
        send(parent, OK);
}
void bonustest_read(pid32 parent, bsd_t store){
        int pageno = 6000;
        int pagerange = 200;
        bsd_t local_store;
        int count = 0;
        /*while(count < 8 ){ 
            local_store = allocate_bs(pagerange);
            kprintf("GRADING : Bonus Test - allocate_bs gets %d\r\n", local_store);
            if( local_store == store ){
                kprintf("GRADING : Bonus Test - find the same backing store \r\n");
                break;
            }else if( local_store == SYSERR ){
                kprintf("GRADING : Bonus Test -allocate_bs Failed\r\n");
                //send(parent, SYSERR);
                return;
            }
            //deallocate_bs( local_store );

            count++;
        }
        for(count=0;count<8;count++){
            if( count != store )
                deallocate_bs( count );
        }*/
        //local_store = store;
        local_store = allocate_bs(pagerange);

        if(open_bs(local_store) == SYSERR){
                kprintf("GRADING : Bonus Test -open Failed\r\n");
                send(parent, SYSERR);
                return;
        }
        kprintf("GRADING : Bonus Test - before xmmap()\r\n");
        if(xmmap(pageno, 200, MAP_PRIVATE, local_store)== SYSERR){
                kprintf("GRADING : Bonus Test-xmmap Failed\r\n");
                close_bs(local_store);
                send(parent, SYSERR);
                return;
        }
        kprintf("GRADING : Bonus Test - after xmmap(). press enter\r\n");
        char cmd[16];
        read(CONSOLE, cmd, sizeof(cmd));


        int* ptr;
        int pg;
        for(pg= pageno; pg<pageno+5;pg++){
            ptr = (int *)(pageno*4096);
            // perform read operation
            int v = *ptr;
        }

        if(xmunmap(pageno) == SYSERR){
                kprintf("GRADING : Bonus Test -xmunmap failed\r\n");
                close_bs(local_store);
                send(parent, SYSERR);
                return;
        }
        if(close_bs(local_store) == SYSERR){
                kprintf("GRADING : Bonus Test -close Failed\r\n");
                send(parent, SYSERR);
                return;
        }

        //deallocate_bs( local_store );
        kprintf("GRADING : Bonus Test - read only finished\r\n");
        send(parent, OK);

}
void bonustest_write(pid32 parent, bsd_t store){
        int pageno = 5000;
        unsigned int pagerange = 10;
        bsd_t local_store = store;
        while( 1 ){
            local_store = allocate_bs(pagerange);
            if( local_store == store ){
                break;
            }else if( local_store == SYSERR ){
                kprintf("GRADING : Bonus Test -allocate_bs Failed\r\n");
                send(parent, SYSERR);
                return;
            }
            deallocate_bs( local_store );
        }
        
        if(open_bs(local_store) == SYSERR){
                kprintf("GRADING : Bonus Test 2-open Failed\r\n");
                send(parent, SYSERR);
                return;
        }
        if(xmmap(pageno, 10, MAP_SHARED, local_store)== SYSERR){
                kprintf("GRADING : Bonus Test 2-xmmap Failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                send(parent, SYSERR);
                return;
        }

        char *ptr;
        unsigned int pg,offset;
        for(pg=pageno; pg< pageno+pagerange; pg++){
            for(offset=0;offset < NBPG; offset++){
                ptr = (char *)(pg*NBPG+offset);
                
                *ptr = 1;
            }
        }

        if(xmunmap(pageno) == SYSERR){
                kprintf("GRADING : Bonus Test 2-xmunmap failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                send(parent, SYSERR);
                return;
        }
        if(close_bs(local_store) == SYSERR){
                kprintf("GRADING : Bonus Test 2-close Failed\r\n");
                deallocate_bs(store_id[1]);
                send(parent, SYSERR);
                return;
        }

        if(open_bs(local_store) == SYSERR){
                kprintf("GRADING : Bonus Test -open Failed\r\n");
                send(parent, SYSERR);
                return;
        }
        if(xmmap(pageno, 100, MAP_PRIVATE, local_store)== SYSERR){
                kprintf("GRADING : Bonus Test-xmmap Failed\r\n");
                close_bs(local_store);
                send(parent, SYSERR);
                return;
        }


        ptr = (int *)(pageno*4096);
        *ptr = 20;

        if(xmunmap(pageno) == SYSERR){
                kprintf("GRADING : Bonus Test -xmunmap failed\r\n");
                close_bs(local_store);
                send(parent, SYSERR);
                return;
        }
        if(close_bs(local_store) == SYSERR){
                kprintf("GRADING : Bonus Test -close Failed\r\n");
                send(parent, SYSERR);
                return;
        }

        /*deallocate_bs( local_store ); */
        send(parent, OK);

}
int main(int argc, char **argv){

        bsd_t local_store;
        pid32 pid;
        int i=0;
        int j=0;
        int *ptr;
        umsg32 message;

        psinit();
        kprintf("GRADING: Start\r\n");
        goto bonus;
        //goto test6;
test01:
        for(i=0; i< 8; i++){
                allocate_bs(10);
        }
        if(allocate_bs(10) == SYSERR){
                kprintf("GRADING : Initialization Test 1 Successful\r\n");
        }
        else{
                kprintf("GRADING : Initialization Test 1 Failed\r\n");
        }
        for(i=0; i< 8; i++){
                deallocate_bs(i);
                store_id[i] = -1;
        }
test02:
        /* Test case 0-2: score: 0 */
        flag = 0;
        if(open_bs(6) != SYSERR){
                kprintf("GRADING : Initialization Test 2-1 Failed\r\n");
                flag++;
        }
        if(close_bs(1) != SYSERR){
                kprintf("GRADING : Initialization Test 2-2 Failed\r\n");
                flag++;
        }
        if(deallocate_bs(4) != SYSERR){
                kprintf("GRADING : Initialization Test 2-3 Failed\r\n");
                flag++;
        }
        if(flag == 0){
                kprintf("GRADING  : Initialization Test 2 Successful\r\n");
        }
test03:
        /*
         * Test case 3 :  score: 0
         * Make sure I can only call allocate/open/close/deallocate on a backing store
         * ie. Make sure all the calls are allowed only if they happen in a single process
         */
        store_id[0] = allocate_bs(10);
        recvclr();
        pid  = create(out_of_seq_calls, 2048, 30, "Out of Sequence Calls", 2, currpid, store_id[0]);
        resume(pid);
        if(receive() == OK){
                kprintf("GRADING : Initialization Test 3 Succesful\r\n");
        }
        else{
                kprintf("GRADING : Initialization Test 3 Failed\r\n");
        }
        open_bs(store_id[0]);
        close_bs(store_id[0]);
        deallocate_bs(store_id[0]);
        //goto test7;
//----------------------------------------------------------------------------------
// start of lab4 test cases
test1:
        /* Lab4
         * Test case 1
         * 
         * vcreate()
         * */
        recvclr();
        pid = vcreate(test1proc, 2048,30, 30, "test1", 1, currpid);
        resume(pid);

        if(receive() == OK){
                kprintf("GRADING : Test 1 Successful\r\n");
        }
        else{
                kprintf("GRADING : Test 1 Failed\r\n");
        }

test2:
        /*
         * Test case 2 : 
         * Make sure you can access mapped address/write in vcreate'd processes
         * 
         * This was adapted from test case 5, lab3
         */
        flag = 0;
        recvclr();
        pid = vcreate(test2proc, 2048,30, 30, "test2", 1, currpid);
        //pid = create(test2proc, INITSTK,INITPRIO, "Xmmap write", 1, currpid);
        resume(pid);

        if(receive() == OK){
                kprintf("GRADING : Test 2 Successful\r\n");
        }
        else{
                kprintf("GRADING : Test 2 Failed\r\n");
        }
        flag = 0;
        goto test4;
test4:
        /* Lab4
         * Test case 4
         * test vgetmem() - required
         * */
        resume( vcreate(vgetmem_test, 2048, 30, 30, "Vgetmem test", 1, currpid) );

        umsg32 m = receive();

        kprintf("GRADING : Test 4 returned %d\r\n", m);
        if( m >= 4096*4096 ){
                kprintf("GRADING : Test 4 Successful\r\n");
        }else{
                kprintf("GRADING : Test 4 Failed\r\n");
        }

        //goto test6;
test5:
        /* Lab4
         * Test case 5
         * Make sure PRIVATE xmmap mode works for new processes. i.e., copy on write does work
         *
         * */


        // how to test? first, do read-only (map a large number of pages) copy and measure the time is ~ 0
        // and then do a write after copy to make sure it does not fail

        flag = 0;
        store_id[1] = allocate_bs(100);
        open_bs(store_id[1]);
        xmmap(6000, 100, MAP_SHARED, store_id[1]);
        xmunmap(6000);
        // see if access to the mapped address causes page fault.
        resume( vcreate(privatemap,2048, 30, 30, "test5 privatemap process", 2, currpid, OP_READ) );

        local_store = receive();
        // measure the time until the sub proc finishes reading
        if( recvtime( 5*1000*1000 ) == SYSERR ){ // 1000 ms
                // read operation should take no time.
                kprintf("GRADING : Test 5-performed read does not finish in time.\r\n");
                flag++;
        }

        resume( vcreate(privatemap,2048, 30, 30, "test5 privatemap process", 2, currpid, OP_WRITE) );

        local_store = receive();
        // measure the time until the sub proc finishes reading
        if( recvtime( 5*1000*1000 ) == SYSERR ){
                // read operation should take no time.
                kprintf("GRADING : Test 5-performed write and spent > 1 seconds, which is expected.\r\n");
        }else{
            flag ++;
        }

        close_bs(store_id[1]);
        deallocate_bs(store_id[1]);

        if( flag == 0 ){
                kprintf("GRADING : Test 5 Successful.\r\n");
        }

        //goto done;

test6:
        /* Lab4
         * Test case 6
         * Make sure every process has its own page directory mapping
         * -- repeat test case 3, but with create instead of vcreate
         * */
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
        //if(xmmap(5000, 100, MAP_SHARED, store_id[1])== SYSERR){
        if(xmmap(5000, 100, MAP_SHARED, store_id[1])== SYSERR){
                kprintf("GRADING : Test 6-xmmap Failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                goto test7;
        }

        ptr = (int *)(5020*4096);
        kprintf("GRADING : Test 6 Accessing the memory : %d\r\n", *((int32 *) ptr));
        *ptr = 1729;
        kprintf("GRADING : Spawning another process to access the memory\r\n");
        recvclr();
        pid = create(return_memory_value, 2048, 30, "Return Memory Value", 2, currpid, 5020*4096);
        resume(pid);
        sleep(1);
        if( kill(pid) == OK ){
                kprintf("GRADING : Test 6 Failed because it should've cause invalid access.\r\n");
                flag++;
        }else{
                kprintf("GRADING : Test 6 process killed as expected.\r\n");
        }
        kprintf("GRADING : Test 6 Accessing the memory after write : %d\r\n", *((int32 *) ptr));

        xmunmap(5000);
        /*if(xmunmap(5000) == SYSERR){
                kprintf("GRADING : Test 6-xmunmap failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                goto test7;
        }*/
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
                kprintf("GRADING : Test 6 Successful\r\n");
        }
        goto test7;

test7:
        /* Lab4
         * Test case 7 : 
         * Make sure xmmap/write/xunmap/xmmap/read works
         * and can be remapped by another vcreate process.
         */
        flag = 0;
        store_id[2] = -1;
        store_id[3] = -1;
        store_id[2] = allocate_bs(100);
        if(store_id[2] == SYSERR){
                kprintf("GRADING : Test 7-allocate Failed\r\n");
                goto test3;
        }
        if(open_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 7-open Failed\r\n");
                goto test3;
        }
        if(xmmap(6000, 100, MAP_SHARED, store_id[2])== SYSERR){
                kprintf("GRADING : Test 7-xmmap Failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test3;
        }

        ptr = (int *)(6080*4096);
        kprintf("GRADING : Test 7 Accessing the memory : %d\r\n", *((int32 *) ptr));
        *ptr = 17290;
        kprintf("GRADING : Test 7 Accessing the memory after write : %d\r\n", *((int32 *) ptr));

        xmunmap(6000);
        /*if(xmunmap(6000) == SYSERR){
                kprintf("GRADING : Test 7-xmunmap failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test3;
        }*/
        if(close_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 7-close Failed\r\n");
                deallocate_bs(store_id[2]);
                goto test3;
        }
        kprintf("GRADING : Mapping and trying to read again\r\n");
        if(open_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 7-againopen Failed\r\n");
                goto test3;
        }
        if(xmmap(6000, 100, MAP_SHARED, store_id[2])== SYSERR){
                kprintf("GRADING : Test 7-againxmmap Failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test3;
        }

        ptr =(int *)( 6080*4096);
        kprintf("GRADING : Test 7 Accessing the memory should be 17290: %d\r\n", *((int32 *) ptr));
        if(*((int32 *)(ptr)) != 17290){
                kprintf("GRADING : Test 7 failed, memory value not persistent\r\n");
                flag++;
        }
        
        xmunmap(6000);
        /*if(xmunmap(6000) == SYSERR){
                kprintf("GRADING : Test 7-againxmunmap failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test3;
        }*/
        if(close_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 7-againclose Failed\r\n");
                deallocate_bs(store_id[2]);
                goto test3;
        }

        if(deallocate_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 7-deallocate Failed\r\n");
                goto test3;
        }    
        if(flag == 0){
                kprintf("GRADING : Test 7 Successful\r\n");
        }
        goto test3;
test3:
        /*
         * Test case 3 : 
         * Make sure another process(vcreate) can not read the memory I have written
         * ie. Check all tables are not global
         *
         * This was adapted from testcase 6, lab 3
         */
        flag = 0;
        store_id[2] = allocate_bs(100);
        if(store_id[2] == SYSERR){
                kprintf("GRADING : Test 3-allocate Failed\r\n");
                goto test10;
        }
        if(open_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 3-open Failed\r\n");
                goto test10;
        }
        //if(xmmap(5000, 100, MAP_SHARED, store_id[2])== SYSERR){
        if(xmmap(5000, 100, MAP_SHARED, store_id[2])== SYSERR){
                kprintf("GRADING : Test 3-xmmap Failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test10;
        }

        ptr = (int *)(5020*4096);
        kprintf("GRADING : Test 3 Accessing the memory : %d\r\n", *((int32 *) ptr));
        *ptr = 1729;
        kprintf("GRADING : Spawning another process to access the memory\r\n");
        recvclr();
        pid = vcreate(return_memory_value, 2048, 10, 30, "Return Memory Value", 2, currpid, 5020*4096);
        resume(pid);
        //sleep(1);
        if( kill(pid) == OK ){
                kprintf("GRADING : Test 3 Failed because it should've cause invalid access.\r\n");
                flag++;
        }else{
                kprintf("GRADING : Test 3 process killed as expected.\r\n");
        }
        kprintf("GRADING : Test 3 Accessing the memory after write : %d\r\n", *((int32 *) ptr));

        xmunmap(5000);
        /*if(xmunmap(5000) == SYSERR){
                kprintf("GRADING : Test 3-xmunmap failed\r\n");
                close_bs(store_id[2]);
                deallocate_bs(store_id[2]);
                goto test10;
        }*/
        if(close_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 3-close Failed\r\n");
                deallocate_bs(store_id[2]);
                goto test10;
        }
        if(deallocate_bs(store_id[2]) == SYSERR){
                kprintf("GRADING : Test 3-deallocate Failed\r\n");
                goto test10;
        }    
        if(flag == 0){
                kprintf("GRADING : Test 3 Successful\r\n");
        }
        goto test10;
test10:
        recvclr();
        sleep(1); // sleep one second before test10 to reduce interference.
        /* Lab4
         * stress test vgetmem or vcreate?
         * */

        srpolicy( FIFO );
        pid = vcreate(policytest, 2048, 10, 30, "stress test. policytest", 1, currpid);
        resume(pid);

        receive();
test11:
        recvclr();
        sleep(1); // sleep one second before test10 to reduce interference.
        /* Lab4
         * stress test vgetmem or vcreate?
         * */

        srpolicy( MYPOLICY );
        pid = vcreate(policytest, 2048, 10, 30, "stress test. policytest", 1, currpid);
        resume(pid);

        receive();
        srpolicy( FIFO );
test8:
        /* Lab4
         * Test 8
         * Make sure invalid memory access kills() the process
         * vcreate()
         */
        recvclr();
        pid = vcreate(invalid_memory_access, 2048, 10, 40, "Invalid Memory Access", 0);
        resume(pid);
        receive();
        if(kill(pid) != SYSERR){
                kprintf("GRADING : Test 8 Failed  The process [%d] should have been already killed\r\n");
        }
        else{
                kprintf("GRADING : Test 8 Successful\r\n");
        }
test9:
        /* Lab4
         * Test case 9
         * Make sure kill() cleans up mappings and deallocates
         * vcreate()
         */
        recvclr();
        pid = vcreate(xmmap_and_wait_for_kill, 2048, 10, 30, "XMMAP and wait for kill", 1, currpid);
        resume(pid);
        local_store= receive();
        if(local_store == SYSERR){
                kprintf("GRADING : Test 9 Failed the process returned SYSERR\r\n");
                goto test10;
        }
        kill(pid);
        i=0;
        for(i=0; i< 8; i++){
                store_id[i] = allocate_bs(10);
                if(store_id[i] == SYSERR){
                        kprintf("GRADING : Test 9 allocation failed before 8 iterations i=[%d]\r\n", i);
                        break;
                }
                printf("GRADING : Test 9 :The store ID is %d\r\n", store_id[i]);
                if(store_id[i] == local_store){
                        break;
                }
        }
        if(i < 8 && store_id[i] == local_store ){
                kprintf("GRADING : Test 9 Successful , killing the process deallocated the backing store\r\n");
                for(j=0; j < 8; j++){
                        open_bs(store_id[j]);
                        close_bs(store_id[j]);
                        deallocate_bs(store_id[j]);
                }
        }
        else{
                kprintf("GRADING : Test 9 Failed, we could not allocate the store again in 8 iterations\r\n");
                for(j=0; j < 8; j++){
                        open_bs(store_id[j]);
                        close_bs(store_id[j]);
                        deallocate_bs(store_id[j]);
                }
        }

bonus:

//#ifdef BONUS
    {
        int pageno = 5000;
        int pagerange = 200;
        char cmd[16];
        kprintf("GRADING : bonus points section\r\n");
        /* Lab4
         * bonus points: 50 points capped at 280
         * Make sure PRIVATE xmmap mode works for new processes. i.e., copy on write does work
         *
         * */
        flag = 0;
        int accessrange = 10;

        // how to test? first, do read-only (map a large number of pages) copy and measure the time is ~ 0
        // and then do a write after copy to make sure it does not fail

        /*store_id[1] = allocate_bs(pagerange);

        if(open_bs(store_id[1]) == SYSERR){
                kprintf("GRADING : Bonus Test (prep)-open Failed\r\n");
                deallocate_bs( store_id[1] );
                goto done;
        }
        kprintf("GRADING : Bonus Test (prep) store id = %d\r\n", store_id[1]);
        if(xmmap(pageno, pagerange, MAP_SHARED, store_id[1])== SYSERR){
                kprintf("GRADING : Bonus Test (prep)-xmmap Failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs( store_id[1] );
        }
        // make sure the pages are resident
        int pg;
        int offset;
        //for(pg=pageno; pg< pageno+pagerange-1; pg++){
        for(pg=pageno; pg< pageno+accessrange; pg++){
            kprintf("GRADING : Writing to page %d.\r\n", pg);
            offset=0;
            //for(offset=0;offset < NBPG; offset++){
            //for(offset=0;offset < 100; offset++){
                ptr = (char *)(pg*NBPG+offset);
                
                *ptr = (pg*NBPG+offset)%100 ;

                //kprintf("\roffset = %d", offset);
            //}
        }
        if(xmunmap(pageno) == SYSERR){
                kprintf("GRADING : Bonus Test (prep)-xmunmap failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs( store_id[1] );
                goto done;
        }
        if(close_bs(store_id[1]) == SYSERR){
                kprintf("GRADING : Bonus Test (prep)-close Failed\r\n");
                deallocate_bs( store_id[1] );
                goto done;
        }

        deallocate_bs( store_id[1] );
        // wait for my command
        kprintf("GRADING : press enter\r\n");
        read(CONSOLE, cmd, sizeof(cmd));
        */

        kprintf("GRADING : Bonus Test1 - read operation should return immediately. \r\n");
        // see if access to the mapped address causes page fault.
        resume( vcreate(bonustest_read,2048, 30, 30, "copy-on-write readonly test", 2, currpid, store_id[1]) );
        //bonustest_read(currpid, store_id[1]);

        // measure the time until the sub proc finishes reading
        //if( recvtime( 2*1000 ) == SYSERR ){ // 1000 ms
        if( receive(  ) == SYSERR ){ // 1000 ms
                // read operation should take no time.
                kprintf("GRADING : Bonus Test 1 -performed read does not finish in time.\r\n");
                flag++;
        }
        recvclr();

        // wait for my command
        kprintf("GRADING : press enter\r\n");
        read(CONSOLE, cmd, sizeof(cmd));

        // write to the backing store
        // find the store number

        // /////////////////////
        resume( vcreate(bonustest_write,2048, 30, 30, "Bonus Test 3 bonustest process", 2, currpid, store_id[1]) );
        //bonustest_write(currpid, store_id[1]);

        /*if( receive() == SYSERR ){
            flag++;
            goto done;
        }*/

        kprintf("GRADING : press enter\r\n");
        read(CONSOLE, cmd, sizeof(cmd));

        // make sure the backing store is not changed.
        /*while(1){
            bsd_t reopenbsd = allocate_bs(pagerange);
            if( store_id[1] == reopenbsd ){
                break;
            }

            deallocate_bs( reopenbsd );
        }*/
        
        if(open_bs(store_id[1]) == SYSERR){
                kprintf("GRADING : Bonus Test 4-open Failed\r\n");
                goto done;
        }
        if(xmmap(pageno, pagerange, MAP_SHARED, store_id[1])== SYSERR){
                kprintf("GRADING : Bonus Test 4-xmmap Failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                goto done;
        }

        //for(pg=pageno; pg< pageno+pagerange; pg++){
        int pg;
        int offset;
        for(pg=pageno; pg< pageno+accessrange; pg++){
            offset = 0;
            //for(offset=0;offset < NBPG; offset++){
                ptr = (char *)(pageno*NBPG+offset);
                
                if( *ptr != (pageno*NBPG+offset)%100 ){
                    kprintf("GRADING : Bonus Test 4 Failed because backing store was changed. Expect: %d, Value: %d\r\n", offset%100 ,*ptr);
                    flag ++;
                    break;
                }
            //}
        }


        if(xmunmap(pageno) == SYSERR){
                kprintf("GRADING : Bonus Test 4-xmunmap failed\r\n");
                close_bs(store_id[1]);
                deallocate_bs(store_id[1]);
                goto done;
        }
        if(close_bs(store_id[1]) == SYSERR){
                kprintf("GRADING : Bonus Test 4-close Failed\r\n");
                deallocate_bs(store_id[1]);
                goto done;
        }

        deallocate_bs(store_id[1]);
        store_id[1] = -1;

        if( flag == 0 ){
                kprintf("GRADING : Bonus Test Successful.\r\n");
        }
    }

//#endif
done:
        kprintf("GRADING: Finished\r\n");
        return OK;
/* test11: test multiple processes using backing store */
//-----------------------------------------------------------------------------------------------------------
}

