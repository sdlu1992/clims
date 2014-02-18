#include "drawnlist.h"

int isChanged = 0;
Mission* headMission = NULL;
Mission* tailMission = NULL;

Mission* findMission();
Entries* findEntries();
Sportsman* findSportsman();
void addInfo2FatherE(Mission* pM,Entries* pE,Sportsman* pS);
void addInfo2FatherM(Mission* pM,Entries* pE);

/*初始化列表，从文件中读取*/
void initList()
{
	Mission *pM,*hpM = NULL;
	Entries *pE;
	Sportsman *pS;
	FILE *finMission,*finEntries,*finSportsman;
	if((finMission = fopen("mission.info","rb"))==NULL){
        finMission = fopen("mission.info","wb");  
        fclose(finMission);
    }
	if((finEntries = fopen("entries.info","rb"))==NULL){
        fopen("entries.info","wb"); 
        fclose(finEntries);
    }
	if((finSportsman = fopen("sportsman.info","rb"))==NULL){
        fopen("sportsman.info","wb") ;
        fclose(finSportsman);
        return;
    }
	//创建后进先出式链表
	while(!feof(finMission)){
		pM = (Mission *)malloc(sizeof(Mission));
		fread(pM,sizeof(Mission),1,finMission);
		pM->headEntries = NULL;
		if(!feof(finMission)){
			pM->next = hpM;
			hpM = pM;
		}
	}
	headMission = hpM;
	while(!feof(finEntries)){
		pE = (Entries *)malloc(sizeof(Entries));
		fread(pE,sizeof(Entries),1,finEntries);
		if(!feof(finEntries)){
			pM = headMission;
			while(pM!=NULL){
				if(!strcmp(pM->index,pE->missionIndex)){
					pE->headSportsman = NULL;
					pE->next = pM->headEntries;
					pM->headEntries = pE;
					break;
				} else
					pM = pM->next;
			}
		}
	}
	while(!feof(finSportsman)){
		pS = (Sportsman *)malloc(sizeof(Sportsman));
		fread(pS,sizeof(Sportsman),1,finSportsman);
		if(!feof(finSportsman)){
			pM = headMission;
			while(pM!=NULL){
				pE = pM->headEntries;
				while(pE!=NULL){
					if(!strcmp(pE->index,pS->entriesIndex)){
						pS->next = pE->headSportsman;
						pE->headSportsman = pS;
						break;
					} else
						pE = pE->next;
				}
				pM = pM->next;
			}
		}
	}
	fclose(finMission);
	fclose(finEntries);
	fclose(finSportsman);
}

/*保存链表到文件中*/
void saveList()
{
	FILE *foutM,*foutE,*foutS;
	Mission* pM = headMission;
	Entries* pE ;
	Sportsman* pS ;
	if((foutM = fopen("mission.info","wb+"))==NULL)
		exit(-1);
	if((foutE = fopen("entries.info","wb+"))==NULL)
		exit(-1);
	if((foutS = fopen("sportsman.info","wb+"))==NULL)
		exit(-1);
	while(pM!=NULL){
		fwrite(pM,sizeof(Mission),1,foutM);
		pE = pM->headEntries;
		while(pE!=NULL){
			fwrite(pE,sizeof(Entries),1,foutE);
			pS = pE->headSportsman;
			while(pS!=NULL){
				fwrite(pS,sizeof(Sportsman),1,foutS);
				pS = pS->next;
			}
			pE = pE->next;
		}
		pM = pM->next;
	}
	fclose(foutM);
	fclose(foutE);
	fclose(foutS);
    isChanged = 0;
}

/*!
添加代表团结点到链表
\param index 当前结点在当前链表的位置
\return 1:成功 0:失败
*/
int addMissionNode(int index)
{
	Mission *p,*pri,*aft;
	int i;
	p = (Mission *)malloc(sizeof(Mission));
	addMissionInfo(&p);//添加数据到结点
	strcpy(p->index,field_buffer(fields[0],0));
	if(index == 0){
        p->next = headMission;
        headMission = p;
    } else{
        pri = headMission;
        for(i=0;i<mIndex-1;i++){
            pri = pri->next;
        }
        aft = pri->next;
        pri->next = p;
        p->next = aft;
    }
    return 1;
}

/*!
  添加参赛项目结点到链表
  \param index 当前结点在当前链表的位置
  \return 1:成功 0:失败
  */
int addEntriesNode(int index)
{
    Entries *p,*pri,*aft;
    int i;
    p = (Entries *)malloc(sizeof(Entries));
    addEntriesInfo(&p);//添加数据到结点
    p->headSportsman = NULL;
    Mission *pM = fMByIndex();
    if(eIndex == 0){
        p->next = pM->headEntries;
        pM->headEntries = p;
    } else{
        pri = pM->headEntries;
        for(i=0;i<eIndex-1;i++){
            pri = pri->next;
        }
        aft = pri->next;
        pri->next = p;
        p->next = aft;
    }
    return 1;
}

/*!
  添加运动员结点到链表
  \param index 当前结点在当前链表的位置
  \return 1:成功 0:失败
  */
int addSportsmanNode(int index)
{
    Sportsman *p,*pri,*aft;
    int i;
    p = (Sportsman *)malloc(sizeof(Sportsman));
    addSportsmanInfo(&p);//添加数据到结点
    Entries *pE = fEByIndex();
    if(sIndex == 0){
        p->next = pE->headSportsman;
        pE->headSportsman = p;
    } else{
        pri = pE->headSportsman;
        for(i=0;i<sIndex-1;i++){
            pri = pri->next;
        }
        aft = pri->next;
        pri->next = p;
        p->next = aft;
    }
    return 1;
}

/*计算对应的代表团节点的教练和运动员人数*/
void addInfo2FatherM(Mission* pM,Entries* pE)
{
    pM->numCoach += pE->numCoach; 
    pM->numSports += 1;
}

void addInfo2FatherE(Mission* pM,Entries* pE,Sportsman* pS)
{
    pE->numSportsman += 1; 
    pM->numSportsman += 1;
}

/*!
  查找代表团节点，用到了全局变量
  \return 根据index找到的代表团结点
  */
Mission* findMission()
{
    Mission *pM = headMission;
    while(strcmp(pM->index,field_buffer(fields[2],0))){
        pM = pM->next;
    }
    return pM;
}

/*!
  查找参赛项目节点，用到了全局变量
  \return 根据index找到的参赛项目结点
  */
Entries* findEntries()
{
    Mission* pM ;
    if((pM=findMission())==NULL) return NULL;
    Entries* pE = pM->headEntries;
    while(strcmp(pE->index,field_buffer(fields[2],0))){
        pE = pE->next;
    }
    return pE;
}

/*!
  当添加修改代表团时，将文本域中的信息放到结点里面
  \param p 希望被添加信息的代表团结点
  */
void addMissionInfo(Mission** p)
{
    strcpy((*p)->index,field_buffer(fields[0],0));
    ((*p)->index)[6] = '\0';
    strcpy((*p)->name,field_buffer(fields[1],0));
    ((*p)->name)[20+10] = '\0';
    strcpy((*p)->country,field_buffer(fields[2],0));
    ((*p)->country)[20+10] = '\0';
    strcpy((*p)->headerName,field_buffer(fields[3],0));
    ((*p)->headerName)[20+10] = '\0';
    strcpy((*p)->headerContact,field_buffer(fields[4],0));
    ((*p)->headerContact)[20+10] = '\0';
    (*p)->numSports = atoi(field_buffer(fields[5],0));
    (*p)->numSportsman = atoi(field_buffer(fields[6],0));
    (*p)->numCoach = atoi(field_buffer(fields[7],0));
    (*p)->numJudge = atoi(field_buffer(fields[8],0));
    (*p)->numOtherman = atoi(field_buffer(fields[9],0));
    strcpy((*p)->address,field_buffer(fields[10],0));
    ((*p)->address)[20+10] = '\0';
    strcpy((*p)->telephone,field_buffer(fields[11],0));
    ((*p)->telephone)[20+10] = '\0';
    (*p)->numRoom = atoi(field_buffer(fields[12],0));
    (*p)->numTranslator = atoi(field_buffer(fields[13],0));
    strcpy((*p)->timeIn,field_buffer(fields[14],0));
    ((*p)->timeIn)[20] = '\0';
    strcpy((*p)->timeOut,field_buffer(fields[15],0));
    ((*p)->timeOut)[20] = '\0';
    isChanged = 1;//数据已改变，未保存
}

/*!
  当添加修改参赛项目时，将文本域中的信息放到结点里面
  \param p 希望被添加信息的参赛项目结点
  */
void addEntriesInfo(Entries **p)
{
    strcpy((*p)->index,field_buffer(fields[0],0));
    ((*p)->index)[4] = '\0';
    strcpy((*p)->name,field_buffer(fields[1],0));
    ((*p)->name)[20+10] = '\0';
    strcpy((*p)->missionIndex,field_buffer(fields[2],0));
    ((*p)->missionIndex)[6] = '\0';
    strcpy((*p)->leaderName,field_buffer(fields[3],0));
    ((*p)->leaderName)[8+4] = '\0';
    strcpy((*p)->leaderContact,field_buffer(fields[4],0));
    ((*p)->leaderContact)[20+10] = '\0';
    (*p)->numCoach = atoi(field_buffer(fields[5],0));
    (*p)->numSportsman = atoi(field_buffer(fields[6],0));
    strcpy((*p)->bestAch,field_buffer(fields[7],0));
    ((*p)->bestAch)[20+10] = '\0';
    strcpy((*p)->bestAchTime,field_buffer(fields[8],0));
    ((*p)->bestAchTime)[20+10] = '\0';
    strcpy((*p)->bestAchLoc,field_buffer(fields[9],0));
    ((*p)->bestAchLoc)[20+10] = '\0';
    strcpy(&((*p)->isBan),field_buffer(fields[10],0));
    isChanged = 1;//数据已改变，未保存
}

/*!
  当添加修改运动员时，将文本域中的信息放到结点里面
  \param p 希望被添加信息的运动员结点
  */
void addSportsmanInfo(Sportsman **p)
{
    strcpy((*p)->index,field_buffer(fields[0],0));
    ((*p)->index)[8] = '\0';
    strcpy((*p)->entriesIndex,field_buffer(fields[1],0));
    ((*p)->entriesIndex)[4] = '\0';
    strcpy((*p)->missionIndex,field_buffer(fields[2],0));
    ((*p)->missionIndex)[6] = '\0';
    strcpy((*p)->name,field_buffer(fields[3],0));
    ((*p)->name)[8+4] = '\0';
    strcpy(&((*p)->sex),field_buffer(fields[4],0));
    strcpy((*p)->birthday,field_buffer(fields[5],0));
    ((*p)->birthday)[12] = '\0';
    strcpy((*p)->hometown,field_buffer(fields[6],0));
    ((*p)->hometown)[20+10] = '\0';
    (*p)->height = atoi(field_buffer(fields[7],0));
    (*p)->weight = atoi(field_buffer(fields[8],0));
    strcpy((*p)->finalistAch,field_buffer(fields[9],0));
    ((*p)->finalistAch)[20+10] = '\0';
    strcpy((*p)->bestAch,field_buffer(fields[10],0));
    ((*p)->bestAch)[20+10] = '\0';
    strcpy((*p)->hobby,field_buffer(fields[11],0));
    ((*p)->hobby)[256] = '\0';
    isChanged = 1;//数据已改变，未保存
}

/*!
  将代表团结点中的数据set给文本域以显示
  \param p 希望被显示的结点
  */
void setMInfo2Fields(Mission *p)
{
    set_field_buffer(fields[0],0,p->index);
    set_field_buffer(fields[1],0,p->name);
    set_field_buffer(fields[2],0,p->country);
    set_field_buffer(fields[3],0,p->headerName);
    set_field_buffer(fields[4],0,p->headerContact);
    char* str = (char*)malloc(sizeof(char)*20);
    sprintf(str,"%d",p->numSports);
    set_field_buffer(fields[5],0,str);
    sprintf(str,"%d",p->numSportsman);
    set_field_buffer(fields[6],0,str);
    sprintf(str,"%d",p->numCoach);
    set_field_buffer(fields[7],0,str);
    sprintf(str,"%d",p->numJudge);
    set_field_buffer(fields[8],0,str);
    sprintf(str,"%d",p->numOtherman);
    set_field_buffer(fields[9],0,str);
    set_field_buffer(fields[10],0,p->address);
    set_field_buffer(fields[11],0,p->telephone);
    sprintf(str,"%d",p->numRoom);
    set_field_buffer(fields[12],0,str);
    sprintf(str,"%d",p->numTranslator);
    set_field_buffer(fields[13],0,str);
    set_field_buffer(fields[14],0,p->timeIn);
    set_field_buffer(fields[15],0,p->timeOut);
}

/*!
  将参赛项目结点中的数据set给文本域以显示
  \param p 希望被显示的结点
  */
void setEInfo2Fields(Entries *p)
{
    set_field_buffer(fields[0],0,p->index);
    set_field_buffer(fields[1],0,p->name);
    set_field_buffer(fields[2],0,p->missionIndex);
    set_field_buffer(fields[3],0,p->leaderName);
    set_field_buffer(fields[4],0,p->leaderContact);
    char* str = (char*)malloc(sizeof(char)*20);
    sprintf(str,"%d",p->numCoach);
    set_field_buffer(fields[5],0,str);
    sprintf(str,"%d",p->numSportsman);
    set_field_buffer(fields[6],0,str);
    set_field_buffer(fields[7],0,p->bestAch);
    set_field_buffer(fields[8],0,p->bestAchTime);
    set_field_buffer(fields[9],0,p->bestAchLoc);
    sprintf(str,"%c",p->isBan);
    set_field_buffer(fields[10],0,str);
}

/*!
  将运动员结点中的数据set给文本域以显示
  \param p 希望被显示的结点
  */
void setSInfo2Fields(Sportsman *p)
{
    set_field_buffer(fields[0],0,p->index);
    set_field_buffer(fields[1],0,p->entriesIndex);
    set_field_buffer(fields[2],0,p->missionIndex);
    set_field_buffer(fields[3],0,p->name);
    set_field_buffer(fields[4],0,&(p->sex));
    set_field_buffer(fields[5],0,p->birthday);
    set_field_buffer(fields[6],0,p->hometown);
    char* str = (char*)malloc(sizeof(char)*20);
    sprintf(str,"%d",p->height);
    set_field_buffer(fields[7],0,str);
    sprintf(str,"%d",p->weight);
    set_field_buffer(fields[8],0,str);
    set_field_buffer(fields[9],0,p->finalistAch);
    set_field_buffer(fields[10],0,p->bestAch);
    set_field_buffer(fields[11],0,p->hobby);
}

/*!
  根据type分类set哪一类别的Info到文本域
  \param type 当前结点的类型
 * */
void setInfo2Fields(int type)
{
    Mission *pM = headMission;
    Entries *pE;
    Sportsman *pS;
    switch(-type%10){
        case 0:
            pM = fMByIndex();
            if(pM!=NULL)	setMInfo2Fields(pM);
            break;
        case 1:
            pE = fEByIndex();
            if(pE!=NULL)	setEInfo2Fields(pE);
            break;
        case 2:
            pS = fSByIndex();
            if(pS!=NULL)	setSInfo2Fields(pS);
            break;
        default:
            break;
    }
}

/*!
  根据全局变量mIndex查找代表团结点
  \return 被查找的结点，如无返回NULL
  */
Mission* fMByIndex()
{
    Mission* pM = headMission;
    int i;
    for(i = 0;i<mIndex;i++){
        pM = pM->next;
    }
    return pM;
}

/*!
  根据全局变量mIndex,eIndex查找参赛项目结点
  \return 被查找的结点，如无返回NULL
  */
Entries* fEByIndex()
{
    Mission *pM = fMByIndex();
    if(pM==NULL) return NULL;
    Entries *pE = pM->headEntries;
    int i;
    for(i = 0;i<eIndex;i++){
        pE = pE->next;
    }
    return pE;
}

/*!
  根据全局变量mIndex,eIndex,sIndex查找运动员结点
  \return 被查找的结点，如无返回NULL
  */
Sportsman* fSByIndex()
{
    Entries *pE = fEByIndex();
    if(pE == NULL) return NULL;
    Sportsman *pS = pE->headSportsman;
    int i;
    for(i = 0;i<sIndex;i++){
        pS = pS->next;
    }
    return pS;
}

/*!
  当新建结点时只显示它上一级链表的index
  \param type 当前结点的类型
  */
void setIndex2Fields(int type)
{
    Mission* pM = headMission;
    Entries* pE ;
    switch(type){
        case CREATE_ENTRIES:
            pM = fMByIndex();
            if(pM!=NULL)
                set_field_buffer(fields[2],0,pM->index);
            break;
        case CREATE_SPORTSMAN:
            pE = fEByIndex();
            if(pE!=NULL){
                set_field_buffer(fields[1],0,pE->index);
                set_field_buffer(fields[2],0,pE->missionIndex);
            }
            break;
    }
}

/*!
  根据全局变量mIndex,eIndex,sIndex选择删除的结点
  \param type 根据type选择删除的结点类型
  */
void delNode(int type)
{
    Mission* pM = headMission,*priM = headMission;
    Entries* pE,*priE;
    Sportsman *priS;
    switch(-type%10){
        case 0:
            if(mIndex == 0)
                priM = NULL;
            else{
                mIndex--;
                priM = fMByIndex();
                mIndex++;
            }
            delMissionNode(priM);
            break;
        case 1:
            pM = fMByIndex();
            if(eIndex == 0){
                priE = NULL;
                delEntriesNode(&(pM->headEntries),NULL);
            } else {
                eIndex--;
                priE = fEByIndex();
                eIndex++;
                delEntriesNode(&(pM->headEntries),&priE);
            }
            break;
        case 2:
            pE = fEByIndex();
            if(sIndex == 0){
                delSportsmanNode(&(pE->headSportsman),NULL);
            } else{
                sIndex--;
                priS = fSByIndex();
                sIndex++;
                delSportsmanNode(&(pE->headSportsman),&priS);
            }
            break;
    }
    isChanged = 1;
}

/*删除代表团结点*/
void delMissionNode(Mission *priM)
{
    Mission *pM;
    Entries *pE;
    if(priM == NULL){
        pM = headMission;
        headMission = pM->next;
    } else {
        pM = priM->next;
        priM->next = pM->next;
    }
    while(pE){
        delEntriesNode(&(pM->headEntries),NULL);
        pE = pE->next;
    }
    free(pM);
}

/*删除参赛项目结点*/
void delEntriesNode(Entries** phE,Entries** priE)
{
    Entries *pE;
    Sportsman *pS;
    if(*phE == NULL) return;
    if(priE == NULL){
        pE = *phE;
        *phE = pE->next;
    } else {
        pE = (*priE)->next;
        (*priE)->next = pE->next;
    }
    while(pS){
        delSportsmanNode(&(pE->headSportsman),NULL);
        pS = pS->next;
    }
    free(pE);
}

/*删除运动员结点*/
void delSportsmanNode(Sportsman** phS,Sportsman** priS)
{
    Sportsman* pS;
    if(*phS == NULL) return;
    if(priS == NULL){
        pS = *phS;
        *phS = pS->next;
    } else {
        pS = (*priS)->next;
        (*priS)->next = pS->next;
    }
    free(pS);
}

/*将节点信息复制到域中*/
void set2Fields(int type,int flag)
{
    switch(type/10){
        //  info就是在这输入的
        case CREATE:
            setIndex2Fields(type);
            if(flag==0){
                setInfo2Fields(type);
            }
            break;
        case CHANGE:
        case DELETE:
            setInfo2Fields(type);
            break;
        case -6://总体的统计
            calStaNSet2Fields();
            break;
        case -7://排序的那个
            if (eIndex!=-1) {
                calSigNSet2Fields();
            }
            break;
        case -8://其他统计
            calOthNSet2Fields();
            break;
        default:
            break;
    }
}

/*计算统计中总体那个选项
 * 并且把值赋到文本域中*/
void calStaNSet2Fields()
{
    Mission* pM = headMission;
    Entries* pE ;
    int entriesNum = 0;
    int sportsmanNum = 0;
    int judgeNum = 0;
    int coachNum = 0;
    int maleNum = 0,femaleNum = 0;
    char *str = (char *)malloc(sizeof(int));
    while (pM) {
        pE = pM->headEntries;
        while(pE){
            entriesNum += 1;
            pE = pE->next;
        }
        sportsmanNum += calMSportsmanNum(pM,&maleNum,&femaleNum);
        judgeNum += pM->numJudge;
        coachNum += pM->numCoach;
        pM = pM->next;
    }
    //set2Field
    sprintf(str,"%d",entriesNum);
    set_field_buffer(fields[0],0,str);
    sprintf(str,"%d",sportsmanNum);
    set_field_buffer(fields[1],0,str);
    sprintf(str,"%d",judgeNum);
    set_field_buffer(fields[2],0,str);
    sprintf(str,"%d",coachNum);
    set_field_buffer(fields[3],0,str);
    sprintf(str,"%d",maleNum);
    set_field_buffer(fields[4],0,str);
    sprintf(str,"%d",femaleNum);
    set_field_buffer(fields[5],0,str);
}

/*计算统计中其他选项
 * 并且把值赋到文本域中*/
void calOthNSet2Fields()
{
    Entries* pEn[3],*pE;
    Mission* pM = headMission;
    Sportsman* pSn[3],*pS;
    while(pM){
        if((pEn[0]=pM->headEntries) != NULL)
            break; 
        pM = pM->next; 
    }
    pM = headMission;
    while(pM){
        if((pE = pM->headEntries) == NULL){
            pM = pM->next;
            continue;
        }
        while (pE->headSportsman==NULL) {
            pE = pE->next;
            if(pE == NULL)
                break;
        } 
        if (pE == NULL) {
            pM = pM->next;
            continue;
        }
        break;

    }
    if(pEn[0] == NULL)
        return;
    pEn[1] = pEn[0];
    pEn[2] = pEn[0];
    pSn[0] = pE->headSportsman;
    int i,j;
    FILE* finEntries ;
    if((finEntries = fopen("entries.info","rb"))==NULL)
        return;
    while(!feof(finEntries)) {
        pE = (Entries *)malloc(sizeof(Entries));
        fread(pE,sizeof(Entries),1,finEntries);
        if(feof(finEntries)) break;
        for (i=0; i<3; i++) {
            if(pE->numSportsman<pEn[i]->numSportsman){
                continue;
            }
            for (j=2; j>i; j--) {
                pEn[j] = pEn[j-1];
            }
            pEn[i] = pE; 
            i++;
        }
    }
    fclose(finEntries);
    for (i=0; i<3; i++) {
        set_field_buffer(fields[i],0,pEn[i]->name);
    }

    if (pSn[0] == NULL) 
        return;
    pSn[1] = pSn[0];
    pSn[2] = pSn[0];
    FILE* finSportsman; 
    if ((finSportsman = fopen("sportsman.info","rb"))==NULL)
        return;
    while (!feof(finSportsman)) {
        pS = (Sportsman*)malloc(sizeof(Sportsman));
        fread(pS,sizeof(Sportsman),1,finSportsman);
        if(feof(finSportsman)) break;
        for (i=0; i<3; i++) {
            if(pS->weight<pSn[i]->weight)
                continue;
            for (j=2 ; j>i ; j--) {
                pSn[j] = pSn[j-1];
            }
            pSn[i] = pS; 
            i++;
            break;
        }
    }
    for (i=0; i<3; i++) {
        set_field_buffer(fields[i+3],0,pSn[i]->name);
    }

    fclose(finSportsman);
}

/*计算统计中单项选项
 * 并且把值赋到文本域中*/
void calSigNSet2Fields()
{
    Entries* pE = fEByIndex();    
    if(pE == NULL) return;
    Sportsman* pS = pE->headSportsman;
    int sportsmanNum = 0;
    int banMissionNum = 0;
    while(pS){
        pS = pS->next; 
        sportsmanNum++;
    }
    Mission* pM = headMission;
    while (pM) {
        Entries* pE1 = pM->headEntries;
        while (pE1) {
            if(!strcmp(pE1->index,pE->index)){
                if (pE1->isBan == 'Y' || pE1->isBan == 'y') {
                    banMissionNum++;
                }
            }
            pE1 = pE1->next;
        }
        pM = pM->next;
    }
    char* str = (char*)malloc(sizeof(int));
    sprintf(str,"%d",sportsmanNum);
    set_field_buffer(fields[0],0,str);
    sprintf(str,"%d",pE->numCoach);
    set_field_buffer(fields[1],0,str);
    sprintf(str,"%d",banMissionNum);
    set_field_buffer(fields[2],0,str);
}

/*! 计算一个代表团中的运动员总数
\param pM 被求的代表团结点 
\param mN 男运动员数，会被改动
\param fN 女运动员数，会被改动
\return 总运动员数
 * */
int calMSportsmanNum(Mission* pM,int* mN, int* fN)
{
    int sNum = 0;
    if(pM->headEntries == NULL) return 0;
    Entries* pE = pM->headEntries;
    while(pE){
        sNum += calESportsmanNum(pE,mN,fN);
        pE = pE->next; 
    }
    return sNum;
}

/*计算一个参赛项目中的运动员总数*/
int calESportsmanNum(Entries* pE,int* mN,int* fN)
{
    int sNum = 0;
    if(pE->headSportsman == NULL) return 0; 
    Sportsman* pS = pE->headSportsman;
    while(pS){
        sNum++;
        (pS->sex=='M'||pS->sex=='m') ? (*mN)++ : (*fN)++;
        pS = pS->next;
    }
    return sNum;
}

/*计算输入时数据是否合法*/
int checkIsStd(type)
{
    char* str ;
    switch(-type%10){
        case 0:
            break;
        case 1:
            str = field_buffer(fields[10],0);
            if(*str == 'Y'||*str == 'y'||*str == 'N'||*str == 'n'){

            } else{
                return 11; 
            }
            break;
        case 2:
            str = field_buffer(fields[4],0);
            if(*str == 'M'||*str == 'm'||*str == 'F'||*str == 'f'){

            } else{
                return 5; 
            }
            break; 
    }
    return 0;
}

/*!
根据编号或名称查找结点
\param type 需要查找的结点的类型
\param name 查找的数据，有可能是字符串，有可能是数字
\return 0:没找到 1:找到了 
 * */
int queryGoal(int type,char* name)
{
    Mission* pM = headMission;
    Entries* pE;
    Sportsman* pS;
    mIndex = 0; 
    int index = atoi(name); //若是字符串则index为0
    if (index) {//以index查询
        while (pM) {
            if(type == QUERY_MISSION){
                int i = atoi(pM->index);
                if(i != index){
                    pM = pM->next;
                    mIndex++;
                    continue; 
                } else {
                    return 1;//找到
                }
            }
            pE = pM->headEntries;
            eIndex = 0;
            while (pE) {
                int i = atoi(pE->index);
                if(type == QUERY_ENTRIES){
                    if (i != index) {
                        pE = pE->next;
                        eIndex++;
                        continue;
                    } else {
                        return 1; 
                    } 
                }
                pS = pE->headSportsman;
                sIndex = 0;
                while (pS) {
                    if(type == QUERY_SPORTSMAN){
                        int i = atoi(pS->index);
                        if(i != index){
                            pS = pS->next; 
                            sIndex++;
                            continue;
                        } else{
                            return 1; 
                        }
                    }
                    sIndex++;
                    pS = pS->next;
                }
                eIndex++;
                pE = pE->next;
            }
            mIndex++;
            pM = pM->next;
        }
        return 0;
    } else{//以name查询
        while (pM) {
            if(type == QUERY_MISSION){
                name[30] = '\0';
                if(strcmp(name,pM->name)){
                    pM = pM->next;
                    mIndex++;
                    continue; 
                } else {
                    return 1;//找到
                }
            }
            pE = pM->headEntries;
            eIndex = 0;
            while (pE) {
                if(type == QUERY_ENTRIES){
                    name[30] = '\0';
                    if (strcmp(name,pE->name)) {
                        pE = pE->next;
                        eIndex++;
                        continue;
                    } else {
                        return 1; 
                    } 
                }
                pS = pE->headSportsman;
                sIndex = 0;
                while (pS) {
                    if(type == QUERY_SPORTSMAN){
                        name[12] = '\0';
                        if(strcmp(name,pS->name)){
                            pS = pS->next; 
                            sIndex++;
                            continue;
                        } else{
                            return 1; 
                        }
                    }
                    sIndex++;
                    pS = pS->next;
                }
                eIndex++;
                pE = pE->next;
            }
            mIndex++;
            pM = pM->next;
        }
        return 0;
    }
}

/*清空，删除全部*/
void deleteAllList()
{
    fclose(fopen("mission.info","wb+"));
    fclose(fopen("entries.info","wb+"));
    fclose(fopen("sportsman.info","wb+"));
    while (headMission) {
        delMissionNode((Mission*)NULL);
    }
    isChanged = 0;
}
