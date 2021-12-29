#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include "stone.h"
#include <QLabel>

class Board : public QWidget
{
    Q_OBJECT

public:
    Board(QWidget *parent = 0);
    ~Board();

    QPoint center(int row, int col);
    void init();
    void checkWin();
protected:
    // 绘图事件
    void paintEvent(QPaintEvent *event);
    // 鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *e);
    // 画棋子
    void drawChess(QPainter* p, int id);
    // 当前点击是否有效
    bool getCurPos(QPoint pos, int &col, int &row);
    // 判断是否可以移动
    bool canMove(int id, int row, int col, int clickId);
    //统计直线上棋子个数
    int countAtLine(int row1, int col1, int row2, int col2);
    //判断该行列位置有没没棋子，有就返回ID，没有就返回-1
    int getStoneId(int row, int col);
private:
    int _d; // 直径
    Stone _stone[32];   // 棋子
    int _selectId;  // 选中的棋子
    // int _clickId;   // 当前点击的位置
    bool _redRound; // 是哪个回合
    QLabel *_label;  // 显示回合
};

#endif // BOARD_H
