#include <iostream>
#include <chrono>
#include <windows.h>
#include <string>
#include <thread>
using namespace std;
const int N=5000,T=100;
double sum[N],b[N][N],a[N];
void pre(int n){
    for(int i=0;i<n;i++)
        a[i]=i/2.0;
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            b[i][j]=1.0*i/(j+1);
}
double cal1(int n){//平凡算法
    double head,tail,freq,head1,tail1,timess=0;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq );
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
    for(int i=0;i<n;i++){
        sum[i]=0.0;
        for(int j=0;j<n;j++)
            sum[i]+=b[j][i]*a[j];
    }
    QueryPerformanceCounter ((LARGE_INTEGER *)& tail) ;
    return (tail-head)*1000.0 / freq;
}
double cal2(int n){//cache优化
    double head,tail,freq,head1,tail1,timess=0;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq );
    QueryPerformanceCounter((LARGE_INTEGER *)&head);

    for(int i=0;i<n;i++)
        sum[i]=0.0;
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            sum[j]+=b[i][j]*a[i];

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
        //cout<<"平凡算法第"<<i+1<<"次运行时间： "<<temptime<<" ms"<<endl;
	}
	cout<<sumtime/T<<" ms"<<endl;//平凡算法平均运行时间
    sumtime=0.0;
    cal2(n);
    for(int i=0;i<T;i++){
        temptime=cal2(n);
        sumtime+=temptime;
        //cout<<"cache优化算法第"<<i+1<<"次运行时间： "<<temptime<<" ms"<<endl;
	}
	cout<<sumtime/T<<" ms"<<endl;//cache优化算法平均运行时间
    return 0;
}
