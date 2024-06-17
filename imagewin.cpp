#include "imagewin.h"
#include "ui_imagewin.h"

# pragma execution_character_set("utf-8")

imageWin::imageWin(ListWidgetItemChild *item, QRect rect, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::imageWin)
{
    ui->setupUi(this);

    qApp->installEventFilter(this);
    this->setGeometry(rect);//设置显示图片窗口的x、y、w、h
    this->setFixedSize(rect.width(), rect.height());//设置窗口固定大小
    this->setWindowIcon(item->icon());
    this->setWindowTitle("查看图片");
    this->setWindowModality(Qt::ApplicationModal);//阻塞除当前窗体外的其他所有窗体

    //通过QLabel加载item上的图片
    QLabel *lab = new QLabel(this);
    lab->setFixedSize(this->width(), this->height());
    lab->setPixmap(item->icon().pixmap(QSize(this->width(), this->height())).scaled(lab->width(),lab->height()));


}

imageWin::~imageWin()
{
    delete ui;
}
