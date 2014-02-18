#include "drawnlist.h"

WINDOW	*menu_win[MAIN_MENU_NUM];//一级子菜单窗口
WINDOW	*sub_win[SUB_MENU_NUM];//二级子菜单窗口
WINDOW	*editWin;//主界面

FIELD *fields[17];//输入框

int		screenHeight,screenWidth;//屏幕高度，宽度
int		isOn = 1;
//三个级别的菜单的标号，-1代表不是当前菜单
int		mIndex = 0,eIndex = -1,sIndex = -1;
int shortcutsF(int);//F(n)快捷键
int getFieldLength(int type,int i);//定义文本域长度
void changeMESIndex(int uod,int size);//改变List标号
int drawSelDialog(char* str,int index);//绘画选择对话框
void drawOKDialog(char* str);//绘画通知对话框
void createStaEditbox(int type,int index,int flag);
int getFieldsNum(int type);
void initFields(WINDOW* formWin,int type);
void drawStaListview(int type,int typeindex,int index);
void createQueryEditbox(int type);
int drawSaveDialog(void);
void createHelpEditbox();

/*各种名称*/
char *info[9][17]={//代表团，参赛项目，运动员
    {//16+1
        "代表团编号",
        "代表团名称",
        "所在国",
        "团长姓名",
        "团长联系方式",
        "参赛运动项目数",
        "参赛运动员人数",
        "教练员人数",
        "裁判人数",
        "其他辅助人员人数",
        "代表团入住地地址",
        "代表团入住地电话",
        "预订房间数",
        "需配备翻译人数",
        "入住奥运村时间",
        "离开奥运村时间",
        (char *)NULL,
    },
    {//11+1
        "参赛项目编号",
        "参赛项目名称",
        "代表团编号",
        "项目领队姓名",
        "领队联系方式",
        "教练员人数",
        "参赛运动员人数",
        "历次去的最好成绩",
        "去的最好成绩时间",
        "去的最好成绩地点",
        "违禁记录",
        (char *)NULL,
    },
    {//12+1
        "参赛选手编号",
        "参赛项目编号",
        "代表团编号",
        "参赛选手姓名",
        "性别",
        "出生日期",
        "出生地",
        "身高",
        "体重",
        "入围成绩",
        "最好成绩",
        "兴趣爱好",
        (char *)NULL,
    },
    {//6+1
        "总参赛运动项目数", 
        "总参赛运动员数",
        "总裁判员数",
        "总教练数",
        "男运动员数",
        "女运动员数",
        (char *)NULL,
    },
    {//3+1
        "运动员人数",  
        "教练员人数",
        "有违禁记录代表团数",
        (char*)NULL,
    },
    {//2+1
        "运动项目人数排名", 
        "",
        "",
        "运动员体重排名",
        "",
        "",
        (char *)NULL,
    },
    {
        "代表团名称或编号", 
        (char *)NULL,
    },
    {
        "项目名称或编号", 
        (char *)NULL,
    },
    {
        "运动员名称或编号", 
        (char *)NULL,
    },
};

/*button的名称*/
char *stringButton[][3]={
    {
        " 保存&&继续 ",
        "    清空    ",
        "    退出    ",
    },
    {
        "   新建   ",
        "   修改   ",
    }
};

/*初始化屏幕*/
void initScreen(void)
{
    setlocale(LC_ALL,"");//使支持utf-8
    initscr();	//初始化ncurses
    noecho();	//输入的字符不可见
    cbreak();	//直接被利用
    getmaxyx(stdscr,screenHeight,screenWidth);//获取屏幕高宽
    start_color();
    curs_set(0);//使光标隐藏
}

/*初始化各种颜色*/
void initColor(void)
{
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_CYAN, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_YELLOW);
    init_pair(7 ,COLOR_BLACK, COLOR_YELLOW);
}

/*
 *setMenuText
 * */
void initMenu(void)
{
    int		i;
    for (i = 0; i < MAIN_MENU_NUM; ++i)
        menu_win[i] = NULL;
    for (i = 0; i < SUB_MENU_NUM; ++i)
        sub_win[i] = NULL;
    strcpy(menuList[0].menuname, "文件<F2>");
    strcpy(menuList[0].str[0],"<------->");
    strcpy(menuList[0].str[1], "保存");
    menuList[0].type[1] = SAVE;
    strcpy(menuList[0].str[2], "清空");
    menuList[0].type[2] = DELETE;
    strcpy(menuList[0].str[3], "--------");
    menuList[0].type[3] = SKIP;
    strcpy(menuList[0].str[4], "退出");
    menuList[0].type[4] = EXIT;
    menuList[0].n = 5;

    strcpy(menuList[1].menuname, "维护<F3>");
    strcpy(menuList[1].str[0],"<------->");
    strcpy(menuList[1].str[1], "输入 ->");
    menuList[1].type[1] = 1;
    strcpy(menuList[1].str[2], "修改");
    //	menuList[1].type[2] = 3;
    menuList[1].type[2] = CHANGE_MISSION;
    strcpy(menuList[1].str[3],"删除");
    //  menuList[1].type[3] = 4;
    menuList[1].type[3] = DELETE_MISSION;
    menuList[1].n = 4;

    strcpy(menuList[2].menuname, "查询<F4>");
    strcpy(menuList[2].str[0],"<------->");
    strcpy(menuList[2].str[1], "代表团");
    menuList[2].type[1] = QUERY_MISSION;
    strcpy(menuList[2].str[2], "项目");
    menuList[2].type[2] = QUERY_ENTRIES;
    strcpy(menuList[2].str[3], "运动员");
    menuList[2].type[3] = QUERY_SPORTSMAN;
    menuList[2].n = 4;

    strcpy(menuList[3].menuname, "统计<F5>");
    strcpy(menuList[3].str[0],"<------->");
    strcpy(menuList[3].str[1], "总体");
    menuList[3].type[1] = STA_ALL;
    strcpy(menuList[3].str[2], "单项");
    menuList[3].type[2] = STA_SINGER;
    strcpy(menuList[3].str[3],"其他");
    menuList[3].type[3] = STA_OTHER;
    menuList[3].n = 4;

    strcpy(menuList[4].menuname, "帮助<F6>");
    strcpy(menuList[4].str[0],"<------->");
    strcpy(menuList[4].str[1], "关于");
    menuList[4].type[1] = -2;
    strcpy(menuList[4].str[2], "帮助");
    menuList[4].type[2] = HELP;
    menuList[4].n = 3;

    //以下是子菜单
    strcpy(menuList[5].str[0], "代表团");
    menuList[5].type[0] = CREATE_MISSION;
    strcpy(menuList[5].str[1], "项目");
    menuList[5].type[1] = CREATE_ENTRIES;
    strcpy(menuList[5].str[2], "运动员");
    menuList[5].type[2] = CREATE_SPORTSMAN;
    menuList[5].n = 3;
}

/*画主菜单*/
void draw_main_menu(WINDOW *win)
{
    int		i;
    for (i = 0; i < MAIN_MENU_NUM; ++i)
        mvwprintw(win, 2, 4+i*screenWidth/5, menuList[i].menuname);
}

/*!
  画一级子菜单
  \param m:主菜单第m项 t:子菜单第t项
  */
void drawMenuList(int m, int t)
{
    int		i;
    curs_set(0);
    wattron(editWin, COLOR_PAIR(3));
    box(editWin, 0, 0);
    wattroff(editWin, COLOR_PAIR(3));
    if (NULL == menu_win[m]) {
        menu_win[m] = newwin(2+menuList[m].n, 10, 3, 4+m*screenWidth/5);
        wbkgd(menu_win[m], COLOR_PAIR(4));
    }

    touchwin(editWin);
    wrefresh(editWin);
    box(menu_win[m], 0, 0);

    //画各项，选中的反色显示
    for (i = 0; i < menuList[m].n; ++i) {
        if (t == i) wattron(menu_win[m], A_REVERSE);
        mvwprintw(menu_win[m], i+1, 1, menuList[m].str[i]);
        if (t == i) wattroff(menu_win[m], A_REVERSE);
    }
    wrefresh(menu_win[m]);
    moveMenuList(m, t);//进入监听
    delwin(menu_win[m]);
}

/*!
  在一级子菜单下对键盘的监听
  \param m:主菜单第m项 t:子菜单第t项
  */
void moveMenuList(int m, int t)
{
    int		ch;
    bool	bdomenu = TRUE;

    keypad(menu_win[m], TRUE);
    while (bdomenu) {
        ch = wgetch(menu_win[m]);
        //对快捷键F(n)判断
        if((shortcutsF(ch))) bdomenu =FALSE;
        switch (ch) {
            case KEY_ESC:
                drawMenuList(m, 0);
                bdomenu = FALSE;
                break;
            case 'q':
                drawMenuList(0, 5);
                bdomenu = FALSE;
                break;
            case KEY_LEFT:
                drawMenuList((m+MAIN_MENU_NUM-1)%MAIN_MENU_NUM, 0);
                bdomenu = FALSE;
                break;
            case KEY_RIGHT:
                //有子菜单则画出子菜单，否则往右移动
                if(menuList[m].type[t]>0){
                    drawSubmenuList(m, t, MAIN_MENU_NUM+menuList[m].type[t]-1);
                    drawMenuList(m, 0);
                } else{
                    drawMenuList((m+MAIN_MENU_NUM+1)%MAIN_MENU_NUM, 0);
                }

                bdomenu = FALSE;
                break;
            case KEY_UP:
                mvwprintw(menu_win[m], t+1, 1, menuList[m].str[t]);
                t = (t+menuList[m].n-1)%menuList[m].n;
                moveMenuItem(&m,&t,-1);
                wattron(menu_win[m], A_REVERSE);
                mvwprintw(menu_win[m], t+1, 1, menuList[m].str[t]);
                wattroff(menu_win[m], A_REVERSE);
                break;
            case KEY_DOWN:
                mvwprintw(menu_win[m], t+1, 1, menuList[m].str[t]);
                t = (t+menuList[m].n+1)%menuList[m].n;
                moveMenuItem(&m,&t,1);
                wattron(menu_win[m], A_REVERSE);
                mvwprintw(menu_win[m], t+1, 1, menuList[m].str[t]);
                wattroff(menu_win[m], A_REVERSE);
                break;
            case ENTER:
                //做下一步
                if(doNext(m,t,&bdomenu)) break;
                drawMenuList(0,0);
                bdomenu = FALSE;
                break;
            default:
                break;
        }
    }
}

/*!
  偷懒做的函数，上下移并判断是否有SKIP以跳过
  \param uod 1down,-1up
  */
void moveMenuItem(int *m,int *t,int uod)
{
    if(menuList[*m].type[*t] == SKIP)
        *t +=uod;
}

/*!
  很重要的函数，对不同的情况判断决定一下步走向
  \param bdomenu:使之变为false以结束调用该函数的函数的循环
  \return 不画子菜单为0,反之为1
  */
int doNext(int m,int t ,_Bool *bdomenu)
{
    int type = menuList[m].type[t];
    switch(type){
        case EXIT:
            //退出
            //如果保存了就直接退出
            if(!isChanged){
                exitScreen(); 
            }
            //如果没保存就询问
            switch(drawSaveDialog()){
                case 0:
                    saveList();
                    drawOKDialog("保存成功！");
                    exitScreen();
                    break;
                case 1:
                    exitScreen();
                    break;
                case 2:
                    drawMenuList(0,0);
                    break;
                default:
                    break; 
            }
        case DIALOG://关于对话框
            drawOKDialog("伦敦奥运会管理系统\n\t@author 路少德");
            drawMenuList(4,1);
            break;
        case SAVE:
            if (drawSelDialog("确认保存吗？",0)) {
                saveList();
                drawOKDialog("保存成功");
            } else{

            }
            touchwin(editWin);
            refresh();
            break;
        case DELETE:
            if((drawSelDialog("确定要删除全部吗？",1))){
                deleteAllList(); 
                drawOKDialog("全部删除成功！");
            }
            break;
        case SKIP:
            break;
        case CHANGE_MISSION:
        case DELETE_MISSION:
            mIndex = 0;eIndex = -1; sIndex = -1;
            drawListview(type,0);
            break;
        case STA_ALL:
        case STA_OTHER:
            if (!isChanged) {
                createStaEditbox(type,0,1);
            } else if (drawSelDialog("只有保存才能进入\n\t是否保存？",0)) {
                saveList();
                drawOKDialog("保存成功！");
                createStaEditbox(type,0,1);
            }
            break;
        case STA_SINGER:
            // drawList
            //加一个listview
            mIndex = 0; eIndex = -1;sIndex = -1;
            drawStaListview(type,0,0);
            break;
        case QUERY_MISSION:
        case QUERY_ENTRIES:
        case QUERY_SPORTSMAN:
            createQueryEditbox(type);
            break;
        case HELP:
            createHelpEditbox();
        case 0 ://do some things
            break;
        default:
            drawSubmenuList(m,t,MAIN_MENU_NUM+type-1);
            *bdomenu = FALSE;
            return 1;
    }
    return 0;
}

/*!
  画二级子菜单
  \param k: 需要写的子菜单在menuList里的下标
  */
void drawSubmenuList(int m, int t, int k)
{
    int		i, sub_index;

    wattron(menu_win[m], A_REVERSE);
    mvwprintw(menu_win[m], t+1, 1, menuList[m].str[t]);
    wrefresh(menu_win[m]);
    wattroff(menu_win[m], A_REVERSE);
    for(sub_index=0;sub_index<SUB_MENU_NUM;sub_index++){
        int listIndex = sub_index+MAIN_MENU_NUM;
        if (NULL == sub_win[sub_index]) {
            int w,h;
            switch(sub_index){//规定sub_win的位置
                case 0:
                    w=1;h=4;break;
            }
            sub_win[sub_index] = newwin(menuList[listIndex].n+2, 15, h, w*screenWidth/5+14);
            wbkgd(sub_win[sub_index], COLOR_PAIR(4));
            box(sub_win[sub_index], 0, 0);
            for (i = 0; i < menuList[listIndex].n; ++i) {
                mvwprintw(sub_win[sub_index], i+1, 1, menuList[listIndex].str[i]);
            }
        }
    }
    touchwin(editWin);
    sub_index = k-MAIN_MENU_NUM;
    wattron(sub_win[sub_index], A_REVERSE);
    mvwprintw(sub_win[sub_index], 1, 1, menuList[k].str[0]);
    wattroff(sub_win[sub_index], A_REVERSE);
    touchwin(sub_win[sub_index]);
    refresh();
    moveSubmenuList(m, t, k);
    delwin(sub_win[sub_index]);
    return;
}

/*在二级子菜单下监听*/
void moveSubmenuList(int m, int t, int k)
{
    bool	bdosubmenu = TRUE;
    int		ch, sub_index, choice;

    if (menuList[m].type[t]>0) {
        //sub_index = k-MAIN_MENU_NUM;//sub_index=3 ???
        sub_index = 0;
    }
    keypad(sub_win[sub_index], TRUE);
    choice = 0;
    while (bdosubmenu) {
        ch = wgetch(sub_win[sub_index]);
        if((shortcutsF(ch))) bdosubmenu = FALSE;
        mvwprintw(sub_win[sub_index], choice+1, 1, menuList[k].str[choice]);
        switch (ch) {
            case KEY_ESC:
                //drawMenuList(m,t);
                return;
                bdosubmenu = FALSE;
            case KEY_RIGHT:
                drawMenuList(m+1,t);
                bdosubmenu = FALSE;
                break;
            case KEY_LEFT:
                if (0 <= sub_index)
                    drawMenuList(m, t);
                bdosubmenu = FALSE;
                break;
            case KEY_UP:
                choice = (choice+menuList[k].n-1)%menuList[k].n;
                break;
            case KEY_DOWN:
                choice = (choice+menuList[k].n+1)%menuList[k].n;
                break;
            case ENTER:
                touchwin(editWin);
                mvwprintw(sub_win[sub_index],choice+1,1,menuList[k].str[choice]);
                switch (-menuList[k].type[choice]%10) {
                    case 2:
                        sIndex = 0;
                    case 1:
                        eIndex = 0;
                    case 0:
                        mIndex = 0;
                        break;
                    default:
                        break;
                }
                drawListview(menuList[k].type[choice],0);//制作一个列表显示代表团等信息
                break;
            default:
                break;
        }
        wattron(sub_win[sub_index], A_REVERSE);
        mvwprintw(sub_win[sub_index], choice+1, 1, menuList[k].str[choice]);
        wattroff(sub_win[sub_index], A_REVERSE);
        touchwin(editWin);
    }
}

/*!
  显示某类链表的数据
  \param type:0～3三类链表 index:链表内第index项
  */
void drawListview(int type,int index)
{
    touchwin(editWin);
    wattron(editWin,COLOR_PAIR(7));
    mvwprintw(editWin,screenHeight-5,5," |双击ESC : 退出到首页|");
    mvwprintw(editWin,screenHeight-5,30," |Enter : 在选中结点之前新建结点|");
    mvwprintw(editWin,screenHeight-5,65," |时间格式：YYYY-MM-DD|");
    wattroff(editWin,COLOR_PAIR(7));
    wrefresh(editWin);
    //读取链表
    WINDOW* listWin;
    listWin = newwin(21,30,5,4);
    wbkgd(listWin,COLOR_PAIR(3));
    wattron(listWin,COLOR_PAIR(7));
    box(listWin,0,0);
    mvwprintw(listWin,20,1,"  → :下级链表  ← :上级链表");
    wattron(listWin,COLOR_PAIR(6));
    int i = 0;
    Mission *cM = headMission;
    Entries *cE ;
    Sportsman *cS;
    keypad(listWin,TRUE);
    switch(-type%10){
        case 0:
            while(cM){
                if(index == i) wattron(listWin,A_REVERSE);
                mvwprintw(listWin,i+3,3,"%s",cM->index);
                if(index == i) wattroff(listWin,A_REVERSE);
                cM = cM->next;
                i++;
            }
            if(index == i) wattron(listWin,A_REVERSE);
            mvwprintw(listWin,i+3,3,"%s",cM->index);
            if(index == i) wattroff(listWin,A_REVERSE);
            i++;
            mvwprintw(listWin,0,2,"代表团");
            break;
        case 1:
            cM = fMByIndex();
            cE = cM->headEntries;
            while(cE){
                if(index == i) wattron(listWin,A_REVERSE);
                mvwprintw(listWin,i+3,3,"%s",cE->index);
                if(index == i) wattroff(listWin,A_REVERSE);
                cE = cE->next;
                i++;
            }
            if(index == i) wattron(listWin,A_REVERSE);
            mvwprintw(listWin,i+3,3,"%s",cE->index);
            if(index == i) wattroff(listWin,A_REVERSE);
            i++;
            mvwprintw(listWin,0,2,"参赛项目");
            break;
        case 2:
            cE = fEByIndex();
            cS = cE->headSportsman;
            while(cS){
                if(index == i) wattron(listWin,A_REVERSE);
                mvwprintw(listWin,i+3,3,"%s",cS->index);
                if(index == i) wattroff(listWin,A_REVERSE);
                cS = cS->next;
                i++;
            }
            if(index == i) wattron(listWin,A_REVERSE);
            mvwprintw(listWin,i+3,3,"%s",cS->index);
            if(index == i) wattroff(listWin,A_REVERSE);
            i++;
            mvwprintw(listWin,0,2,"参赛运动员");
            break;
        default:
            break;
    }
    createEditbox(type,index,0);
    wrefresh(listWin);
    curs_set(0);
    int ch,flag = 1;
    while(flag){
        ch = wgetch(listWin);
        if((shortcutsF(ch))) break;
        switch(ch){
            case KEY_ESC:
                eIndex = -1;
                sIndex = -1;
                drawMenuList(1,0);
                flag = 0;
                break;
            case KEY_UP:
                changeMESIndex(-1,i);
                drawListview(type,(index+i-1)%i);
                flag = 0;
                break;
            case KEY_DOWN:
                changeMESIndex(1,i);
                drawListview(type,(index+i+1)%i);
                flag = 0;
                break;
            case KEY_LEFT:
                if(sIndex != -1){
                    sIndex = -1;
                    drawListview(type+1,eIndex);
                } else if(eIndex != -1){
                    eIndex = -1;
                    drawListview(type+1,mIndex);
                }
                break;
            case KEY_RIGHT:
                if(eIndex == -1){
                    if(mIndex == i-1) break;
                    eIndex = 0;
                    drawListview(type-1,eIndex);
                    flag = 0;
                } else if(sIndex == -1){
                    if(eIndex == i-1) break;
                    sIndex = 0;
                    drawListview(type-1,sIndex);
                    flag = 0;
                }
                break;
            case ENTER:
                switch(type/10){
                    case CREATE:
                    case CHANGE:
                        createEditbox(type,index,1);
                        flag = 0;
                        break;
                    case DELETE:
                        if(index == i-1) break;
                        if((drawSelDialog("确定要删除？",0))){
                            delNode(type);
                            drawOKDialog("已成功删除！");
                            drawListview(type,index);
                            flag = 0;
                        } else {

                        }
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    refresh();
    delwin(listWin);
}

/*!
  显示某个结点的详细数据
  \param flag 是否进行监听
  */
void createEditbox(int type,int index,int flag)
{
    FORM *myForm;
    WINDOW *formWin;
    int rows,cols;

    formWin = newwin(21, 64, 5,40);
    wbkgd(formWin,COLOR_PAIR(3));
    wattron(formWin,COLOR_PAIR(7));
    box(formWin, 0, 0);
    mvwprintw(formWin,20,1,"  ENTER :确定  ESC :返回");
    mvwprintw(formWin,20,43,"| Backspace : 返回 |");
    initFields(formWin,type);
    set2Fields(type,flag);
    /* Create the form and post it */
    myForm = new_form(fields);
    /* Calculate the area required for the form */
    scale_form(myForm, &rows, &cols);

    keypad(formWin, TRUE);
    /* Set main window and sub window */
    set_form_win(myForm, formWin);
    set_form_sub(myForm, derwin(formWin, rows, cols, 2, 20));

    drawButtons(&formWin,-1,0,index,type);

    /* Print a border around the main window and print a title */
    post_form(myForm);
    curs_set(1);
    wrefresh(formWin);
    int ch;
    while(flag){//wgetch的参数是谁就相当与实现了一次touchwin
        ch=wgetch(formWin);
        if((shortcutsF(ch))) break;
        switch(ch){
            case KEY_ESC:
                drawListview(type,index);
                flag = 0;
                break;
            case '\t':
            case KEY_DOWN:
                form_driver(myForm, REQ_NEXT_FIELD);
                form_driver(myForm, REQ_END_LINE);
                break;
            case KEY_UP:
                form_driver(myForm, REQ_PREV_FIELD);
                form_driver(myForm, REQ_END_LINE);
                break;
            case KEY_LEFT:
                form_driver(myForm,REQ_LEFT_CHAR);
                break;
            case KEY_RIGHT:
                form_driver(myForm,REQ_RIGHT_CHAR);
                break;
            case KEY_BACKSPACE:
                form_driver(myForm, REQ_LEFT_CHAR);
                form_driver(myForm, REQ_DEL_CHAR);
                break;
            case KEY_DC:
                form_driver(myForm,REQ_DEL_CHAR);
                break;
            case KEY_HOME:
                form_driver(myForm,REQ_BEG_FIELD);
                break;
            case KEY_END:
                form_driver(myForm,REQ_END_FIELD);
                break;
            case ENTER:
                form_driver(myForm,REQ_NEXT_FIELD);
                curs_set(0);
                if(drawButtons(&formWin,0,0,index,type)){
                    flag = 0;
                }
                curs_set(1);
                form_driver(myForm,REQ_FIRST_FIELD);
                break;
                break;
            default:
                form_driver(myForm, ch);
                break;
        }
    }
    delwin(formWin);
    unpost_form(myForm);
}

/*!
  画button们
  \param mWin:被画的窗口 now:第now个button flag:第flag组string\param fields: form中用于显示及输入的域
  \todo 保存到链表
  */
int drawButtons(WINDOW **mWin,int now,int flag,int index,int type)
{
    int i;
    int id = 0;
    wattron(*mWin,COLOR_PAIR(6));
    int arraySize = ARRAY_SIZE(stringButton[flag]);
    char* str = (char*)malloc(sizeof(char)*20);
    for(i=0;i<arraySize;i++){
        if(now == i) wattron(*mWin,A_REVERSE);
        mvwprintw(*mWin,5+i*4,45,"%s",stringButton[flag][i]);
        if(now == i) wattroff(*mWin,A_REVERSE);
    }
    wrefresh(*mWin);
    int ch,isGoing = (now == -1)? 0 : 1 ;
    while(isGoing){
        ch=wgetch(*mWin);
        if((shortcutsF(ch))) break;
        Mission* pM;
        Entries* pE;
        Sportsman* pS;
        switch(ch){
            case KEY_UP:
                if(!drawButtons(mWin,(now+arraySize-1)%arraySize,flag,index,type)) return 0;
                break;
            case KEY_DOWN:
                if(!drawButtons(mWin,(now+arraySize+1)%arraySize,flag,index,type)) return 0;
                break;
            case KEY_LEFT:
            case KEY_ESC:
            case KEY_BACKSPACE:
                createEditbox(type,0,1);
                isGoing = false;
                return 0;
            case ENTER:
                switch(now){
                    case 0:
                        if((id=checkIsStd(type))>0){
                            sprintf(str,"第%d项不符合规范",id);
                            drawOKDialog(str); 
                            drawListview(type,index);
                            break;
                        }
                        //还要写保存到链表的代码
                        switch(type){
                            case CREATE_MISSION:
                                addMissionNode(index);
                                break;
                            case CREATE_ENTRIES:
                                addEntriesNode(index);
                                break;
                            case CREATE_SPORTSMAN:
                                addSportsmanNode(index);
                                break;
                            case CHANGE_MISSION:
                                pM = fMByIndex();
                                if(pM!=NULL)
                                    addMissionInfo(&pM);
                                break;
                            case CHANGE_ENTRIES:
                                pE = fEByIndex();
                                if(pE!=NULL)
                                    addEntriesInfo(&pE);
                                break;
                            case CHANGE_SPORTSMAN:
                                pS = fSByIndex();
                                if(pS!=NULL)
                                    addSportsmanInfo(&pS);
                                break;
                            default:
                                break;
                        }
                        drawListview(type,index);
                        isGoing = 0;
                        break;
                    case 1:
                        //清空
                        cleanFields();
                        mvwprintw(*mWin,5+4,45,"%s",stringButton[flag][1]);
                        isGoing = 0;
                        break;
                    case 2:
                        //退出到显示菜单
                        drawMenuList(0,0);
                        isGoing = 0;
                        break;
                }
                break;
        }
    }
    return 1;
}

/*!
  \param str 主要内容
  \param index 0:yes 1:no
  \return 1:yes 0:no
  */
int drawSelDialog(char* str,int index)
{
    WINDOW* diaWin;
    int i;
    diaWin = newwin(8,30,7,screenWidth/2-20);
    wbkgd(diaWin,COLOR_PAIR(4));
    mvwprintw(diaWin,2,5,"%s",str);
    wattron(diaWin,COLOR_PAIR(3));
    box(diaWin,0,0);
    wattroff(diaWin,COLOR_PAIR(3));
    char *yon[2] = {
        "Yes",
        "No",
    };
    for (i=0; i<2; i++) {
        if(index == i) wattron(diaWin,A_REVERSE);
        mvwprintw(diaWin,5,9*(i+1),"%s",yon[i]);
        if(index == i) wattroff(diaWin,A_REVERSE);
    }
    keypad(diaWin,TRUE);
    wrefresh(diaWin);
    int flag = 1,ch;
    while(flag){
        ch = wgetch(diaWin);
        if((shortcutsF(ch))) break;
        switch(ch){
            case KEY_LEFT:
                index = (index-1+2)%2;
                for (i=0; i<2; i++) {
                    if(index == i) wattron(diaWin,A_REVERSE);
                    mvwprintw(diaWin,5,9*(i+1),"%s",yon[i]);
                    if(index == i) wattroff(diaWin,A_REVERSE);
                }
                break;
            case KEY_RIGHT:
                index = (index+1+2)%2;
                for (i=0; i<2; i++) {
                    if(index == i) wattron(diaWin,A_REVERSE);
                    mvwprintw(diaWin,5,9*(i+1),"%s",yon[i]);
                    if(index == i) wattroff(diaWin,A_REVERSE);
                }
                break;
            case ENTER:
                delwin(diaWin);
                touchwin(editWin);
                flag = 0;
                return !index;
                break;
            default:
                break;
        }
    }
    touchwin(editWin);
    delwin(diaWin);
    return 0;
}

/*!
  通知型对话框,只有一个OK键
  \param str 显示在对话框里的字符串
 * */
void drawOKDialog(char* str)
{
    WINDOW* diaWin;
    diaWin = newwin(8,30,7,screenWidth/2-20);
    wbkgd(diaWin,COLOR_PAIR(4));
    mvwprintw(diaWin,3,5,"%s",str);
    wattron(diaWin,COLOR_PAIR(4));
    wattron(diaWin,A_REVERSE);
    box(diaWin,0,0);
    wattroff(diaWin,A_REVERSE);
    wattroff(diaWin,COLOR_PAIR(4));
    char *yon ="  OK  ";
    wattron(diaWin,COLOR_PAIR(4));
    wattron(diaWin,A_REVERSE);
    mvwprintw(diaWin,5,12,"%s",yon);
    wattroff(diaWin,A_REVERSE);
    wattroff(diaWin,COLOR_PAIR(4));
    touchwin(diaWin);
    wrefresh(diaWin);
    wgetch(diaWin);
    delwin(diaWin);
    touchwin(editWin);
    refresh();
}

/*为方便将对F(n)的判断做成一个ie函数*/
int shortcutsF(int ch)
{
    switch(ch){
        case KEY_F(2):
            drawMenuList(0,0);
            return 1;
        case KEY_F(3):
            drawMenuList(1, 0);
            return 1;
        case KEY_F(4):
            drawMenuList(2, 0);
            return 1;
        case KEY_F(5):
            drawMenuList(3, 0);
            return 1;
        case KEY_F(6):
            drawMenuList(4, 0);
            return 1;
        default:
            return 0;
    }
}

/*退出关闭屏幕*/
void exitScreen()
{
    int i;
    delwin(editWin);
    for (i = 0; i < MAIN_MENU_NUM; ++i)
        if (menu_win[i] != NULL)
            delwin(menu_win[i]);
    for (i = 0; i < SUB_MENU_NUM; ++i)
        if (sub_win[i] != NULL)
            delwin(sub_win[i]);
    endwin();	/* leave ncurses mode */
    exit(0);
}

/*!
  根据实现的定义给文本域长度
  \param type 当前链表类型
  \param i 当前第i个文本域
  \return 文本域长度
  */
int getFieldLength(int type,int i)
{
    switch(-type%10){
        case 0:
            switch(i){
                case 0:
                    return 6;
                default:
                    return 20;
            }
            break;
        case 1:
            switch(i){
                case 0:
                    return 4;
                case 2:
                    return 6;
                case 3:
                    return 8;
                case 10:
                    return 1;
                default:
                    return 20;
            }
            break;
        case 2:
            switch(i){
                case 0:
                case 3:
                    return 8;
                case 1:
                    return 4;
                case 2:
                    return 6;
                case 4:
                    return 1;
                case 5:
                    return 12;
                case 11:
                    return 20;
                default:
                    return 20;
            }
        default:
            return 20;
    }
}

/*清空文本域*/
void cleanFields()
{
    int i;
    for(i=0;i<16;i++){
        set_field_buffer(fields[i],0,"");
    }
}

/*!
  改变三级链表的标号
  \param uod up or down up:-1 down:-1
  \param size 当前链表一共的个数
  */
void changeMESIndex(int uod,int size)
{
    if(sIndex != -1){
        sIndex = (sIndex+uod+size)%size;
        return ;
    }
    if(eIndex != -1){
        eIndex = (eIndex+uod+size)%size;
        return ;
    }
    if(mIndex != -1){
        mIndex = (mIndex+uod+size)%size;
        return ;
    }
}

/*!
  显示统计信息的Editbox
 * */
void createStaEditbox(int type,int index,int flag)
{
    FORM *myForm;
    WINDOW *formWin;
    int rows = 15,cols = 60;

    curs_set(0);
    formWin = newwin(21, 64, 5,40);
    wbkgd(formWin,COLOR_PAIR(3));
    wattron(formWin,COLOR_PAIR(7));
    box(formWin, 0, 0);
    initFields(formWin,type);
    set2Fields(type,0);//将信息set进fields里面
    myForm = new_form(fields);
    /* Calculate the area required for the form */
    scale_form(myForm, &rows, &cols);
    keypad(formWin, TRUE);
    /* Set main window and sub window */
    set_form_win(myForm, formWin);
    set_form_sub(myForm, derwin(formWin, rows, cols, 2, 20));
    post_form(myForm);
    wrefresh(formWin);
    int ch;
    while(flag){//wgetch的参数是谁就相当与实现了一次touchwin
        ch=wgetch(formWin);
        if((shortcutsF(ch))) break;
        switch(ch){
            case KEY_ESC:
                curs_set(1);
                drawMenuList(3,0);
                flag = 0;
                break;
            default:
                break;
        }
    }
    curs_set(1);
    delwin(formWin);
    unpost_form(myForm);

}

/*根据类型获得文本域的个数*/
int getFieldsNum(int type)
{
    switch((-type)%10){
        case 0:
            return 16;
        case 1:
            return 11;
        case 2:
            return 12;
        case 3:
            return 6;
        case 4:
            return 3;
        case 5:
            return 6;
        case 6:
        case 7:
        case 8:
            return 1;
        default:
            return 0;
    }
}

/*初始化文本域*/
void initFields(WINDOW* mWin,int type)
{
    int i;
    int arraySize = getFieldsNum(type);
    for(i=0;i<arraySize;i++){
        int length = getFieldLength(type,i);
        fields[i] = new_field(1,length,i,2,0,0);
        set_field_back(fields[i], A_UNDERLINE);
        field_opts_off(fields[i], O_AUTOSKIP); /* Don't go to next field when this */
        wattron(mWin, A_REVERSE);
        mvwprintw(mWin,i+2,3,"%s: ",info[-type%10][i]);
        wattroff(mWin,A_REVERSE);
    }
    fields[arraySize] = NULL;
}

/*画统计中单项的列表*/
void drawStaListview(int type ,int typeindex,int index)
{
    touchwin(editWin);
    //读取链表
    WINDOW* listWin;
    listWin = newwin(21,30,5,4);
    wbkgd(listWin,COLOR_PAIR(3));
    wattron(listWin,COLOR_PAIR(6));
    box(listWin,0,0);
    int i = 0;
    Mission *cM = headMission;
    Entries *cE ;

    keypad(listWin,TRUE);
    switch(typeindex){
        case 0:
            while(cM){
                if(index == i) wattron(listWin,A_REVERSE);
                mvwprintw(listWin,i+3,3,"%s",cM->index);
                if(index == i) wattroff(listWin,A_REVERSE);
                cM = cM->next;
                i++;
            }
            if(index == i) wattron(listWin,A_REVERSE);
            mvwprintw(listWin,i+3,3,"%s",cM->index);
            if(index == i) wattroff(listWin,A_REVERSE);
            i++;
            mvwprintw(listWin,0,2,"代表团");
            break;
        case 1:
            cM = fMByIndex();
            cE = cM->headEntries;
            while(cE){
                if(index == i) wattron(listWin,A_REVERSE);
                mvwprintw(listWin,i+3,3,"%s",cE->index);
                if(index == i) wattroff(listWin,A_REVERSE);
                cE = cE->next;
                i++;
            }
            if(index == i) wattron(listWin,A_REVERSE);
            mvwprintw(listWin,i+3,3,"%s",cE->index);
            if(index == i) wattroff(listWin,A_REVERSE);
            i++;
            mvwprintw(listWin,0,2,"参赛项目");
            break;
        default:
            break;
    }
    if (typeindex) {
        createStaEditbox(type,0,0);
    } 
    wrefresh(listWin);
    curs_set(0);
    int ch,flag = 1;
    while(flag){
        ch = wgetch(listWin);
        if((shortcutsF(ch))) break;
        switch(ch){
            case KEY_ESC:
                eIndex = -1;
                sIndex = -1;
                drawMenuList(1,0);
                flag = 0;
                break;
            case KEY_UP:
                changeMESIndex(-1,i);
                drawStaListview(type,typeindex,(index+i-1)%i);
                flag = 0;
                break;
            case KEY_DOWN:
                changeMESIndex(1,i);
                drawStaListview(type,typeindex,(index+i+1)%i);
                flag = 0;
                break;
            case KEY_LEFT:
                if(eIndex != -1){
                    eIndex = -1;
                    drawStaListview(type,typeindex-1,mIndex);
                }
                break;
            case KEY_RIGHT:
                if(eIndex == -1){
                    if(mIndex == i-1) break;
                    eIndex = 0;
                    drawStaListview(type,typeindex+1,eIndex);
                    flag = 0;
                } 
                break;
            case ENTER:
                break;
            default:
                break;
        }
    }

}

/*画查询后显示的Editbox*/
void createQueryEditbox(int type)
{
    FORM *myForm;
    WINDOW *formWin;
    int rows = 15,cols = 60;
    int flag = 1;
    int index;
    char *str = (char*)malloc(30*sizeof(char));

    formWin = newwin(21, 64, 5,40);
    wbkgd(formWin,COLOR_PAIR(3));
    wattron(formWin,COLOR_PAIR(7));
    box(formWin, 0, 0);
    initFields(formWin,type);
    myForm = new_form(fields);
    scale_form(myForm, &rows, &cols);
    keypad(formWin, TRUE);
    set_form_win(myForm, formWin);
    set_form_sub(myForm, derwin(formWin, rows, cols, 2, 20));
    post_form(myForm);
    curs_set(1);
    wrefresh(formWin);
    int ch;
    while(flag){
        ch = wgetch(formWin); 
        if((shortcutsF(ch))) break;
        switch(ch){
            case ENTER: 
                form_driver(myForm,REQ_NEXT_FIELD);
                strcpy(str,field_buffer(fields[0],0));
                if (queryGoal(type,str)) {
                    switch(type){
                        case QUERY_MISSION:
                            index = mIndex; 
                            eIndex = -1;
                            sIndex = -1;
                            break;
                        case QUERY_ENTRIES:
                            index = eIndex;
                            sIndex = -1;
                            break;
                        case QUERY_SPORTSMAN:
                            index = sIndex;
                            break;
                        default:
                            index = 0;
                            break;
                    }
                    drawListview(type+66,index);
                } else{
                    mIndex = -1;
                    eIndex = -1;
                    sIndex = -1;
                    drawOKDialog("没有该数据！"); 
                }
                flag = 0;
                break;
            case KEY_ESC:
                drawMenuList(0,1);
                break;
            case KEY_LEFT:
                form_driver(myForm,REQ_LEFT_CHAR);
                break;
            case KEY_RIGHT:
                form_driver(myForm,REQ_RIGHT_CHAR);
                break;
            case KEY_BACKSPACE:
                form_driver(myForm, REQ_LEFT_CHAR);
                form_driver(myForm, REQ_DEL_CHAR);
                break;
            case KEY_DC:
                form_driver(myForm,REQ_DEL_CHAR);
                break;
            case KEY_HOME:
                form_driver(myForm,REQ_BEG_FIELD);
                break;
            case KEY_END:
                form_driver(myForm,REQ_END_FIELD);
                break;
            default:
                form_driver(myForm, ch);
                break;
        }
    }
}

/*!
  画退出时提醒是否保存的对话框
  \return 0:保存退出 1:不保存退出 2:取消
 * */
int drawSaveDialog(){
    WINDOW* diaWin;
    int i;
    int index = 0;
    diaWin = newwin(8,30,7,screenWidth/2-20);
    wbkgd(diaWin,COLOR_PAIR(4));
    mvwprintw(diaWin,3,5,"是否保存？");
    wattron(diaWin,COLOR_PAIR(3));
    box(diaWin,0,0);
    wattroff(diaWin,COLOR_PAIR(3));
    char *yon[3] = {
        "保存",
        "退出",
        "取消",
    };
    for (i=0; i<3; i++) {
        if(index == i) wattron(diaWin,A_REVERSE);
        mvwprintw(diaWin,5,6*(i+1),"%s",yon[i]);
        if(index == i) wattroff(diaWin,A_REVERSE);
    }
    keypad(diaWin,TRUE);
    wrefresh(diaWin);
    int flag = 1,ch;
    while(flag){
        ch = wgetch(diaWin);
        if((shortcutsF(ch))) break;
        switch(ch){
            case KEY_LEFT:
                index = (index-1+3)%3;
                for (i=0; i<3; i++) {
                    if(index == i) wattron(diaWin,A_REVERSE);
                    mvwprintw(diaWin,5,6*(i+1),"%s",yon[i]);
                    if(index == i) wattroff(diaWin,A_REVERSE);
                }
                break;
            case KEY_RIGHT:
                index = (index+1+3)%3;
                for (i=0; i<3; i++) {
                    if(index == i) wattron(diaWin,A_REVERSE);
                    mvwprintw(diaWin,5,6*(i+1),"%s",yon[i]);
                    if(index == i) wattroff(diaWin,A_REVERSE);
                }
                break;
            case ENTER:
                delwin(diaWin);
                touchwin(editWin);
                flag = 0;
                return index;
                break;
            default:
                break;
        }
    }
    touchwin(editWin);
    delwin(diaWin);
    return 0;
}

/*画帮助对话框*/
void createHelpEditbox(){
    WINDOW *formWin;
    int flag = 1;
    formWin = newwin(21, 64, 5,40);
    wbkgd(formWin,COLOR_PAIR(3));
    wattron(formWin,COLOR_PAIR(7));
    box(formWin, 0, 0);
    keypad(formWin, TRUE);
    curs_set(0);
    mvwprintw(formWin,3,4,"暂无");
    wrefresh(formWin);
    int ch;
    while(flag){
        ch = wgetch(formWin); 
        if((shortcutsF(ch))) break;
        switch (ch) {
            case KEY_ESC:
            case ENTER:
                flag = 0;
                drawMenuList(4,2);
                break;
            default:
                break;
        }
    }
    delwin(formWin);
}
