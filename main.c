#include "drawnlist.h"

int main(int argc, char **argv)
{
	initScreen();       //初始化屏幕
	initColor();        //初始化颜色
	bkgd(COLOR_PAIR(1));//给背景设定颜色
	box(stdscr, 0, 0);  //画边框
	editWin = newwin(LINES-4, COLS-4, 3, 2);
	wbkgd(editWin, COLOR_PAIR(2));
	wattron(editWin, COLOR_PAIR(3));    //当前使用该色彩
	box(editWin, 0, 0);
	wattroff(editWin, COLOR_PAIR(3));   //不使用该色彩了
    wattron(editWin,COLOR_PAIR(7));
	initMenu(); //初始化菜单
	draw_main_menu(stdscr); //画菜单
	initList(); //初始化链表
	while(isOn){
		wnoutrefresh(stdscr);
		wnoutrefresh(editWin);
		doupdate();
		drawMenuList(0, 0);
		touchwin(editWin);
		wrefresh(editWin);
	}
	exitScreen();   //退出函数
	return 0;
}

