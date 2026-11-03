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
}; //方向枚举

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
	// WaitGame(); //显示等待界面（注释掉以直接开始游戏）
	Game(); //开始游戏
	return 0; //返回0表示程序正常退出
}

/*
 *	函数作用:初始化屏幕
 *	函数参数:无
 *	函数返回值:无
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
 *	函数返回值:无
 */
void WaitGame(void) //等待游戏开始函数
{
	int offsetx = (COLS - WORLD_WIDTH) / 2; //计算水平偏移量
	int offsety = (LINES - WORLD_HEIGHT) / 2; //计算垂直偏移量
	int i = 1, n = 0; //i用于文字移动位置，n用于控制移动方向
	while ((ch = getch()) != 'b') //循环显示直到用户按'b'
	{
		wclear(snakeys_world); //清空窗口
		mvprintw(offsety + WORLD_HEIGHT / 2 - 5, offsetx + 16, "PieceOfCakeSnake"); //显示游戏标题
		mvprintw(offsety + WORLD_HEIGHT / 2 - 1, offsetx + i, "Please enter 'B' to start"); //显示开始提示（移动）
		mvprintw(1, offsetx + WORLD_HEIGHT / 2 + 10, "Score: %d", Score); //显示分数
		mvprintw(WORLD_HEIGHT - 3, offsetx + 5, "Copyright © Littleor ALL right reserverd"); //显示版权信息

		box(snakeys_world, 0, 0); //绘制窗口边框
		wrefresh(snakeys_world); //刷新窗口显示
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
}

/*
 *	函数作用:创建一条蛇
 *	函数参数:无
 *	函数返回值:无
 */
void CreateSnake(void) //创建蛇函数
{
	wclear(snakeys_world); //清空游戏窗口
	for (int i = 0; i < SnakeLength; i++) //循环初始化蛇的各个身体部分
	{
		snakey[i].x = 20 - i; //从左到右排列蛇身（相对于窗口坐标）
		snakey[i].y = 5; //设置蛇的y坐标在中间（相对于窗口坐标）
	}
	mvwaddch(snakeys_world, snakey[0].y, snakey[0].x, '0'); //绘制蛇头（'0'）
	for (int i = 1; i < SnakeLength; i++) //循环绘制蛇身
	{
		mvwaddch(snakeys_world, snakey[i].y, snakey[i].x, '#'); //用'#'绘制蛇身
	}
	box(snakeys_world, 0, 0); //绘制窗口边框
	mvwprintw(snakeys_world, 1, WORLD_WIDTH / 2 - 5, "Score: %d", Score); //显示分数
	wrefresh(snakeys_world); //刷新窗口显示
}

/*
 *	函数作用:生成食物
 *	函数参数:无
 *	函数返回值:bool
 */
bool RandomFood(void) //生成随机食物函数
{
	srand(time(NULL)); //用当前时间作为随机数种子
	int food_y = (rand() % (WORLD_HEIGHT - 2)) + 1; //生成随机y坐标（1到WORLD_HEIGHT-2）
	int food_x = (rand() % (WORLD_WIDTH - 2)) + 1; //生成随机x坐标（1到WORLD_WIDTH-2）
	for (int i = 0; i < SnakeLength; i++) //检查食物是否与蛇身重叠
	{
		if (snakey[i].x == food_x && snakey[i].y == food_y) //若食物位置在蛇身上
		{
			return false; //返回false表示位置不合适
		}
	}

	food.y = food_y; //保存食物y坐标
	food.x = food_x; //保存食物x坐标
	return true; //返回true表示食物位置有效
}

/*
 *	函数作用:获取用户输入并改变蛇的方向
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
void MoveHead(void) //移动蛇头函数
{

	switch (direction) //根据当前方向移动蛇头
	{
	//控制头部的移动方向
	case UP: //蛇向上移动
		(snakey[0].y - 1 == 0) ? snakey[0].y = WORLD_HEIGHT - 2 : snakey[0].y--; //若触到上边界则穿过到下边，否则y坐标-1
		break; //结束本case
	case DOWN: //蛇向下移动
		(snakey[0].y + 1 == WORLD_HEIGHT - 1) ? snakey[0].y = 1 : snakey[0].y++; //若触到下边界则穿过到上边，否则y坐标+1
		break; //结束本case
	case RIGHT: //蛇向右移动
		(snakey[0].x + 1 == WORLD_WIDTH - 1) ? snakey[0].x = 1 : snakey[0].x++; //若触到右边界则穿过到左边，否则x坐标+1
		break; //结束本case
	case LEFT: //蛇向左移动
		(snakey[0].x - 1 == 0) ? snakey[0].x = WORLD_WIDTH - 2 : snakey[0].x--; //若触到左边界则穿过到右边，否则x坐标-1
		break; //结束本case
	default: //默认情况
		break; //结束本case
	}
	for (int i = 1; i < SnakeLength; i++) //检查蛇头是否与蛇身碰撞
	{
		if (snakey[0].x == snakey[i].x && snakey[0].y == snakey[i].y) //若蛇头位置等于蛇身某节点
		{
			GameOver(); //游戏结束
		}
	}
	if (snakey[0].y == food.y && snakey[0].x == food.x) //检查蛇头是否吃到食物
	{
		snakey[SnakeLength].x = tem.x; //为新增的蛇身节点设置x坐标
		snakey[SnakeLength].y = tem.y; //为新增的蛇身节点设置y坐标
		SnakeLength++; //蛇长度+1
		Score++; //分数+1
		speed = speed - Score; //游戏加速
		if (speed <= 50) //若速度过快则限制最小速度
		{
			speed = 50; //设置最小速度为50毫秒
		}
		while (!RandomFood()) //重新生成食物，直到生成有效位置
			; //空循环
		timeout(speed); //更新游戏速度
	}
	if (food.y > 0 && food.x > 0 && food.y < WORLD_HEIGHT && food.x < WORLD_WIDTH)
	{
		mvwaddch(snakeys_world, food.y, food.x, '*'); //在窗口中绘制食物
	}
	mvwaddch(snakeys_world, snakey[0].y, snakey[0].x, '@'); //在窗口中绘制蛇头
}

/*
 *	函数作用:移动蛇身
 *	函数参数:无
 *	函数返回值:无
 */
void Move(void) //移动蛇身函数
{
	tem.y = snakey[SnakeLength - 1].y; //保存蛇尾的y坐标
	tem.x = snakey[SnakeLength - 1].x; //保存蛇尾的x坐标
	for (int i = SnakeLength - 1; i > 0; i--) //从蛇尾向蛇头遍历
	{
		snakey[i].x = snakey[i - 1].x; //当前蛇身节点取前一节点的x坐标
		snakey[i].y = snakey[i - 1].y; //当前蛇身节点取前一节点的y坐标
		mvwaddch(snakeys_world, snakey[i].y, snakey[i].x, '#'); //在窗口中绘制蛇身节点
	}
}

/*
 *	函数作用:游戏主循环
 *	函数参数:无
 *	函数返回值:无
 */
void Game(void) //主游戏函数
{
	CreateSnake(); //初始化蛇
	while (!RandomFood()) //生成有效的食物位置
		; //空循环
	while ((ch = getch()) != 'x') //游戏主循环（按'x'退出）
	{
		wclear(snakeys_world); //清空窗口
		GetDir(); //获取用户输入并改变蛇的方向
		Move(); //移动蛇身
		MoveHead(); //移动蛇头
		mvwprintw(snakeys_world, 1, WORLD_WIDTH / 2 + 10, "Score: %d ", Score); //显示分数
		box(snakeys_world, 0, 0); //绘制窗口边框
		wrefresh(snakeys_world); //刷新窗口显示
	}
}

/*
 *	函数作用:游戏结束界面
 *	函数参数:无
 *	函数返回值:无
 */
void GameOver(void) //游戏结束函数
{
	int offsetx = (COLS - WORLD_WIDTH) / 2; //计算水平偏移量
	int offsety = (LINES - WORLD_HEIGHT) / 2; //计算垂直偏移量
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
