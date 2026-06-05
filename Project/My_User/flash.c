#include "flash.h"

char Arry[10]="";

float MOD_me(float num)
{
float result=0.0;
result =(int32)(1000.0 * num + 0.5) / 1000.0;
return result;
}


//将字符型的数字转化为float
float Str2num(char *str_ptr)
{
char IS_FLOAT=0;
int i=0;
char pos=0;
float result=0;
while(*(str_ptr+i)!='\0')
{
if(*(str_ptr+i)=='.')
{
IS_FLOAT=1;	
pos=i;
}
i++;
}
if(IS_FLOAT)
{
int j=0;
for(j=0;j<pos;j++)
{result+=(*(str_ptr+j)-0x30)*pow(10,pos-1-j);}
for(j=pos+1;j<i;j++)
result+=(*(str_ptr+j)-0x30)*pow(0.1,j-pos);
}
else if(!IS_FLOAT)
{
int j=0;
for(j=0;j<i;++j)
result+=(*(str_ptr+j)-0x30)*pow(10,i-j-1);
}
return MOD_me(result);
}


//将float的数字转化为字符型,结果存储在Arry中
void Num2str(float NUM)
{
int i=0,count=0;
float Copy=NUM;
for(;i<10;++i)
Arry[i]='\0';
while(1)
{
count+=1;
if((int)NUM/(int)pow(10,count)==0)
break;
}
if(NUM-(int)NUM>=-0.00001&&NUM-(int)NUM<=0.00001)//输入是整形
{
for(i=0;i<4-count;++i)
Arry[i]='0';
for(i=4-count;i<4;++i)
{
Arry[i]=(Copy/(int)pow(10,3-i))+0x30;
Copy=Copy-(Arry[i]-0x30)*pow(10,3-i);
}
}
else//输入是浮点数
{
float Float=MOD_me(NUM-(int)NUM);
for(i=0;i<count;++i)
{
Arry[i]=(Copy/pow(10,count-i-1))+0x30;
Copy=Copy-(Arry[i]-0x30)*pow(10,count-i-1);
}
Arry[count]='.';
i=1;
while(1)
{
Arry[count+i]=MOD_me((Float/pow(0.1,i)))+0x30;
Float-=(Arry[count+i]-0X30)*pow(0.1,i);
Float=MOD_me(Float);
i++;
if(Float==0)
{break;}
}
}
return;
}