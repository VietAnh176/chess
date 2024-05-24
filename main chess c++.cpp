#include <SFML/Graphics.hpp>
#include<iostream>
#include<math.h>
#include <time.h>
#include<stack>
#include<algorithm>// min, max
using namespace sf;
using namespace std;

int size = 56;
Vector2f offset(28,28);

int board[8][8] =
{-1,-2,-3,-4,-5,-3,-2,-1,
-6,-6,-6,-6,-6,-6,-6,-6,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
6, 6, 6, 6, 6, 6, 6, 6,
1, 2, 3, 4, 5, 3, 2, 1};

typedef struct QuanCo
{
 Sprite s;
 int index ,cost;
} QuanCo; 

class GameManager
{
public:
 QuanCo f[33]; 
 Vector2f positiveMove[32];
 int positiveCount;
 stack<Vector2f> posS;
 stack<int> nS;

 void move(int n,Vector2f oldPos,Vector2f newPos);
 void Undo();
 void Create();
 void Play();

 void PositiveXe(int n,int x,int y,int grid[9][9]);
 void PositiveTuong(int n,int x,int y,int grid[9][9]);
 void PositiveMa(int n,int x,int y,int grid[9][9]);
 void PositiveVua(int n,int x,int y,int grid[9][9]);
 void PositiveTot(int n,int x,int y,int grid[9][9]);

 void IncreasePositive(int i,int j);
 void PositiveMoving(int n);
 //AI
 int CostMove();
 //int Minimax(int depth,bool luot);// Chien luoc Minimax
 int Alpha_Beta(int depth,bool luot,int alpha,int beta);
 Vector2f getNextMove(bool luot);
};

int GameManager::Alpha_Beta(int depth,bool luot,int alpha,int beta)
{
 if(depth==0){
  return CostMove();
 }
 Vector2f positiveMovetemp[32];
 if(luot==true){
  int bestMove=-10000;
  for(int j=16;j<32;j++)
  {
   if(f[j].s.getPosition()==Vector2f(-100,-100)) continue;
   PositiveMoving(j);
   int coun=positiveCount;
   positiveCount=0;
   for (int i = 0; i < coun; i++)  positiveMovetemp[i]=positiveMove[i];  
   for(int i=0;i<coun;i++)
   {
    move(j,f[j].s.getPosition(),positiveMovetemp[i]);
    bestMove=max(bestMove,Alpha_Beta(depth-1,!luot,alpha,beta));
  
    Undo();
    alpha=max(alpha,bestMove);
    if(beta<=alpha)  return bestMove;
   }
  }
  return bestMove;
 }
 else {
  int bestMove=10000;
  for(int j=0;j<16;j++)
  {
   if(f[j].s.getPosition()==Vector2f(-100,-100)) continue;
   PositiveMoving(j);
   int coun=positiveCount;
   positiveCount=0;
   for (int i = 0; i < coun; i++)  positiveMovetemp[i]=positiveMove[i];  
   for(int i=0;i<coun;i++)
   {
    move(j,f[j].s.getPosition(),positiveMovetemp[i]);
    bestMove=min(bestMove,Alpha_Beta(depth-1,!luot,alpha,beta));
    //undo
    Undo();
    beta=min(beta,bestMove);
    if(beta<=alpha)  return bestMove;
   }
  }
  return bestMove;
 }
}

void GameManager::IncreasePositive(int i,int j){
 positiveMove[positiveCount]=Vector2f(i*size,j*size)+offset;
 positiveCount++;
}

void GameManager::move(int n,Vector2f oldPos,Vector2f newPos)
{ 
 posS.push(oldPos);
 posS.push(newPos);
 nS.push(n);
 int y=int((newPos-offset).y/size);
 //phong hau cho tot
 if(y==0 && f[n].index==6){
  nS.push(100);
  f[n].index=4;
  f[n].cost=90;
  f[n].s.setTextureRect(IntRect( 3*size,size,size,size));
 }
 if(y==7 && f[n].index==-6){
  nS.push(-100);
  f[n].index=-4;
  f[n].cost=-90;
  f[n].s.setTextureRect(IntRect( 3*size,0,size,size));
 }
 //di chuyen con an vao vi tri moi 
 for(int i=0;i<32;i++){
  if (f[i].s.getPosition()==newPos) {
   f[i].s.setPosition(-100,-100);
   posS.push(newPos);
   posS.push(Vector2f(-100,-100));
   nS.push(i);
   break;
  }
 }
 f[n].s.setPosition(newPos);
}

void GameManager::Undo()
{
 int n= nS.top();
 nS.pop();
 Vector2f p=posS.top();
 posS.pop();
 Vector2f q=posS.top();
 posS.pop();
 if(n==100) {
  n=nS.top();
  nS.pop();
  f[n].index=6;
  f[n].cost=10;
  f[n].s.setTextureRect(IntRect( 5*size,size,size,size));
 }
 if(n==-100){
  n=nS.top();
  nS.pop();
  f[n].index=-6;
  f[n].cost=-10;
  f[n].s.setTextureRect(IntRect( 5*size,0,size,size));
 }
 f[n].s.setPosition(q);

 if(p==Vector2f(-100,-100))  Undo();
}

void GameManager::Create()
 positiveCount=0;
 int k=0;
 for(int i=0;i<8;i++){
  for(int j=0;j<8;j++)
  {
   int n = board[i][j];   
   if (!n) continue;
   int x = abs(n)-1;
   int y = n>0?1:0;
   f[k].index=n;
   f[k].s.setTextureRect( IntRect(size*x,size*y,size,size) );
   f[k].s.setPosition(size*j+offset.x,size*i+offset.y);
   //cost
   int v=0,g; 
   g=abs(f[k].index);
   if(g==1)  v=50;
   else if(g==2|| g==3)  v=30;
   else if(g==4)  v=90;
   else if(g==5)  v=900;
   else if(g==6)  v=10;
   f[k].cost=f[k].index/g*v;
   k++;
  }
 }
}

Vector2f GameManager::getNextMove(bool luot)
{
 Vector2f oldPos,newPos,oldPostemp,newPostemp;
 int minimaxtemp=10000,minimax=10000;
 int count1,n;
 Vector2f positiveMovetemp[32];

 for(int i=0;i<16;i++)
 {
  if(f[i].s.getPosition()==Vector2f(-100,-100)) continue;
  //////
  PositiveMoving(i);
  count1=positiveCount;
  positiveCount=0;
  ///set///
  for (int k = 0; k < count1; k++) positiveMovetemp[k]=positiveMove[k];
  //set oldPos va newPos  tam thoi
  oldPostemp=f[i].s.getPosition();
  //newPostemp=positiveMove[0];
  for(int j=0;j<count1;j++)
  {
   move(i,oldPostemp,positiveMovetemp[j]);
   int alpha=-9999,beta=9999;
   int temp=Alpha_Beta(3,!luot,alpha,beta);
   if(minimaxtemp>temp){
    newPostemp=positiveMovetemp[j];
    minimaxtemp=temp;
   }
   Undo();
  }
  if(minimax>minimaxtemp){
   minimax=minimaxtemp;
   oldPos=oldPostemp;
   newPos=newPostemp;
   n=i;
  }
 }
 //lay cac thong tin nuoc di
 posS.push(oldPos);
 nS.push(n);
 return newPos;
}

int GameManager::CostMove()
{
 int s=0;
 for(int i=0;i<32;i++)
 {  
  if(f[i].s.getPosition()==Vector2f(-100,-100)) continue;
  s+=f[i].cost;
 }
 return s;
}

void GameManager::PositiveTot(int n,int x,int y,int grid[9][9])
{
 int k=grid[x][y]/abs(grid[x][y]);// 1 hoac -1
 if((y==1 || y==6) &&grid[x][y-k]==0 && grid[x][y-2*k]==0 && y-2*k>=0 && y-2*k<8) IncreasePositive(x,y-2*k);
 if(grid[x][y-k]==0 && y-k>=0 && y-k<8)            IncreasePositive(x,y-k);
 if(grid[x+1][y-k]*grid[x][y]<0 && y-k>=0 && y-k<8 && x+1<8)       IncreasePositive(x+1,y-k);
 if(grid[x-1][y-k]*grid[x][y]<0 && y-k>=0 && y-k<8 && x-1>=0)      IncreasePositive(x-1,y-k);
}

void GameManager::PositiveVua(int n,int x,int y,int grid[9][9])
{
 if((grid[x+1][y]==0||grid[x][y]*grid[x+1][y]<0) && x+1<8)     IncreasePositive(x+1,y);
 if((grid[x-1][y]==0||grid[x][y]*grid[x-1][y]<0) && x-1>=0)     IncreasePositive(x-1,y);
 if((grid[x+1][y+1]==0||grid[x][y]*grid[x+1][y+1]<0) && x+1<8 && y+1<8)  IncreasePositive(x+1,y+1);
 if((grid[x-1][y+1]==0||grid[x][y]*grid[x-1][y+1]<0) && x-1>=0 && y+1<8)  IncreasePositive(x-1,y+1);
 if((grid[x][y+1]==0||grid[x][y]*grid[x][y+1]<0) && y+1<8)     IncreasePositive(x,y+1);
 if((grid[x-1][y-1]==0||grid[x][y]*grid[x-1][y-1]<0) && x-1>=0 && y-1>=0) IncreasePositive(x-1,y-1);
 if((grid[x+1][y-1]==0||grid[x][y]*grid[x+1][y-1]<0) && y-1>=0 && x+1<8)  IncreasePositive(x+1,y-1);
 if((grid[x][y-1]==0||grid[x][y]*grid[x][y-1]<0) && y-1>=0)     IncreasePositive(x,y-1);
}

void GameManager::PositiveMa(int n,int x,int y,int grid[9][9])
{
 if((grid[x+2][y+1]==0||grid[x][y]*grid[x+2][y+1]<0) && x+2<8 && y+1<8)  IncreasePositive(x+2,y+1);
 if((grid[x+2][y-1]==0||grid[x][y]*grid[x+2][y-1]<0) && y-1>=0 && x+2<8)  IncreasePositive(x+2,y-1);
 if((grid[x-2][y+1]==0||grid[x][y]*grid[x-2][y+1]<0) && x-2>=0 && y+1<8)  IncreasePositive(x-2,y+1);
 if((grid[x-2][y-1]==0||grid[x][y]*grid[x-2][y-1]<0) && x-2>=0 && y-1>=0) IncreasePositive(x-2,y-1);
 if((grid[x+1][y+2]==0||grid[x][y]*grid[x+1][y+2]<0) && x+1<8 && y+2<8)  IncreasePositive(x+1,y+2);
 if((grid[x-1][y+2]==0||grid[x][y]*grid[x-1][y+2]<0) && x-1>=0 && y+2<8)  IncreasePositive(x-1,y+2);
 if((grid[x+1][y-2]==0||grid[x][y]*grid[x+1][y-2]<0) && y-2>=0&& x+1<8)  IncreasePositive(x+1,y-2);
 if((grid[x-1][y-2]==0||grid[x][y]*grid[x-1][y-2]<0) && x-1>=0 && y-2>=0) IncreasePositive(x-1,y-2);
}

void GameManager::PositiveTuong(int n,int x,int y,int grid[9][9])
{
 for(int i=x+1,j=y+1;(i<8&&j<8);i++,j++)
 {
  if(grid[i][j]!=0) {
   if(grid[i][j]*grid[x][y]<0)  IncreasePositive(i,j);
   break;
  }
  IncreasePositive(i,j);
 }
 for(int i=x+1,j=y-1;(i<8&&j>=0);i++,j--)
 {
  if(grid[i][j]!=0) {
   if(grid[i][j]*grid[x][y]<0)  IncreasePositive(i,j);
   break;
  }
  IncreasePositive(i,j);
 }
 for(int i=x-1,j=y+1;(i>=0&&j<8);i--,j++)
 {
  if(grid[i][j]!=0) {
   if(grid[i][j]*grid[x][y]<0)  IncreasePositive(i,j);
   break;
  }
  IncreasePositive(i,j);
 }
 for(int i=x-1,j=y-1;(i>=0 && j>=0);i--,j--)
 {
  if(grid[i][j]!=0) {
   if(grid[i][j]*grid[x][y]<0)  IncreasePositive(i,j);
   break;
  }
  IncreasePositive(i,j);
 }
}

void GameManager::PositiveXe(int n,int x,int y,int grid[9][9])
{
 for(int i=x+1;i<8;i++)
 {
  if(grid[i][y]!=0) {
   if(grid[i][y]*grid[x][y]<0)  IncreasePositive(i,y); 
   break;
  }
  IncreasePositive(i,y); 
 }
 for(int i=x-1;i>=0;i--)
 {
  if(grid[i][y]!=0) {
   if(grid[i][y]*grid[x][y]<0)  IncreasePositive(i,y); 
   break;
  }
  IncreasePositive(i,y); 
 }
 for(int j=y+1;j<8;j++)
 {
  if(grid[x][j]!=0) {
   if(grid[x][j]*grid[x][y]<0)  IncreasePositive(x,j);
   break;
  }
  IncreasePositive(x,j);
 }
 for(int j=y-1;j>=0;j--)
 {
  if(grid[x][j]!=0) {
   if(grid[x][j]*grid[x][y]<0)  IncreasePositive(x,j);
   break;
  }
  IncreasePositive(x,j);
 }
}

void GameManager::PositiveMoving(int n)
{
 Vector2f pos=f[n].s.getPosition()-offset;
 int x=pos.x/size;
 int y=pos.y/size;

 int grid[9][9];
 Vector2i vtri;
 for(int i=0;i<8;i++)
  for(int j=0;j<8;j++)
   grid[i][j]=0;
 for(int j=0;j<32;j++)
 {
  vtri=Vector2i( f[j].s.getPosition()-offset);
  grid[vtri.x/size][vtri.y/size]=f[j].index;
 }

 if(abs(f[n].index)==1)  PositiveXe(n,x,y,grid);
 else if(abs(f[n].index)==2)  PositiveMa(n,x,y,grid);
 else if(abs(f[n].index)==3)  PositiveTuong(n,x,y,grid);
 else if(abs(f[n].index)==4)
 {
  PositiveXe(n,x,y,grid);
  PositiveTuong(n,x,y,grid);
 }
 else if(abs(f[n].index)==5)  PositiveVua(n,x,y,grid);
 else   PositiveTot(n,x,y,grid);
}

void GameManager::Play()
{
 RenderWindow window(VideoMode(504, 504), "The Chess! Alpha Beta Pruning");
 Texture t1,t2,t3;
 t1.loadFromFile("figures.png");
 t2.loadFromFile("board1.png");
 t3.loadFromFile("no.png");

 for(int i=0;i<32;i++) f[i].s.setTexture(t1);
 Sprite sBoard(t2);
 Sprite sPositive(t3);

 Create();

 bool LuotChoi=true;
 Vector2f oldPos,newPos;
 int n=0,click=0,count=0;
 Vector2i pos;
 while (window.isOpen())
 {
  sf::Event e;
  while (window.pollEvent(e))
  {
   if (e.type == Event::Closed)
    window.close();
   ////move back//////
   if (e.type == Event::KeyPressed)
    if (e.key.code == Keyboard::BackSpace)
    { Undo(); }
   /////click///////
   if (e.type == sf::Event::MouseButtonReleased)
    if (e.mouseButton.button == sf::Mouse::Left)
    {
     pos = Mouse::getPosition(window) - Vector2i(offset);
     click++;
    }     
  }
  if(LuotChoi==true)
  {
   if(click==1){
    bool isMove=false;
    for(int i=16;i<32;i++)
    {
     if(f[i].s.getGlobalBounds().contains(pos.x+offset.x,pos.y+offset.y))
     {
      isMove=true;
      n=i;
      f[n].s.setColor(Color::Green);
      oldPos=f[n].s.getPosition();
     }
    }
    if(!isMove)  click=0;
    else    {PositiveMoving(n);count=positiveCount;positiveCount=0;}
   }
   if(click==2)
   {
    f[n].s.setColor(Color::White);
    int x=pos.x/size;   int y=pos.y/size;
    newPos=Vector2f(x*size,y*size)+offset;
    //chi di chuyen trong vung positiveMove
    for(int i=0;i<count;i++)
    {
     if (positiveMove[i]==newPos){
      move(n,oldPos,newPos);
      LuotChoi=!LuotChoi; 
     }
    }
    //reset
    count=0;
    click=0;
   } 
  }
  else  //computer moving
  {
   newPos= getNextMove(LuotChoi);
   int c=nS.top();   nS.pop();
   oldPos=posS.top();  posS.pop();
   move(c,oldPos,newPos);
   LuotChoi=!LuotChoi;
   //reset 
   click=0;
  }
  ////// draw  ///////
  window.draw(sBoard);
  for(int i=0;i<count;i++){
   sPositive.setPosition(positiveMove[i]);
   window.draw(sPositive);
  }
  for(int i=0;i<32;i++) {
   window.draw(f[i].s);
  }
  window.display();
 }
}

int main()
{
 GameManager gm;
 gm.Play();

}
