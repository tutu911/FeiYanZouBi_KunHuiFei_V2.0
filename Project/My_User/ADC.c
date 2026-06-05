#include "ADC.h"

float ADC_GYH[ADC_Num]={0};					    //ADC归一化值
uint16 ADC_Get[ADC_Num]={0};				    //ADC处理后原始值
unsigned int ADC_Sum[ADC_Num]={0};			//ADC处理加和数组 
unsigned char ADC_Temp[ADC_Num]={0};		//ADC处理交换数组 
int ADC_Original[ADC_Num][ADC_Avenum] = {0};//ADC采集原始值
uint32  ADC_Max[5]={150,150,150,150,150};	//ADC归一化最大值数组  //是否需要修改最大值

/**
 * @description: 交换函数
 */	
void Swap(int *a, int *b)    
{  
int temp;  
temp = *a;  
*a = *b;  
*b = temp;  
return ;  
}  


/**
 * @description: ADC初始化
 */	
void ADC_Init1(void)
{                                      //五路电感引脚预设置
adc_init(ADC_P11, ADC_SYSclk_DIV_2);   //左横
adc_init(ADC_P05, ADC_SYSclk_DIV_2);	 //左竖				
adc_init(ADC_P03, ADC_SYSclk_DIV_2);	 //中横					
adc_init(ADC_P15, ADC_SYSclk_DIV_2);	 //右竖				
adc_init(ADC_P02, ADC_SYSclk_DIV_2);   //右横
adc_init(ADC_P16, ADC_SYSclk_DIV_2);	 //电池电压检测
adc_init(ADC_P10, ADC_SYSclk_DIV_2);	 //按键电压信号检测
}


/**
 * @description: ADC三次取中值枚举函数
 */	
uint16 ADC_Mid_Sample(ADCN_enum adcn)
{
uint16 i,j,k,tmp;
i = adc_once(adcn,ADC_8BIT);
j = adc_once(adcn,ADC_8BIT);
k = adc_once(adcn,ADC_8BIT);
if(i>j)
{
tmp=i;
i=j;
j=tmp;
}
if(k>j)tmp = j;
else if(k>i)tmp = k;
else tmp=i;
return tmp;
}


/**
 * @description: ADC通道均进行数据采集
 */	
void ADC_Sample()
{
int i,j,k,l;
for(i=0;i<ADC_Avenum;i++)
{   
ADC_Original[0][i] = ADC_Mid_Sample(ADC_P11);
ADC_Original[1][i] = ADC_Mid_Sample(ADC_P05);
ADC_Original[2][i] = ADC_Mid_Sample(ADC_P03);
ADC_Original[3][i] = ADC_Mid_Sample(ADC_P15); 
ADC_Original[4][i] = ADC_Mid_Sample(ADC_P02);
}
for(j=0;j<ADC_Avenum;j++)
{Quicksort(ADC_Original[j],ADC_Avenum,0, ADC_Avenum -1);}
for(k=0;k<ADC_Num;k++)
{   
for(l=1;l<ADC_Avenum-1;l++)
{ADC_Sum[k] += ADC_Original[k][l];}
}}


/**
 * @description: 电感平均值计算
 */	
void ADC_Average()
{
int i;
for(i=0;i<ADC_Num;i++)
{
ADC_Temp[i]=(ADC_Temp[i]*3+ADC_Sum[i]/(ADC_Avenum-2)*2)/5;
ADC_Get[i]=ADC_Temp[i];
ADC_Sum[i]=0;
}}


/**
 * @description: 电感归一化
 */
void ADC_Normalizing()  
{
int i=0;
for(i=0;i<ADC_Num;i++) 
{ADC_GYH[i]=100*ADC_Get[i]/ADC_Max[i];}
if(ADC_GYH[0]>100)ADC_GYH[0]=100;
if(ADC_GYH[1]>100)ADC_GYH[1]=100;
if(ADC_GYH[2]>100)ADC_GYH[2]=100;
if(ADC_GYH[3]>100)ADC_GYH[3]=100;
if(ADC_GYH[4]>100)ADC_GYH[4]=100;   
}


/**
 * @description: 快速排序函数
 */	
void Quicksort(int array[], int maxlen, int begin, int end) reentrant
{  
int i, j;  
if(begin < end)  
{  
i = begin + 1;   
j = end;        
while(i < j)  
{  
if(array[i] > array[begin])   
{  
Swap(&array[i], &array[j]);    
j--;  
}
else  
{i++;}    
}  
if(array[i] >= array[begin])  
{i--;}  
Swap(&array[begin], &array[i]);      
Quicksort(array, maxlen, begin, i);  
Quicksort(array, maxlen, j, end);  
}  
}
