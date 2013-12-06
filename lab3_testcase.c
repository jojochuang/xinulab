/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>
extern void cpuintensive(void);
extern void iointensive(void);
extern void hybridprocess(void);
//extern void starvationPrev(uint32);
void create_cpuintensive(){

	resume(create(cpuintensive, 40960, 20, "CPU-intensive1", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive2", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive3", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive4", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive5", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive6", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive7", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive8", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive9", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive10", 0));
}
void create_iointensive(){

	resume(create(iointensive, 40960, 20, "IO-intensive1", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive2", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive3", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive4", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive5", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive6", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive7", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive8", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive9", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive10", 0));
}
void create_halfhalf(){
	resume(create(iointensive, 40960, 20, "IO-intensive6", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive7", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive8", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive9", 0));
	resume(create(iointensive, 40960, 20, "IO-intensive10", 0));

	resume(create(cpuintensive, 40960, 20, "CPU-intensive1", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive2", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive3", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive4", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive5", 0));
}
void create_hybrid(){

	resume(create(hybridprocess, 40960, 20, "HYBRID1", 0));
	resume(create(cpuintensive, 40960, 20, "CPU-intensive1", 0));
}
int main(int argc, char **argv)
{
	//resume(create(create_cpuintensive, 40960, 58 , "proc creator", 0));
	//resume(create(create_iointensive, 40960, 58 , "proc creator", 0));
	//resume(create(create_halfhalf, 40960, 58 , "proc creator", 0));
	resume(create(create_hybrid, 40960, 58 , "proc creator", 0));

	return OK;
}
