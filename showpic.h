#ifndef SHOWPIC_H
#define SHOWPIC_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/types_c.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QFile>
#include <QInputDialog>
#include "relistwidget_item.h"
#include "imagewin.h"
#include <QVBoxlayout>

namespace Ui {
class showpic;
}

class showpic : public QWidget
{
    Q_OBJECT

public:
    explicit showpic(char*, QWidget *parent = 0);
//    explicit showpic(QWidget *parent = 0);
    ~showpic();
    QVector<char*> res_split;//存储分割后的字符串
    ListWidgetRe *listWidget;

    QString LightPath;
    QString MiddlePath;
    QString DarkPath;
    void LoadPicture(QString file, QStringList list, QVector<char*>);
//    bool CompareItem(ListWidgetItemChild* item1, ListWidgetItemChild* item2);

public slots:
    void enlargeImage(QListWidgetItem *item);//放大图片

signals:
    void PictureReady();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_light_clicked();

    void on_pushButton_middle_clicked();

    void on_pushButton_dark_clicked();

private:
    Ui::showpic *ui;



};

#endif // SHOWPIC_H
