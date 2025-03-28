#include <iostream>
#include <chrono>
#include <windows.h>
#include <string>
#include <thread>
using namespace std;
const int N=4096*4096,T=100;
double sum[N],b[N],a[N];
void pre(int n){
    for(int i=0;i<n;i++)
        a[i]=i/3.0;
}

void cal1(int n){//平凡算法
    double sum=0.0;
    for(int i=0;i<n;i++)
        sum+=a[i];
}

void cal2_2(int n){//两路优化
    double sum,sum1=0.0,sum2=0.0;
    for(int i=0;i<n;i+=2){
        sum1+=a[i];
        sum2+=a[i+1];
    }
    sum=sum1+sum2;
}

void cal2_4(int n){//四路优化
    double sum,sum1=0.0,sum2=0.0,sum3=0.0,sum4=0.0;
    for(int i=0;i<n;i+=4){
        sum1+=a[i];sum2+=a[i+1];sum3+=a[i+2];sum4+=a[i+3];
    }
    sum=sum1+sum2+sum3+sum4;
}

void cal2_8(int n){//八路优化
    double sum,sum1=0.0,sum2=0.0,sum3=0.0,sum4=0.0,sum5=0.0,sum6=0.0,sum7=0.0,sum8=0.0;
    for(int i=0;i<n;i+=8){
        sum1+=a[i];sum2+=a[i+1];sum3+=a[i+2];sum4+=a[i+3];sum5+=a[i+4];sum6+=a[i+5];sum7+=a[i+6];sum8+=a[i+7];
    }
    sum=sum1+sum2+sum3+sum4+sum5+sum6+sum7+sum8;
}
void cal2_16(int n){//十六路优化
    double sum,sum1=0.0,sum2=0.0,sum3=0.0,sum4=0.0,sum5=0.0,sum6=0.0,sum7=0.0,sum8=0.0;
    double sum9=0.0,sum10=0.0,sum11=0.0,sum12=0.0,sum13=0.0,sum14=0.0,sum15=0.0,sum16=0.0;
    for(int i=0;i<n;i+=16){
        sum1+=a[i];sum2+=a[i+1];sum3+=a[i+2];sum4+=a[i+3];sum5+=a[i+4];sum6+=a[i+5];sum7+=a[i+6];sum8+=a[i+7];
        sum9+=a[i+8];sum10+=a[i+9];sum11+=a[i+10];sum12+=a[i+11];sum13+=a[i+12];sum14+=a[i+13];sum15+=a[i+14];sum16+=a[i+15];
    }
    sum=sum1+sum2+sum3+sum4+sum5+sum6+sum7+sum8+sum9+sum10+sum11+sum12+sum13+sum14+sum15+sum16;
}

void cal2_32(int n){//三十二路优化
    double sum,sum1=0.0,sum2=0.0,sum3=0.0,sum4=0.0,sum5=0.0,sum6=0.0,sum7=0.0,sum8=0.0;
    double sum9=0.0,sum10=0.0,sum11=0.0,sum12=0.0,sum13=0.0,sum14=0.0,sum15=0.0,sum16=0.0;
    double sum17=0.0,sum18=0.0,sum19=0.0,sum20=0.0,sum21=0.0,sum22=0.0,sum23=0.0,sum24=0.0;
    double sum25=0.0,sum26=0.0,sum27=0.0,sum28=0.0,sum29=0.0,sum30=0.0,sum31=0.0,sum32=0.0;
    for(int i=0;i<n;i+=32){
        sum1+=a[i];sum2+=a[i+1];sum3+=a[i+2];sum4+=a[i+3];sum5+=a[i+4];sum6+=a[i+5];sum7+=a[i+6];sum8+=a[i+7];
        sum9+=a[i+8];sum10+=a[i+9];sum11+=a[i+10];sum12+=a[i+11];sum13+=a[i+12];sum14+=a[i+13];sum15+=a[i+14];sum16+=a[i+15];
        sum17+=a[i+16];sum18+=a[i+17];sum19+=a[i+18];sum20+=a[i+19];sum21+=a[i+20];sum22+=a[i+21];sum23+=a[i+22];sum24+=a[i+23];
        sum25+=a[i+24];sum26+=a[i+25];sum27+=a[i+26];sum28+=a[i+27];sum29+=a[i+28];sum30+=a[i+29];sum31+=a[i+30];sum32+=a[i+31];
    }
    sum=sum1+sum2+sum3+sum4+sum5+sum6+sum7+sum8+sum9+sum10+sum11+sum12+sum13+sum14+sum15+sum16+sum17+sum18+sum19+sum20+sum21+sum22+sum23+sum24+sum25+sum26+sum27+sum28+sum29+sum30+sum31+sum32;
}

void cal3(int n){//递归优化
    double sum=0.0;
    for(int i=0;i<n;i++)
        b[i]=a[i];
    for(int m=n;m>1;m/=2)
        for(int i=0;i<m/2;i++)
            b[i]=b[i<<1]+b[i<<1|1];
    sum=b[0];

}

int main(int argc, char *argv[])
{
	int n = stoi(argv[1]);
	int op = stoi(argv[2]);
	pre(n);
	switch(op){
        case 1:cal1(n);for(int i=0;i<T;i++)cal1(n); break;
        case 2:cal2_2(n);for(int i=0;i<T;i++)cal2_2(n); break;
        case 3:cal2_4(n);for(int i=0;i<T;i++)cal2_4(n); break;
        case 4:cal2_8(n);for(int i=0;i<T;i++)cal2_8(n); break;
        case 5:cal2_16(n);for(int i=0;i<T;i++)cal2_16(n); break;
        case 6:cal2_32(n);for(int i=0;i<T;i++)cal2_32(n); break;
        case 7:cal3(n);for(int i=0;i<T;i++)cal3(n); break;
	}

    return 0;
}
