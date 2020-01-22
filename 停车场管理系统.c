#include<stdio.h>
#include<stdlib.h>
#include <windows.h>			//"cls"
#include<string.h>              //strcmp
#include<time.h>                //time_t

#define MAXSIZE 100
#define M 4
#define N 5                    //停车场有M行N列

//车辆信息节点 
typedef struct carnode
{
	int row, line;
	int inHour;        //入场时间 1
	int inMin;         //30
	int liveHour;       //待多久1
	int liveMin;        //40
	int outHour;   //出场小时 3
	int outMin;    // 分钟 10
	char License[10];
}CarNode;
//用来存放时间的链表 
typedef struct carlist
{
	CarNode *data;          //data是结构体数组 
	struct carlist *next;
}CarList;
//声明每个全局变量
CarList *timeQ[25];
int park[M][N]={0};
int flag=0;                                    //判断停车场是否已满 

int oddEven(int n);
void printPark();                              //用二维数组存放停车场
void initList();                               //初始化链表
int *localTime();                              //求系统时间
CarNode *InCar(int r,int l);                   //车辆进入 
CarNode *initCarNode(int row,int line,char License[10]);
void List_add(CarList *list,CarNode *car);     //构造链表比较出场时间
void leaveCar();                               //新用户进来是否等待
void printCar(CarList *q);                     //打印车辆信息
void Welcome();					               //显示菜单
void smallwelcome();                           //子菜单
void HideCursor();                             //完成清屏功能（调用系统函数）
void printCars();                              //停车场已满，等待
CarNode *InCar(int r,int l);
int main()
{
	srand((unsigned)time(NULL));
	initList();
	Welcome();
	while(1){
	system("cls");						   // 清屏 
	Welcome();							   // 重新显示界面
		int i, cho;
		scanf("%d", &i);
		if (i == 1) {
			smallwelcome();
			printPark();
			if (flag>M*N) {
				printCars();
				break;
			}
		} 
		if (i == 2) {
			printf("\n欢迎您再次使用本系统呦 ε=ε=ε=(~￣▽￣)~\n\n");
			break;
		}
		printf("---------------------------------------\n");
		printf("\n若车已停好，请输入0\n");
		begin:								// goto 标志位 
		scanf("%d", &cho);
		if(cho == 0){
			continue;
		}
		else{
			printf("您的输入有误，请重新输入\n");
			goto begin;						// goto 到指定标志位 begin
		}
	}
	return 0;
}

int oddEven(int n)
{
	if(n%2!=0){                  //奇数 
		return 1;
	}else{
		return 0;
	}
} 
//初始化链表
void initList()
{
	int i;
	for(i=1; i<25; i++){
		timeQ[i] = (CarList *)malloc(sizeof(CarList));
		timeQ[i]->next=NULL;
	}
}
//判断后打印车辆信息
void printCar(CarList *q)
{
	printf("\t\t车牌号为%s的车主 \n",q->data->License);
	printf("\t\t会在北京时间%d时%d分离开停车场\n",q->data->outHour,q->data->outMin);
	printf("\t\t请您将车停在第%d行第%d列的位置上\n",q->data->row+1,q->data->line+1); 
}
//用二维数组存放停车场
void printPark()
{
	int i, j;
	for(i=0; i<M*2+1; i++) {
		for(j=0; j<N*2+1; j++) {
			if(!oddEven(i) && !oddEven(j)) {
				printf(" ");
			} else if(!oddEven(i) && oddEven(j)) {
				printf("---");
			} else if(oddEven(i) &&!oddEven(j)) {
				printf("|");
			} else if(oddEven(i) && oddEven(j)) {
				if(park[(i-1)/2][(j-1)/2]) {
					printf(" P ");
				}else{
					printf("   ");
				}
			}
		}
		printf("\n");
	}
}
//求系统时间 
int *localTime()
{
	time_t tmpcal_ptr;
	struct tm *tmp_ptr = NULL;
	time(&tmpcal_ptr);
	tmp_ptr = localtime(&tmpcal_ptr);
	int *a = (int*)malloc(sizeof(int)*2);
	a[0]= tmp_ptr->tm_hour;
	a[1]= tmp_ptr->tm_min;
	return a;
}
//车辆进入 
CarNode *InCar(int r,int l)
{
	if (park[r][l] == 0) {
		char *License=(char *)malloc(sizeof(char)*10);
		printf("输入车牌号:");
		getchar();
		gets(License);
		printf("---------------------------------------\n");
		getchar();
		CarNode *car = initCarNode(r,l,License);
		List_add(timeQ[localTime()[0]],car);
		park[r][l] = 1;
		flag++;
		return car;
	} else {
		printf("该车位已经被占了哦！\n");
		printf("请重新输入：");
		scanf("%d %d",&r,&l);
		return InCar(r,l);
	}
}

//汽车信息
CarNode *initCarNode(int row,int line,char License[10])
{
	CarNode *car = (CarNode *)malloc(sizeof(CarNode));
	car->row = row;
	car->line = line;
	
	int liveHour = rand()%4;
	int liveMin = rand()%61;
	car->liveHour = liveHour;
	car->liveMin = liveMin;
	
	car->inHour = localTime()[0];
	car->inMin = localTime()[1];
	
	car->outMin = (localTime()[1]+liveMin)%60;
	car->outHour = localTime()[0]+car->liveHour+(localTime()[1]+liveMin)/60;
	strcpy(car->License,License);
	return car;
}
//构造timeQ[25]链表
void List_add(CarList *list,CarNode *car)
{
	CarList *p = list;
	CarList *q = (CarList *)malloc(sizeof(CarList));
	q->data = car;
	while(p->next) {
		if(p->next->data->outMin < car->outMin) {
			break;
		}
		p = p->next;
	}
	q->next = p->next;
	p->next = q;
}
//新用户进来是否等待
void leaveCar()
{
	CarList *p = timeQ[localTime()[0]];
	CarList *q = NULL;
	
	while(p->next){
		if(p->next->data->outMin == localTime()[1]) {
			q = p->next;
			p->next = q->next;
			park[p->data->row][p->data->line] = 0;
			break;
		}
		p = p->next;
	}
	
	if(q)
	printCar(q);
}
//完成清屏时间控制功能（调用系统函数） 
void HideCursor()
{
 CONSOLE_CURSOR_INFO cursor_info = {1, 0};
 SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}
//显示菜单 
void Welcome()
{
	time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ("\t\t\t%s", asctime(timeinfo) );
    HideCursor();
	printf ("\t*******************目前停车场状况***********************\n");
    printf ("\t        停车场共有%d个车位,当前停车场共有%d辆车\n",M*N,flag);
    printf ("\t********************************************************\n");
    printf ("\t--------------Welcome to LZH's Car Parking--------------\n");
    printf ("\t*                                                      *\n");
    printf ("\t*           该停车场是一个%d行%d列的矩形停车场         *\n",M,N);
    printf ("\t*         系统会自动帮您分配停车时间，请合理安排时间   *\n");
    printf ("\t*                  1.将您的爱车停在该停车场            *\n");
    printf ("\t*                  2.退出管理系统                      *\n");
	printf ("\t*                                                      *\n");
    printf ("\t*------------------------------------------------------*\n");
    printf ("\t---------------Press key(1/2/) to run program ----------\n");
}    
//欢迎光临
void smallwelcome() 
{
    printf ("%d %d", localTime()[0],localTime()[1] );
	int r,l;
	printf("亲~请选择停车区域哦！（行数<%d，列数<%d）",M+1,N+1); 
	scanf("%d %d",&r,&l);

	if (r>M||l>N) {
		printf("\t\t本停车场没有你想象中的大哦！\n");
		smallwelcome(); 
		return ;
	}

	CarNode *car = InCar(r-1,l-1);
	List_add(timeQ[ localTime()[0] ],car);
	
	printf("\t\t请牌照为%s的汽车停入停车位的第%d行第%d列的位置上\n",car->License,r,l);
	printf("\t\t现在为北京时间:%d点%d分\n",localTime()[0],localTime()[1]);
	printf("\t\t为您分配的停车时间为：%d小时%d分钟\n",car->liveHour,car->liveMin); 
	printf("\t\t请您在%d点%d分之前离开停车场\n",car->outHour,car->outMin);
}
//停车场已满，打印最早离开的车辆信息
void printCars()
{
	int i;
	for(i=1; i<25; i++) {
		CarList *timeq = timeQ[i];
		while(timeq->next != NULL) {
			printCar(timeq->next);
			timeq = timeq->next;
			break;
		}
	}
}
