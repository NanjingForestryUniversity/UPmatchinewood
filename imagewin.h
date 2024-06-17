#ifndef IMAGEWIN_H
#define IMAGEWIN_H

#include <QWidget>
#include <QListWidget>
#include <QRect>
#include <QLabel>
#include "relistwidget_item.h"
namespace Ui {
class imageWin;
}

class imageWin : public QWidget
{
    Q_OBJECT

public:
    explicit imageWin(ListWidgetItemChild *item, QRect rect, QWidget *parent = 0);
    ~imageWin();


private:
    Ui::imageWin *ui;
};

#endif // IMAGEWIN_H
