//
//  main.c
//  HW1
//
//  Created by Shouzhi Huang on 3/8/16.
//  Copyright © 2016 Shouzhi Huang. All rights reserved.
//
//#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>         //for pause() used to debug
#include <math.h>
#define VARIANCE 83        //for part2
#define LOOPNUM 1000        //for part2
#define POPULATION 1000     //for part3


struct Position{                              //structure for store the Position
    char amino_acid;
    int x;
    int y;
    struct Position* next;
    struct Position* pre;
};

struct Direction{      //structure for store every steps' possible direction 1 represent has been tested 0 means not.
    int right;
    int up;
    int left;
    int down;
    int final_direction;                 // final_direction used to signal the final direction
    struct Direction * next;
    struct Direction * pre;
};
struct P_head_tail{
    struct Position * head;
    struct Position * tail;
};
struct D_head_tail{
    struct Direction *head;
    struct Direction *tail;
};
struct Configuration{
    struct P_head_tail P;
    struct D_head_tail D;
};
struct Population{
    struct Configuration generation[POPULATION];
    struct Population *nextGeneration;
};
int direction=0;                                //part1 & part2 direction generated by rand()
/*--------------------------------------------------------------------------------------*/
struct Configuration getConfiguration(char * data);   //p1
/*---convert a random number (range from 0 to 4 integer)to next amino acid Position------*/
void construct(int direction, struct Position* target);    //p1
/*--------------------------------------------------------------------------------------*/
struct P_head_tail createPosition();         //p1
/*--------------------------------------------------------------------------------------*/
struct D_head_tail createDirection();            //p1
/*--------------------------------------------------------------------------------------*/
void insertPosition(struct P_head_tail *P,struct Position * pointer);    //p1
/*--------------------------------------------------------------------------------------*/
void insertDirection(struct D_head_tail *D,struct Direction * pointer);  //p1
/*-------------------check if the Position of new amino acid--------------------------*/
int isDuplicate(struct Position* source, struct Position* newpoint);   //p1
/*--------------------------------------------------------------------------------------*/
int isTested(int direction, struct Direction* test);      //p1
/*--------------------------------------------------------------------------------------*/
void Remove_Position_sp(struct P_head_tail * P);             //p1 remove single point of position list
/*--------------------------------------------------------------------------------------*/
void Remove_Direction_sp(struct D_head_tail * D);             // p1 remove single point of direction list
/*--------------------------------------------------------------------------------------*/
void initialize(struct Direction * target);                 // p1 initialize a Direction structrue
/*--------------------------------------------------------------------------------------*/
int isfull(struct Direction * target);             // p1 check if all direction have been tested
/*--------------------------------------------------------------------------------------*/
void setDirection(int direction,struct Direction * target);   //p1   set direction according to the number
/*--------------------------------------------------------------------------------------*/
int energyCalculate_sum(struct Configuration * C);      //p1
/*--------------------------------------------------------------------------------------*/
int energyCalculate_single(struct Position * head);    //p1
/*--------------------------------------------------------------------------------------*/
double distanceCalculate(struct Position * start, struct Position * end);    //p1
/*--------------------------------------------------------------------------------------*/
void printProteinsequence(struct Configuration *C);    //p1
/*--------------------------------------------------------------------------------------*/
void dropConfiguration(struct Configuration *C);
/*--------------------------------------------------------------------------------------*/
void changeConfiguration(struct Configuration*C);
/*--------------------------------------------------------------------------------------*/
void acceptConfiguration(struct Configuration*C1,struct Configuration *C2);
/*--------------------------------------------------------------------------------------*/
struct Configuration copyProtein(struct Configuration *C);
/*--------------------------------------------------------------------------------------*/
int isOpposite(int first,int second);
/*--------------------------------------------------------------------------------------*/
int isCorrect(struct Configuration *C);
/*--------------------------------------------------------------------------------------*/
int isCircle(struct Direction * D);
/*--------------------------------------------------------------------------------------*/
int F(struct Configuration * C);
/*--------------------------------------------------------------------------------------*/
struct Configuration crossOver(struct Configuration * C1,struct Configuration * C2);
/*--------------------------------------------------------------------------------------*/
struct Configuration argmax(struct Configuration *g);
/*--------------------------------------------------------------------------------------*/
void dropGeneration(struct Population * P);
/*--------------------------------------------------------------------------------------*/
int main()
{
    srand((unsigned)time(NULL));
    /*-------------------------------------------------part1----------------------------------------------*/
    struct Configuration Protein;
    char data[100]={'P','H','P','H','P','H','P','H','P','P'
        ,'H','P','H','P','P','P','P','H','H','P','P','P','H','P','H','P','P','H',
        'P','P','P','H','P','P','H','P','H','H','P','P','P','H','H','H','H','P','H',
        'P','H','P','P','H','P','H','H','P','H','H','H','H','P','P','H','P','H','P',
        'P','H','P','H','H','P','H','H','P','H','P','H','P','P','H','P','H','H','H','H','E'};   //store input data
    printf("PartA:\n");
    printf("Formulate a configuration using Lattice model:\n");
    Protein=getConfiguration(data);          //generate valid configuration
    printProteinsequence(&Protein);
    printf("Energy is:%d\n",energyCalculate_sum(&Protein));
    printf("----------------------------------\n");
    /*---------------------------------------------part2------------------------------------------------*/
    int a[VARIANCE]={0};                        //index means reverse of energy value means number
    int E_old=0;                  // part2 old configuration energy
    int E_new=0;                   //part2 new configuration energy
    int Temp=0;                       //part2 Temperature
    struct Configuration newProtein;  //new configuration
    /*-----------------------------------------------------------------*/
    printf("PartB:\n");
    printf("Implement of the Metropolis algorithm to simulate the ensemble of configuration obeying Boltzmann distribution:\n");
    printf("Please give the temperature:");
    scanf("%d",&Temp);
    newProtein=copyProtein(&Protein);
    for (int j = 0; j < LOOPNUM; ++j)
    {
        acceptConfiguration(&Protein,&newProtein);   //every time we start from the accepted old configuration
        changeConfiguration(&newProtein);
        E_old=energyCalculate_sum(&Protein);
        E_new=energyCalculate_sum(&newProtein);
        if(E_new<E_old){
            acceptConfiguration(&newProtein,&Protein);
            a[-E_new]++;
        }
        else if(((double)rand()/(double)RAND_MAX)<(exp(-E_new/Temp)/exp(-E_old/Temp))){
            acceptConfiguration(&newProtein,&Protein);
            a[-E_new]++;
        }
        else{
            printf("reject\n");
        }
    }
    printf("The distribution is represented by an array,the index is the reverse value of energy and each element's value is the number of a given energy.\n");
    for (int i=0; i<VARIANCE; i++) {
        printf("%d,",a[i]);
    }
    printf("\n");
    dropConfiguration(&Protein);
    dropConfiguration(&newProtein);
    printf("\n");
    printf("------------------------------------------------------\n");
    /*------------------------part3---------------------------*/
    printf("PartC\n");
    printf("Implement a genetic algorithm to find the lowest energy configuration:\n");
    struct Population *P;           //define a set of population
    int rand1=0;
    int rand2=0;
    double z=0;
    P=(struct Population*)malloc(sizeof(struct Population));
    for(int i=0;i<POPULATION;i++){                //get 1000 valid configuration
        P->generation[i]=getConfiguration(data);
    }
    P->nextGeneration=(struct Population*)malloc(sizeof(struct Population));
    if(P->nextGeneration==NULL){
        printf("Point error\n");
        exit(0);
    }
    double t=10;              //initial temperature
    int n=0;
    double ave=0;
    double F_C=0;
    double F_best=0;
    double F_new=0;
    double T=0;
    struct Configuration best;
    best=argmax(P->generation);         //find current best configuration
    do{
        //struct Configuration best_old;
        struct Configuration best_new;      //find best configration of next generation
        struct Population *parent=P;         //record the parent generation
        if(P->nextGeneration==NULL){
            P->nextGeneration=(struct Population*)malloc(sizeof(struct Population));
        }
        if(P->nextGeneration==NULL){
            printf("POINT ERROR!\n");
        }
        n=0;
        T=10;
        while(n<POPULATION){
             struct Configuration C;
             rand1=rand()%POPULATION;
             rand2=rand()%POPULATION;
             C=crossOver(&(P->generation[rand1]), &(P->generation[rand2]));
             if(isCorrect(&C)){
                 ave=(F(&(P->generation[rand1]))+F(&(P->generation[rand2])))/2;
                 F_C=F(&C);
                 if(F_C>=ave){
                     P->nextGeneration->generation[n]=copyProtein(&C);       //place to next generation
                     dropConfiguration(&C);
                     n++;
                     
                 }
                 else{
                     z=(double)rand()/(double)RAND_MAX;
                     if(z<exp(-(ave-F_C)/T)){
                         P->nextGeneration->generation[n]=copyProtein(&C);
                                // printf("next generation constructing\n");
                         dropConfiguration(&C);
                         n++;
                     }
                 }
                 
             }
             else{
                  dropConfiguration(&C);
                  continue;
             }
            T=t/log(n);
         }

        P=P->nextGeneration;
        dropGeneration(parent);
        best_new=argmax(P->generation);
        F_best=F(&best);
        F_new=F(&best_new);
        printf("%f,%f\n",F_new,F_best);
        if((F_new-F_best)*(F_new-F_best)<=1){
            if(F_new>F_best){acceptConfiguration(&best_new,&best);}
            dropConfiguration(&best_new);
            break;
        }
        if(F_new==0){break;}
        if(F_new>F_best){
            acceptConfiguration(&best_new,&best);
        }
        dropConfiguration(&best_new);
    }while(1);
    printf("best configuration :\n");
    printProteinsequence(&best);
    printf("Energy is:%d\n",energyCalculate_sum(&best));
    dropConfiguration(&best);
    dropGeneration(P);
    return 0;
}


/*----------every thing below this line are implements of those fuctions have been defined at beginning------------------*/
/*---------------------------------------------------------------------------------------------------------------*/
struct Configuration getConfiguration(char* data){
    struct Configuration C;
    int i=0;                                       //loop counter
    int success=0;                                  // 1 means next Position of amino acid is valid 0 versus.
    struct Position * pointer=NULL;                // new amino acid Position
    struct Direction * D_temp=NULL;                  //part1  used to store new direction node
    struct Position * temp_head;               //used for record head
    struct P_head_tail P_result;
    struct D_head_tail D_result;
    P_result=createPosition();            //create linklist store position
    D_result=createDirection();         //create linklist store direction
    while((data[i]=='H')||(data[i]=='P')){
        /*insert a new point before the tail*/
        if(!pointer){
            pointer=(struct Position*)malloc(sizeof(struct Position));
            insertPosition(&P_result, pointer);       //insert point
            pointer->amino_acid=data[i];
        }
        if(P_result.tail->pre->pre==P_result.head){
            P_result.tail->pre->x=0;
            P_result.tail->pre->y=0;
            P_result.tail->pre->amino_acid=data[i];
        }
        /*randomly generate next point and check its validation*/
        else{
            if(!D_temp){         //  means D_temp haven't been initialized
                D_temp=(struct Direction*)malloc(sizeof(struct Direction));
                insertDirection(&D_result,D_temp);
                initialize(D_temp);
            }
            temp_head=P_result.head;       //keep linklist's head
           	while(!isfull(D_temp)){
           	    direction=rand()%4;         //randomly generate number 0 to 3
           	    if(!isTested(direction,D_temp)){    //if this direction hasn't been trialed
                    setDirection(direction,D_temp);
                    construct(direction,pointer);
                    if(!isDuplicate(temp_head,pointer)){
           	            success=1;
           	            break;
           	        }
           	        else{
                        continue;
           	        }
           	    }
           	    else{
                    continue;
           	    }
            }
            if(success){
                D_temp->final_direction=direction;      //record the correct direction
                D_temp=NULL;      //ready for next direction point
                pointer=NULL;     //ready for next Position point
                i++;
                success=0;       //reset
            }
            else if(isfull(D_temp)&&!success){
                D_temp=D_temp->pre;         //point to previous node
                pointer=pointer->pre;       //point to previous node
                Remove_Direction_sp(&D_result);         //delete current direction node
                Remove_Position_sp(&P_result);
                i--;
                continue;
            }
            else{
            }
        }
        if(i==0){
            pointer=NULL;
            i++;
        }
        
    }
    C.P=P_result;
    C.D=D_result;
    return C;
}
/*--------------convert a random number (range from 0 to 4 integer)to next amino acid Position-------------------*/
struct P_head_tail createPosition(){
    struct P_head_tail P;
    struct Position * head;
    struct Position * tail;
    head=(struct Position*)malloc(sizeof(struct Position));
    head->amino_acid='S';
    tail=(struct Position*)malloc(sizeof(struct Position));
    tail->amino_acid='E';
    head->next=tail;
    tail->pre=head;
    P.head=head;
    P.tail=tail;
    return P;
}
/*---------------------------------------------------------------------------------------------------------------*/
struct D_head_tail createDirection(){
    struct D_head_tail D;
    struct Direction * head;
    struct Direction * tail;
    head=(struct Direction*)malloc(sizeof(struct Direction));
    tail=(struct Direction*)malloc(sizeof(struct Direction));
    head->next=tail;
    tail->pre=head;
    D.head=head;
    D.tail=tail;
    return D;
}
/*---------------------------------------------------------------------------------------------------------------*/
void insertPosition(struct P_head_tail *P,struct Position * pointer){
    P->tail->pre->next=pointer;
    pointer->pre=P->tail->pre;
    pointer->next=P->tail;
    P->tail->pre=pointer;
}
/*---------------------------------------------------------------------------------------------------------------*/
void insertDirection(struct D_head_tail *D,struct Direction * pointer){
    D->tail->pre->next=pointer;
    pointer->pre=D->tail->pre;
    pointer->next=D->tail;
    D->tail->pre=pointer;
}
/*---------------------------------------------------------------------------------------------------------------*/

void construct(int direction, struct Position* target){
    if(direction==0){
        target->x=target->pre->x+1;
        target->y=target->pre->y;
    }
    else if(direction==1){
        target->x=target->pre->x;
        target->y=target->pre->y+1;
    }
    else if(direction==2){
        target->x=target->pre->x-1;
        target->y=target->pre->y;
    }
    else{
        target->x=target->pre->x;
        target->y=target->pre->y-1;
    }
}

/*--------------------------------------------------------------------------------------*/
int isDuplicate(struct Position* source, struct Position* newpoint){
    struct Position * temp_head=source->next;
    while(temp_head!=newpoint){
        if((newpoint->x==temp_head->x)&&(newpoint->y==temp_head->y)){
            return 1;
        }
        temp_head=temp_head->next;
    }
    return 0;
}
/*--------------------------------------------------------------------------------------*/
int isTested(int direction, struct Direction* test){
    if((direction==0)&&(test->right==0)){
        //test->right=1;
        return 0;
    }
    else if((direction==1)&&(test->up==0)){
        //test->up=1;
        return 0;
    }
    else if((direction==2)&&(test->left==0)){
        //test->left=1;
        return 0;
    }
    else if((direction==3)&&(test->down==0)){
        //test->down=1;
        return 0;
    }
    else{
        return 1;
    }
}
/*--------------------------------------------------------------------------------------*/
void Remove_Position_sp(struct P_head_tail *P){
    if(P!=NULL){
    struct Position* temp=NULL;
    temp=P->tail->pre;
    temp->pre->next=P->tail;
    P->tail->pre=temp->pre;
    free(temp);
    temp=NULL;
    }
    else{
        printf("NULL POINTER!\n");
    }
}

/*--------------------------------------------------------------------------------------*/
void Remove_Direction_sp(struct D_head_tail *D){
    if(D!=NULL){
    struct Direction* temp=NULL;
    temp=D->tail->pre;
    temp->pre->next=D->tail;
    D->tail->pre=temp->pre;
    free(temp);
    temp=NULL;
    }
    else{
        printf("NULL POINTER!\n");
    }
}
/*--------------------------------------------------------------------------------------*/
void initialize(struct Direction * target){
    target->right=0;
    target->up=0;
    target->left=0;
    target->down=0;
    target->final_direction=0;
}
/*--------------------------------------------------------------------------------------*/
int isfull(struct Direction * target){
    if(target->right&&target->up&&target->left&&target->down){
        return 1;
    }
    else{
        return 0;
    }
}
/*--------------------------------------------------------------------------------------*/
void setDirection(int direction,struct Direction * target){
    if(direction==0){
        target->right=1;
    }
    else if(direction==1){
        target->up=1;
    }
    else if(direction==2){
        target->left=1;
    }
    else{
        target->down=1;
    }
}
/*--------------------------------------------------------------------------------------*/
int energyCalculate_sum(struct Configuration * C){
    int sum=0;
    struct Position *target=C->P.head->next;
    while(target->amino_acid!='E'){
        sum=energyCalculate_single(target)+sum;
        target=target->next;
    }
    return sum;
}

/*--------------------------------------------------------------------------------------*/
int energyCalculate_single(struct Position * target){
    int sum=0;
    struct Position * temp=target->next;
    
   	if(target->amino_acid=='H'){
   	   	while(temp->amino_acid!='E'){
            if((temp->amino_acid=='H')&&(distanceCalculate(target,temp)==1)){ //only hydrophobic amino acid and distance is 1 can affect
                sum--;
   	        }
   	        temp=temp->next;
   	    }
   	}
   	return sum;
}
/*--------------------------------------------------------------------------------------*/
double distanceCalculate(struct Position * start, struct Position * end){
    return sqrt((start->x-end->x)*(start->x-end->x)+(start->y-end->y)*(start->y-end->y));
}
/*--------------------------------------------------------------------------------------*/
struct Configuration copyProtein(struct Configuration * C){         //target is the configuration need to copy
    struct Configuration C_copy;
    struct Configuration C_temp;
    struct Position* point;
    struct Direction * D_temp;
    /*-------------------------------------------*/
    C_copy.P.head=(struct Position*)malloc(sizeof(struct Position));
    C_copy.P.head->amino_acid='S';                                     //this character means start
    C_copy.P.tail=(struct Position*)malloc(sizeof(struct Position));
    C_copy.P.tail->amino_acid='E';
    C_copy.P.head->next=C_copy.P.tail;
    C_copy.P.tail->pre=C_copy.P.head;
    /*-------------------------------------------*/
    C_copy.D.head=(struct Direction*)malloc(sizeof(struct Direction));                       //this character means start
    C_copy.D.tail=(struct Direction*)malloc(sizeof(struct Direction));
    C_copy.D.head->next=C_copy.D.tail;
    C_copy.D.tail->pre=C_copy.D.head;
    /*------------------------------------------*/
    /*------------------------------------------*/
    C_temp.D.head=C->D.head;     // C record
    C_temp.P.head=C->P.head;      // C record
    /*------------------------------------------*/
    C_temp.P.head=C_temp.P.head->next;
    while(C_temp.P.head->amino_acid!='E'){
        point=(struct Position*)malloc(sizeof(struct Position));
        C_copy.P.tail->pre->next=point;
        point->pre=C_copy.P.tail->pre;
        point->next=C_copy.P.tail;
        C_copy.P.tail->pre=point;
        point->amino_acid=C_temp.P.head->amino_acid;
        point->x=C_temp.P.head->x;
        point->y=C_temp.P.head->y;
        C_temp.P.head=C_temp.P.head->next;
    }
    C_temp.D.head=C_temp.D.head->next;
    while(C_temp.D.head!=C->D.tail){
        D_temp=(struct Direction*)malloc(sizeof(struct Direction));
        C_copy.D.tail->pre->next=D_temp;
        D_temp->pre=C_copy.D.tail->pre;
        D_temp->next=C_copy.D.tail;
        C_copy.D.tail->pre=D_temp;
        D_temp->final_direction=C_temp.D.head->final_direction;
        C_temp.D.head=C_temp.D.head->next;
    }
    return C_copy;
}
/*--------------------------------------------------------------------------------------*/
void changeConfiguration(struct Configuration * C){
    struct Position * temp;
    struct Direction * D_temp;
    struct Direction * D_temp_record;
    int old_direction=0;
    int rand_change=0;
    while(1){
        D_temp=C->D.head;
        temp=C->P.head->next;
        rand_change=rand()%VARIANCE+1;
        for (int i=0;i<rand_change;i++){
            D_temp=D_temp->next;
        }
        old_direction=D_temp->final_direction;
        D_temp_record=D_temp;
        while(1){
            direction=rand()%4;
            if(!isOpposite(D_temp->pre->final_direction,direction)&&(D_temp->final_direction!=direction)){
                D_temp->final_direction=direction;
                if(rand_change>=4){
                    if(!isCircle(D_temp)){
                        break;
                    }
                    else{
                        D_temp->final_direction=old_direction;
                    }
                }
                else{
                    break;
                }
            }
        }
        for (int i=0; i<rand_change; i++){
            temp=temp->next;
        }
        while (temp!=C->P.tail){
            construct(D_temp->final_direction,temp); //reconstruct configuration from change point
            temp=temp->next;
            if(D_temp!=C->D.tail){
               D_temp=D_temp->next;
            }
        }
        if(isCorrect(C)){
            break;
        }
        else{
            D_temp_record->final_direction=old_direction;
        }
    }
    
}
/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int isOpposite(int first,int second){
   if((first-second)*(first-second)==4)
   {return 1;}
   else{return 0;}
}
/*--------------------------------------------------------------------------------*/
void printProteinsequence(struct Configuration *C){
    struct Position * temp_head=C->P.head->next;
    while(temp_head->amino_acid!='E'){
        printf("(%d,%d)",temp_head->x,temp_head->y);
        printf("%c ",temp_head->amino_acid);
        temp_head=temp_head->next;
    }
    printf("\n");
}
/*--------------------------------------------------------------------------------*/
int isCorrect(struct Configuration *C){
    struct Position * pointer_inner=C->P.head->next;
    struct Position * pointer_outter=C->P.head->next;
    while(pointer_outter!=C->P.tail){
        pointer_inner=pointer_outter->next;
        while(pointer_inner!=C->P.tail){
            if((pointer_inner->x==pointer_outter->x)&&(pointer_inner->y==pointer_outter->y)){
                return 0;
            }
            pointer_inner=pointer_inner->next;
        }
        pointer_outter=pointer_outter->next;
    }
    return 1;
}
/*--------------------------------------------------------------------------------*/
int isCircle(struct Direction * D){
    struct Direction * C=D->pre;
    struct Direction * B=C->pre;
    struct Direction * A=B->pre;
    if(D->final_direction==0){
        if((A->final_direction==1)&&(B->final_direction==2)&&(C->final_direction==3)){
            return 1;
        }
    }
    else if(D->final_direction==1){
        if((A->final_direction==2)&&(B->final_direction==3)&&(C->final_direction==0)){
            return 1;
        }
    }
    else if(D->final_direction==2){
        if((A->final_direction==3)&&(B->final_direction==0)&&(C->final_direction==1)){
            return 1;
        }
    }
    else{
        if((A->final_direction==0)&&(B->final_direction==1)&&(C->final_direction==2)){
            return 1;
        }
    }
    return 0;
}
/*--------------------------------------------------------------------------------*/
void dropConfiguration(struct Configuration *C){
    if(C!=NULL){
    while(C->D.head->next!=C->D.tail){
        Remove_Direction_sp(&(C->D));
    }
    free(C->D.head);C->D.head=NULL;free(C->D.tail);C->D.tail=NULL;
    
    while(C->P.head->next!=C->P.tail){
        Remove_Position_sp(&(C->P));
    }
    free(C->P.head);C->P.head=NULL;free(C->P.tail);C->P.tail=NULL;
    }
    else{
        printf("NULL POINTER!\n");
    }
}
/*--------------------------------------------------------------------------------*/
void acceptConfiguration(struct Configuration *C1,struct Configuration *C2){   //copy C1 to C2
    struct Position *P_temp1=C1->P.head->next;
    struct Direction *D_temp1=C1->D.head->next;
    /*---------------------------------------*/
    struct Position *P_temp2=C2->P.head->next;
    struct Direction *D_temp2=C2->D.head->next;
    
    while(P_temp1!=C1->P.tail){
        P_temp2->amino_acid=P_temp1->amino_acid;
        P_temp2->x=P_temp1->x;
        P_temp2->y=P_temp1->y;
        P_temp1=P_temp1->next;
        P_temp2=P_temp2->next;
        if(D_temp1!=C1->D.tail){
            D_temp2->final_direction=D_temp1->final_direction;
            D_temp1=D_temp1->next;
            D_temp2=D_temp2->next;
        }
    }
}
/*--------------------------------------------------------------------------------*/
int F(struct Configuration * C){         //compute fit level
    if(C!=NULL){
        return -energyCalculate_sum(C);
    }
    else{
        printf("NULL\n");
        return 0;
    }
}
/*--------------------------------------------------------------------------------*/
struct Configuration crossOver(struct Configuration * C1,struct Configuration * C2){ //randomly find the crossover point
    struct Configuration c;
    
    struct Direction *temp1=C1->D.head;   //record head of C1  because we need front part of C1 as c's front part
    struct Direction *temp2=C2->D.tail;   //record tail of C2 because we need latter part of C2 as c's latter part
    struct Direction * pointer_D;
    struct Position * pointer_P;
    struct Position * pointer_amino_acid=C1->P.head->next;
    int counter=0;
    int random=0;
    while(temp1->next!=C1->D.tail){
        temp1=temp1->next;
        counter++;
    }
    
    //pause();
    random=(rand()%VARIANCE)+2;
    c.P=createPosition();
    c.D=createDirection();
    struct Direction *temp_c_head=c.D.head;
    struct Direction *temp_c_tail=c.D.tail;
    for(int i=0;i<counter;i++){                                            //insert point according to the counter
        pointer_D=(struct Direction*)malloc(sizeof(struct Direction));
        insertDirection(&(c.D), pointer_D);
    }
    temp1=C1->D.head;                           //reset temp1 to copy the front part of C1
    for(int i=0;i<random;i++){                  //random number represent the number of direction point get from C1
        temp1=temp1->next;
        temp_c_head=temp_c_head->next;
        temp_c_head->final_direction=temp1->final_direction;
    }
    for(int i=0;i<counter-random;i++){
        temp2=temp2->pre;
        temp_c_tail=temp_c_tail->pre;
        temp_c_tail->final_direction=temp2->final_direction;
    }
    pointer_P=(struct Position*)malloc(sizeof(struct Position));
    insertPosition(&(c.P), pointer_P);
    pointer_P->amino_acid=C1->P.head->next->amino_acid;
    pointer_P->x=C1->P.head->next->x;
    pointer_P->y=C1->P.head->next->y;
    temp_c_head=c.D.head->next;
    while(temp_c_head!=c.D.tail){
        pointer_P=(struct Position*)malloc(sizeof(struct Position));
        insertPosition(&(c.P), pointer_P);
        construct(temp_c_head->final_direction, pointer_P);
        pointer_P->amino_acid=pointer_amino_acid->amino_acid;
        pointer_amino_acid=pointer_amino_acid->next;
        temp_c_head=temp_c_head->next;
    }
    return c;
}
/*--------------------------------------------------------------------------------*/
struct Configuration argmax(struct Configuration *g){
    struct Configuration best;
    int a[POPULATION]={0};
    int max=0;              //find max F index
    for(int i=0;i<POPULATION;i++){
        a[i]=F((&(g[i])));
    }
    for(int i=0;i<POPULATION;i++){
        if(a[max]<=a[i]){
            max=i;
        }
    }
    best=copyProtein(&(g[max]));
    return best;
}
/*--------------------------------------------------------------------------------*/
void dropGeneration(struct Population *P){
    struct Population *temp=P;
    for (int i=0; i<POPULATION; i++) {
        dropConfiguration(&(temp->generation[i]));
    }
    if(!temp){free(temp);}
    printf("generation has been dropped!\n");
    
}
