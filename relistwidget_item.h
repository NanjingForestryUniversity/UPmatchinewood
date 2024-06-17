#ifndef RELISTWIDGET_ITEM_H
#define RELISTWIDGET_ITEM_H


#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>

class ListWidgetItemChild : public QListWidgetItem
{

public:
    explicit ListWidgetItemChild(QListWidget *parent = 0);

    ~ListWidgetItemChild();
    QString LBrightnessValue;
    QString ARedGreenValue;
    QString BYellowBlueValue;

    bool operator<(const QListWidgetItem &other) const
    {
        int a, b;
        a = this->text().mid(9,3).toInt();
        b = other.text().mid(9,3).toInt();
        return a < b;
    }


};

class ListWidgetRe : public QListWidget
{
    Q_OBJECT
    friend class ListWidgetItemChild;
 public:
    explicit ListWidgetRe(QWidget *parent = nullptr);
    ~ListWidgetRe();

Q_SIGNALS:
//    void itemDoubleClicked(ListWidgetItemChild *item);
};
#endif // RELISTWIDGET_ITEM_H
