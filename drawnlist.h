#include <ncurses.h>	/* stdio.h 也被包含在ncurses.h里面*/
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <form.h>

#define MAIN_MENU_NUM	5
#define SUB_MENU_NUM	1
#define KEY_ESC			27
#define ENTER			10
#define MENUNAME_LEN	20
#define MAX_MENU_ITEMS	10

#define NORMAL 0
#define SKIP -1
#define DIALOG -2
#define SAVE -3
#define CREATE -3
#define CREATE_MISSION -30
#define CREATE_ENTRIES -31
#define CREATE_SPORTSMAN -32
#define CHANGE -4
#define CHANGE_MISSION -40
#define CHANGE_ENTRIES -41
#define CHANGE_SPORTSMAN -42
#define DELETE -5
#define DELETE_MISSION -50
#define DELETE_ENTRIES -51
#define DELETE_SPORTSMAN -52
#define STA_ALL -63
#define STA_SINGER -74
#define STA_OTHER -85
#define QUERY -9
#define QUERY_MISSION -96
#define QUERY_ENTRIES -97
#define QUERY_SPORTSMAN -98
#define HELP -10
#define EXIT -'q'

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

struct MenuList {
	char	menuname[MENUNAME_LEN];
	char	str[MAX_MENU_ITEMS][MENUNAME_LEN];
	int		type[MAX_MENU_ITEMS];//正数代表有子菜单且子菜单序号，负数代表其他功能
	int		n;
} menuList[MAIN_MENU_NUM + SUB_MENU_NUM];

/*UTF8中每个汉字3个字节，
 * 因此以下凡是有可能出现汉字的字符串
 * 长度一律×1.5
 */
typedef struct _mission {               //代表团结构体
    char index[6+1];                    //代表团编号 
    char name[20+10+1];                 //代表团名称
    char country[20+10+1];              //所在国
    char headerName[20+10+1];           //团长姓名
    char headerContact[20+10+1];        //团长联系方式
    int numSports;                      //参赛运动项目数
    int numSportsman;                   //参赛运动员人数
    int numCoach;                       //教练员人数
    int numJudge;                       //裁判人数
    int numOtherman;                    //其他辅助人员人 
    char address[20+10+1];              //代表团入住地地 
    char telephone[20+10+1];            //代表团入住地电 
    int numRoom;                        //预定房间数
    int numTranslator;                  //需配备翻译人数
    char timeIn[20+10+1];               //入住奥运村时间
    char timeOut[20+10+1];              //离开奥运村时间
    struct _mission* next;              //下一个结点
    struct _entries* headEntries;       //参赛项目头指针
}Mission;
  
typedef struct _entries{                //参赛项目结构体
    char index[4+1];                    //参赛项目编号
    char name[20+10+1];                 //参赛项目名称
    char missionIndex[6+1];             //代表团编号
    char leaderName[8+4+1];             //项目领队姓名
    char leaderContact[20+10+1];        //领队联系方式
    int numCoach;                       //教练员人数
    int numSportsman;                   //参赛运动员人数
    char bestAch[20+10+1];              //历次取得最好成绩
    char bestAchTime[20+10+1];          //取得最好成绩时间
    char bestAchLoc[20+10+1];           //取得最好成绩地点
    char isBan;                         //违禁记录        
    struct _entries* next;              //下一个结点     
    struct _sportsman* headSportsman ;  //参赛选手头指针
}Entries;
 
typedef struct _sportsman{
    char index[8+1];                    //参赛选手编号
    char entriesIndex[4+1];             //参赛项目编号 
    char missionIndex[6+1];             //代表团编号
    char name[8+4+1];                   //参赛选手姓 
    char sex;                           //性别
    char birthday[12+1];                //出生日期
    char hometown[20+10+1];             //出生地
    int height;                         //身高
    int weight;                         //体重
    char finalistAch[20+10+1];          //入围成绩
    char bestAch[20+10+1];              //最好成绩
    char hobby[256+1];                  //兴趣爱好     
    struct _sportsman* next;
}Sportsman;


extern void initScreen(void);
extern void initColor(void);
extern void initMenu(void);
extern void draw_main_menu(WINDOW *win);
extern void drawMenuList(int m, int t);
extern void moveMenuList(int m, int n);
extern void drawSubmenuList(int m, int n, int k);
extern void moveSubmenuList(int m, int t, int k);
extern void moveMenuItem(int *,int *,int);
extern int doNext(int ,int ,_Bool *);
extern void exitScreen(void);
extern void drawListview(int,int);
extern void createEditbox(int,int,int);
extern int drawButtons(WINDOW**,int,int,int,int type);
extern void cleanFields();
extern void createStaEditbox(int type,int index,int flag);

extern void initList(void);
extern void saveList();

extern Mission* headMission ;
extern Mission* tailMission ;

extern WINDOW	*menu_win[MAIN_MENU_NUM];
extern WINDOW	*sub_win[SUB_MENU_NUM];
extern WINDOW	*editWin;
extern WINDOW   *helpWin;

extern int		screenHeight,screenWidth;
extern int		isOn ;
extern int      isChanged;
extern int		mIndex,eIndex,sIndex;
extern char *info[9][17];
extern char *stringButton[][3];

extern FIELD *fields[17];

extern int addMissionNode(int );
extern int addEntriesNode(int );
extern int addSportsmanNode(int );
extern void delNode(int type);
extern void delMissionNode(Mission* priM);
extern void delEntriesNode(Entries** phE,Entries** priE);
extern void delSportsmanNode(Sportsman** phS,Sportsman** priS);
extern void addMissionInfo(Mission **p);
extern void addEntriesInfo(Entries**);
extern void addSportsmanInfo(Sportsman**);

extern void setMInfo2Fields(Mission *pM);
extern void setEInfo2Fields(Entries *pE);
extern void setSInfo2Fields(Sportsman *pS);
extern void setInfo2Fields(int type);
extern void setIndex2Fields(int type);
extern Mission* fMByIndex();
extern Entries* fEByIndex();
extern Sportsman* fSByIndex();
extern void set2Fields(int type,int flag);
extern void calStaNSet2Fields();
extern void calOthNSet2Fields();
extern void calSigNSet2Fields();
extern int calMSportsmanNum(Mission* pM,int* mN, int* fN);
extern int calESportsmanNum(Entries* pE,int* mN,int* fN);
extern int checkIsStd(int type);
extern int queryGoal(int type,char* name);
extern void deleteAllList();
