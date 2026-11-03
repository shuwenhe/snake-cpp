#include <ncurses.h> //引入ncurses库头文件
#include <stdlib.h> //引入标准库
#include <time.h> //引入时间库
#define WORLD_WIDTH 50 //定义游戏世界宽度为50
#define WORLD_HEIGHT 20 //定义游戏世界高度为20

typedef struct spart //定义蛇的身体部分结构体
{
	int x; //x坐标
	int y; //y坐标
} snakeypart; //结构体别名

enum direction //定义蛇的移动方向枚举
{
	UP, //向上
	DOWN, //向下
	RIGHT, //向右
	LEFT //向左
}; //方向枚举						//
WINDOW *snakeys_world; //游戏窗口指针
int SnakeLength = 5; //蛇的长度初始为5
int Score = 0; //游戏分数初始为0
snakeypart snakey[120]; //蛇身体数组，最多120个节点
snakeypart food; //食物坐标
snakeypart tem; //临时变量，存储蛇尾坐标
int direction = RIGHT; //初始方向向右
int ch; //存储用户输入的字符
int speed = 400; //游戏速度（毫秒）

void Initialization(void); //初始化函数声明
void WaitGame(void); //等待游戏开始函数声明
void Game(void); //主游戏函数声明
void GameOver(void); //游戏结束函数声明
void CreateSnake(void); //创建蛇函数声明
bool RandomFood(void); //生成食物函数声明

int main() //主函数
{
	Initialization(); //初始化游戏
	Game(); //开始游戏
	return 0; //返回0表示程序正常退出
}
/*
 *	函数作用:初始化屏幕
 *	函数参数:无
 *	函数返回值:无/
 */
void Initialization(void) //初始化函数
{
	initscr(); //初始化ncurses库并清空屏幕
	noecho(); //设置输入不回显到屏幕
	cbreak(); //禁用行缓冲，立即处理用户输入
	timeout(speed); //设置非阻塞输入，等待指定毫秒
	keypad(stdscr, TRUE); //启用功能键识别（方向键等）
	int offsetx = (COLS - WORLD_WIDTH) / 2; //计算窗口水平偏移量（居中）
	int offsety = (LINES - WORLD_HEIGHT) / 2; //计算窗口垂直偏移量（居中）
	snakeys_world = newwin(WORLD_HEIGHT, WORLD_WIDTH, offsety, offsetx); //创建新窗口
}
/*
 *	函数作用:等待开始游戏界面
 *	函数参数:无
 *	函数返回值:无/
 */
void WaitGame(void)
{
	int offsetx = (COLS - WORLD_WIDTH) / 2;
	int offsety = (LINES - WORLD_HEIGHT) / 2;
	int i = 1, n = 0;
	while ((ch = getch()) != 'b')
	{
		wclear(snakeys_world);
		mvprintw(offsety + WORLD_HEIGHT / 2 - 5, offsetx + 16, "PieceOfCakeSnake");
		mvprintw(offsety + WORLD_HEIGHT / 2 - 1, offsetx + i, "Please enter 'B' to start");
		mvprintw(1, offsetx + WORLD_HEIGHT / 2 + 10, "Score: %d", Score);
		mvprintw(WORLD_HEIGHT - 3, offsetx + 5, "Copyright © Littleor ALL right reserverd");

		box(snakeys_world, 0, 0);
		wrefresh(snakeys_world);
		if (offsetx + i < WORLD_WIDTH - 12 && n % 2 == 0)
		{
			i++;
		}
		else if (i > 1 && n % 2 != 0)
		{
			i--;
		}
		else if (offsetx + i >= WORLD_WIDTH / 2 || i <= 1)
		{
			n++;
		}
		else
		{
		}
	}
}

/*
 *	函数作用:创建一条蛇
 *	函数参数:无
 *	函数返回值:无/
 */

void CreateSnake(void) //创建蛇的函数
{
	int offsetx = (COLS - WORLD_WIDTH) / 2; //计算水平偏移
	int offsety = (LINES - WORLD_HEIGHT) / 2; //计算垂直偏移
	wclear(snakeys_world); //清空游戏窗口
	for (int i = 0; i < SnakeLength; i++) //循环初始化蛇的各个身体部分
	{
		snakey[i].x = offsetx + 10 - i; //从左到右排列蛇身
		snakey[i].y = offsety + WORLD_HEIGHT / 2; //设置蛇的y坐标在中间
	}
	mvwaddch(snakeys_world, snakey[0].y, snakey[0].x, '0'); //绘制蛇头（'0'）
	for (int i = 1; i < SnakeLength; i++) //循环绘制蛇身
	{
		mvwaddch(snakeys_world, snakey[i].y, snakey[i].x, '#'); //用'#'绘制蛇身
	}
	mvwaddch(snakeys_world, food.y, food.x, 'x'); //显示初始食物（'x'）
	mvprintw(1, offsetx + WORLD_HEIGHT / 2 + 10, "Score: %d", Score); //显示分数
	box(snakeys_world, 0, 0); //绘制窗口边框
	wrefresh(snakeys_world); //刷新窗口显示内容
}
/*
 *	函数作用:生成食物
 *	函数参数:无
 *	函数返回值:bool
 */
bool RandomFood(void) //生成随机食物的函数
{
	srand(time(NULL)); //用当前时间作为随机数种子
	int food_y = (rand() % (WORLD_HEIGHT - 1)); //生成随机y坐标
	int food_x = (rand() % (WORLD_WIDTH - 1)); //生成随机x坐标
	int n = 0; //临时变量（未使用）
	for (int i = 0; i < SnakeLength; i++) //检查食物是否与蛇身重叠
	{
		if (snakey[i].x == food_x && snakey[i].y == food_y) //若食物位置在蛇身上
		{
			return false; //返回false表示位置不合适
		}
		else if (food_y == 0 || food_x == 0) //若食物位置在边界上
		{
			return false; //返回false表示位置不合适
		}
	}

	food.y = food_y; //保存食物y坐标
	food.x = food_x; //保存食物x坐标
	return true; //返回true表示食物位置有效
}
/*
 *	函数作用:切换蛇的方向
 *	函数参数:无
 *	函数返回值:无
 */
void GetDir(void) //获取用户输入并改变蛇的方向
{

	if (ch != ERR) //检查是否有输入（ERR表示无输入）
	{
		switch (ch) //根据输入的键进行判断
		{
		case KEY_UP: //按下向上键
			direction = (direction == DOWN) ? DOWN : UP; //若当前向下则不变，否则改为向上（防止反向）
			break; //结束本case
		case KEY_DOWN: //按下向下键
			direction = (direction == UP) ? UP : DOWN; //若当前向上则不变，否则改为向下
			break; //结束本case
		case KEY_RIGHT: //按下向右键
			direction = (direction == LEFT) ? LEFT : RIGHT; //若当前向左则不变，否则改为向右
			break; //结束本case
		case KEY_LEFT: //按下向左键
			direction = (direction == RIGHT) ? RIGHT : LEFT; //若当前向右则不变，否则改为向左
			break; //结束本case
		default: //其他键
			break; //结束本case
		}
	}
}
/*
 *	函数作用:移动蛇头
 *	函数参数:无
 *	函数返回值:无
 */
void MoveHead(void)
{

	switch (direction)
	{
	//控制头部的移动方向
	case UP:
		(snakey[0].y - 1 == 0) ? snakey[0].y = WORLD_HEIGHT - 2 : snakey[0].y--;
		break;
	case DOWN:
		(snakey[0].y + 1 == WORLD_HEIGHT - 1) ? snakey[0].y = 1 : snakey[0].y++;
		break;
	case RIGHT:
		(snakey[0].x + 1 == WORLD_WIDTH - 1) ? snakey[0].x = 1 : snakey[0].x++;
		break;
	case LEFT:
		(snakey[0].x - 1 == 0) ? snakey[0].x = WORLD_WIDTH - 2 : snakey[0].x--;
		break;
	default:
		break;
	}
	for (int i = 1; i < SnakeLength; i++)
	{
		if (snakey[0].x == snakey[i].x && snakey[0].y == snakey[i].y)
		{
			GameOver();
		}
	}
	if (snakey[0].y == food.y && snakey[0].x == food.x)
	{
		snakey[SnakeLength].x = tem.x;
		snakey[SnakeLength].y = tem.y;
		SnakeLength++;
		Score++;
		speed = speed - Score;
		if (speed <= 50)
		{
			speed = 50;
		}
		while (!RandomFood())
			;
		timeout(speed); //设置运动间隔时间
	}
	mvwaddch(snakeys_world, food.y, food.x, 'x');			//在指定位置输出食物
	mvwaddch(snakeys_world, snakey[0].y, snakey[0].x, '0'); //在指定位置输出蛇头
}
/*
 *	函数作用:移动蛇身
 *	函数参数:无
 *	函数返回值:无
 */
void Move(void) //移动蛇身的函数
{
	em.y = snakey[SnakeLength - 1].y; //保存蛇尾的y坐标
	em.x = snakey[SnakeLength - 1].x; //保存蛇尾的x坐标
	for (int i = SnakeLength - 1; i > 0; i--) //从蛇尾向蛇头遍历
	{
		snakey[i].x = snakey[i - 1].x; //当前蛇身节点取前一节点的x坐标
		snakey[i].y = snakey[i - 1].y; //当前蛇身节点取前一节点的y坐标
		mvwaddch(snakeys_world, snakey[i].y, snakey[i].x, '#'); //在窗口中绘制蛇身节点
	}
}
/*
 *	函数作用:游戏中
 *	函数参数:无
 *	函数返回值:无
 */
void Game(void) //主游戏循环函数
{
	int offsetx = (COLS - WORLD_WIDTH) / 2; //计算窗口水平偏移
	int offsety = (LINES - WORLD_HEIGHT) / 2; //计算窗口垂直偏移
	CreateSnake(); //初始化蛇
	while (!RandomFood()) //生成有效的食物位置
		; //空循环
	while ((ch = getch()) != 'x') //游戏主循环（按'x'退出）
	{
		wclear(snakeys_world); //清空窗口
		GetDir(); //获取用户输入并改变蛇的方向
		Move(); //移动蛇身
		MoveHead(); //移动蛇头
		mvprintw(1, offsetx + WORLD_HEIGHT / 2 + 10, "Score: %d ", Score); //显示分数
		box(snakeys_world, 0, 0); //绘制窗口边框
		wrefresh(snakeys_world); //刷新窗口显示
	}
}

void GameOver(void) //游戏结束界面函数
{
	int offsetx = (COLS - WORLD_WIDTH) / 2; //计算窗口水平偏移
	int offsety = (LINES - WORLD_HEIGHT) / 2; //计算窗口垂直偏移
	int i = 1, n = 0; //i用于文字移动位置，n用于控制移动方向
	while ((ch = getch()) != 'q') //循环显示直到用户按'q'
	{
		wclear(snakeys_world); //清空窗口
		mvprintw(offsety + WORLD_HEIGHT / 2 - 5, offsetx + WORLD_WIDTH / 2 - 5, "GameOver"); //显示游戏结束文本
		mvprintw(offsety + WORLD_HEIGHT / 2 - 4, offsetx + WORLD_WIDTH / 2 - 8, "YourScore is %d", Score); //显示最终分数
		mvprintw(offsety + WORLD_HEIGHT / 2 - 1, offsetx + i, "Please enter 'q' to quit"); //显示退出提示（移动）
		mvprintw(WORLD_HEIGHT - 3, offsetx + 5, "Copyright © Littleor ALL right reserverd"); //显示版权信息
		box(snakeys_world, 0, 0); //绘制窗口边框
		wrefresh(snakeys_world); //刷新显示
		if (offsetx + i < WORLD_WIDTH - 12 && n % 2 == 0) //若文字未到右边界且向右移动
		{
			i++; //增加位置值，文字向右移动
		}
		else if (i > 1 && n % 2 != 0) //若文字未到左边界且向左移动
		{
			i--; //减少位置值，文字向左移动
		}
		else if (offsetx + i >= WORLD_WIDTH / 2 || i <= 1) //到达边界时改变方向
		{
			n++; //改变方向标志
		}
		else //其他情况
		{
		}
	}
	delwin(snakeys_world); //删除游戏窗口
	endwin(); //关闭ncurses库
}