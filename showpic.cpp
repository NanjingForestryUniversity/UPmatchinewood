#include "showpic.h"
#include "ui_showpic.h"


# pragma execution_character_set("utf-8")



using namespace std;
void deleteImage(QString folderPath, QString imageName);
QString FindImage(QString folderPath, QString imageName);

extern QString DirPath;
//QString DirPath = "D:/picture/524test";


QStringList GetFileNameList(const QString &strDirpath)//该函数的作用是获取一个目录下所有的文件名，并将文件名存储在QStringList中返回。
{
    QDir dir(strDirpath);
    QFileInfoList Info_list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    QListIterator<QFileInfo> Iterator(Info_list);
    QStringList strFileNameList;
    QFileInfo Info;
    while (Iterator.hasNext())
    {
        Info = Iterator.next();
        if (Info.isFile())
        {
            strFileNameList << Info.fileName();
        }
    }

    //排序3.28
    qSort(strFileNameList.begin(), strFileNameList.end(), [](const QString& s1, const QString& s2){
                return s1.mid(3,2).toInt() < s2.mid(3,2).toInt(); });

    return strFileNameList;
}

showpic::showpic(char* buf1, QWidget *parent) :
    QWidget(parent),
//showpic::showpic(QWidget *parent) :
//    QWidget(parent),
    ui(new Ui::showpic)
{
    ui->setupUi(this);
//切割char数组
    const char split[] = ",";
    char* res = strtok(buf1, split);//image_name必须为char[]
    while (res != NULL)//切割buf,将有用的数据全部在res_split内，可通过res_split.at(i)调用
    {
//        qDebug() << "res" << res;
        res_split.push_back(res);//??
        res = strtok(NULL, split);//??

    }
    qDebug() << "res_split.size()" << res_split.size();

    LightPath = DirPath +"/light/";
    MiddlePath = DirPath +"/Middle/";
    DarkPath = DirPath +"/dark/";

    QStringList listLight =  GetFileNameList(LightPath);//应该是一个全局变量3.28
    QStringList listMiddle =  GetFileNameList(MiddlePath);
    QStringList listDark =  GetFileNameList(DarkPath);

//    QStringList listLight =  GetFileNameList("D:/wood_color317/data/2023318/light/");//应该是一个全局变量3.28
//    QStringList listMiddle =  GetFileNameList("D:/wood_color317/data/2023318/middle/");
//    QStringList listDark =  GetFileNameList("D:/wood_color317/data/2023318/dark/");
//将90个lab值分三类
    QVector<char*> Light_lab;
    QVector<char*> Middle_lab;
    QVector<char*> Dark_lab;
/*    for(int i = 0; i < 90; i++)
    {
        if(i < 30)
        {
            Light_lab.append(res_split.at(i));
//            qDebug() << Light_lab.at(i) << i;

        }
        else if(i > 29 && i < 60)
        {
            Middle_lab.append(res_split.at(i));
//            qDebug() << Middle_lab.at(i-30) << i;
        }
        else if(i > 59 && i < 90)
        {
            Dark_lab.append(res_split.at(i));
//            qDebug() << Dark_lab.at(i-60) << i;
        }
    }
    */
    for(int i = 0; i < (listLight.size() + listMiddle.size() + listDark.size()); i++)
    {
        if(i < listLight.size())
        {
            Light_lab.append(res_split.at(i));
            qDebug() << Light_lab.at(i) << i;

        }
        else if(i > (listLight.size() - 1) && i < (listLight.size() + listMiddle.size()))
        {
            Middle_lab.append(res_split.at(i));
            qDebug() << Middle_lab.at(i-listLight.size()) << i;
        }
        else /*if(i > (listLight.size() + listMiddle.size() - 1) && i < 90)*/
        {
            Dark_lab.append(res_split.at(i));
            qDebug() << Dark_lab.at(i-listLight.size() - listMiddle.size()) << i;
        }
    }
    listWidget = new ListWidgetRe(this);
    listWidget->setViewMode(QListWidget::IconMode);//显示模式
    listWidget->setIconSize(QSize(400, 90));//设置图片大小
    listWidget->setSpacing(10);//间距
    listWidget->setResizeMode(QListView::Adjust);//适应布局调整
    listWidget->setMovement(QListView::Static);//不能移动


//    QString file = "D:/wood_color317/data/2023318/light/";
    QString file = LightPath;
    LoadPicture(file, listLight, Light_lab);

//    QListWidgetItem *separate0 = new QListWidgetItem;
//    separate0->setText("之后为中色");
//    separate0->setFlags(separate0->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
//    listWidget->addItem(separate0);


//    file = "D:/wood_color317/data/2023318/middle/";
    file = MiddlePath;
    LoadPicture(file, listMiddle, Middle_lab);

//    QListWidgetItem *separate1 = new QListWidgetItem;
//    separate1->setFlags(separate0->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
//    separate1->setText("之后为深色");
//    listWidget->addItem(separate1);

//    file = "D:/wood_color317/data/2023318/dark/";
    file = DarkPath;
    LoadPicture(file, listDark, Dark_lab);

    listWidget->sortItems();
    listWidget->setSelectionMode(QAbstractItemView::MultiSelection);//设置多选模式
    QVBoxLayout* VBox = new QVBoxLayout;
    QHBoxLayout* HBox = new QHBoxLayout;
    ui->pushButton->setFixedSize(140,70);
    ui->pushButton_2->setFixedSize(140,70);
    ui->pushButton_3->setFixedSize(140,70);
    ui->pushButton_light->setFixedSize(140,70);
    ui->pushButton_middle->setFixedSize(140,70);
    ui->pushButton_dark->setFixedSize(140,70);


    HBox->addWidget(ui->pushButton);
    HBox->addWidget(ui->pushButton_2);
    HBox->addWidget(ui->pushButton_3);
    HBox->addWidget(ui->pushButton_light);
    HBox->addWidget(ui->pushButton_middle);
    HBox->addWidget(ui->pushButton_dark);


    VBox->addWidget(listWidget);
    VBox->addLayout(HBox);

    this->setLayout(VBox);

    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(enlargeImage(QListWidgetItem*)));

    qDebug() << "showpic ready";

    emit PictureReady();

}

showpic::~showpic()
{
    delete ui;
}

void showpic::enlargeImage(QListWidgetItem *item)
{

    QRect rect =  this->geometry();//获取当前窗口坐标及大小 x、y、w、h
//    static_cast<ListWidgetItemChild*>(listWidget->itemWidget(item));
    ListWidgetItemChild *item0 = static_cast<ListWidgetItemChild *>(item);
    //通过自定义的窗口显示图片
//    imageWin *showImageWidget = new imageWin(item , QRect(rect.x(), rect.y()+rect.y()*1/4, rect.width(), rect.height()*2/3));
    imageWin *showImageWidget = new imageWin(item0 , QRect(rect.x(), rect.y()+rect.y()*1/4, rect.width(), rect.height()*1.7/3));
    showImageWidget->show();

    //显示条目名称
    qDebug() << item0->text();
    QString item_name = item0->text();
    if(item_name.at(4) != '.')
    {
        QString a = item_name.at(3);
        a.append(item_name.at(4));
        qDebug() << "a" << a.toInt();
//        int b = a.toInt();
    int L = item0->LBrightnessValue.toInt();
    int A = item0->ARedGreenValue.toInt();
    int B = item0->BYellowBlueValue.toInt();
        qDebug() << "LAB" << L << A << B;
        //显示色块
        cv::Mat mat = cv::Mat(100, 100, CV_8UC3, cv::Scalar(L, A, B));
        cv::cvtColor(mat, mat, cv::COLOR_Lab2BGR);
        cv::imshow("sekuai", mat);
    }
    else
    {
        qDebug() << "--";
    }

}
//rgb00.png175136163QQ
//拼图片路径，并添加到imageItem，再添加到listWidget
void showpic::LoadPicture(QString file, QStringList list, QVector<char*> lab)
{
    for(int i = 0; i < list.size(); i++)
    {
        qDebug() << lab.at(i) << i;
        QString LAB = lab.at(i);
        QString tmp = file + list.at(i);//拼出图片绝对路径

        ListWidgetItemChild *imageItem = new ListWidgetItemChild;
        imageItem->LBrightnessValue =LAB.mid(0,3);
        imageItem->ARedGreenValue = LAB.mid(3,3);
        imageItem->BYellowBlueValue = LAB.mid(6,3);

        imageItem->setIcon(QIcon(tmp));
        imageItem->setText(list.at(i) + LAB.mid(0,3) + LAB.mid(9,2));
//            qDebug() << "luminance1" << lab.at(i) << i;
        if(LAB.mid(9,1) == LAB.mid(10,1))
        {
        }
        else
        {
            imageItem->setTextColor(Qt::red);
        }
        imageItem->setSizeHint(QSize(400, 120));
        listWidget->addItem(imageItem);
    }


}
//rgb00.png000
//bool showpic::CompareItem(ListWidgetItemChild *item1, ListWidgetItemChild *item2)
//{
//    int luminance1 = item1->text().mid(9,3).toInt();
//    qDebug() << "luminance1" << luminance1;
//    int luminance2 = item2->text().mid(9,3).toInt();
//    qDebug() << "luminance2" << luminance2;

//    return luminance1 < luminance2;

//}



void showpic::on_pushButton_clicked()
{
    // 获取选中的item
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
//    QListWidgetItem *selectedItem = listWidget->currentItem();
    if (selectedItems.size() != 1) {
        // 如果选中的item数量不等于2，则弹出提示框并返回
        QMessageBox::warning(this, "Error", "请至少选择一张图片");
        return;
    }
    //并从文件夹中删除
    QListWidgetItem* item1 = selectedItems[0];
    QString DelPicName = item1->text().mid(0,9);
    qDebug() << "DelPicName" << DelPicName;
    deleteImage(DirPath, DelPicName);

    // 如果有选中的item，则删除它
    if (item1 != nullptr) {
        delete item1;
    }

}



void showpic::on_pushButton_2_clicked()
{

        // 获取选中的两个item
        QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
        if (selectedItems.size() != 2) {
            // 如果选中的item数量不等于2，则弹出提示框并返回
            QMessageBox::warning(this, "Error", "请选中2张图片进行操作：先选的图片插入到后选的后面");
            return;
        }
        QListWidgetItem* item1 = selectedItems[0];
        QListWidgetItem* item2 = selectedItems[1];

        // 获取它们在listWidget中的位置
        int index1 = listWidget->row(item1);
        int index2 = listWidget->row(item2);
        qDebug() << "index1" << index1;
        qDebug() << "index2" << index2;

        // 交换它们的位置,!!!!!!!!!有点问题
        listWidget->takeItem(index1);
        listWidget->takeItem(index2);
        listWidget->insertItem(index1, item2);
        listWidget->insertItem(index2, item1);

        index1 = listWidget->row(item1);
        index2 = listWidget->row(item2);
        qDebug() << "index1new" << index1;
        qDebug() << "index2new" << index2;

        // 取消选中状态
        item1->setSelected(false);
        item2->setSelected(false);
}

void showpic::on_pushButton_3_clicked()
{
    if(listWidget->currentItem() == nullptr)
    {
        QMessageBox::warning(this, "Error", "请点击一个图片进行操作");

        return;
    }

    QListWidgetItem *selectedItem = listWidget->currentItem();


    QString PicNamePath = FindImage(DirPath, selectedItem->text().mid(0, 9));//
//    sprintf(PicNamePath, PicNamePath);
    cv::Mat Pic = cv::imread(PicNamePath.toLatin1().data());// .toLatin1().data(): string转char*

    QInputDialog *inputDialog_item=new QInputDialog(this);
    QStringList list;
    list<<"浅色"<<"中色"<<"深色";
    bool getItem;
    QString  season = inputDialog_item->getItem(this,"选择文件夹","文件夹类别",list,0,false,&getItem);
    if(getItem)
    {
        if("浅色" == season)
        {
            QString RePicName = LightPath + selectedItem->text().mid(0,9);
            //        PicImg.save(RePicName, "PNG", -1);
            cv::imwrite(RePicName.toLatin1().data(), Pic);
            //调用删除图片函数
            QString DelPicName = selectedItem->text().mid(0,9);
            qDebug() << "DelPicName" << DelPicName;
            deleteImage(MiddlePath, DelPicName);
            deleteImage(DarkPath, DelPicName);

            if (selectedItem != nullptr) {
                delete selectedItem;
            }
        }

        if("中色" == season)
        {
            QString RePicName = MiddlePath + selectedItem->text().mid(0,9);
            //        PicImg.save(RePicName, "PNG", -1);
            cv::imwrite(RePicName.toLatin1().data(), Pic);

            //调用删除图片函数
            QString DelPicName = selectedItem->text().mid(0,9);
            qDebug() << "DelPicName" << DelPicName;
            deleteImage(LightPath, DelPicName);
            deleteImage(DarkPath, DelPicName);

            if (selectedItem != nullptr) {
                delete selectedItem;
            }
        }

        if("深色" == season)
        {
            QString RePicName = DarkPath + selectedItem->text().mid(0,9);
            //        PicImg.save(RePicName, "PNG", -1);
            cv::imwrite(RePicName.toLatin1().data(), Pic);

            //调用删除图片函数
            QString DelPicName = selectedItem->text().mid(0,9);
            qDebug() << "DelPicName" << DelPicName;
            deleteImage(LightPath, DelPicName);
            deleteImage(MiddlePath, DelPicName);

            if (selectedItem != nullptr) {
                delete selectedItem;
            }
        }
    }
}

//按键，多选移动rgb00.png000QQ
void showpic::on_pushButton_light_clicked()
{
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();

    int Numbers = selectedItems.size();
    QListWidgetItem* Item = new QListWidgetItem[Numbers];
    for(int i = 0; i < Numbers; i++)
    {
//        qDebug() <<
        Item[i] = *selectedItems[i];
        if(Item[i].text().mid(12, 1) == 'Q')
        {
            return;
        }
        if(Item[i].text().mid(12, 1) == 'S')
        {
//            QString PicNamePath = FindImage("D:/2023/wooden/2023318/", Item[i].text().mid(0,9));
            QString PicNamePath = FindImage(DirPath, Item[i].text().mid(0,9));
            cv::Mat Pic = cv::imread(PicNamePath.toLatin1().data());

//            QString RePicName = "D:/2023/wooden/2023318/light/" + Item[i].text().mid(0,9);
            QString RePicName = LightPath + Item[i].text().mid(0,9);
            cv::imwrite(RePicName.toLatin1().data(), Pic);

            //调用删除图片函数
            QString DelPicName = Item[i].text().mid(0,9);
            qDebug() << "DelPicName" << DelPicName;
//            deleteImage("D:/2023/wooden/2023318/dark/", DelPicName);
            deleteImage(DarkPath, DelPicName);
            if (selectedItems[i] != nullptr) {
                delete selectedItems[i];
            }
        }

        if(Item[i].text().mid(12, 1) == 'Z')
        {
//            QString PicNamePath = FindImage("D:/2023/wooden/2023318/", Item[i].text().mid(0,9));
            QString PicNamePath = FindImage(DirPath, Item[i].text().mid(0,9));
            cv::Mat Pic = cv::imread(PicNamePath.toLatin1().data());

//            QString RePicName = "D:/2023/wooden/2023318/light/" + Item[i].text().mid(0,9);
            QString RePicName = LightPath + Item[i].text().mid(0,9);
            cv::imwrite(RePicName.toLatin1().data(), Pic);

            //调用删除图片函数
            QString DelPicName = Item[i].text().mid(0,9);
            qDebug() << "DelPicName" << DelPicName;
//            deleteImage("D:/2023/wooden/2023318/middle/", DelPicName);
            deleteImage(MiddlePath, DelPicName);

            if (selectedItems[i] != nullptr) {
                delete selectedItems[i];
            }
        }
    }
}

void showpic::on_pushButton_middle_clicked()
{
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();

    int Numbers = selectedItems.size();
    QListWidgetItem* Item = new QListWidgetItem[Numbers];
    for(int i = 0; i < Numbers; i++)
    {
        Item[i] = *selectedItems[i];
        if(Item[i].text().mid(12, 1) == 'Z')
        {
            return;
        }
        if(Item[i].text().mid(12, 1) == 'S')
        {
//            QString PicNamePath = FindImage("D:/2023/wooden/2023318/", Item[i].text().mid(0,9));
            QString PicNamePath = FindImage(DirPath, Item[i].text().mid(0,9));

            cv::Mat Pic = cv::imread(PicNamePath.toLatin1().data());

//            QString RePicName = "D:/2023/wooden/2023318/middle/" + Item[i].text().mid(0,9);
            QString RePicName = MiddlePath + Item[i].text().mid(0,9);

            cv::imwrite(RePicName.toLatin1().data(), Pic);

            //调用删除图片函数
            QString DelPicName = Item[i].text().mid(0,9);
            qDebug() << "DelPicName" << DelPicName;
//            deleteImage("D:/2023/wooden/2023318/dark/", DelPicName);
            deleteImage(DarkPath, DelPicName);

            if (selectedItems[i] != nullptr) {
                delete selectedItems[i];
            }
        }

        if(Item[i].text().mid(12, 1) == 'Q')
        {
//            QString PicNamePath = FindImage("D:/2023/wooden/2023318/", Item[i].text().mid(0,9));
            QString PicNamePath = FindImage(DirPath, Item[i].text().mid(0,9));
            cv::Mat Pic = cv::imread(PicNamePath.toLatin1().data());

//            QString RePicName = "D:/2023/wooden/2023318/middle/" + Item[i].text().mid(0,9);
            QString RePicName = MiddlePath + Item[i].text().mid(0,9);
            cv::imwrite(RePicName.toLatin1().data(), Pic);

            //调用删除图片函数
            QString DelPicName = Item[i].text().mid(0,9);
            qDebug() << "DelPicName" << DelPicName;
//            deleteImage("D:/2023/wooden/2023318/light/", DelPicName);
            deleteImage(LightPath, DelPicName);


            if (selectedItems[i] != nullptr) {
                delete selectedItems[i];
            }
        }
    }
}

void showpic::on_pushButton_dark_clicked()
{
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();

    int Numbers = selectedItems.size();
    QListWidgetItem* Item = new QListWidgetItem[Numbers];
    for(int i = 0; i < Numbers; i++)
    {
        Item[i] = *selectedItems[i];
        if(Item[i].text().mid(12, 1) == 'S')
        {
            return;
        }
        if(Item[i].text().mid(12, 1) == 'Q')
        {
//            QString PicNamePath = FindImage("D:/2023/wooden/2023318/", Item[i].text().mid(0,9));
            QString PicNamePath = FindImage(DirPath, Item[i].text().mid(0,9));

            cv::Mat Pic = cv::imread(PicNamePath.toLatin1().data());

//            QString RePicName = "D:/2023/wooden/2023318/dark/" + Item[i].text().mid(0,9);
            QString RePicName = DarkPath + Item[i].text().mid(0,9);
            cv::imwrite(RePicName.toLatin1().data(), Pic);

            //调用删除图片函数
            QString DelPicName = Item[i].text().mid(0,9);
            qDebug() << "DelPicName" << DelPicName;
//            deleteImage("D:/2023/wooden/2023318/light/", DelPicName);
            deleteImage(LightPath, DelPicName);

            if (selectedItems[i] != nullptr) {
                delete selectedItems[i];
            }
        }

        if(Item[i].text().mid(12, 1) == 'Z')
        {
//            QString PicNamePath = FindImage("D:/2023/wooden/2023318/", Item[i].text().mid(0,9));
            QString PicNamePath = FindImage(DirPath, Item[i].text().mid(0,9));
            cv::Mat Pic = cv::imread(PicNamePath.toLatin1().data());

//            QString RePicName = "D:/2023/wooden/2023318/dark/" + Item[i].text().mid(0,9);
            QString RePicName = DarkPath + Item[i].text().mid(0,9);

            cv::imwrite(RePicName.toLatin1().data(), Pic);

            //调用删除图片函数
            QString DelPicName = Item[i].text().mid(0,9);
            qDebug() << "DelPicName" << DelPicName;
//            deleteImage("D:/2023/wooden/2023318/middle/", DelPicName);
            deleteImage(MiddlePath, DelPicName);


            if (selectedItems[i] != nullptr) {
                delete selectedItems[i];
            }
        }
    }
}

//从文件夹中删除图片
void deleteImage(QString folderPath, QString imageName) {
    QDir folder(folderPath);
    QFileInfoList entries = folder.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for (const QFileInfo &entry : entries) {
        if (entry.isFile() && entry.fileName() == imageName) {
            // 找到了指定图片，删除它
            qDebug() << "entry.absoluteFilePath()" << entry.absoluteFilePath();
            if (QFile::remove(entry.absoluteFilePath())) {
                QMessageBox::information(nullptr, "Success", "Image deleted successfully");
            } else {
//                QMessageBox::warning(nullptr, "Error", "Failed to delete image");
            }
            return;
        } else if (entry.isDir()) {
            // 递归进入子文件夹查找图片
            deleteImage(entry.absoluteFilePath(), imageName);
        }
    }

    // 如果遍历完当前文件夹和子文件夹都没有找到指定图片，则弹出失败提示框
//    QMessageBox::warning(nullptr, "Error", "Failed to find image");
}

QString FindImage(QString folderPath, QString imageName)
{

    QDir folderDir(folderPath);

    // 获取文件夹中的所有文件和文件夹
    QStringList entries = folderDir.entryList();

    // 遍历文件夹中的所有文件和文件夹
    foreach(QString entry, entries)
    {
        QString entryPath = folderPath + "/" + entry;

        // 如果是子文件夹，进入子文件夹查找图片
        if(QFileInfo(entryPath).isDir())
        {
            QDir subFolderDir(entryPath);
            QStringList subEntries = subFolderDir.entryList();

            // 遍历子文件夹中的所有文件
            foreach(QString subEntry, subEntries)
            {
                QString subEntryPath = entryPath + "/" + subEntry;

                // 如果是目标图片，返回其绝对路径
                if(subEntry == imageName)
                {
                    QString imagePath = QFileInfo(subEntryPath).absoluteFilePath();
                    return imagePath;
                }
            }
        }
        // 如果是目标图片，返回其绝对路径
        else if(entry == imageName)
        {
            QString imagePath = QFileInfo(entryPath).absoluteFilePath();
            return imagePath;
        }
    }

    // 如果未找到目标图片，返回空字符串
    return "";
}





