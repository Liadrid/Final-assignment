#include "GameModel.h"

#include <math.h>
#include <cstdlib>

double GameModel::len(int x1,int y1,int x2,int y2)
{
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

void GameModel::startGame(GameMode mode)
{
    gameMode=mode;
    playerTurn=blackturn;

    //初始化棋盘
    for (int i=0;i<LINE_NUM;i++)
    for (int j=0;j<LINE_NUM;j++)
    {
        chessPiece[i][j]=null;
        if (gameMode==PvE) chessValue[i][j]=0;
    }
}

void GameModel::move_in_chess(int row, int col)
{
    if (!(row>=0&&row<LINE_NUM&&col>=0&&col<LINE_NUM)) return;
    chessNum++;
    if (playerTurn==blackturn) chessPiece[row][col]=black;
    if (playerTurn==whiteturn) chessPiece[row][col]=white;
    judge(row,col);
}

void GameModel::judge(int row, int col)
{
    bool win=false;
    for (int i=-5;i<=0;i++)
    {
        if (row+i<0||row+i+5>=LINE_NUM) continue;
        bool cont=true;
        for (int j=0;j<6;j++)
            if (chessPiece[row+i+j][col]!=chessPiece[row][col]){
                cont=false;
                break;
            }
        if (cont){
            win=true;
            break;
        }
    }
    for (int i=-5;i<=0;i++)
    {
        if (col+i<0||col+i+5>=LINE_NUM) continue;
        bool cont=true;
        for (int j=0;j<6;j++)
            if (chessPiece[row][col+i+j]!=chessPiece[row][col]){
                cont=false;
                break;
            }
        if (cont){
            win=true;
            break;
        }
    }
    for (int i=-5;i<=0;i++)
    {
        if (row+i<0||row+i+5>=LINE_NUM) continue;
        if (col+i<0||col+i+5>=LINE_NUM) continue;
        bool cont=true;
        for (int j=0;j<6;j++)
            if (chessPiece[row+i+j][col+i+j]!=chessPiece[row][col]){
                cont=false;
                break;
            }
        if (cont){
            win=true;
            break;
        }
    }
    for (int i=-5;i<=0;i++)
    {
        if (row+i<0||row+i+5>=LINE_NUM) continue;
        if (col-i-5<0||col-i>=LINE_NUM) continue;
        bool cont=true;
        for (int j=0;j<6;j++)
            if (chessPiece[row+i+j][col-i-j]!=chessPiece[row][col]){
                cont=false;
                break;
            }
        if (cont){
            win=true;
            break;
        }
    }

    //胜利
    if (win)
    {
        if (chessPiece[row][col]==black) gameStatus=BLACKWIN;
        if (chessPiece[row][col]==white) gameStatus=WHITEWIN;
        return;
    }

    //和局
    if ((!win)&&chessNum==LINE_NUM*LINE_NUM){
        gameStatus=DRAW;
        return;
    }
}

void GameModel::AIchess(int *row, int *col)
{
    if (chessNum==0){
        *row=9;
        *col=9;
        return;
    }
    PlayerTurn AiTurn=playerTurn,peopleTurn;
    int max=-1,maxrow,maxcol;
    for (int i=0;i<LINE_NUM;i++)
    for (int j=0;j<LINE_NUM;j++)
    {
        int tempmax=-1;
        if (chessPiece[i][j]==null)
        {
            tempmax=getvalue(i,j,AiTurn);
            if(AiTurn==blackturn)
            {
                chessPiece[i][j]=black;
                peopleTurn=whiteturn;
            }
            else
            {
                chessPiece[i][j]=white;
                peopleTurn=blackturn;
            }
            int tempmin=-1;
            for (int k=0;k<LINE_NUM;k++)
            for (int l=0;l<LINE_NUM;l++){
                if(chessPiece[i][j]==null){
                    if(getvalue(k,l,peopleTurn)>tempmin) tempmin=getvalue(k,l,peopleTurn);
                }
            }
            tempmax-=tempmin;
            if(tempmax<0) tempmax=0;
            chessPiece[i][j]=null;
        }
        if(tempmax>max){
            max=tempmax;
            maxrow=i;
            maxcol=j;
        }
    }
    *row=maxrow;
    *col=maxcol;
}

int GameModel::getvalue(int i,int j,PlayerTurn thisTurn)
{
    int tempvalue=0;
    ChessPiece present=null,opposite=null;
    if (thisTurn==blackturn) present=black;
    else present=white;
    if (present==black) opposite=white;
    else opposite=black;

    {
        int l=1,r=1;
        for (; l<6; l++)
            if (i-l<0||chessPiece[i-l][j]==opposite) break;
        l--;
        for (; r<6; r++)
            if (i+r==LINE_NUM||chessPiece[i+r][j]==opposite) break;
        r--;
        if (l+r+1<6) tempvalue+=0;
        else {
            int k,h;
            for (k=i-1; k>=i-l; k--)
                if (chessPiece[k][j]==null) break;
            k++;
            for (h=i+1; h<=i+r; h++)
                if (chessPiece[h][j]==null) break;
            h--;
            if (h-k+1>=6) tempvalue+=3e7;
            if (h-k+1==5&&k>i-l&&h<i+r) tempvalue+=1e5;
            if (h-k+1==5&&(k==i-l||h==i+r)) tempvalue+=2e3;
            if (h-k+1==4) {
                if (k==i-l||h==i+r) tempvalue+=10;
                else if (k-1>i-l||h+1<i+r) tempvalue+=500;
                else tempvalue+=200;
            }
            if (h-k+1==3) {
                if (k==i-l||h==i+r) tempvalue+=4;
                else if ((k-2>i-l&&chessPiece[k-3][j]==present&&chessPiece[k-2][j]==present)||
                         (h+2<i+r&&chessPiece[h+3][j]==present&&chessPiece[h+2][j]==present))
                    tempvalue+=1e3;
                else tempvalue+=15;
            }
            if (h-k+1==2) {
                if (k==i-l||h==i+r) tempvalue+=1;
                else if ((k-3>i-l&&chessPiece[k-4][j]==present&&chessPiece[k-3][j]==present&&chessPiece[k-2][j]==present)||
                         (h+3<i+r&&chessPiece[h+4][j]==present&&chessPiece[h+3][j]==present&&chessPiece[h+2][j]==present))
                    tempvalue+=500;
                else tempvalue+=2;
            }
        }
    }
    {
        int l=1,r=1;
        for (; l<6; l++)
            if (i-l<0||chessPiece[i-l][j]==present) break;
        l--;
        for (; r<6; r++)
            if (i+r==LINE_NUM||chessPiece[i+r][j]==present) break;
        r--;
        if (l+r+1<6) tempvalue+=0;
        else {
            int k,h;
            for (k=i-1; k>=i-l; k--)
                if (chessPiece[k][j]==null) break;
            k++;
            for (h=i+1; h<=i+r; h++)
                if (chessPiece[h][j]==null) break;
            h--;
            if (h-k+1>=6) tempvalue+=1e7;
            if (h-k+1==5&&k>i-l&&h<i+r) tempvalue+=1e4;
            if (h-k+1==5&&(k==i-l||h==i+r)) tempvalue+=1e3;
            if (h-k+1==4) {
                if (k==i-l||h==i+r) tempvalue+=20;
                else if (k-1>i-l||h+1<i+r) tempvalue+=500;
                else tempvalue+=150;
            }
            if (h-k+1==3) {
                if (k==i-l||h==i+r) tempvalue+=2;
                else if ((k-2>i-l&&chessPiece[k-3][j]==opposite&&chessPiece[k-2][j]==opposite)||
                         (h+2<i+r&&chessPiece[h+3][j]==opposite&&chessPiece[h+2][j]==opposite))
                    tempvalue+=200;
                else tempvalue+=10;
            }
            if (h-k+1==2) {
                if (k==i-l||h==i+r) tempvalue+=0;
                else if ((k-3>i-l&&chessPiece[k-4][j]==opposite&&chessPiece[k-3][j]==opposite&&chessPiece[k-2][j]==opposite)||
                         (h+3<i+r&&chessPiece[h+4][j]==opposite&&chessPiece[h+3][j]==opposite&&chessPiece[h+2][j]==opposite))
                    tempvalue+=50;
                else tempvalue+=1;
            }
        }
    }

    {
        int l=1,r=1;
        for (; l<6; l++)
            if (j-l<0||chessPiece[i][j-l]==opposite) break;
        l--;
        for (; r<6; r++)
            if (j+r==LINE_NUM||chessPiece[i][j+r]==opposite) break;
        r--;
        if (l+r+1<6) tempvalue+=0;
        else {
            int k,h;
            for (k=j-1; k>=j-l; k--)
                if (chessPiece[i][k]==null) break;
            k++;
            for (h=j+1; h<=j+r; h++)
                if (chessPiece[i][h]==null) break;
            h--;
            if (h-k+1>=6) tempvalue+=3e7;
            if (h-k+1==5&&k>j-l&&h<j+r) tempvalue+=1e5;
            if (h-k+1==5&&(k==j-l||h==j+r)) tempvalue+=2e3;
            if (h-k+1==4) {
                if (k==j-l||h==j+r) tempvalue+=10;
                else if (k-1>j-l||h+1<j+r) tempvalue+=500;
                else tempvalue+=200;
            }
            if (h-k+1==3) {
                if (k==j-l||h==j+r) tempvalue+=4;
                else if ((k-2>j-l&&chessPiece[i][k-3]==present&&chessPiece[i][k-2]==present)||
                         (h+2<j+r&&chessPiece[i][h+3]==present&&chessPiece[i][h+2]==present))
                    tempvalue+=1e3;
                else tempvalue+=15;
            }
            if (h-k+1==2) {
                if (k==j-l||h==j+r) tempvalue+=1;
                else if ((k-3>j-l&&chessPiece[i][k-4]==present&&chessPiece[i][k-3]==present&&chessPiece[i][k-2]==present)||
                         (h+3<j+r&&chessPiece[i][h+4]==present&&chessPiece[i][h+3]==present&&chessPiece[i][h+2]==present))
                    tempvalue+=500;
                else tempvalue+=2;
            }
        }
    }
    {
        int l=1,r=1;
        for (; l<6; l++)
            if (j-l<0||chessPiece[i][j-l]==present) break;
        l--;
        for (; r<6; r++)
            if (j+r==LINE_NUM||chessPiece[i][j+r]==present) break;
        r--;
        if (l+r+1<6) tempvalue+=0;
        else {
            int k,h;
            for (k=j-1; k>=j-l; k--)
                if (chessPiece[i][k]==null) break;
            k++;
            for (h=j+1; h<=j+r; h++)
                if (chessPiece[i][h]==null) break;
            h--;
            if (h-k+1>=6) tempvalue+=1e7;
            if (h-k+1==5&&k>j-l&&h<j+r) tempvalue+=1e4;
            if (h-k+1==5&&(k==j-l||h==j+r)) tempvalue+=1e3;
            if (h-k+1==4) {
                if (k==j-l||h==j+r) tempvalue+=20;
                else if (k-1>j-l||h+1<j+r) tempvalue+=500;
                else tempvalue+=150;
            }
            if (h-k+1==3) {
                if (k==j-l||h==j+r) tempvalue+=2;
                else if ((k-2>j-l&&chessPiece[i][k-3]==opposite&&chessPiece[i][k-2]==opposite)||
                         (h+2<j+r&&chessPiece[i][h+3]==opposite&&chessPiece[i][h+2]==opposite))
                    tempvalue+=200;
                else tempvalue+=10;
            }
            if (h-k+1==2) {
                if (k==j-l||h==j+r) tempvalue+=0;
                else if ((k-3>j-l&&chessPiece[i][k-4]==opposite&&chessPiece[i][k-3]==opposite&&chessPiece[i][k-2]==opposite)||
                         (h+3<j+r&&chessPiece[i][h+4]==opposite&&chessPiece[i][h+3]==opposite&&chessPiece[i][h+2]==opposite))
                    tempvalue+=50;
                else tempvalue+=1;
            }
        }
    }

    {
        int l=1,r=1;
        for (; l<6; l++)
            if (i-l<0||j-l<0||chessPiece[i-l][j-l]==opposite) break;
        l--;
        for (; r<6; r++)
            if (i+r==LINE_NUM||j+r==LINE_NUM||chessPiece[i+r][j+r]==opposite) break;
        r--;
        if (l+r+1<6) tempvalue+=0;
        else {
            int k,h;
            for (k=i-1; k>=i-l; k--)
                if (chessPiece[k][j-i+k]==null) break;
            k++;
            for (h=i+1; h<=i+r; h++)
                if (chessPiece[h][j-i+h]==null) break;
            h--;
            if (h-k+1>=6) tempvalue+=3e7;
            if (h-k+1==5&&k>i-l&&h<i+r) tempvalue+=1e5;
            if (h-k+1==5&&(k==i-l||h==i+r)) tempvalue+=2e3;
            if (h-k+1==4) {
                if (k==i-l||h==i+r) tempvalue+=10;
                else if (k-1>i-l||h+1<i+r) tempvalue+=500;
                else tempvalue+=200;
            }
            if (h-k+1==3) {
                if (k==i-l||h==i+r) tempvalue+=4;
                else if ((k-2>i-l&&chessPiece[k-3][j-i+k-3]==present&&chessPiece[k-2][j-i+k-2]==present)||
                         (h+2<i+r&&chessPiece[h+3][j-i+h+3]==present&&chessPiece[h+2][j-i+h+2]==present))
                    tempvalue+=1e3;
                else tempvalue+=15;
            }
            if (h-k+1==2) {
                if (k==i-l||h==i+r) tempvalue+=1;
                else if ((k-3>i-l&&chessPiece[k-4][j-i+k-4]==present&&chessPiece[k-3][j-i+k-3]==present&&chessPiece[k-2][j-i+k-2]==present)||
                         (h+3<i+r&&chessPiece[h+4][j-i+h+4]==present&&chessPiece[h+3][j-i+h+3]==present&&chessPiece[h+2][j-i+h+2]==present))
                    tempvalue+=500;
                else tempvalue+=2;
            }
        }
    }
    {
        int l=1,r=1;
        for (; l<6; l++)
            if (i-l<0||j-l<0||chessPiece[i-l][j-l]==present) break;
        l--;
        for (; r<6; r++)
            if (i+r==LINE_NUM||j+r==LINE_NUM||chessPiece[i+r][j+r]==present) break;
        r--;
        if (l+r+1<6) tempvalue+=0;
        else {
            int k,h;
            for (k=i-1; k>=i-l; k--)
                if (chessPiece[k][j-i+k]==null) break;
            k++;
            for (h=i+1; h<=i+r; h++)
                if (chessPiece[h][j-i+h]==null) break;
            h--;
            if (h-k+1>=6) tempvalue+=1e7;
            if (h-k+1==5&&k>i-l&&h<i+r) tempvalue+=1e5;
            if (h-k+1==5&&(k==i-l||h==i+r)) tempvalue+=1e3;
            if (h-k+1==4) {
                if (k==i-l||h==i+r) tempvalue+=20;
                else if (k-1>i-l||h+1<i+r) tempvalue+=500;
                else tempvalue+=150;
            }
            if (h-k+1==3) {
                if (k==i-l||h==i+r) tempvalue+=2;
                else if ((k-2>i-l&&chessPiece[k-3][j-i+k-3]==opposite&&chessPiece[k-2][j-i+k-2]==opposite)||
                         (h+2<i+r&&chessPiece[h+3][j-i+h+3]==opposite&&chessPiece[h+2][j-i+h+2]==opposite))
                    tempvalue+=200;
                else tempvalue+=10;
            }
            if (h-k+1==2) {
                if (k==i-l||h==i+r) tempvalue+=0;
                else if ((k-3>i-l&&chessPiece[k-4][j-i+k-4]==opposite&&chessPiece[k-3][j-i+k-3]==opposite&&chessPiece[k-2][j-i+k-2]==opposite)||
                         (h+3<i+r&&chessPiece[h+4][j-i+h+4]==opposite&&chessPiece[h+3][j-i+h+3]==opposite&&chessPiece[h+2][j-i+h+2]==opposite))
                    tempvalue+=50;
                else tempvalue+=1;
            }
        }
    }

    {
        int l=1,r=1;
        for (; l<6; l++)
            if (i-l<0||j+l==LINE_NUM||chessPiece[i-l][j+l]==opposite) break;
        l--;
        for (; r<6; r++)
            if (i+r==LINE_NUM||j-r>0||chessPiece[i+r][j-r]==opposite) break;
        r--;
        if (l+r+1<6) tempvalue+=0;
        else {
            int k,h;
            for (k=i-1; k>=i-l; k--)
                if (chessPiece[k][j+i-k]==null) break;
            k++;
            for (h=i+1; h<=i+r; h++)
                if (chessPiece[h][j+i-h]==null) break;
            h--;
            if (h-k+1>=6) tempvalue+=3e7;
            if (h-k+1==5&&k>i-l&&h<i+r) tempvalue+=1e5;
            if (h-k+1==5&&(k==i-l||h==i+r)) tempvalue+=2e3;
            if (h-k+1==4) {
                if (k==i-l||h==i+r) tempvalue+=10;
                else if (k-1>i-l||h+1<i+r) tempvalue+=500;
                else tempvalue+=200;
            }
            if (h-k+1==3) {
                if (k==i-l||h==i+r) tempvalue+=4;
                else if ((k-2>i-l&&chessPiece[k-3][j+i-k+3]==present&&chessPiece[k-2][j+i-k+2]==present)||
                         (h+2<i+r&&chessPiece[h+3][j+i-h-3]==present&&chessPiece[h+2][j+i-h-2]==present))
                    tempvalue+=1e3;
                else tempvalue+=15;
            }
            if (h-k+1==2) {
                if (k==i-l||h==i+r) tempvalue+=1;
                else if ((k-3>i-l&&chessPiece[k-4][j+i-k+4]==present&&chessPiece[k-3][j+i-k+3]==present&&chessPiece[k-2][j+i-k+2]==present)||
                         (h+3<i+r&&chessPiece[h+4][j+i-h-4]==present&&chessPiece[h+3][j+i-h-3]==present&&chessPiece[h+2][j+i-h-2]==present))
                    tempvalue+=500;
                else tempvalue+=2;
            }
        }
    }
    {
        int l=1,r=1;
        for (; l<6; l++)
            if (i-l<0||j+l==LINE_NUM||chessPiece[i-l][j+l]==present) break;
        l--;
        for (; r<6; r++)
            if (i+r==LINE_NUM||j-r<0||chessPiece[i+r][j-r]==present) break;
        r--;
        if (l+r+1<6) tempvalue+=0;
        else {
            int k,h;
            for (k=i-1; k>=i-l; k--)
                if (chessPiece[k][j+i-k]==null) break;
            k++;
            for (h=i+1; h<=i+r; h++)
                if (chessPiece[h][j+i-h]==null) break;
            h--;
            if (h-k+1>=6) tempvalue+=1e7;
            if (h-k+1==5&&k>i-l&&h<i+r) tempvalue+=1e5;
            if (h-k+1==5&&(k==i-l||h==i+r)) tempvalue+=1e3;
            if (h-k+1==4) {
                if (k==i-l||h==i+r) tempvalue+=20;
                else if (k-1>i-l||h+1<i+r) tempvalue+=500;
                else tempvalue+=150;
            }
            if (h-k+1==3) {
                if (k==i-l||h==i+r) tempvalue+=2;
                else if ((k-2>i-l&&chessPiece[k-3][j+i-k+3]==opposite&&chessPiece[k-2][j+i-k+2]==opposite)||
                         (h+2<i+r&&chessPiece[h+3][j+i-h-3]==opposite&&chessPiece[h+2][j+i-h-2]==opposite))
                    tempvalue+=200;
                else tempvalue+=10;
            }
            if (h-k+1==2) {
                if (k==i-l||h==i+r) tempvalue+=0;
                else if ((k-3>i-l&&chessPiece[k-4][j+i-k+4]==opposite&&chessPiece[k-3][j+i-k+3]==opposite&&chessPiece[k-2][j+i-k+2]==opposite)||
                         (h+3<i+r&&chessPiece[h+4][j+i-h-4]==opposite&&chessPiece[h+3][j+i-h-3]==opposite&&chessPiece[h+2][j+i-h-2]==opposite))
                    tempvalue+=50;
                else tempvalue+=1;
            }
        }
    }

    return tempvalue;
}

void GameModel::cheat(int row, int col)
{
    if (!(row>=0&&row<LINE_NUM&&col>=0&&col<LINE_NUM)) return;
    if (playerTurn==blackturn) chessPiece[row][col]=black;
    if (playerTurn==whiteturn) chessPiece[row][col]=white;
}

