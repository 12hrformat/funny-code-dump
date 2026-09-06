// DOOM-ish terminal raycaster: one deliberately cursed C++ source file.
// Build: g++ -std=c++17 -O2 doom_terminal_warcrime.cpp -o doom.exe
// Run: .\doom.exe
// Controls: W/S move, A/D turn, Space shoot, R restart, Q quit.

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif
using namespace std;

#ifndef _WIN32
struct _{termios a{};int b{};_(){tcgetattr(0,&a);auto c=a;c.c_lflag&=~(ICANON|ECHO);tcsetattr(0,TCSANOW,&c);b=fcntl(0,F_GETFL);fcntl(0,F_SETFL,b|O_NONBLOCK);}~_(){tcsetattr(0,TCSANOW,&a);fcntl(0,F_SETFL,b);}};
#endif

struct E{double x,y,h,v;int hp,kind;};
static constexpr int W=120,H=42,M=32;
static double X=2.5,Y=2.5,A=0,HP=100,AM=50,flash=0;
static bool dead=0,win=0;
static mt19937 G((unsigned)chrono::high_resolution_clock::now().time_since_epoch().count());

static vector<string> m={
"################################",
"#..............#...............#",
"#.######.#####.#.#####.#######.#",
"#.#....#.....#.#.....#.......#.#",
"#.#.##.#####.#.#####.#######.#.#",
"#...##.....#.#.....#.....#...#.#",
"#####.#####.#.#####.#####.#.###.#",
"#.....#...#.#.....#.....#.#.....#",
"#.#####.#.#######.#####.#.#####.#",
"#.....#.#.......#.....#.#.....#.#",
"###.#.#.#######.#####.#.#####.#.#",
"#...#.#.....#...#.....#.....#.#.#",
"#.###.#####.#.###.#########.#.#.#",
"#...#.....#.#...#.....#.....#...#",
"###.#####.#.###.#####.#.#######.#",
"#...#.....#...#.....#.#.......#.#",
"#.###.#######.#####.#.#######.#.#",
"#.....#.....#.....#.#.....#...#.#",
"#.#####.###.#####.#.#####.#.###.#",
"#.#.....#.#.....#.#.#...#.#.....#",
"#.#.#####.#####.#.#.#.#.#######.#",
"#.#...........#.#...#.#.........#",
"#.###########.#.#####.#########.#",
"#.....#.....#.#.....#.........#.#",
"#####.#.###.#.#####.#########.#.#",
"#.....#.#...#.....#.........#.#.#",
"#.#####.#.#######.#########.#.#.#",
"#.#.....#.......#.....#.....#.#.#",
"#.#.###########.#####.#.#####.#.#",
"#.#...........#.....#.#.....#.#.#",
"#.###########.#####.#.#####.#.#.#",
"################################"};

static vector<E> e;
static inline double Q(double a,double b){return sqrt(a*a+b*b);}
static inline bool wall(double x,double y){return x<0||y<0||x>=M||y>=M||m[(int)y][(int)x]=='#';}
static inline int key(){
#ifdef _WIN32
 return _kbhit()?_getch():-1;
#else
 int c=getchar();return c==EOF?-1:c;
#endif
}
static void reset(){
 X=2.5;Y=2.5;A=0;HP=100;AM=50;flash=0;dead=win=0;e.clear();
 vector<pair<double,double>> p={{6,3},{11,7},{17,4},{23,9},{28,15},{7,20},{15,17},{21,22},{27,26},{11,29},{18,28},{25,30}};
 for(size_t i=0;i<p.size();++i)e.push_back({p[i].first,p[i].second,0,0,35+(int)(i%3)*15,0});
}
static bool LOS(double x,double y,double tx,double ty){
 double dx=tx-x,dy=ty-y,d=Q(dx,dy),s=.08;
 for(double t=0;t<d;t+=s)if(wall(x+dx*t/d,y+dy*t/d))return false;
 return true;
}
static void shoot(){
 if(AM<=0||dead||win)return;--AM;flash=.09;
 int best=-1;double bd=1e9;
 for(int i=0;i<(int)e.size();++i)if(e[i].hp>0){
   double dx=e[i].x-X,dy=e[i].y-Y,d=Q(dx,dy);
   double z=atan2(dy,dx)-A;while(z>M_PI)z-=2*M_PI;while(z<-M_PI)z+=2*M_PI;
   if(fabs(z)<.075+1.2/max(d,1.0)&&d<bd&&LOS(X,Y,e[i].x,e[i].y)){best=i;bd=d;}
 }
 if(best>=0){e[best].hp-=25+(int)(G()%20);if(e[best].hp<=0)e[best].h=2.5;}
}
int main(){
#ifndef _WIN32
 _ terminal;
#endif
 reset(); string b(W*H,' '); auto last=chrono::steady_clock::now();double fps=0;
 cout<<"\x1b[2J\x1b[?25l";
 while(true){
  auto now=chrono::steady_clock::now();double dt=min(.05,chrono::duration<double>(now-last).count());last=now;fps=1/max(dt,.001);
  int c;while((c=key())!=-1){
   if(c=='q'||c=='Q')goto out;if(c=='r'||c=='R')reset();
   if(!dead&&!win){
    if(c=='a'||c=='A')A-=2.4*dt;if(c=='d'||c=='D')A+=2.4*dt;
    if(c==' '||c=='f'||c=='F')shoot();
    double s=(c=='w'||c=='W'?1:c=='s'||c=='S'?-1:0)*4.2*dt,nx=X+cos(A)*s,ny=Y+sin(A)*s;
    if(s&&!wall(nx,Y))X=nx;if(s&&!wall(X,ny))Y=ny;
   }
  }
  for(auto &z:e)if(z.hp>0&&!dead){
   double dx=X-z.x,dy=Y-z.y,d=Q(dx,dy);
   if(d<11&&LOS(z.x,z.y,X,Y)){
    double nx=z.x+dx/max(d,.01)*1.1*dt,ny=z.y+dy/max(d,.01)*1.1*dt;
    if(!wall(nx,z.y))z.x=nx;if(!wall(z.x,ny))z.y=ny;
    if(d<1.0)HP-=22*dt;
   }
   if(z.h>0)z.h-=dt;
  }
  if(HP<=0){HP=0;dead=1;}if(all_of(e.begin(),e.end(),[](E&z){return z.hp<=0;}))win=1;
  fill(b.begin(),b.end(),' ');
  vector<double> depth(W,24);
  for(int x=0;x<W;x++){
   double a=A+((double)x/W-.5)*1.05,d=0;bool hit=0;
   while(!hit&&d<24){d+=.025;double rx=X+cos(a)*d,ry=Y+sin(a)*d;if(wall(rx,ry))hit=1;}
   double D=d*cos(a-A);depth[x]=D;int ce=(int)(H*.5-H/max(D,.01)),fl=H-ce;
   static const string S="@#8&%*+=-:. ";
   for(int y=0;y<H;y++)b[y*W+x]=y<ce?' ':y<=fl?S[min((int)S.size()-1,(int)(D*1.1))]:(y>H/2+(int)(D*1.8)?'.':'-');
  }
  // Sprite projection: crude, but it moves and attacks.
  for(auto const&z:e)if(z.hp>0){
   double dx=z.x-X,dy=z.y-Y,d=Q(dx,dy),ang=atan2(dy,dx)-A;
   while(ang>M_PI)ang-=2*M_PI;while(ang<-M_PI)ang+=2*M_PI;
   if(fabs(ang)<.62){
    int sx=(int)((ang/.525+.5)*W),sh=(int)(H/max(d,.1)*.75),sy=H/2-sh/2;
    for(int yy=max(0,sy);yy<min(H,sy+sh);yy++)for(int xx=max(0,sx-sh/5);xx<min(W,sx+sh/5);xx++)
      if(d<depth[xx])b[yy*W+xx]=((xx+yy)&1)?'M':'W';
   }
  }
  // Weapon. Entirely ASCII. Entirely unreasonable.
  string gun=flash>0?"     \\\\====[BOOM]====//     ":"       \\\\___[GUN]___//       ";
  int gy=H-5,gx=W/2-(int)gun.size()/2;for(int i=0;i<(int)gun.size();i++)if(gx+i>=0&&gx+i<W)b[gy*W+gx+i]=gun[i];
  for(int yy=0;yy<4;yy++)for(int xx=0;xx<12;xx++){int mx=min(M-1,max(0,(int)X+xx-6)),my=min(M-1,max(0,(int)Y+yy-2));b[(yy+1)*W+xx]=(mx==(int)X&&my==(int)Y)?'P':m[my][mx];}
  string hud=" HP:"+to_string((int)HP)+"  AMMO:"+to_string(AM)+"  DEMONS:"+to_string((int)count_if(e.begin(),e.end(),[](E&z){return z.hp>0;}))+"  FPS:"+to_string((int)fps)+"   W/S MOVE A/D TURN SPACE FIRE Q QUIT ";
  for(int i=0;i<(int)hud.size()&&i<W;i++)b[i]=hud[i];
  if(dead||win){string s=dead?" YOU DIED. PRESS R TO RESTART ":" LEVEL CLEARED. PRESS R TO RESPAWN DEMONS ";int y=H/2,x=(W-(int)s.size())/2;for(int i=0;i<(int)s.size();i++)if(x+i>=0&&x+i<W)b[y*W+x+i]=s[i];}
  cout<<"\x1b[H";for(int y=0;y<H;y++)cout.write(&b[y*W],W),cout<<'\n';cout.flush();
  flash=max(0.0,flash-dt);this_thread::sleep_for(chrono::milliseconds(6));
 }
out: cout<<"\x1b[?25h\x1b[0m\n";
}
