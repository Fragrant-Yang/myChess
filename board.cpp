#include "board.h"
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <QDebug>
#include <QFont>
#include <QMessageBox>


Board::Board(QWidget *parent)
    : QWidget(parent)
{
    _label = new QLabel(this);
    init();
}

Board::~Board()
{

}

void Board::init()
{
    _redRound = true;
    _selectId = -1;
    for(int i=0; i<32; i++)
    {
        _stone[i].init(i);
    }

    this->resize(768, 512);

    _label->setText("红方回合");
    QFont ft;
    ft.setPointSize(18);
    _label->setFont(ft);
    _label->move(this->width() - _label->width()-30, 0);
    _label->show();
}

QPoint Board::center(int col, int row)
{
    int x = (col+1)*_d;
    int y = (row+1)*_d;

    return QPoint(x, y);
}

void Board::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 绘棋盘
    _d = std::min(this->width(), this->height())/11;
    int d = _d;
    // 10条横线
    for(int i=1; i<=10; i++)
    {
        painter.drawLine(QPoint(d, i*d), QPoint(9*d, i*d));
    }
    // 9条竖线
    for(int i=1; i<=9; i++)
    {
        if(i==1 || i==9){
            painter.drawLine(QPoint(i*d, d), QPoint(i*d, 10*d));
        }
        else {
            painter.drawLine(QPoint(i*d, d), QPoint(i*d, 5*d));
            painter.drawLine(QPoint(i*d, 6*d), QPoint(i*d, 10*d));
        }
    }
    // 九宫格
    painter.drawLine(QPoint(4*d, d), QPoint(6*d, 3*d));
    painter.drawLine(QPoint(6*d, d), QPoint(4*d, 3*d));
    // 下九宫
    painter.drawLine(QPoint(4*d, 10*d), QPoint(6*d, 8*d));
    painter.drawLine(QPoint(6*d, 10*d), QPoint(4*d, 8*d));



    // 画棋子
    for(int i=0; i<32; i++)
    {
        drawChess(&painter, i);
        //painter.drawText();
    }

    if(true == _redRound) _label->setText("红方回合");
    else _label->setText("黑方回合");

}

// 鼠标点击释放事件
void Board::mouseReleaseEvent(QMouseEvent *e)
{
    QPoint curPos = e->pos();
    // _selectId = -1;
    int col, row;   // 当前点击位置
    // 判断点是否落在棋盘
    bool ret = getCurPos(curPos, col, row);
    if(!ret) return;    // 落在棋盘外，忽略

    int clickId = -1;   // 本次点击的棋子
    // 获取本次点击的棋子
    clickId = getStoneId(row, col);

    // 如果是第一次点击到棋子
    if(_selectId == -1)
    {
        // 不是当前回合
        if(clickId != -1 && _stone[clickId]._red != _redRound) return;
        // 设置 _selectId
        _selectId = clickId;

    }
    else { // 如果之前点击了，实现移动和吃子逻辑

        if(clickId == -1)
        {
            // 可不可以移动
            if(canMove(_selectId, row, col, clickId))
            {
                _stone[_selectId]._row = row;
                _stone[_selectId]._col = col;
                _redRound = !_redRound; // 切换回合
            }
            _selectId = -1;

        }
        else {
            if(_stone[_selectId]._red == _stone[clickId]._red)
            {
                _selectId = clickId;
            }else {
                // 可不可以吃
                if(canMove(_selectId, row, col, clickId))
                {
                    _stone[_selectId]._row = row;
                    _stone[_selectId]._col = col;
                    _stone[clickId]._dead = true;
                    _redRound = !_redRound;
                }
                _selectId = -1;
            }
        }
    }

    update();
    checkWin();
}

// 画棋子
void Board::drawChess(QPainter *p, int id)
{

    if(_stone[id]._dead == true) return;
    if(_selectId == id)
    {
        p->setBrush(Qt::gray);
    }
    else {
        p->setBrush(Qt::yellow);
    }

    QPoint c = center(_stone[id].col(), _stone[id].row());
    //qDebug() <<"draw" <<  _stone[id]._row << " : " << _stone[id]._col;
    p->setPen(Qt::black);
    if(true == _stone[id]._red)
        p->setPen(Qt::red);
    p->drawEllipse(c, _d/2, _d/2);

    p->setFont(QFont("system", _d/2, 700));
    // 画字
    QRect rect = QRect(c.x()-_d/2, c.y()-_d/2, _d, _d);
    p->drawText(rect, _stone[id].getText(), QTextOption(Qt::AlignCenter));

}

// 当前点击是否有效
bool Board::getCurPos(QPoint pos, int &col, int &row)
{
    for(int i=0; i<10; i++)
    {
        for(int j=0; j<9; j++)
        {
            QPoint c = center(j, i);
            if(pos.x() < c.x()+_d/2 && pos.x() > c.x()-_d/2)
            {
                if(pos.y() < c.y()+_d/2 && pos.y() > c.y() - _d/2){
                    col = j;
                    row = i;
                    return true;
                }
            }
        }
    }
    return false;
}

// 走棋逻辑
bool Board::canMove(int id, int row, int col, int clickId)
{
    switch (_stone[id]._type) {
    case Stone::JIANG:
        if (col > 2 && col < 6 &&
            ((_stone[id]._red && row < 3 || !_stone[id]._red && row > 6) && (abs(row - _stone[id]._row) + abs(col - _stone[id]._col) == 1)) //0+1=1
            || countAtLine(_stone[4]._row, _stone[4]._col, _stone[20]._row, _stone[20]._col) == 0)
            return true;
        break;
    case Stone::CHE:
        if (countAtLine(row, col, _stone[id]._row, _stone[id]._col) == 0)
            return true;
        break;
    case Stone::MA:
        if ( (abs(row-_stone[id]._row)==1 && abs(col-_stone[id]._col)==2 //左右跳
             && getStoneId(_stone[id]._row,(col+_stone[id]._col)>>1) == -1 //没拐脚
             ) || (abs(row-_stone[id]._row)==2 && abs(col-_stone[id]._col)==1 //上下跳
                   && getStoneId((row+_stone[id]._row)>>1,_stone[id]._col) == -1)) //没拐脚
            return true;
        break;
    case Stone::PAO:
        if (getStoneId(row, col) == -1 && countAtLine(row, col, _stone[id]._row, _stone[id]._col) == 0 //移动
            || getStoneId(row, col) != -1 && countAtLine(row, col, _stone[id]._row, _stone[id]._col) == 1) //吃子
            return true;
        break;
    case Stone::XIANG:
        if ((_stone[id]._red && row < 5 || !_stone[id]._red && row > 4) //没过河
            && abs(row - _stone[id]._row) == 2 && abs(col - _stone[id]._col) == 2 //象步
            && getStoneId((row+_stone[id]._row)>>1, (col+_stone[id]._col)>>1) == -1 ) //没象眼
            return true;
        break;
    case Stone::SHI:
        if (col > 2 && col < 6 && (_stone[id]._red && row < 3 || !_stone[id]._red && row > 6)
            && abs(row - _stone[id]._row) == 1 && abs(col - _stone[id]._col) == 1)
            return true;
        break;
    case Stone::BING:
        if (abs(row - _stone[id]._row) + abs(col - _stone[id]._col) != 1)
            break;
        if (_stone[id]._red) { //红棋在上
            if (row < _stone[id]._row) break;
            if (_stone[id]._row <= 4 && row == _stone[id]._row) break;
        } else { //黑棋在下
            if (row > _stone[id]._row) break;
            if (_stone[id]._row >= 5 && row == _stone[id]._row) break;
        }
        return true;
    }
    return false;
}


//统计直线上棋子个数
int Board::countAtLine(int row1, int col1, int row2, int col2)
{
    int min, max, cnt = 0;
    if (row1 != row2 && col1 != col2)
        return -1;
    if (row1 == row2) {
        if (col1 < col2) {
            min = col1;
            max = col2;
        } else {
            min = col2;
            max = col1;
        }
        for (int col = min+1; col < max; col++)
            if (getStoneId(row1, col) >= 0)
                cnt++;
    } else if (col1 == col2) {
        if (row1 < row2) {
            min = row1;
            max = row2;
        } else {
            min = row2;
            max = row1;
        }
        for (int row = min+1; row < max; row++)
            if (getStoneId(row, col1) >= 0)
                cnt++;
    }
    return cnt;
}

//判断该行列位置有没没棋子，有就返回ID，没有就返回-1
int Board::getStoneId(int row, int col)
{
    for (int i = 0; i < 32; i++)
        if (row == _stone[i]._row && col == _stone[i]._col && !_stone[i]._dead)
            return i; //有棋子，返回棋子ID
    return -1; //该行列位置没棋子
}

void Board::checkWin()
{
    if (_stone[4]._dead) {
        QMessageBox::information(this, "结束啦", "黑胜");
        init();
    } else if (_stone[20]._dead) {
        QMessageBox::information(this, "结束啦", "红胜");
        init();
    }
}
