#include <iostream>
#include <chrono>
// #include <windows.h>
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
void cal1(int n){//ƽ���㷨
    double head,tail,freq,head1,tail1,timess=0;
    for(int i=0;i<n;i++){
        sum[i]=0.0;
        for(int j=0;j<n;j++)
            sum[i]+=b[j][i]*a[j];
    }
}
void cal2(int n){//cache�Ż�
    double head,tail,freq,head1,tail1,timess=0;
    for(int i=0;i<n;i++)
        sum[i]=0.0;
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            sum[j]+=b[i][j]*a[i];
}
int main(int argc, char *argv[])
{
	int n = stoi(argv[1]);
	int op =stoi(argv[2]);
	pre(n);
	if(op==1)
        for(int i=0;i<T;i++)
            cal1(n);
    else
        for(int i=0;i<T;i++)
            cal2(n);
    return 0;
}
