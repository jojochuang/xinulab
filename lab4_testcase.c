
/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>
//#include <tsinit.h>

#define PRI 20

#define _PART1_
//#define _PART2_
//#define _PART3_
//#define _BONUS_

#ifdef _PART1_

sid32 sem1, sem2, sem3;
void _process11();
void _process12();
void _process21();
void _process22();
void _process23();
void _process41(int proc);
void lab4_part1_test1(){

  int n;
  for(n=0;n<100;n++){
    if(waitd(sem1) != OK) {
      kprintf("Test1: failed to acquire a semaphore\n");
      return;
    }
    if( signal(sem1) != OK ){
      kprintf("Test1: failed to release a semaphore\n");
      return;
    }
  }

  kprintf("Test1: successful\n");
  
}
void lab4_part1_test2(){

	resume( create(_process11, 1024, 20, "p1", 0) );
	sleepms(100);
	resume( create(_process12, 1024, 20, "p2", 0) );
}
void lab4_part1_test3() {

	resume( create(_process21, 1024, 20, "p1", 0) );
	sleepms(100);
	resume( create(_process22, 1024, 20, "p2", 0) );
	sleepms(100);
	resume( create(_process23, 1024, 20, "p3", 0) );
}
int test4_success = 1;
int test4_proc_finish[3] = {0};
void lab4_part1_test4(){

	resume( create(_process41, 1024, 20, "p1", 1, 0) );
	//sleepms(1);
	resume( create(_process41, 1024, 20, "p2", 1, 1) );
	//sleepms(1);
	resume( create(_process41, 1024, 20, "p3", 1, 2) );

  while(1){
    sleep(1);
    if( test4_proc_finish[0] && test4_proc_finish[1] && test4_proc_finish[2] )
      break;
  }
  if( test4_success == 1 ){
    kprintf("Test4: successful\n");
  }else{
    kprintf("Test4: failed\n");

  }
}

void _process41(int proc){
  int n;
  for(n=0;n<100;n++){
    if(waitd(sem1) != OK) {
      kprintf("Test4: failed to acquire semaphore 1\n");
      test4_success = 0;
      test4_proc_finish[proc] = 1;
      return;
    }
    if(waitd(sem2) != OK) {
      kprintf("Test4: failed to acquire semaphore 2\n");
      test4_success = 0;
      test4_proc_finish[proc] = 1;
      return;
    }
    sleepms(10);
    if( signal(sem2) != OK ){
      kprintf("Test4: failed to release semaphore 2\n");
      test4_success = 0;
      test4_proc_finish[proc] = 1;
      return;
    }
    if( signal(sem1) != OK ){
      kprintf("Test4: failed to release semaphore 1\n");
      test4_success = 0;
      test4_proc_finish[proc] = 1;
      return;
    }
    //kprintf("proc %d n = %d\n", proc,n );
  }
  kprintf("Test4: process %d finished\n", proc);
  test4_proc_finish[proc] = 1;
  
}
void _process11() {
	if(waitd(sem1) != OK) {
		kprintf("Test2: failed.  process 1 failed to acquire sempahore 1\n");
		return;
	}
	//kprintf("p1 aquired sem %d\n", sem1);

	sleepms(500);

	if(waitd(sem2) != OK) {
		kprintf("Test2: successful. process 1 failed to acquire semaphore 2. deadlock detected\n");
		return;
	}
	//kprintf("p1 aquired sem %d\n", sem2);

	signal(sem2);
	//kprintf("p1 signaled sem %d\n", sem2);
	signal(sem1);
	//kprintf("p1 signaled sem %d\n", sem1);

	kprintf("p1 terminated successfully\n");
}

void _process12() {
	if(waitd(sem2) != OK) {
		kprintf("Test2: failed. prcoess 2 failed to acquire sempahore 1\n");
		return;
	}
	//kprintf("p2 aquired sem %d\n", sem2);

	if(waitd(sem1) != OK) {
		kprintf("Test2: failed. process 2 failed to acquire semaphore 2\n");
		return;
	}
	//kprintf("p2 aquired sem %d\n", sem1);

	signal(sem1);
	//kprintf("p2 signaled sem %d\n", sem1);
	signal(sem2);
	//kprintf("p2 signaled sem %d\n", sem2);

	kprintf("p2 terminated successfully\n");
}
void _process21() {
	if(waitd(sem2) != OK) {
		kprintf("Test3: failed. p1 failed to acquire semaphore 2\n");
		return;
	}
	//kprintf("p1 aquired sem %d\n", sem2);
	sleepms(200);

	if(waitd(sem1) != OK) {
		kprintf("DEADLOCK DETECTED\n");
		return;
	}
	//kprintf("p1 aquired sem %d\n", sem1);
	sleepms(200);

	signal(sem1);
	//kprintf("p1 signaled sem %d\n", sem1);
	signal(sem2);
	//kprintf("p1 signaled sem %d\n", sem2);

	kprintf("Test3: failed. p1 terminated without deadlock detected\n");
}

void _process22() {
	if(waitd(sem1) != OK) {
		kprintf("Test3: failed. p2 failed to acquire semaphore 2\n");
		return;
	}
	//kprintf("p2 aquired sem %d\n", sem1);
	sleepms(200);

	if(waitd(sem3) != OK) {
		kprintf("DEADLOCK DETECTED\n");
		return;
	}
	//kprintf("p2 aquired sem %d\n", sem3);
	sleepms(200);

	if(waitd(sem2) != OK) {
		kprintf("DEADLOCK DETECTED\n");
		return;
	}
	//kprintf("p2 aquired sem %d\n", sem2);
	sleepms(200);

	signal(sem2);
	//kprintf("p2 signaled sem %d\n", sem2);
	signal(sem3);
	//kprintf("p2 signaled sem %d\n", sem3);
	signal(sem1);
	//kprintf("p2 signaled sem %d\n", sem1);

	kprintf("Test3: failed. p2 terminated without deadlock detected\n");
}

void _process23() {
	if(waitd(sem3) != OK) {
		kprintf("Test3: failed. process 3 failed to acquire semaphore 3\n");
		return;
	}
	//kprintf("p3 aquired sem %d\n", sem3);
	sleepms(200);

	if(waitd(sem1) != OK) {
		kprintf("DEADLOCK DETECTED\n");
		return;
	}
	//kprintf("p3 aquired sem %d\n", sem1);
	sleepms(200);

	signal(sem1);
	//kprintf("p3 signaled sem %d\n", sem1);
	signal(sem3);
	//kprintf("p3 signaled sem %d\n", sem3);

	kprintf("Test3: failed. p3 terminated without deadlock detected\n");
}
#endif


#ifdef _PART2_
static int score = 0;

sid32 sema;
//sid32 sema0;
static int test0_success = 0;
static int test0_sender_returned = 0;
static void receiver0(){

  sleep(1);

  kprintf("Test0: call receive()\r\n");
  if( test0_sender_returned ){
    test0_success = 1;
  }else{
    kprintf("Test0: fail: timeout, the first sendb() should return immediately.\r\n");
  }
  uint32 msg = receive();
  if( test0_success ){
    score+= 10;
    kprintf("Test0: Pass\r\n");
  }
}
static void sender0(int pid){
  sendb( pid, 1 );
  kprintf("Test0: sender returned\r\n");
  test0_sender_returned = 1;
  //signal( sema0 );
}
#define TEST1_MESSAGE 5566
static void _receiver1(){
  uint32 msg = receive();
  if( msg == TEST1_MESSAGE ){
    kprintf("Test1: Pass\r\n");
    score+=10;
  }else{
    kprintf("Test1: fail: received incrrect value from the sender. received %d, expect %d\r\n", msg, TEST1_MESSAGE);
  }
  signal( sema );
}
static void _sender1(int pid){
  sendb( pid, TEST1_MESSAGE );
}
#define TEST2_OFFSET 1024
static void _receiver2(){
  int n;
  for(n = 0; n< 100; n++ ){
    uint32 msg = receive();
    if( msg != n+TEST2_OFFSET ){
      kprintf("Test2: fail: did not receive in sending order at %d th message.\r\n", n);
      signal( sema );
      return;
    }
  }
  kprintf("Test2: pass\r\n");
  score+=10;
  signal( sema );
}
static void _sender2(int pid){
  int n;
  for(n = 0; n< 100; n++ ){
    sendb( pid, n+TEST2_OFFSET );
  }
}
static int test3_result = 0;
static void _receiver3(){
  int n;
  for(n = 0; n< 5; n++ ){
    uint32 msg = receive();
    test3_result++;
  }
}
static void _sender3(int pid){
  sendb( pid, 1 );
}
#define TEST4_OFFSET 765
static void receiver4(){
  int n;
  sleep(6); // sleep several seconds to force all senders to wait
  for(n = 0; n< 5; n++ ){
    uint32 msg = receive();
    if( msg != n+TEST4_OFFSET ){
      kprintf("Test4: fail: did not receive in sending order at %d th sender\r\n", n);
      signal( sema );
      return;
    }
  }
  kprintf("Test4: pass\r\n");
  score+=10;
  signal( sema );
}
static void sender4(int pid, int n){
  sendb( pid, n+TEST4_OFFSET );
}
#define TEST5_SENDER 5
static int msgarr[ TEST5_SENDER*100 ] = {0};
static int test5_received_messages = 0;
static int test5_fail = 0;
static int test5_success = 0;
static void receiver5(){
  int n;
  for(n = 0; n< TEST5_SENDER*100; n++ ){
    uint32 msg = receive();
    //kprintf("received %d th message\r\n", n );
    if( msgarr[ msg ] == 1 || msg >= TEST5_SENDER*100 ){
      kprintf("Test5: fail: receive duplicate messages or unsent messages. msg=%d\r\n", msg);
      test5_fail = 1;
      test5_success = 0;
      return;
    }
    msgarr[ msg ] = 1;
    test5_received_messages++;
  }
  kprintf("Test5: pass\r\n");
  score+=10;

  test5_fail = 0;
  test5_success = 1;
}
static void sender5(int pid, int mysenderid){
  int n;
  for(n = 0; n < 100; n++ ){
    sendb( pid, n+ (mysenderid*100) );
  }
}
#define TEST6_OFFSET 47906
int test6_result[2] = {0};
static void receiver6(int receiver_id){
  int n;
  uint32 msg = receive();
  if( msg < 0+TEST6_OFFSET || msg >= 2+TEST6_OFFSET ){
    intmask	mask;
    mask = disable();
    kprintf("Test6: fail: incorrect msg received. expect %d <= msg < %d. received %d instead\r\n", TEST6_OFFSET, TEST6_OFFSET+2, msg);
    test6_result[ receiver_id ] = 0;
		restore(mask);
    return;
  }
  
  kprintf("Test6: receiver process %d finished\r\n", currpid );
  test6_result[ receiver_id ] = 1;
}
static void sender6(int pid, int mysenderid){
  sendb( pid, mysenderid+TEST6_OFFSET );
}
int test7_result[2] = {0};
static void receiver7(int receiver_id){
  int n;
  int msgs[2] = {0};
  intmask	mask;
  for(n=0;n<2;n++){
    uint32 msg = receive();
    if( msg < 0 || msg >= 4 ){
      mask = disable();
      kprintf("Test7: fail: incorrect msg received. expect 0<=msg<4 received %d instead\r\n", msg);
      restore(mask);
      test7_result[ receiver_id ] = 0;
      signal(sema );
      return;
    }
    if( msgs[msg] == 1 ){
      mask = disable();
      kprintf("Test7: fail: received duplicated message\r\n");
      restore(mask);
      test7_result[ receiver_id ] = 0;
      signal(sema );
      return;
    }
    msgs[msg] = 1;
  }
  test7_result[ receiver_id ] = 1;
  kprintf("Test7: receiver process %d finished\r\n", currpid );
  signal( sema  );
}
static void sender7(int pid, int mysenderid){
  sendb( pid, mysenderid/2 );
}
static int test8_received_message[2] = {0};
static void receiver8(int myid){
  int n;
  for(n=0;n<2*100;n++){
    uint32 msg = receive();
    test8_received_message[ myid ]++;
  }
  kprintf("Test8: receiver process %d finished\r\n", currpid );
  //signal( sema );
}
static void sender8(int pid, int mysenderid){
  int n;
  for(n=0;n<100;n++){
    sendb( pid, mysenderid );
  }
}
#define TEST9_OFFSET 47906
static int test9_received_message[2] = {0};
static int test9_result = 1;
static void receiver9(int myid){
  int test9_msgs[200] = {0};
  int n;
  intmask	mask;
  for(n=0;n<2*100;n++){
    uint32 msg = receive();
    if( msg < 0+TEST9_OFFSET || msg >= 200+TEST9_OFFSET ){
      mask = disable();
      kprintf("Test9: fail: incorrect msg received. expect %d <= msg < %d. received %d instead\r\n", TEST9_OFFSET, TEST9_OFFSET+200, msg);
      restore(mask);
      test9_result = 0;
      return;
    }
    if( test9_msgs[msg-TEST9_OFFSET] == 1 ){
      mask = disable();
      kprintf("Test9: fail: received duplicated message\r\n");
      restore(mask);
      test9_result = 0;
      return;
    }
    test9_msgs[msg-TEST9_OFFSET] = 1;
    test9_received_message[ myid ]++;
  }
  kprintf("Test9: receiver process %d finished\r\n", currpid );
}
static void sender9(int pid, int mysenderid){
  int n;
  for(n=0;n<100;n++){
    sendb( pid, mysenderid*100+n+TEST9_OFFSET );
  }
}
// grading criteria: 
// programming 100 points
// (1) every sent messages must arrive
// (2) receiving order conforms sending order
//
//
//
// written 20 points
void weichiu_test0(){
  kprintf("Test 0 -- One sender one receiver. the sender should return immediately. 10 points\r\n");

  //sema0 = semcreate( 0 );
  int pid = create(receiver0, 1024, PRI, "receiver0", 0);
  resume(pid);
  
  int senderpid = create(sender0, 1024, PRI, "sender0", 1, pid);
  resume(senderpid);

  sleep(3);
  if( kill( pid ) == OK ){
    kprintf("test 0 failed: receiver did not return\r\n");
  }
  kill( senderpid );
  kprintf("Test0 finished\r\n");
  sleep(1);
}

void weichiu_test1(){
  kprintf("Test 1 -- One sender one receiver. one message per sender. verify receiver gets the right message. 10 points\r\n");
  int pid = create(_receiver1, 1024, PRI, "receiver1", 0);
  resume(pid);
  
  resume(create(_sender1, 1024, PRI, "sender1", 1, pid));

  wait( sema );
  kprintf("Test 1 finished\r\n");
  sleep(1);
}

void weichiu_test2(){
  kprintf("Test 2 -- One sender one receiver. multiple messages per sender. 10 points\r\n");
  int pid = create(_receiver2, 1024, PRI, "receiver2", 0);
  resume(pid);
  
  resume(create(_sender2, 1024, PRI, "sender2", 1, pid));

  wait( sema );
  kprintf("Test 2 finished\r\n");
  sleep(1);
}

void weichiu_test3(){
  kprintf("Test 3 -- Multi-sender one receiver. one message per sender. 10 points \r\n");
  int pid = create(_receiver3, 1024, PRI, "receiver3", 0);
  resume(pid);
  
  int n;
  for( n= 0; n< 5;n++){
    resume(create(_sender3, 1024, PRI, "sender3", 1, pid));
  }

  sleep(3);
  if( test3_result == 5 ){
    kprintf("Test3: pass\r\n");
    score+=10;
  }else{
    kprintf("Test 3: fail: timeout waiting for messages\r\n");
  }
  kprintf("Test 3 finished\r\n");
  sleep(1);
}
void weichiu_test4(){
  kprintf("Test 4 -- Multi-sender one receiver. one message per sender . is receiving order the same as sending order? 10 points \r\n");
  int pid = create(receiver4, 1024, PRI, "receiver4", 0);
  resume(pid);
  
  int n;
  int senderpid[5];
  for( n= 0; n< 5;n++){
    senderpid[ n ] = create(sender4, 1024, PRI, "sender4", 2, pid, n);
    resume(senderpid[ n ] );
    sleep(1); // sleep 1 second to let the previous sender block sending.
  }
  wait( sema );
  sleep(1); // sleep 1 second to let all processes finish
  for(n=0;n<5;n++){
    kill( senderpid[n] );
  }
  kprintf("Test 4 finished\r\n");
}

void weichiu_test5(){
  kprintf("Test 5 -- Multi-sender one-receiver. multiple messages per sender. 10 points\r\n");
  int n;
  int pid = create(receiver5, 1024, PRI, "receiver5", 0);
  resume(pid);
  
  for( n= 0; n< TEST5_SENDER;n++){
    resume(create(sender5, 1024, PRI, "sender5", 2, pid, n));
  }
  int last_received_message = 0;
  for( n=0;n< 50; n++){ // wait for 50 seconds, if at any point receiver does not make progress, abort
    sleep(1);
    if( test5_success ){ break; }
    if( test5_fail ){ break; }
    if( test5_received_messages == last_received_message ){
      kprintf("Test 5 fail: receiver does not receive any more messages after %d th message\r\n", test5_received_messages );
      kill( pid ); // kill receiver
      break;
    }
    last_received_message = test5_received_messages;
  }
  kprintf("Test 5 finished\r\n");
  sleep(1);
}
void weichiu_test6(){
  kprintf("Test 6 -- Multi-sender multi-receiver. one sender sends to one receiver. one message per sender - are all messages received? 10 points\r\n");
  int n;
  int receiverpids[ 2 ];
  for( n= 0; n< 2;n++){
    int pid = create(receiver6, 1024, PRI, "receiver6", 1, n);
    receiverpids[ n ] = pid;
    resume(pid);
  }
  
  for( n= 0; n< 2;n++){
    resume(create(sender6, 1024, PRI, "sender6", 2, receiverpids[ n ], n));
  }
  sleep(2);
  if( test6_result[ 0 ] && test6_result[ 1 ]){
    score+= 10;
    kprintf("Test 6 pass\r\n");
  }
  kprintf("Test 6 finished\r\n");
  sleep(1);
}
void weichiu_test7(){
  kprintf("Test 7 -- Multi-sender multi-receiver. two senders per receiver. one message per sender - are all messages received? 10 points\r\n");
  int n;
  int receiverpids[ 2 ];
  for( n= 0; n< 2;n++){
    int pid = create(receiver7, 1024, PRI, "receiver7", 1, n);
    receiverpids[ n ] = pid;
    resume(pid);
  }
  
  for( n= 0; n< 4;n++){
    resume(create(sender7, 1024, PRI, "sender7", 2, receiverpids[ n%2 ], n));
  }
  // wait for both receivers to finish
  for( n=0; n< 2; n++){
    wait( sema );
  }
  if( test7_result[ 0 ] && test7_result[ 1 ]){
    kprintf("Test 7 pass\r\n");
    score+= 10;
  }
  sleep(1);
  kprintf("Test 7 finished\r\n");
}
void weichiu_test8(){
  kprintf("Test 8 -- Multi-sender multi-receiver. two senders per receiver. Multiple messages per sender - make sure messages are received. 10 points\r\n");
  int n;
  int receiverpids[ 2 ];
  for( n= 0; n< 2;n++){
    int pid = create(receiver8, 1024, PRI, "receiver8", 1, n);
    receiverpids[ n ] = pid;
    resume(pid);
  }
  
  for( n= 0; n< 4;n++){
    resume(create(sender8, 1024, PRI, "sender8", 2, receiverpids[ n%2 ], n%2));
  }
  // wait for both receivers to finish
  /*for( n=0; n< 2; n++){
    wait( sema );
  }*/
  int last_received_message0 = 0;
  int last_received_message1 = 0;
  for( n=0;n< 50; n++){ // wait for 50 seconds, if at any point receiver does not make progress, abort
    sleep(1);
    if( test8_received_message[0] == last_received_message0 ){
      kprintf("Test 8 fail: receiver 0 does not receive any more messages after %d th message\r\n", last_received_message0 );
      kill( receiverpids[0] ); // kill receiver
      break;
    }
    if( test8_received_message[1] == last_received_message1 ){
      kprintf("Test 8 fail: receiver 1 does not receive any more messages after %d th message\r\n", last_received_message1 );
      kill( receiverpids[1] ); // kill receiver
      break;
    }
    last_received_message0 = test8_received_message[0];
    last_received_message1 = test8_received_message[1];

    if( test8_received_message[1] == 200 && test8_received_message[0] == 200 ){
      break;
    }
  }
  if( test8_received_message[0] == 200 && test8_received_message[1] == 200 ){
    kprintf("Test 8 pass\r\n");
    score+= 10;
  }
  sleep(1);
  kprintf("Test 8 finished\r\n");
}
void weichiu_test9(){
  kprintf("Test 9 -- Multi-sender multi-receiver. two senders per receiver. Multiple messages per sender. check correctness. 10 points\r\n");
  int n;
  // WC: one of the receiver does not receive at all, and the other pair of sender receiver proceeds normally
  int receiverpids[ 2 ];
  for( n= 0; n< 2;n++){
    int pid = create(receiver9, 10240, PRI, "receiver9", 1, n);
    receiverpids[ n ] = pid;
    resume(pid);
  }
  
  int m;
  for( n= 0; n< 2;n++){
    for(m=0; m< 2;m++){
      resume(create(sender9, 1024, PRI, "sender9", 2, receiverpids[ n ], m ));
    }
  }
  int last_received_message0 = 0;
  int last_received_message1 = 0;
  for( n=0;n< 50;n++){ // wait for 50 seconds
    sleep(1);
    if( test9_received_message[0] == last_received_message0 ){
      kprintf("Test 9 fail: receiver 0 does not receive any more messages after %d th message\r\n", last_received_message0 );
      kill( receiverpids[0] ); // kill receiver
      test9_result = 0;
      break;
    }
    if( test9_received_message[1] == last_received_message1 ){
      kprintf("Test 9 fail: receiver 1 does not receive any more messages after %d th message\r\n", last_received_message1 );
      kill( receiverpids[1] ); // kill receiver
      test9_result = 0;
      break;
    }
    if( test9_result == 0 ){
      break;
    }
    if( test9_received_message[1] == 200 && test9_received_message[0] == 200 ){
      break;
    }
    last_received_message0 = test9_received_message[0];
    last_received_message1 = test9_received_message[1];

  }
  if( test9_result ){
    kprintf("Test 9 pass\r\n");
    score+=10;
  }
  kprintf("Test 9 finished\r\n");
  sleep(1);
}
#endif
static int _running = 0;
#ifdef _PART3_
static umsg32 __buf = 0;
static pid32 sender_pid, receiver_pid;
static msg_count= 0;
#define N_MESSAGES 100
static umsg32 all_messages[ N_MESSAGES ] = {0};
static int test1_ok = 1;
int part3_test1_recvhandler(void){
    if( currpid != sender_pid   && _running == 1){
      kprintf("Test1: Failed callback pid = %d, sender pid=%d, receiver pid=%d\n", currpid, sender_pid, receiver_pid);
      test1_ok = 0;
    }

    msg_count++;
    kprintf("msg received = %d\n", __buf);

    all_messages[ __buf ] = 1;
    return(OK);
}
static void  lab4_part3_test1_receiver(){
  receiver_pid = getpid();
  registercb( &__buf, &part3_test1_recvhandler );
  while(1){
    sleepms(1);
  }
}
static void  lab4_part3_test1_sender(){
  sender_pid = getpid();
  int n;
  for( n= 0; n< N_MESSAGES; n++ ){
    send(receiver_pid, n);
    sleepms(5);
  }
}
static void  lab4_part3_test1(){
  int n;
  // create a receiver
  resume(create(lab4_part3_test1_receiver, 1024, PRI, "receiver", 0));
  //
  sleep(1);
  // create a sender
  resume(create(lab4_part3_test1_sender, 1024, PRI, "sender", 0));

  sleep(20);
  if( msg_count > 0 ){
    kprintf("Test2: OK received at least one message\n");
  }else{
    kprintf("Test2: Failed. did not receive any messages\n");
  }
  if( msg_count == N_MESSAGES ){
    kprintf("Test3: OK. received all messages\n");
  }else{
    kprintf("Test3: Failed. received %d messages, expect %d messages\n", msg_count, N_MESSAGES);
  }
  // check correctness.
  int test3 = 1;
  for( n=0; n< N_MESSAGES; n++ ){
    if( all_messages[ n ] == 0 ){
      kprintf("Test4: Failed. %d th message not received\n", n );
      test3 = 0;
    }
  }
  if( test3 == 1 ){
    kprintf("Test4: OK. all messages received\n" );
  }
  if( test1_ok == 1   && _running == 1){
    kprintf("Test1: OK. callback process is the sender\n" );

  }
}

#endif

#ifdef _BONUS_
//static umsg32 __buf = 0;
static pid32 sender_pid, receiver_pid;
static msg_count= 0;
#define N_MESSAGES 100
static umsg32 all_messages[ N_MESSAGES ] = {0};
static int test1_ok = 1;
int bonus_test1_recvhandler(void){
    umsg32 m = receive();
    if( m == SYSERR ){
      kprintf("TestX: Failed. receive() returns SYSERR\n");
      return SYSERR;
    }

    if( currpid != receiver_pid   && _running == 1){
      kprintf("Test1: Failed callback pid = %d, sender pid=%d, receiver pid=%d\n", currpid, sender_pid, receiver_pid);
      test1_ok = 0;
    }

    msg_count++;
    kprintf("msg received = %d\n", m);

    all_messages[ m ] = 1;
    return(OK);
}
static void  lab4_bonus_test1_receiver(){
  receiver_pid = getpid();
  registercb2( &bonus_test1_recvhandler );
  while(1){
    sleepms(1);
  }

  sleep(21);
}
static void  lab4_bonus_test1_sender(){
  sender_pid = getpid();
  int n;
  for( n= 0; n< N_MESSAGES; n++ ){
    send(receiver_pid, n);
    sleepms(5);
  }
  sleep(21);
}
static void  lab4_bonus_test1(){
  int n;
  // create a receiver
  resume(create(lab4_bonus_test1_receiver, 1024, PRI, "receiver", 0));
  //
  sleep(1);
  // create a sender
  resume(create(lab4_bonus_test1_sender, 1024, PRI, "sender", 0));

  sleep(20);
  if( msg_count > 0 ){
    kprintf("Test2: OK received at least one message\n");
  }else{
    kprintf("Test2: Failed. did not receive any messages\n");
  }
  if( msg_count == N_MESSAGES ){
    kprintf("Test3: OK. received all messages\n");
  }else{
    kprintf("Test3: Failed. received %d messages, expect %d messages\n", msg_count, N_MESSAGES);
  }
  // check correctness.
  int test3 = 1;
  for( n=0; n< N_MESSAGES; n++ ){
    if( all_messages[ n ] == 0 ){
      kprintf("Test4: Failed. %d th message not received\n", n );
      test3 = 0;
    }
  }
  if( test3 == 1 ){
    kprintf("Test4: OK. all messages received\n" );
  }
  if( test1_ok == 1  && _running == 1){
    kprintf("Test1: OK. callback process is the receiver\n" );

  }
}
#endif

int main(int argc, char **argv)
{
  

  kprintf("======================Start Testing=====================\r\n");
  _running = 1;
#ifdef _PART1_
	sem1 = semcreate(1);
	sem2 = semcreate(1);	
	sem3 = semcreate(1);

  /*lab4_part1_test1();
  sleep(5);*/
  /*lab4_part1_test2();
  sleep(5);*/
  lab4_part1_test3();
  sleep(5);
  /*lab4_part1_test4();*/
#endif
#ifdef _PART2_
  uint32 retv ;
  int i;

  sema = semcreate(0);

  weichiu_test0();

  weichiu_test1();

  weichiu_test2();

  weichiu_test3();

  weichiu_test4();
  
  weichiu_test5();

  weichiu_test6();

  weichiu_test7();

  weichiu_test8();

  weichiu_test9();

  kprintf("Total Score: %d\r\n", score );
#endif

#ifdef _PART3_
  lab4_part3_test1();
#endif

#ifdef _BONUS_
  lab4_bonus_test1();

#endif
  kprintf("======================End of Test=====================\r\n");
  _running = 0;

  return OK;
}
