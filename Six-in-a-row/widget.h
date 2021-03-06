#ifndef WIDGET_H
#define WIDGET_H

#include "GameModel.h"

#include <QWidget>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    GameModel *game;
    GameMode game_mode=unknownMode;

    //鼠标
    int mousePosRow=-1,mousePosCol=-1;//鼠标所在行列
    bool rightMousePos=false;//是否是合法的选中位置
    bool mousePosPvP=false,mousePosPvE=false;//鼠标在按钮上
    bool mouseClickPvP=false,mouseClickPvE=false;//鼠标点击按钮

    //绘制
    void paintEvent(QPaintEvent *event);

    //鼠标位置
    void mouseMoveEvent(QMouseEvent *event);

    //鼠标点击
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    //键盘输入
    void keyPressEvent(QKeyEvent *event);

    //落子
    void chessByPerson();
    void chessByAI();

    //初始化
    void initGame();
    void initPvPGame();
    void initPvEGame();
};
#endif // WIDGET_H
