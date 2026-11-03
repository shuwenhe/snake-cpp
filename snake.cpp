#include <ncurses.h>		// 引入ncurses库头文件
#include <iostream>			// C++标准输入输出库
#include <cstdlib>			// C++标准库
#include <ctime>			// 时间库
#include <cstring>			// 字符串库

using namespace std;

// 定义蛇的身体部分结构体
struct SnakePart {
	int x;	// x坐标
	int y;	// y坐标
};

// 定义蛇的移动方向枚举
enum Direction {
	UP,		// 向上
	DOWN,	// 向下
	RIGHT,	// 向右
	LEFT	// 向左
};

// 全局变量
WINDOW *snakeys_world;			// 游戏窗口指针
int WORLD_WIDTH = 50;			// 游戏世界宽度（默认值）
int WORLD_HEIGHT = 20;			// 游戏世界高度（默认值）
int SnakeLength = 5;			// 蛇的长度初始为5
int Score = 0;					// 游戏分数初始为0
SnakePart snakey[120];			// 蛇身体数组，最多120个节点
SnakePart food;					// 食物坐标
SnakePart tem;					// 临时变量，存储蛇尾坐标
int direction = RIGHT;			// 初始方向向右
int ch;							// 存储用户输入的字符
int speed = 400;				// 游戏速度（毫秒）

// 函数声明
void Initialization();
void Game();
void GameOver();
void CreateSnake();
bool RandomFood();
void GetDir();
void MoveHead();
void Move();

// 主函数
int main() {
	Initialization();	// 初始化游戏
	Game();				// 开始游戏
	return 0;			// 返回0表示程序正常退出
}

// 初始化屏幕
void Initialization() {
	initscr();						// 初始化ncurses库并清空屏幕
	noecho();						// 设置输入不回显到屏幕
	cbreak();						// 禁用行缓冲，立即处理用户输入
	timeout(speed);					// 设置非阻塞输入，等待指定毫秒
	keypad(stdscr, TRUE);			// 启用功能键识别（方向键等）
	
	// 获取终端大小，设为游戏界面大小
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);
	WORLD_HEIGHT = max_y - 2;		// 留出2行显示信息
	WORLD_WIDTH = max_x;			// 使用全部宽度
	
	// 创建游戏窗口（覆盖全屏）
	snakeys_world = newwin(WORLD_HEIGHT, WORLD_WIDTH, 1, 0);
}

// 创建一条蛇
void CreateSnake() {
	wclear(snakeys_world);			// 清空游戏窗口
	for (int i = 0; i < SnakeLength; i++) {
		snakey[i].x = 20 - i;		// 从左到右排列蛇身
		snakey[i].y = WORLD_HEIGHT / 2;	// 设置蛇的y坐标在中间
	}
	// 绘制蛇头
	mvwaddch(snakeys_world, snakey[0].y, snakey[0].x, '@');
	// 循环绘制蛇身
	for (int i = 1; i < SnakeLength; i++) {
		mvwaddch(snakeys_world, snakey[i].y, snakey[i].x, '*');
	}
	// 显示分数（在屏幕顶部）
	mvprintw(0, 0, "Score: %d", Score);
	wrefresh(snakeys_world);		// 刷新窗口显示
}

// 生成食物
bool RandomFood() {
	srand(time(NULL));				// 用当前时间作为随机数种子
	int food_y = (rand() % (WORLD_HEIGHT - 2)) + 1;	// 生成随机y坐标
	int food_x = (rand() % (WORLD_WIDTH - 2)) + 1;	// 生成随机x坐标
	
	// 检查食物是否与蛇身重叠
	for (int i = 0; i < SnakeLength; i++) {
		if (snakey[i].x == food_x && snakey[i].y == food_y) {
			return false;			// 位置不合适
		}
	}
	
	food.y = food_y;				// 保存食物y坐标
	food.x = food_x;				// 保存食物x坐标
	return true;					// 位置有效
}

// 获取用户输入并改变蛇的方向
void GetDir() {
	if (ch != ERR) {				// 检查是否有输入
		switch (ch) {
		case KEY_UP:				// 按下向上键
			direction = (direction == DOWN) ? DOWN : UP;
			break;
		case KEY_DOWN:				// 按下向下键
			direction = (direction == UP) ? UP : DOWN;
			break;
		case KEY_RIGHT:				// 按下向右键
			direction = (direction == LEFT) ? LEFT : RIGHT;
			break;
		case KEY_LEFT:				// 按下向左键
			direction = (direction == RIGHT) ? RIGHT : LEFT;
			break;
		default:
			break;
		}
	}
}

// 移动蛇头
void MoveHead() {
	switch (direction) {
	case UP:						// 蛇向上移动
		(snakey[0].y - 1 == 0) ? snakey[0].y = WORLD_HEIGHT - 2 : snakey[0].y--;
		break;
	case DOWN:						// 蛇向下移动
		(snakey[0].y + 1 == WORLD_HEIGHT - 1) ? snakey[0].y = 1 : snakey[0].y++;
		break;
	case RIGHT:						// 蛇向右移动
		(snakey[0].x + 1 == WORLD_WIDTH - 1) ? snakey[0].x = 1 : snakey[0].x++;
		break;
	case LEFT:						// 蛇向左移动
		(snakey[0].x - 1 == 0) ? snakey[0].x = WORLD_WIDTH - 2 : snakey[0].x--;
		break;
	default:
		break;
	}
	
	// 检查蛇头是否与蛇身碰撞
	for (int i = 1; i < SnakeLength; i++) {
		if (snakey[0].x == snakey[i].x && snakey[0].y == snakey[i].y) {
			GameOver();				// 游戏结束
		}
	}
	
	// 检查蛇头是否吃到食物
	if (snakey[0].y == food.y && snakey[0].x == food.x) {
		snakey[SnakeLength].x = tem.x;
		snakey[SnakeLength].y = tem.y;
		SnakeLength++;				// 蛇长度+1
		Score++;					// 分数+1
		speed = speed - Score;		// 游戏加速
		if (speed <= 50) {
			speed = 50;				// 设置最小速度
		}
		while (!RandomFood())		// 重新生成食物
			;
		timeout(speed);				// 更新游戏速度
	}
	
	// 绘制食物
	if (food.y > 0 && food.x > 0 && food.y < WORLD_HEIGHT && food.x < WORLD_WIDTH) {
		mvwaddch(snakeys_world, food.y, food.x, '#');
	}
	// 绘制蛇头
	mvwaddch(snakeys_world, snakey[0].y, snakey[0].x, '@');
}

// 移动蛇身
void Move() {
	tem.y = snakey[SnakeLength - 1].y;	// 保存蛇尾的y坐标
	tem.x = snakey[SnakeLength - 1].x;	// 保存蛇尾的x坐标
	
	// 从蛇尾向蛇头遍历
	for (int i = SnakeLength - 1; i > 0; i--) {
		snakey[i].x = snakey[i - 1].x;	// 当前蛇身节点取前一节点的x坐标
		snakey[i].y = snakey[i - 1].y;	// 当前蛇身节点取前一节点的y坐标
		mvwaddch(snakeys_world, snakey[i].y, snakey[i].x, '*');
	}
}

// 游戏主循环
void Game() {
	CreateSnake();					// 初始化蛇
	while (!RandomFood())			// 生成有效的食物位置
		;
	mvwaddch(snakeys_world, food.y, food.x, '#');	// 初始绘制食物
	wrefresh(snakeys_world);
	
	while ((ch = getch()) != 'x') {	// 游戏主循环（按'x'退出）
		wclear(snakeys_world);		// 清空窗口
		GetDir();					// 获取用户输入并改变蛇的方向
		Move();						// 移动蛇身
		MoveHead();					// 移动蛇头
		mvprintw(0, 0, "Score: %d | Press 'x' to exit", Score);	// 显示分数
		wrefresh(snakeys_world);	// 刷新窗口显示
	}
}

// 游戏结束界面
void GameOver() {
	wclear(snakeys_world);
	delwin(snakeys_world);
	endwin();						// 关闭ncurses库
	
	cout << "\n========== GAME OVER ==========" << endl;
	cout << "Your Final Score: " << Score << endl;
	cout << "===============================" << endl;
	exit(0);						// 退出程序
}
