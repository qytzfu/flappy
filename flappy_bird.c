// 大纲
// 1.先柱子
// 2.然后多个柱子
// 3.然后移动柱子
// 4.加鸟
// 5.移动
// 6.判断碰撞


#include <unistd.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>
// pillar wide
#define WIDE 3
#define MOVE_SPEED 10
#define JUMP_UP 3
#define JUMP_DOWN 1
#define PILLAR_WIDE 5
#define FINAL_INTERVAL 5


//修改一下图形块 version 2

typedef struct pillar{
    int y;
    int x;
    int mid;
    struct pillar *next;
}Pillar;

typedef struct bird{
    int y;
    int x;
}Bird;
//version 2 鸟和柱子可以分开到两个不同的窗口

typedef Pillar *ptr_pillar;
void erasePillar(ptr_pillar ptr);
void printPillar(ptr_pillar ptr);
void addPillar();
void delPillar();
void printBird();
void eraseBird();
void birdMove();
int collision();

ptr_pillar head,tail,tmp;
Bird little_bird = {5,5};

int lose = 0;

int collision()
{
    if(little_bird.y<0 || little_bird.y > LINES-1)
        return 1;
    ptr_pillar tmp3 = head;
    while(tmp3->next)
    {
        int tmp_x = tmp3->next->x;
        int tmp_mid = tmp3->next->mid;
        //&& 和 || 优先级
        if(tmp_x<=little_bird.x &&tmp_x+3>little_bird.x && (little_bird.y<tmp_mid || little_bird.y >= tmp_mid+PILLAR_WIDE))
            return 1;
        tmp3 = tmp3->next;
    }
    return 0;
}

void birdMove()
{
    char c = getch();
    if(c == ' ')
        //这里在version 2 里面, 可以改成 自由落体
        little_bird.y -= JUMP_UP;
    if(c == ERR)
        little_bird.y += JUMP_DOWN;
}

void printBird()
{
    mvprintw(little_bird.y,little_bird.x,">");
}
void eraseBird()
{
    mvprintw(little_bird.y,little_bird.x," ");
}


void printPillar(ptr_pillar ptr)
{
    for(int y=0;y<LINES;y++)
        if(y < ptr->mid || y>=ptr->mid+PILLAR_WIDE)
        {
            for(int x=0;x<WIDE;x++)
            {
                if(ptr->x + x<COLS && ptr->x+3>=0)
                    //这里超过界的是靠库函数处理的, 应该要自己处理,version2里面该一改
                    mvprintw(y,ptr->x + x,"@");
            }
        }
}

void erasePillar(ptr_pillar ptr)
{
    for(int y=0;y<LINES;y++)
        for(int x=0;x<WIDE;x++)
        {
            if(ptr->x + x<COLS && ptr->x+3>=0)
                mvprintw(y,ptr->x + x," ");
        }
}
void addPillar()
{
    tail->next = malloc(sizeof(Pillar));
    tail = tail->next;
    //根本碰不到墙壁:
    //初始的地点换一个随机,....version2
    // tail->x = COLS-1;
    //好像这个random 连续性太强了....version 2 修改
    srand((unsigned) time(NULL));
    //这个5大小是自己控制的
    tail-> x = COLS-1-rand()%FINAL_INTERVAL;


    //随机数


    //去除极端的上下,还有管道宽
    tail->mid = rand()%(LINES-10-PILLAR_WIDE) + 5;
    printPillar(tail);

}
void delPillar()
{
    if(!head->next)
        return;
    if(head->next->x+3<0)
    {
        ptr_pillar tmp2;
        tmp2 = head->next;
        head->next = tmp2->next;
        //怪不得打印的时候有问题:
        //这里的指针已经丢了..,这里删的是错的指针..

        //还有错..删除一个之后就不运行了,,,有问题..
        //我尝试了一下,不知道为什么, 把指针改成null 就可以,
        //难道是free之后 还会造成什么影响么.
        tmp2->next = NULL;

        free(tmp2);
    }
}

int main()
{

    //version 1
    //柱子个数不做在pillar 结构中

    char c= 'a';
    initscr();
    //init
    head = malloc(sizeof(Pillar));
    tail = head;
    tmp = head;
    head->next =NULL;
    //cbreak 模式
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);



    printBird();

    while(!lose)
    {

        eraseBird();
        birdMove();
        printBird();
        // 先移动在删减
        tmp = head;
        while(tmp->next)
        {
            erasePillar(tmp->next);
            tmp->next->x -= MOVE_SPEED;

            printPillar(tmp->next);
            tmp = tmp->next;
        }

        addPillar();
        delPillar();

        //测试
        // tmp = head;
        // int count = 0;
        // while(tmp->next)
        // {
        //     mvprintw(i,5+3*count,"%d",tmp->next->x);
        //     count++;
        //     tmp = tmp->next;
        // }
        // mvprintw(i,0,"%d",count);
        refresh();
        lose = collision();
        // sleep(1);
        //这里没有sleep,是因为还没去看sleep是不是最少1秒的..
        usleep(40*1000);

    }


    mvprintw(0,0,"you have lose");
    refresh();
    sleep(3);

    endwin();
    exit(EXIT_SUCCESS);
}