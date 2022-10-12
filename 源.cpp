#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#define MY_BUFSIZE 1024    //控制台窗口标题的缓冲区大小
#define INITIALTIME 200

//蛇的状态(上,下,左,右)
#define U 1    
#define D 2    
#define L 3    
#define R 4    



typedef struct Snake
{
	int x;                  //节点的x坐标
	int y;                  //节点的y坐标
	struct Snake* next;     //蛇身的下一个节点
}snake;

int score = 0, add = 0;          //总得分与每次吃食物的得分
int HighScore = 0;               //最高的得分
int status;                      //蛇的前进状态
int sleeptime = INITIALTIME;     //蛇的时间间隔
int endgamestatus = 0;           //游戏的结束的情况: 撞到墙,咬到自己,主动退出游戏

HANDLE hOut;                     //控制台句柄
snake* head, * food;             //蛇头指针,食物指针
snake* q;                        //遍历蛇时用到的指针

void gotoxy(int x, int y);       //设置光标位置
void color(int col);             //更改文字颜色
void printsnake();               //字符画,蛇
void welcometogame();            //开始界面
void createMap();                //绘制地图
void scoreandtips();             //游戏右侧的得分和小提示
void initsnake();                //初始化蛇身(画出蛇身)
void createfood();               //创见并随机出击食物
bool biteself();                  //判断是否咬到自己
void cantcrosswall();            //判断是否撞墙
void speedup();                  //加速
void speedown();                 //减速
void snakemove();                //控制蛇的前进方向
void keyboardControl();          //控制键盘按键
void Lostdraw();                 //游戏结束界面
void endgame();                  //游戏结束
void choose();                   //游戏失败之后的选择
void File_out();                 //从文件中读取最高分
void File_in();                  //将最高分存入文件
void explation();                //游戏说明
HWND GetConsoleHwnd();           //获取当前句柄
int main(void)
{
	//设置窗口
	system("mode con cols=100 lines=30");
	SetWindowLongPtr(GetConsoleHwnd(), GWL_STYLE, WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

	printsnake();
	welcometogame();
	File_out();
	keyboardControl();
	endgame();
}

//设置颜色
void color(int col)
{
	//调用API改变控制台字体颜色
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
}

//改变字体位置
void gotoxy(int x, int y)
{
	COORD Position;
	Position.X = x;
	Position.Y = y;
	//调用API改变字体位置
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Position);
}

//字符画,蛇
void printsnake()
{
	color(6);                   //字体颜色设置为黄色 

	gotoxy(32, 3);
	printf("~~~~=_=~~~~\\\\   <----------------------说出来你信吗,我是条蛇..");

	gotoxy(44, 4);
	printf("\\\\");

	gotoxy(45, 5);
	printf("\\\\");

	gotoxy(46, 6);
	printf("\\\\");

	gotoxy(47, 7);
	printf("\\\\");

	gotoxy(48, 8);
	printf("=========================");

	gotoxy(49, 26);
	return;
}

//开始界面
void welcometogame()
{
	int choice;
	int x, y;
	int count1 = 0, count2 = 0;
	gotoxy(37, 15);
	color(11);
	printf("贪   吃   蛇   大   作   战");

	for (x = 20; x < 80; x++)
	{
		count2++;
		for (y = 17; y < 26; y++)
		{
			count1++;
			if (x == 20 || x == 79)
			{
				if (count1 % 2 == 0)
					color(9);
				else
					color(13);

				gotoxy(x, y);
				printf("|");
			}

			if (y == 17 || y == 25)
			{
				if (count2 % 2 == 0)
					color(9);
				else
					color(13);

				gotoxy(x, y);
				printf("-");
			}
		}
	}

	gotoxy(32, 19);
	color(13);
	printf("1:开始游戏");

	gotoxy(58, 19);
	printf("2:游戏说明");

	gotoxy(32, 23);
	printf("3:退出游戏");

	gotoxy(43, 26);
	printf("请选择[1 2 3]:[ ]\b\b");
	scanf("%d", &choice);

	switch (choice)
	{
	case 1:
		system("cls");
		createMap();
		initsnake();
		createfood();
		keyboardControl();
		break;

	case 2:
		explation();
		break;

	case 3:  exit(0);          break;

	default:

		gotoxy(40, 28);
		color(12);
		printf("输入错误,请输入1~3的数!");
		_getch();             //除了vs系列之外的编译器需要把这一行改成: getch();
		system("cls");
		printsnake();
		welcometogame();
		break;

	};
	return;
}

//打印地图中的方块
void createMap()
{
	int i, j;
	int count = 0;
	for (i = 0; i < 58; i += 2)
	{
		color(13);         //
		gotoxy(i, 0);
		printf("■");
		gotoxy(i, 26);
		printf("■");
	}

	for (i = 1; i < 26; i++)
	{
		color(13);         //
		gotoxy(0, i);
		printf("■");
		gotoxy(56, i);
		printf("■");
	}

		/*之间是动画效果(虽然不太好看)如果不要可以删除*/


		color(13);
	for (j = 1; j < 26; j += 2)
	{
		for (i = 2; i < 56; i += 4)
		{
			gotoxy(i, j);
			printf("■");
			Sleep(2);       //增加点延时效果
		}
	}

	color(10);
	for (i = 2; i < 56; i += 4)
	{
		for (j = 2; j < 26; j += 2)
		{
			gotoxy(i, j);
			printf("■");
			Sleep(2);       //增加点延时效果
		}
	}


	/*结束*/



	color(3);
	for (i = 2; i < 56; i += 2)
	{
		for (j = 1; j < 26; j++)
		{
			gotoxy(i, j);
			printf("■");
		}
		Sleep(10);       //增加点延时效果
	}
	return;
}

//游戏右侧的得分和小提示
void scoreandtips()
{
	int i;
	//File_out();         //读取文件中的最高分数
	gotoxy(64, 4);
	color(11);
	printf("☆历史最高分为: %d ☆", 1);

	gotoxy(64, 8);
	color(14);
	printf("得分: %d", 2);

	gotoxy(73, 11);
	color(13);
	printf("小 提 示");

	gotoxy(60, 13);              //第一条线 
	color(9);
	printf("╬ ");
	for (i = 63; i < 92; i++)
	{
		if (i % 2 == 0)
			color(9);
		else
			color(13);
		printf("-");
		gotoxy(i, 13);
	}
	gotoxy(91, 13);
	color(9);
	printf("╬ ");


	/*以下是两条线中间的内容*/

	color(3);
	gotoxy(64, 14);
	printf("每个食物得分: %d分", add);
	gotoxy(64, 16);
	printf("不能撞墙和咬到自己哦~");
	gotoxy(64, 18);
	printf("用 ↑ ↓ ← → 分别控制蛇的移动");
	gotoxy(64, 20);
	printf("F1:加速。F2:减速");
	gotoxy(64, 22);
	printf("空格键：暂停游戏");
	gotoxy(64, 24);
	printf("Esc：暂停游戏");

	/*线束*/
		gotoxy(60, 25);              //第二条线 
	color(13);
	printf("╬ ");
	for (i = 63; i < 92; i++)
	{
		if (i % 2 == 0)
			color(9);
		else
			color(13);
		printf("-");
		gotoxy(i, 25);
	}
	gotoxy(91, 25);
	color(13);
	printf("╬ ");
	return;
}

//从文件中读取最高分
void File_out()
{
	FILE* fp = NULL;
	fp = fopen("snake.data", "r+");
	if (fp == NULL)     //判断文件是否打开成功,失败则提示并退出游戏
	{
		//下面这一行是vs的问题,其它编译器需要用MessageBox把MessageBox给替换掉(其它编译器中 char* 类型是可以直接转换为 LPCWSTR 类型的而vs系列中改为Unicode字符集还是不可以)
		MessageBoxA(NULL, "Data file read / write failure,Please press OK to exit", "Greedy_snake", MB_OK | MB_ICONERROR);
		exit(1);
	}

	fscanf(fp, "%d", &HighScore);    //读取数据
	fclose(fp);                      //关闭文件
}

//初始化蛇身
void initsnake()
{
	snake* tail;
	int i;
	tail = (snake*)malloc(sizeof(snake));       //这是蛇尾从蛇尾开始画(链表的最后一代节点),用头插法,以x,y设为开始位置

	tail->x = 24;        //初始化位置为24,5
	tail->y = 5;
	tail->next = NULL;

	for (i = 0; i <= 4; i++)        //设置蛇身长度为:5
	{
		head = (snake*)malloc(sizeof(snake));
		head->next = tail;             //蛇头的下一位置为蛇尾

		head->x = 24 + 2 * i;          //设置蛇头的位置
		head->y = 5;

		tail = head;                   //蛇头变成蛇尾,然后重复循环(可能有点绕,画画就理解了)
	}

	while (tail != NULL)
	{
		color(14);
		gotoxy(tail->x, tail->y);
		printf("■");               //蛇身用 ■ 表示
		tail = tail->next;         //蛇身一点一点输出,一直输出到蛇尾
	}
	return;
}

//随机产生食物
void createfood()
{
	snake* food_1 = NULL;

	food_1 = (snake*)malloc(sizeof(snake));

	food_1->x = 1;
	food_1->y = 0;
	food_1->next = NULL;

	srand((unsigned)time(NULL));     //初始化随机数 

	//food_1->x = rand() % 52 + 2;

	while ((food_1->x % 2) != 0)
	{
		food_1->x = rand() % 52 + 2;          //食物的x坐标(52是右边倒数第二列的x坐标, +2是因为防止出现在边框上)
	}

	food_1->y = rand() % 24 + 1;               //食物的y坐标(为什么要 %24 和 +1 的原因和上面一样)  

	q = head;

	while (q->next == NULL)                    //!!!!!!!!!!!!!!!!!!!!!!!!!1
	{
		if (q->x == food_1->x && q->y == food_1->y)
		{
			free(food_1);    //如果蛇与食物重合则释放食物指针

			createfood();    //重新创建食物
		}
		q = q->next;
	}
	gotoxy(food_1->x, food_1->y);  //设置食物位置

	food = food_1;
	color(12);
	printf("●");               //以红色颜色输出食物

	return;
}

//判断是否咬到自己
bool biteself()
{
	snake* self;               //self是蛇身上的一个节点(除了蛇身之外的节点)
	self = head->next;

	while (self != NULL)       //判断蛇头是否与蛇身相同,相同就是咬到自己了,咬到自己就返回true否则返回fslse
	{
		if (self->x == head->x && self->y == head->y)
			return true;
		self = self->next;
	}
	return false;
}

//判断是否撞墙
void cantcrosswall()
{
	if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)  //判断是否撞到墙
	{
		endgamestatus = 1;              //结束游戏的一种情况,endgamestatus等于1时为失败界面
		endgame();
	}
}

//加速
void speedup()
{
	if (sleeptime >= 50)       //如果时间大于或等于50执行下面的代码
	{
		sleeptime += 10;       //时间间隔减10
		add += 2;              //每次吃到的食物的分数加2
	}
}

//减速
void speeddown()
{
	if (sleeptime < 350)       //如果时间大于或等于50执行下面的代码
	{
		sleeptime += 30;       //时间间隔减10
		add += 2;              //每次吃到的食物的分数加2
		if (sleeptime == 350)
		{
			add = 1;           //保证最低分为 1 分
		}
	}
}

//蛇的移动方向
void snakemove()
{
	snake* nexthead;

	cantcrosswall();
	nexthead = (snake*)malloc(sizeof(snake));


	/*
		下面语句中只有下面这两段代码需要更改其它的都不而要复制一下就好
			nexthead->x = head->x;
			nexthead->y = head->y - 1;
	*/

	if (status == U)    //向上移动
	{
		nexthead->x = head->x;
		nexthead->y = head->y - 1;

		nexthead->next = head;
		head = nexthead;
		q = head;          //指向链表头准备遍历链表

		if (nexthead->x == food->x && nexthead->y == food->y)      //如果遇到了食物
		{
			color(14);
			while (q != NULL)            //打印蛇身
			{
				gotoxy(q->x, q->y);
				printf("■");
				q = q->next;
			}
			score += add;               //吃了食物后加分
			speedup();                  //并提升速度
			createfood();               //吃了食物后再创建一个
		}
		else           //如果没遇到食物
		{
			color(14);
			while (q->next->next != NULL)               //蛇往前移动后前面加一,后面减一   画出移动后蛇身前n-1个格子(画一画就理解了) 
			{
				gotoxy(q->x, q->y);
				printf("■");
				q = q->next;
			}

			color(3);
			gotoxy(q->x, q->y);
			printf("■");              //画出蛇身倒数第二个蛇身格子
			free(q->next);            //释放掉原来的最后一个蛇身格子
			q->next = NULL;
		}
	}
	if (status == D)     //向下移动
	{

		nexthead->x = head->x;
		nexthead->y = head->y + 1;

		nexthead->next = head;
		head = nexthead;
		q = head;          //指向链表头准备遍历链表

		if (nexthead->x == food->x && nexthead->y == food->y)      //如果遇到了食物
		{
			color(14);
			while (q != NULL)            //打印蛇身
			{
				gotoxy(q->x, q->y);
				printf("■");
				q = q->next;
			}
			score += add;               //吃了食物后加分
			speedup();                  //并提升速度
			createfood();               //吃了食物后再创建一个
		}
		else
		{
			color(14);
			while (q->next->next != NULL)               //蛇往前移动后前面加一,后面减一   画出移动后蛇身前n-1个格子(画一画就理解了) 
			{
				gotoxy(q->x, q->y);
				printf("■");
				q = q->next;
			}

			color(3);
			gotoxy(q->x, q->y);
			printf("■");              //画出蛇身倒数第二个蛇身格子
			free(q->next);            //释放掉原来的最后一个蛇身格子
			q->next = NULL;
		}
	}

	if (status == L)     //向左移动
	{

		nexthead->x = head->x - 2;
		nexthead->y = head->y;

		nexthead->next = head;
		head = nexthead;
		q = head;          //指向链表头准备遍历链表

		if (nexthead->x == food->x && nexthead->y == food->y)      //如果遇到了食物
		{
			color(14);
			while (q != NULL)            //打印蛇身
			{
				gotoxy(q->x, q->y);
				printf("■");
				q = q->next;
			}
			score += add;               //吃了食物后加分
			speedup();                  //并提升速度
			createfood();               //吃了食物后再创建一个
		}
		else
		{
			color(14);
			while (q->next->next != NULL)               //蛇往前移动后前面加一,后面减一   画出移动后蛇身前n-1个格子(画一画就理解了) 
			{
				gotoxy(q->x, q->y);
				printf("■");
				q = q->next;
			}

			color(3);
			gotoxy(q->x, q->y);
			printf("■");              //画出蛇身倒数第二个蛇身格子
			free(q->next);            //释放掉原来的最后一个蛇身格子
			q->next = NULL;
		}
	}

	if (status == R)     //向右移动
	{

		nexthead->x = head->x + 2;
		nexthead->y = head->y;

		nexthead->next = head;
		head = nexthead;
		q = head;          //指向链表头准备遍历链表

		if (nexthead->x == food->x && nexthead->y == food->y)      //如果遇到了食物
		{
			color(14);
			while (q != NULL)            //打印蛇身
			{
				gotoxy(q->x, q->y);
				printf("■");
				q = q->next;
			}
			score += add;               //吃了食物后加分
			speedup();                  //并提升速度
			createfood();               //吃了食物后再创建一个
		}
		else
		{
			color(14);
			while (q->next->next != NULL)               //蛇往前移动后前面加一,后面减一   画出移动后蛇身前n-1个格子(画一画就理解了) 
			{
				gotoxy(q->x, q->y);
				printf("■");
				q = q->next;
			}

			color(3);
			gotoxy(q->x, q->y);
			printf("■");              //画出蛇身倒数第二个蛇身格子
			free(q->next);            //释放掉原来的最后一个蛇身格子
			q->next = NULL;
		}
	}

	if (biteself() == true)           //判断是否咬到自己
	{
		endgamestatus = 2;
		endgame();
	}
	return;
}

//控制键盘按键
void keyboardControl()
{
	status = R;     //初始化向右移动

	while (1)
	{
		scoreandtips();      //游戏右边的小提示和得分

		//GetAsyncKeyState函数用来判断调用时指定虚假键的状态
		if (GetAsyncKeyState(VK_UP) && status != D)         //如果按的不是 下 键则向 上 移动
			status = U;
		else if (GetAsyncKeyState(VK_DOWN) && status != U)  //如果按的不是 上 键则向 下 移动
			status = D;
		else if (GetAsyncKeyState(VK_LEFT) && status != R)  //如果按的不是 右 键则向 左 移动
			status = L;
		else if (GetAsyncKeyState(VK_RIGHT) && status != L) //如果按的不是 左 键则向 右 移动
			status = R;

		if (GetAsyncKeyState(VK_SPACE))          //按空格键暂停
		{
			while (1)
			{
				Sleep(300);
				if (GetAsyncKeyState(VK_SPACE))          //再按一次空格键继续
					break;
			}
		}
		else if (GetAsyncKeyState(VK_ESCAPE))            //按Esc键转到结束界面
		{
			endgamestatus = 3;
			break;
		}
		else if (GetAsyncKeyState(VK_F1))                //F1加速
			speedup();
		else if (GetAsyncKeyState(VK_F2))                //F2减速
			speeddown();

		Sleep(sleeptime);            //移动速度
		snakemove();                 //没有按这几个键时继续移动
	}
	return;
}

//失败界面
void Lostdraw()
{
	int i;
	system("cls");

	//四个角上的五角星			
	color(13);
	gotoxy(4, 6);
	printf("★");
	gotoxy(96, 6);
	printf("★");
	gotoxy(4, 29);
	printf("★");
	gotoxy(96, 29);
	printf("★");

	//打印边框的四条边
	for (i = 6; i < 96; i++)
	{
		if (i % 2 == 0)
			color(3);
		else
			color(13);

		gotoxy(i, 6);
		printf("-");
		gotoxy(i, 29);
		printf("-");
	}

	for (i = 7; i < 29; i++)
	{
		if (i % 2 == 0)
			color(3);
		else
			color(13);
		gotoxy(4, i);
		printf("|");
		gotoxy(97, i);
		printf("|");
	}

	//打印边框上的装饰
	color(14);
	gotoxy(30, 6);
	printf("o00o");
	gotoxy(71, 6);
	printf("o00o");

	gotoxy(48, 5);
	color(14);
	printf("<  @ @  >");

	gotoxy(50, 6);
	color(3);
	printf("《_》");

	gotoxy(47, 29);
	color(13);
	printf("☆☆☆☆☆☆");
	return;
}

//结束游戏
void endgame()
{
	system("cls");
	if (endgamestatus == 1)
	{
		Lostdraw();
		gotoxy(35, 9);
		color(12);
		printf("对不起，您撞到墙了，游戏结束!");
	}
	else if (endgamestatus == 2)
	{
		Lostdraw();
		gotoxy(35, 9);
		color(14);
		printf("对不起，您咬到自己了，游戏结束!");
	}
	else if (endgamestatus == 3)
	{
		Lostdraw();
		gotoxy(40, 9);
		color(14);
		printf("您已经结束了游戏。");
	}

	gotoxy(43, 12);
	color(13);
	printf("您的得分是: %d", score);
	if (score >= HighScore)
	{
		color(10);
		gotoxy(33, 16);
		printf("创新纪录！最高分被您刷新啦，真厉害！");
		File_in();
	}
	else
	{
		color(10);
		gotoxy(33, 16);
		printf("继续努力哦,您离最高分只差 %d 喽~", HighScore - score);
	}
	choose();
	return;
}

//把最高分写入文件
void File_in()
{
	FILE* fp;
	fp = fopen("snake.data", "w+");    //以只写的方式打开文件
	fprintf(fp, "%d", score);          //从文件中读出最高分写入到score中
	fclose(fp);                        //关闭文件
}

//边框下面的选项
void choose()
{
	int n;
	gotoxy(25, 23);
	color(10);
	printf("我要重新玩一局------1");
	gotoxy(52, 23);
	printf("不玩了，退出吧------2");
label:
	gotoxy(46, 25);
	color(13);
	printf("选择：[   ]\b\b\b");
	scanf("%d", &n);
	switch (n)
	{
	case 1:
		system("cls");
		score = 0;                     //分数初始化为 0
		sleeptime = INITIALTIME;       //速度初始化为 0
		add = 10;                      //食物得分为初始化为 10
		printsnake();                  //返回欢迎界面
		welcometogame();

	case 2:
		exit(0);
		break;
	default:
		gotoxy(35, 27);
		color(12);
		printf("※※您输入的有误，请从新输入※※");
		system("pause >nul");
		endgame();
		choose();               //边框下的分支选项
		goto label;
		break;
	}
	return;
}

//游戏说明
void explation()
{
	int i, j = 1;
	system("cls");
	gotoxy(42, 4);
	printf("游   戏   说   明");
	color(2);
	for (i = 6; i <= 25; i++)
	{
		for (j = 20; j <= 76; j++)
		{
			gotoxy(j, i);
			if (i == 6 || i == 25)
			{
				if (i % 2 == 0)
					color(3);
				else
					color(13);
				printf("=");
			}
			else if (j == 20 || j == 76)
			{
				if (i % 2 == 0)
					color(3);
				else
					color(13);
				printf("||");
			}
		}
	}

	color(13);
	gotoxy(76, 6);
	printf("★");
	gotoxy(76, 25);
	printf("★");

	color(3);
	gotoxy(30, 8);
	printf("tipl: 不能撞墙，不能咬到自己");

	color(10);
	gotoxy(30, 11);
	printf("tip2: 用↑，↓，←，→分别控制蛇的移动");

	color(13);
	gotoxy(30, 14);
	printf("tip3: F1是加速，F2是减速");

	color(14);
	gotoxy(30, 17);
	printf("tip4: 按空格键暂停游戏，再按一次继续游戏");

	color(4);
	gotoxy(30, 20);
	printf("tip5: Esc键退出游戏");

	color(1);
	gotoxy(30, 23);
	printf("tip6: 自己的废物自己需要清理");

	_getch();                        //按任任意键返回主界面

	system("cls");
	printsnake();
	welcometogame();
}

HWND GetConsoleHwnd()            //获取窗口句柄      
{

	HWND hwndFound;                      //最后获得的句柄 
	WCHAR pszNewWindowTitle[MY_BUFSIZE]; //窗口标题

	WCHAR pszOldWindowTitle[MY_BUFSIZE]; //旧的句柄 


	//获取当前窗口标题

	GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

	//设置唯一窗口名子。 

	char szStr[MY_BUFSIZE];

	sprintf(szStr, "%d%d", GetTickCount(), GetCurrentProcessId());   //因为为了兼容下面的MessagrBox这里用的

	memset(pszNewWindowTitle, 0, sizeof(pszNewWindowTitle));
	MultiByteToWideChar(CP_ACP, 0, szStr, strlen(szStr) + 1, pszNewWindowTitle, sizeof(pszNewWindowTitle) / sizeof(pszNewWindowTitle[0]));

	//更改当前窗口标题 
	SetConsoleTitle(pszNewWindowTitle);

	//确保窗口标题已更新 
	Sleep(40);

	//查找新窗口标题
	hwndFound = FindWindow(NULL, pszNewWindowTitle);

	//还原原始窗口标题
	SetConsoleTitle(pszOldWindowTitle);

	return(hwndFound);        //返回句柄
}
