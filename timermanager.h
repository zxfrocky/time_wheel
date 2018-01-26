#ifndef APE_COMMON_TIMER_MANAGER_H_
#define APE_COMMON_TIMER_MANAGER_H_
#include <stdint.h>
#include <list>

class CThreadTimer;
//#define GRANULARITY 10 //10ms 最低频度
#define GRANULARITY 1 //1ms
#define WHEEL_BITS1 8
#define WHEEL_BITS2 6
#define WHEEL_SIZE1 (1 << WHEEL_BITS1) //256
#define WHEEL_SIZE2 (1 << WHEEL_BITS2) //64
#define WHEEL_MASK1 (WHEEL_SIZE1 - 1)
#define WHEEL_MASK2 (WHEEL_SIZE2 - 1)
#define WHEEL_NUM 5

typedef void (*HandleProc)(void *param);

typedef struct stNodeLink 
{
    stNodeLink *prev;
    stNodeLink *next;
    stNodeLink() {prev = next = this;} //circle
}SNodeLink;

typedef struct stTimerNode 
{
    SNodeLink link;
    uint64_t dead_time;
    HandleProc proc;
    void *param;
    stTimerNode(uint64_t dt,HandleProc handle_proc, void *handle_param) :  
    dead_time(dt), proc(handle_proc),param(handle_param) {}
}STimerNode;

typedef struct stWheel 
{
    SNodeLink *spokes;
    uint32_t size;
    uint32_t spokeindex;
    stWheel(uint32_t n) : size(n), spokeindex(0)
    {
        spokes = new SNodeLink[n];
    }
    ~stWheel() 
    {
        if (spokes) 
        {
            for (int j = 0; j < size; ++j) 
            {
                SNodeLink *link = (spokes + j)->next;
                while (link != spokes + j) 
                {
                    STimerNode *node = (STimerNode *)link;
                    link = node->link.next;
                    delete node;
                }
            }
            delete []spokes;
            spokes = NULL;
        }
    }
}SWheel;

class CTimerManager 
{
  public:
    CTimerManager();
    virtual ~CTimerManager();
    void DetectTimerList();
    STimerNode* AddTimer(uint32_t seconds, HandleProc handle_proc,void *param);
    void RemoveTimer(STimerNode* node);
    virtual void Dump();

  private:
    uint32_t Cascade(uint32_t wheelindex);
    void AddTimerNode(uint32_t milseconds, STimerNode *node);
    void AddToReadyNode(STimerNode *node);
    void DoTimeOutCallBack();

  private:
    SWheel *wheels_[WHEEL_NUM];
    uint64_t checktime_;
    SNodeLink readynodes_;
};
#endif