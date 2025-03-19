#include <iostream>
#include <chrono>
#include <windows.h>
#include <string>
#include <thread>
using namespace std;
const int N=2048,T=100;
double sum[N],b[N],a[N];
void pre(int n){
    for(int i=0;i<n;i++)
        a[i]=i/3.0;
}

double cal1(int n){//平凡算法
    double head,tail,freq,head1,tail1,timess=0;
    double sum=0.0;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq );
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
    for(int i=0;i<n;i++)
        sum+=a[i];
    QueryPerformanceCounter ((LARGE_INTEGER *)& tail) ;
    return (tail-head)*1000.0 / freq;
}

double cal2_2(int n){//两路优化
    double head,tail,freq,head1,tail1,timess=0;
    double sum,sum1=0.0,sum2=0.0;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq );
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
    for(int i=0;i<n;i+=2){
        sum1+=a[i];
        sum2+=a[i+1];
    }
    sum=sum1+sum2;
    QueryPerformanceCounter ((LARGE_INTEGER *)& tail) ;
    return (tail-head)*1000.0 / freq;
}

double cal2_4(int n){//四路优化
    double head,tail,freq,head1,tail1,timess=0;
    double sum,sum1=0.0,sum2=0.0,sum3=0.0,sum4=0.0;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq );
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
    for(int i=0;i<n;i+=4){
        sum1+=a[i];sum2+=a[i+1];sum3+=a[i+2];sum4+=a[i+3];
    }
    sum=sum1+sum2+sum3+sum4;
    QueryPerformanceCounter ((LARGE_INTEGER *)& tail) ;
    return (tail-head)*1000.0 / freq;
}

double cal2_8(int n){//八路优化
    double head,tail,freq,head1,tail1,timess=0;
    double sum,sum1=0.0,sum2=0.0,sum3=0.0,sum4=0.0,sum5=0.0,sum6=0.0,sum7=0.0,sum8=0.0;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq );
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
    for(int i=0;i<n;i+=8){
        sum1+=a[i];sum2+=a[i+1];sum3+=a[i+2];sum4+=a[i+3];sum5+=a[i+4];sum6+=a[i+5];sum7+=a[i+6];sum8+=a[i+7];
    }
    sum=sum1+sum2+sum3+sum4+sum5+sum6+sum7+sum8;
    QueryPerformanceCounter ((LARGE_INTEGER *)& tail) ;
    return (tail-head)*1000.0 / freq;
}

double cal3(int n){//递归优化
    double head,tail,freq,head1,tail1,timess=0;
    double sum=0.0;
    for(int i=0;i<n;i++)
        b[i]=a[i];
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq );
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
    for(int m=n;m>1;m/=2)
        for(int i=0;i<m/2;i++)
            b[i]=b[i<<1]+b[i<<1|1]
    sum=b[0];
    QueryPerformanceCounter ((LARGE_INTEGER *)& tail) ;
    return (tail-head)*1000.0 / freq;

}

int main(int argc, char *argv[])
{
	int n = stoi(argv[1]);
	pre(n);
	double sumtime=0.0,temptime;
	cal1(n);//第一次弃之不用

	for(int i=0;i<T;i++){
        temptime=cal1(n);
        sumtime+=temptime;
	}
	cout<<sumtime/T<<" ms"<<endl;//平凡算法平均运行时间

    sumtime=0.0;
    cal2_2(n);
    for(int i=0;i<T;i++){
        temptime=cal2_2(n);
        sumtime+=temptime;
	}
	cout<<sumtime/T<<" ms"<<endl;//二路优化算法平均运行时间

    sumtime=0.0;
    cal2_4(n);
    for(int i=0;i<T;i++){
        temptime=cal2_4(n);
        sumtime+=temptime;
	}
	cout<<sumtime/T<<" ms"<<endl;//四路优化算法平均运行时间

    sumtime=0.0;
    cal2_8(n);
    for(int i=0;i<T;i++){
        temptime=cal2_8(n);
        sumtime+=temptime;
	}
	cout<<sumtime/T<<" ms"<<endl;//八路优化算法平均运行时间

    sumtime=0.0;
    cal3(n);
    for(int i=0;i<T;i++){
        temptime=cal3(n);
        sumtime+=temptime;
	}
	cout<<sumtime/T<<" ms"<<endl;//递归优化算法平均运行时间

    return 0;
}
