#ifndef STONE_H
#define STONE_H

#include <QString>

class Stone
{
public:
    enum TYPE { CHE, MA, XIANG, SHI, JIANG, PAO, BING};
    Stone();

    void init(int id);
    int row() { return _row; }
    int col() { return _col; }

    QString getText();

public:
    int _col;   // 棋子所在列
    int _row;   // 棋子所在行
    int _id;    // 棋子编号
    TYPE _type;  // 棋子类型
    bool _red;   // 棋子颜色
    bool _dead;  // 棋子是否被吃

};

#endif // STONE_H
