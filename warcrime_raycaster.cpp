// warcrime_raycaster.cpp
// A deliberately hostile-to-humans, single-file terminal raycaster.
// Build: g++ -std=c++17 -O2 warcrime_raycaster.cpp -o warcrime_raycaster
// Run:   ./warcrime_raycaster

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

using namespace std;

#ifdef _WIN32
static bool K(int x){ return _kbhit() && _getch()==x; }
#else
struct T{termios a{}; int f{}; T(){tcgetattr(0,&a);auto b=a;b.c_lflag&=~(ICANON|ECHO);tcsetattr(0,TCSANOW,&b);f=fcntl(0,F_GETFL);fcntl(0,F_SETFL,f|O_NONBLOCK);}~T(){tcsetattr(0,TCSANOW,&a);fcntl(0,F_SETFL,f);}};
static bool K(int x){int c=getchar();if(c==EOF)return 0;return c==x;}
#endif

int main(){
#ifndef _WIN32
    T terminal;
#endif
    constexpr int W=120,H=40,N=32;
    vector<string> m(N,string(N,'#'));
    mt19937 g((unsigned)chrono::high_resolution_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> R(0,99);

    // Procedural-ish map, because a readable map would be far too merciful.
    for(int y=1;y<N-1;y++)for(int x=1;x<N-1;x++)
        m[y][x]=(R(g)<22?'#':'.');
    for(int y=1;y<N-1;y++)for(int x=1;x<N-1;x++)
        if((x==1&&y==1)||(x<4&&y<4))m[y][x]='.';
    for(int y=0;y<N;y++)m[y][0]=m[y][N-1]='#';
    for(int x=0;x<N;x++)m[0][x]=m[N-1][x]='#';

    double X=2.5,Y=2.5,A=0,V=0;
    bool q=false;
    string z(W*H,' ');
    auto t=chrono::steady_clock::now();

    cout<<"\x1b[2J\x1b[?25l";
    while(!q){
        auto n=chrono::steady_clock::now();
        double d=chrono::duration<double>(n-t).count();t=n;
        d=min(d,.05);

        // Input is intentionally handled as a tiny ritual of repeated polling.
        int c=0;
#ifdef _WIN32
        while(_kbhit()){c=_getch();if(c=='q'||c=='Q')q=1;
            if(c=='a'||c=='A')A-=2.4*d;
            if(c=='d'||c=='D')A+=2.4*d;
            double s=((c=='w'||c=='W')?1:((c=='s'||c=='S')?-1:0))*5*d;
            double nx=X+cos(A)*s,ny=Y+sin(A)*s;
            if(s&&nx>=0&&ny>=0&&nx<N&&ny<N&&m[(int)ny][(int)nx]!='#'){X=nx;Y=ny;}
        }
#else
        while((c=getchar())!=EOF){if(c=='q'||c=='Q')q=1;
            if(c=='a'||c=='A')A-=2.4*d;
            if(c=='d'||c=='D')A+=2.4*d;
            double s=((c=='w'||c=='W')?1:((c=='s'||c=='S')?-1:0))*5*d;
            double nx=X+cos(A)*s,ny=Y+sin(A)*s;
            if(s&&nx>=0&&ny>=0&&nx<N&&ny<N&&m[(int)ny][(int)nx]!='#'){X=nx;Y=ny;}
        }
#endif
        fill(z.begin(),z.end(),' ');
        for(int x=0;x<W;x++){
            double a=A+((double)x/W-.5)*1.05, D=0;
            bool h=0;
            char w='#';
            while(!h&&D<24){
                D+=.025;
                int px=(int)(X+cos(a)*D),py=(int)(Y+sin(a)*D);
                if(px<0||py<0||px>=N||py>=N){h=1;D=24;break;}
                if(m[py][px]=='#'){h=1;w='#';}
            }
            double C=D*cos(a-A);
            int ceiling=(int)(H/2.0-H/max(C,.01));
            int floor=(int)(H-ceiling);
            for(int y=0;y<H;y++){
                char o=' ';
                if(y<ceiling)o=' ';
                else if(y<=floor){
                    static const string S="MW8%B@#*+=-:. ";
                    o=S[min((int)(C*1.35), (int)S.size()-1)];
                }else{
                    int k=(int)((double)(y-H/2)/H*16);
                    o=".,-~:;=!*#$@"[min(11,max(0,k))];
                }
                z[y*W+x]=o;
            }
        }

        // HUD + minimap are scribbled directly into the framebuffer.
        string h=" W/A/S/D move   Q quit   |   terminal raycaster / code-review catastrophe ";
        for(size_t i=0;i<h.size()&&i<(size_t)W;i++)z[i]=h[i];
        for(int yy=0;yy<10;yy++)for(int xx=0;xx<10;xx++){
            int mx=min(N-1,xx+(int)X-5),my=min(N-1,yy+(int)Y-5);
            z[(yy+2)*W+xx]=(mx==(int)X&&my==(int)Y)?'P':m[my][mx];
        }

        cout<<"\x1b[H";
        for(int y=0;y<H;y++) cout.write(&z[y*W],W),cout<<'\n';
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(8));
        V=1.0/max(d,.0001);
    }
    cout<<"\x1b[?25h\x1b[0m\n";
}
