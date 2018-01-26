#include "stdio.h"
#include "stdlib.h"
#include <unistd.h> 
#include "timermanager.h"

void TestProc(void *param)
{
	printf("param:%s\n",(char*)param);
}

int main()
{
	CTimerManager *timer_mgr = new CTimerManager();
	char *test = "test";
	STimerNode *node = timer_mgr->AddTimer(10000,TestProc,test);//10秒

	while(1)
	{
		timer_mgr->DetectTimerList();
		usleep(1000);
	}
	
	delete timer_mgr;
	return 0;
}