#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QThread>
#include <opencv2/opencv.hpp>
#include "thread.h"
#include <QString>
#include <QFont>
#include <QFileDialog>
//#include <QSerialPort>
#include "src/modbus.h"

# pragma execution_character_set("utf-8")

#define ResultPath "D:/Log.txt"
Process_img * process_img;

modbus_t* plc1;
camera *m_camera = nullptr;
correct *m_correct = nullptr;

extern QSemaphore usebuff;
QSemaphore plc_signal(0);
int save_flag;    //保存标志位

uint8_t learning_flag;
uint8_t light_flag;
uint8_t middle_flag;
uint8_t dark_flag;
int m_index;

uint8_t adjust_exit_flag;
uint8_t adjust_correct_flag;
uint8_t restart_flag;

int a = 0, b = 0, c = 0;

#define SAVE_IMAGE_PATH "E:/527picture/"
#define WHITE_IMAGE_PATH "C:/Users/USER/Desktop/wood_aduiduiduiplus/build-wooden-Desktop_Qt_5_9_5_MSVC2017_64bit-Release/white"
#define BLACK_IMAGE_PATH "C:/Users/USER/Desktop/wood_aduiduiduiplus/build-wooden-Desktop_Qt_5_9_5_MSVC2017_64bit-Release/black"

#define CORRECT_WIDTH    4096
#define CORRECT_HEIGHT   3000
#define Train_number 30

#define changeModel_path "D:/wood_color317/models"

QString DirPath = "D:/picture/527paper";

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{

//    qDebug() << "1111111111111111";
    ui->setupUi(this);
    m_camera = new camera;
    init_window();
    plc_connect();
    ui->btn_Tab2_stop->setEnabled(false);
    save_flag = 0;
    send_thread = new SendThread();
    recv_thread = new RecvThread();
    send_thread->start();
    recv_thread->start();
    connect_signals();
    ui->btn_Tab2_light->setEnabled(false);
    ui->btn_Tab2_middle->setEnabled(false);
    ui->btn_Tab2_dark->setEnabled(false);
    ui->btn_Tab2_train->setEnabled(false);

    ui->btn_Tab2_Preprocessing->setEnabled(false);
    ui->btn_Tab2_show->setEnabled(false);

    learning_flag = 0;
    light_flag = 0;
    middle_flag = 0;
    dark_flag = 0;
    adjust_exit_flag = 0;
    adjust_correct_flag = 0;
    restart_flag = 0;

    restart_thread = new RestartThread();
    restart_thread->start();

    ui->btn_Tab4_black->hide();
    ui->btn_Tab2_start_2->hide();
    ui->btn_Tab2_start_3->hide();
    ui->btn_Tab2_start_4->hide();

    ui->btn_Tab4_wbcorrect->setEnabled(false);
    ui->btn_Tab4_exit->setEnabled(false);

}

Widget::~Widget()
{
    delete ui;
    delete [] m_camera->black_buf;
    delete [] m_camera->white_buf;

}

void Widget::plc_connect()
{
    int status = -1;
    plc1 = modbus_new_rtu("COM7", 115200, 'N', 8, 1);
    modbus_set_slave(plc1, 4); //设置modbus从机地址
    status = modbus_connect(plc1);
    if(status == -1)
    {
        qDebug() << "modbus connect failed";
    }
    modbus_set_response_timeout(plc1, 0, 1000000);
    qDebug() << "status" << status;
    qDebug() << "connect plc success";
}

void Widget::connect_signals()
{
    //Tab_1
    connect(ui->btn_Tab1_1, SIGNAL(clicked()), this, SLOT(btn_Tab1_1_click()));
    connect(ui->btn_Tab1_2, SIGNAL(clicked()), this, SLOT(btn_Tab1_2_click()));
    //Tab_2
    connect(ui->btn_Tab2_mainMenu, SIGNAL(clicked()), this, SLOT(btn_Tab2_mainMenu_click()));
    connect(ui->btn_Tab2_start, SIGNAL(clicked()), this, SLOT(btn_Tab2_start_click()));
    connect(ui->btn_Tab2_stop, SIGNAL(clicked()), this, SLOT(btn_Tab2_stop_click()));

    connect(ui->btn_Tab2_autoLearning, SIGNAL(clicked()), this, SLOT(btn_Tab2_autoLearning_click()));
    connect(ui->btn_Tab2_light, SIGNAL(clicked()), this, SLOT(btn_Tab2_light_click()));
    connect(ui->btn_Tab2_middle, SIGNAL(clicked()), this, SLOT(btn_Tab2_middle_click()));
    connect(ui->btn_Tab2_dark, SIGNAL(clicked()), this, SLOT(btn_Tab2_dark_click()));
    connect(ui->btn_Tab2_train, SIGNAL(clicked()), this, SLOT(btn_Tab2_train_click()));

    //Tab_3
    connect(ui->btn_Tab3_cameraParasettings, SIGNAL(clicked()), this, SLOT(btn_Tab3_cameraParasettings_click()));
    connect(ui->btn_Tab3_mainMenu, SIGNAL(clicked()), this, SLOT(btn_Tab3_mainMenu_click()));
    connect(ui->btn_Tab3_saveImg, SIGNAL(clicked()), this, SLOT(btn_Tab3_saveImg_click()));
    connect(ui->btn_Tab3_changeModel, SIGNAL(clicked()), this, SLOT(btn_Tab3_changeModel_click()));
    //Tab_4
    connect(ui->btn_Tab4_return, SIGNAL(clicked()), this, SLOT(btn_Tab4_return_click()));
    connect(ui->btn_Tab4_getCorrect, SIGNAL(clicked()), this, SLOT(btn_Tab4_getCorrect_click()));

    connect(ui->btn_Tab4_black, SIGNAL(clicked()), this, SLOT(btn_Tab4_black_click()));
    connect(ui->btn_Tab4_white, SIGNAL(clicked()), this, SLOT(btn_Tab4_white_click()));
    connect(ui->btn_Tab4_wbcorrect, SIGNAL(clicked()), this, SLOT(btn_Tab4_wbcorrect_click()));

    //Tab_5
    connect(ui->btn_Tab5_skip, SIGNAL(clicked()), this, SLOT(btn_Tab5_skip_click()));



    connect(recv_thread, SIGNAL(SendPicInfo(char*)), this, SLOT(showPic(char*)), Qt::BlockingQueuedConnection);

}

void Widget::init_window()
{
    ui->tabWidget->findChildren<QTabBar*>().at(0)->hide();
    ui->btn_Tab2_dark->hide();
    ui->tabWidget->setCurrentIndex(4);

    if(!m_camera->openCamera())
    {
        qDebug() << "open camera failed";
        return;
    }

    //校正
    m_camera->white_mat = cv::Mat(CORRECT_HEIGHT, CORRECT_WIDTH, CV_32FC3, cv::Scalar(0, 0, 0));
    m_camera->black_mat = cv::Mat(CORRECT_HEIGHT, CORRECT_WIDTH, CV_32FC3, cv::Scalar(0, 0, 0));
    m_camera->eps = cv::Mat(CORRECT_HEIGHT, CORRECT_WIDTH, CV_32FC3, cv::Scalar(0.00000001, 0.00000001, 0.00000001));
    m_camera->white_buf = new float[CORRECT_WIDTH*CORRECT_HEIGHT*3*4];
    m_camera->black_buf = new float[CORRECT_WIDTH*CORRECT_HEIGHT*3*4];

    FILE* fp;
    fp = fopen(WHITE_IMAGE_PATH, "rb");
    fread(m_camera->white_buf, 4, CORRECT_WIDTH*CORRECT_HEIGHT*3*4, fp);
    fclose(fp);
    fp = fopen(BLACK_IMAGE_PATH, "rb");
    fread(m_camera->black_buf, 4, CORRECT_WIDTH*CORRECT_HEIGHT*3*4, fp);
    fclose(fp);

    m_camera->white_mat = cv::Mat(CORRECT_HEIGHT, CORRECT_WIDTH, CV_32FC3, m_camera->white_buf);
    m_camera->black_mat = cv::Mat(CORRECT_HEIGHT, CORRECT_WIDTH, CV_32FC3, m_camera->black_buf);

    skip = new QTimer();
    connect(skip, SIGNAL(timeout()), this, SLOT(skip_Notime()));
    skip->start(30000);

}

//Tab1
void Widget::btn_Tab1_1_click()
{
    ui->tabWidget->setCurrentIndex(1);
}

void Widget::btn_Tab1_2_click()
{
    ui->tabWidget->setCurrentIndex(2);
}

//Tab2
void Widget::showlabel(cv::Mat img)
{
//    qDebug() << count++;
//    qDebug() << "save_flag: " << save_flag;
    int counter = 0;
    counter++;
//    qDebug() << "counter: " << counter;
//    if(save_flag && counter == 1)
//    {
////        QDateTime time = QDateTime::currentDateTime();
////        QString str = time.toString("yyyyMMddhhmmss");
////        QString filepath = SAVE_IMAGE_PATH + str + ".png";
////        qDebug() << filepath;
////        cv::imwrite(filepath.toLatin1().data(), img);

//        char fileName[64];
//        sprintf(fileName, "D:/save_image/rgb%d.png", m_index);

//        m_index++;
//        qDebug() << fileName;
//        cv::imwrite(fileName, img);
//        counter = 0;

//    }

    static uint8_t light_index = 0;
    if(learning_flag && counter == 1 && light_flag)
    {
        qDebug() << "learning light saved";
        qDebug() << "light_index: " << light_index;
        char str[32];

        //使用sprintf补0，QString使用.toLatin1().data()改变Path的格式
        char file[16];
        sprintf(file, "rgb%02d.png", light_index);//定义图片保存名file
        QString Name = QString(QLatin1String(file));
        QString fileName = DirPath + "//hua//" + Name;//形如D://2023//wooden//2023318//light//rgb00.png

        sprintf(str, "light number:%d", ++light_index);
        if(light_index == Train_number)
        {
            cv::imwrite(fileName.toLatin1().data(), img);
            ui->label_showSorting->setText("花纹木板采集完成");
            m_camera->stopCapture();
            ui->btn_Tab2_middle->setEnabled(true);
            light_flag = 0;
        }
        else
        {
            QString s = QString(str);
            ui->label_showSorting->setText(s);
            qDebug() << fileName;
            cv::imwrite(fileName.toLatin1().data(), img);
        }
        counter = 0;
    }

    static uint8_t middle_index = 0;
    if(learning_flag && counter == 1 && middle_flag)
    {
        qDebug() << "learning middle saved";
        qDebug() << "middle_index: " << middle_index;
        char str[32];
        char file[16];
        sprintf(file, "rgb%02d.png", middle_index + 30);//定义图片保存名file
        QString Name = QString(QLatin1String(file));
        QString fileName = DirPath + "//zhi//" + Name;

//        sprintf(fileName, "E:/autoLearning/middle/rgb%02d.png", middle_index);
        sprintf(str, "middle number:%d", ++middle_index);
        if(middle_index == Train_number)
        {
            cv::imwrite(fileName.toLatin1().data(), img);
            ui->label_showSorting->setText("直纹木板采集完成");
            m_camera->stopCapture();
            ui->btn_Tab2_dark->setEnabled(true);
            middle_flag = 0;
        }
        else
        {
            QString s = QString(str);
            ui->label_showSorting->setText(s);
            qDebug() << fileName;
            cv::imwrite(fileName.toLatin1().data(), img);
        }
        counter = 0;
    }

//    static uint8_t dark_index = 0;
//    if(learning_flag && counter == 1 && dark_flag)
//    {
//        qDebug() << "learning dark saved";
//        qDebug() << "dark_index: " << dark_index;
//        char str[32];
//        char file[16];
//        sprintf(file, "rgb%02d.png", dark_index + 60);//定义图片保存名file
//        QString Name = QString(QLatin1String(file));
//        QString fileName = DirPath + "//dark//" + Name;
////        sprintf(fileName, "E:/autoLearning/dark/rgb%02d.png", dark_index);
//        sprintf(str, "dark number:%d", ++dark_index);
//        if(dark_index == Train_number)
//        {
//            cv::imwrite(fileName.toLatin1().data(), img);
//            ui->label_showSorting->setText("深色木板采集完成");
//            m_camera->stopCapture();
//            ui->btn_Tab2_Preprocessing->setEnabled(true);
//            dark_flag = 0;
//        }
//        else
//        {
//            QString s = QString(str);
//            ui->label_showSorting->setText(s);
//            qDebug() << fileName;
//            cv::imwrite(fileName.toLatin1().data(), img);
//        }
//        counter = 0;
//    }

//    char fileName[64];
//    sprintf(fileName, "E:/88/rgb%d.png", m_index++);
//    cv::imwrite(fileName, img);

    cv::cvtColor(img, img, CV_BGR2RGB);
    const unsigned char *pSrc = (const unsigned char*)img.data;
    QImage image(pSrc, img.cols, img.rows, img.step, QImage::Format_RGB888);
    pix = QPixmap::fromImage(image.scaled(ui->showlabel->width(), ui->showlabel->height(), Qt::KeepAspectRatio));
    ui->showlabel->setPixmap(pix);
    ui->showlabel->show();
//    Sleep(150);
    m_camera->m_buffer->Clear();
}

void Widget::btn_Tab2_start_click()
{

    learning_flag = 0;
//    m_camera->m_pAcq->SetParameter(CORACQ_PRM_FRAME_LENGTH, CORACQ_VAL_FRAME_LENGTH_VARIABLE, TRUE);
    process_img = new Process_img(this);
    connect(process_img, SIGNAL(show_img(cv::Mat)), this, SLOT(showlabel(cv::Mat)), Qt::BlockingQueuedConnection);
    connect(recv_thread, SIGNAL(send_sortingResult(char*)), this, SLOT(showlabelSorting(char*)), Qt::BlockingQueuedConnection);
//    connect(recv_thread, SIGNAL(SendPicInfo(char*)), this, SLOT(showPic(char*)), Qt::BlockingQueuedConnection);

    m_camera->startCapture();
    process_img->start();

    ui->btn_Tab2_start->setEnabled(false);
    ui->btn_Tab2_stop->setEnabled(true);
//打印分选时间戳
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString tmp1 =curDateTime.toString("yyyy-MM-dd hh:mm:ss");
    QByteArray tmp2 = tmp1.toLatin1();
    tmp2.append(QString::fromUtf8("开始分选\n"));
    int len = tmp2.size();
    char* ch = tmp2.data();
    FILE* fp;
    fp = fopen(ResultPath, "a+");
    fwrite(ch, len, 1, fp);

    fclose(fp);
    ch = NULL;

}

void Widget::btn_Tab2_stop_click()
{
    qDebug() << "S: " << a;
    qDebug() << "Z: " << b;
    qDebug() << "Q: " << c;

/***************打印时间戳分选结果(形如Q:0 Z:0 S:0 sum:0)*************/
    FILE* fp;
    fp = fopen(ResultPath, "a+");
    QString Result = "Q:" + QString::number(c) + " Z:" + QString::number(b) + " S:" + QString::number(a) + " sum:" + QString::number(count);
    char* ch = Result.toLatin1().data();
    int len = Result.size();
    fwrite(ch, len, 1, fp);

    QDateTime curDateTime = QDateTime::currentDateTime();
    QString tmp1 =curDateTime.toString("\nyyyy-MM-dd hh:mm:ss");
    QByteArray tmp2 = tmp1.toLatin1();
    tmp2.append(QString::fromUtf8("结束分选\n\n"));
     len = tmp2.size();
    ch = tmp2.data();

    fwrite(ch, len, 1, fp);

    fclose(fp);
    ch = NULL;
/***     ****        *****     ****         ***/
    disconnect(process_img, SIGNAL(show_img(cv::Mat)), this, SLOT(showlabel(cv::Mat)));
    disconnect(recv_thread, SIGNAL(send_sortingResult(char*)), this, SLOT(showlabelSorting(char*)));
    qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    m_camera->stopCapture();
//    a->closeCamera();
    process_img->exitThread();
    process_img->wait();
    if(process_img->isFinished())
        qDebug() <<"process thread finish";
    delete process_img;

    ui->btn_Tab2_start->setEnabled(true);
    ui->btn_Tab2_stop->setEnabled(false);

    a = 0;
    b = 0;
    c = 0;
    count = 0;
}

void Widget::btn_Tab2_mainMenu_click()
{
    ui->tabWidget->setCurrentIndex(0);
}

void Widget::showlabelSorting(char *buf)
{    

    uint16_t tmp = 0;
//    qDebug() << "AAAAAAAAAAA";
    switch(buf[0])
    {
    case 'H':
        ++a;
//        qDebug() << "S:" << a;
        tmp = 0;
        ui->label_showSorting->setText(QString::fromStdString("花纹"));
        if(modbus_write_registers(plc1, 0x400002, 1, &tmp) == -1)
            qDebug() << "dark write error";
        qDebug() << "识别结果:" << tmp;
        Sleep(10);
        tmp = 1;
        if(modbus_write_registers(plc1, 0x400001, 1, &tmp) == -1)
            qDebug() << "R1 error";
        break;

    case 'Z':
        ++b;
//        qDebug() << "Z:" << b;
        tmp = 1;
        ui->label_showSorting->setText(QString::fromStdString("直纹"));
        if(modbus_write_registers(plc1, 0x400002, 1, &tmp) == -1)
            qDebug() << "middle write error";
        qDebug() << "识别结果:" << tmp;
        Sleep(10);
        tmp = 1;
        if(modbus_write_registers(plc1, 0x400001, 1, &tmp) == -1)
            qDebug() << "R1 error";
        break;

//    case 'Q':
//        ++c;
////        qDebug() << "Q:" << c;
//        tmp = 2;
//        ui->label_showSorting->setText(QString::fromStdString("浅色 "));
//        if(modbus_write_registers(plc1, 0x400002, 1, &tmp) == -1)
//            qDebug() << "light write error";
//        qDebug() << "识别结果:" << tmp;
//        Sleep(10);
//        tmp = 1;
//        if(modbus_write_registers(plc1, 0x400001, 1, &tmp) == -1)
//            qDebug() << "R1 error";
//        break;

    case 'A':
        ui->label_showSorting->setText(QString::fromStdString("训练模型完成"));
        break;

    case 'D':
        ui->label_showSorting->setText(QString::fromStdString("切换模型完成"));
        break;

    default:
        break;
    }

}



void Widget::btn_Tab2_autoLearning_click()
{
    //创建存图文件夹
    QString DirName = QInputDialog::getText(this,"存图路径","请输入文件夹名称（英文与数字）：",QLineEdit::Normal);
    if(DirName.size() == 0)
    {
        QMessageBox::warning(NULL, "警告", "输入图片文件夹名为空,请重新操作！");
        return;
    }
    //创建文件夹
    DirPath = "D:/picture/" + DirName;//DirPath设置全局变量523
    QDir dir(DirPath);
    if (!dir.exists())
    {
        dir.mkdir(DirPath);
    }
    else
    {
        QMessageBox::warning(NULL, "警告", "输入图片文件夹名重复,请重新操作！");
        return;
    }

    ui->btn_Tab2_light->setEnabled(true);
    learning_flag = 1;
    ui->label_showSorting->setText(QString::fromStdString("请点击保存浅色 "));
}

void Widget::btn_Tab2_light_click()
{
    QString DirPathLight = DirPath + "/hua";
    QDir dir(DirPathLight);
    if (!dir.exists())
    {
        dir.mkdir(DirPathLight);
    }

    light_flag = 1;
    ui->label_showSorting->setText(QString::fromStdString("请放花纹木板"));
    process_img = new Process_img(this);
    connect(process_img, SIGNAL(show_img(cv::Mat)), this, SLOT(showlabel(cv::Mat)), Qt::BlockingQueuedConnection);

    m_camera->startCapture();
    process_img->start();
    ui->btn_Tab2_light->setEnabled(false);

}

void Widget::btn_Tab2_middle_click()
{
    QString DirPathMiddle = DirPath + "/zhi";
    QDir dir(DirPathMiddle);
    if (!dir.exists())
    {
        dir.mkdir(DirPathMiddle);
    }
    ui->label_showSorting->setText(QString::fromStdString("请放直纹木板"));
    middle_flag = 1;
    m_camera->startCapture();

    ui->btn_Tab2_middle->setEnabled(false);

}

void Widget::btn_Tab2_dark_click()
{
    QString DirPathDark = DirPath + "/dark";
    QDir dir(DirPathDark);

    if (!dir.exists())
    {
        dir.mkdir(DirPathDark);
    }

    dark_flag = 1;
    ui->label_showSorting->setText(QString::fromStdString("请放深色木板"));

    m_camera->startCapture();

    ui->btn_Tab2_dark->setEnabled(false);

}


void Widget::on_btn_Tab2_Preprocessing_clicked()
{

    char fileName[64];

//    QString DirPathSolid = "D:/picture/524test/";
//    sprintf(fileName, DirPathSolid.toLatin1().data());

    sprintf(fileName, DirPath.toLatin1().data());


    uint8_t* sendbuf = new uint8_t[strlen(fileName)+12];
    sendbuf[0] = 0xAA;
    sendbuf[1] = 0x00;
    sendbuf[2] = 0x00;
    sendbuf[3] = 0x00;
    sendbuf[4] = strlen(fileName)+4;
    sendbuf[5] = ' ';
    sendbuf[6] = ' ';
    sendbuf[7] = 'K';
    sendbuf[8] = 'M';
    memcpy(sendbuf+9, fileName, strlen(fileName));
    sendbuf[strlen(fileName) + 9] = 0xFF;
    sendbuf[strlen(fileName) + 10] = 0xFF;
    sendbuf[strlen(fileName) + 11] = 0xBB;

    send_thread->send_socket->write((const char*)sendbuf, strlen(fileName) + 12);
    send_thread->send_socket->flush();
    qDebug() << "====send DirName success====";


}

void Widget::showPic(char* buf1)
{
    qDebug() << "111";
    pic = new showpic(buf1);

    connect(pic, SIGNAL(PictureReady()), this, SLOT(PictureProcess()));

    ui->btn_Tab2_show->setEnabled(true);
    ui->btn_Tab2_Preprocessing->setEnabled(false);
}

void Widget::on_btn_Tab2_show_clicked()
{
    pic->show();
    //522
    ui->btn_Tab2_show->setEnabled(false);
    ui->btn_Tab2_train->setEnabled(true);
}


void Widget::btn_Tab2_train_click()
{
    ui->btn_Tab2_train->setEnabled(false);
    disconnect(process_img, SIGNAL(show_img(cv::Mat)), this, SLOT(showlabel(cv::Mat)));
    process_img->exitThread();
    process_img->wait();
    if(process_img->isFinished())
        qDebug() <<"process thread finish";
    delete process_img;

    char fileName[64];
    sprintf(fileName, DirPath.toLatin1().data());

//    uint8_t* sendbuf = new uint8_t[strlen(fileName)+12];
////    uint8_t sendbuf[strlen(fileName) + 12];
//    sendbuf[0] = 0xAA;
//    sendbuf[1] = 0x00;
//    sendbuf[2] = 0x00;
//    sendbuf[3] = 0x00;
//    sendbuf[4] = 0x13;
//    sendbuf[5] = ' ';
//    sendbuf[6] = ' ';
//    sendbuf[7] = 'T';
//    sendbuf[8] = 'R';
//    memcpy(sendbuf+9, fileName, 15);
//    sendbuf[strlen(fileName) + 9] = 0xFF;
//    sendbuf[strlen(fileName) + 10] = 0xFF;
//    sendbuf[strlen(fileName) + 11] = 0xBB;

//    send_thread->send_socket->write((const char*)sendbuf, strlen(fileName) + 12);
//    send_thread->send_socket->flush();
//    qDebug() << "====send fileName success====";


    //先定义 标题，提示框
    QString ModelName = QInputDialog::getText(this,"命名","请给本次训练的模型命名：",QLineEdit::Normal);
    if(ModelName.size() == 0)
    {
        QMessageBox::warning(NULL, "警告", "输入模型名为空,请重新操作！");
        return;
    }

    QString cut = "$";
    QString Dir_ModelName = DirPath + cut + ModelName;

    qDebug() << "Dir+DirName" << Dir_ModelName;
    int len = Dir_ModelName.length();
    uint8_t* assign_buf = new uint8_t[len+12];
    char* ba = Dir_ModelName.toLatin1().data();

    if(len > 0)
    {
        assign_buf[0] = 0xAA;
        assign_buf[1] = 0x00;
        assign_buf[2] = 0x00;
        assign_buf[3] = 0x00;
        assign_buf[4] = len+4;
        assign_buf[5] = ' ';
        assign_buf[6] = ' ';
        assign_buf[7] = 'T';
        assign_buf[8] = 'R';
        memcpy(assign_buf+9, ba, len);
        assign_buf[len+9] = 0xFF;
        assign_buf[len+10] = 0xFF;
        assign_buf[len+11] = 0xBB;
        send_thread->send_socket->write((const char*)assign_buf, len+12);
        send_thread->send_socket->flush();
        qDebug() << "====send fileName success====";
    }
    else
    {
        return;
    }
}



//Tab3
void Widget::btn_Tab3_cameraParasettings_click()
{
    ui->tabWidget->setCurrentIndex(3);
    //设置相机参数
    //设置相机采集模式

}

void Widget::btn_Tab3_mainMenu_click()
{
    ui->tabWidget->setCurrentIndex(0);
}

void Widget::showimage_test(cv::Mat img)
{
//    cv::cvtColor(img, img, CV_BGRA2BGR);
//    cv::Mat img_gray;
//    cv::cvtColor(img, img_gray, CV_RGB2BGRA);

    cv::cvtColor(img, img, CV_BGR2RGB);
    int value = 0;
    for(int i=300; i<400; i++)
    {
        for(int j=300; j<400; j++)
        {
            value += img.ptr<uchar>(i)[j];
        }
    }
    float bringhtness = value / 10000.0;
    ui->label_Tab4_2->setText(QString("%1").arg(bringhtness));
    const unsigned char *pSrc = (const unsigned char*)img.data;
    QImage image(pSrc, img.cols, img.rows, img.step, QImage::Format_RGB888);
    pix_test = QPixmap::fromImage(image.scaled(ui->showlabel_set->width(), ui->showlabel_set->height(), Qt::KeepAspectRatio));
    ui->showlabel_set->setPixmap(pix_test);
    ui->showlabel_set->show();

}

void Widget::btn_Tab3_saveImg_click()
{
    if(save_flag == 1)
    {
        save_flag = 0;
        ui->btn_Tab3_saveImg->setText(QString::fromStdString("保存图片 "));
    }
    else
    {
        save_flag = 1;
        ui->btn_Tab3_saveImg->setText(QString::fromStdString(" 不要保存图片 "));
    }
}

void Widget::btn_Tab3_changeModel_click()
{
    QString fileName = QFileDialog::getOpenFileName(NULL, "切换模型", changeModel_path, "*.p");
    int len = fileName.length();
    uint8_t* assign_buf = new uint8_t[len+12];
    char* ba = fileName.toLatin1().data();
    if(len > 0)
    {
        assign_buf[0] = 0xAA;
        assign_buf[1] = 0x00;
        assign_buf[2] = 0x00;
        assign_buf[3] = 0x00;
        assign_buf[4] = len+4;
        assign_buf[5] = ' ';
        assign_buf[6] = ' ';
        assign_buf[7] = 'M';
        assign_buf[8] = 'D';
        memcpy(assign_buf+9, ba, len);
        assign_buf[len+9] = 0xFF;
        assign_buf[len+10] = 0xFF;
        assign_buf[len+11] = 0xBB;
        send_thread->send_socket->write((const char*)assign_buf, len+12);
        send_thread->send_socket->flush();
    }
    else
    {
        return;
    }
}

//Tab4
void Widget::btn_Tab4_return_click()
{
    if(restart_flag)
    {

        ui->btn_Tab4_return->setEnabled(false);
        restart_thread->restr_fun();
        restart_flag = 0;

        connect(dafei, SIGNAL(timeout()), this, SLOT(dafei_slot()));
        ui->btn_Tab4_return->setText("参数设置");
        dafei->start(20000);
        ui->btn_Tab4_return->setEnabled(true);
    }
    else
    {
        ui->tabWidget->setCurrentIndex(2);

    }

}

void Widget::dafei_slot()
{
    ui->btn_Tab4_white->setEnabled(true);
    ui->btn_Tab4_getCorrect->setEnabled(true);
    dafei->stop();
    ui->btn_Tab4_return->setText("返回");
    ui->tabWidget->setCurrentIndex(2);
}

void Widget::btn_Tab4_getCorrect_click()
{
    m_camera->m_pAcq->SetParameter(CORACQ_PRM_EXT_FRAME_TRIGGER_ENABLE, FALSE, TRUE);
    m_camera->m_pAcq->SetParameter(CORACQ_PRM_FRAME_LENGTH, CORACQ_VAL_FRAME_LENGTH_FIX, TRUE);
    adjust_exit_flag = 1;
    adjust_correct_flag = 1;
    adjust_para = new Adjust_para(this);
    connect(adjust_para, SIGNAL(send_image_debug(cv::Mat)), this, SLOT(showimage_test(cv::Mat)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(correct_signal()), adjust_para, SLOT(get_correct_signal()));
    m_camera->startCapture();
    adjust_para->start();
    qDebug() << "get correct";
//    emit correct_signal();

    ui->btn_Tab4_white->setEnabled(false);
    ui->btn_Tab4_exit->setEnabled(true);
    ui->btn_Tab4_return->setEnabled(false);

}

void Widget::on_btn_Tab4_exit_clicked()
{
    disconnect(adjust_para, SIGNAL(send_image_debug(cv::Mat)), this, SLOT(showimage_test(cv::Mat)));
    disconnect(this, SIGNAL(correct_signal()), adjust_para, SLOT(get_correct_signal()));
    m_camera->stopCapture();

    adjust_para->exitThread();
    adjust_para->wait();
    if(adjust_para->isFinished())
    {
        qDebug() << "dajust_para thread finish";
    }
    delete adjust_para;

    adjust_exit_flag = 0;
    adjust_correct_flag = 0;
    //更改相机触发模式为外触发
    if(m_camera->m_pAcq->SetParameter(CORACQ_PRM_FRAME_LENGTH, 0x00000002, FALSE))
        qDebug() << "set success";
    else
        qDebug() << "set error";
    m_camera->m_pAcq->SetParameter(CORACQ_PRM_EXT_FRAME_TRIGGER_ENABLE, TRUE, TRUE);

    if(m_camera->m_buffer && *m_camera->m_buffer)
    {
        m_camera->m_buffer->Destroy();
        qDebug() << "1111";
    }

    if(m_camera->m_pAcq && *m_camera->m_pAcq)
    {
        m_camera->m_pAcq->Destroy();
        qDebug() << "2222";
    }

    if(m_camera->m_trans)
    {
        delete m_camera->m_trans;
        qDebug() << "3333";
    }

    if(m_camera->m_buffer)
    {
        delete m_camera->m_buffer;
        qDebug() << "4444";
    }

    if(m_camera->m_pAcq)
    {
        delete m_camera->m_pAcq;
        qDebug() << "5555";
    }

    if(m_camera->m_loc)
    {
        delete m_camera->m_loc;
        qDebug() << "6666";
    }

    restart_flag = 1;
    ui->btn_Tab4_return->setEnabled(true);

}

void Widget::PictureProcess()
{
    ui->label_showSorting->setText(QString::fromStdString("图片预处理完成"));
}

void Widget::btn_Tab4_black_click()
{
    qDebug() << "black start";
//    ui->btn_Tab4_black->setDisabled(true);
//    ui->btn_Tab4_white->setDisabled(true);

    //加锁
    m_camera->capture_black_flag = true;
//    m_camera->ret_SCorrect = m_camera->startCorrect();
    qDebug() << "black end";
}


void Widget::btn_Tab4_white_click()
{
    m_camera->m_pAcq->SetParameter(CORACQ_PRM_EXT_FRAME_TRIGGER_ENABLE, FALSE, TRUE);
    m_camera->m_pAcq->SetParameter(CORACQ_PRM_FRAME_LENGTH, CORACQ_VAL_FRAME_LENGTH_FIX, TRUE);
    m_camera->capture_white_flag = true;
    m_camera->startCorrect();

    ui->btn_Tab4_getCorrect->setEnabled(false);
    ui->btn_Tab4_wbcorrect->setEnabled(true);
    ui->btn_Tab4_return->setEnabled(false);

}


void Widget::btn_Tab4_wbcorrect_click()
{
    m_camera->stopCorrect();
    //更改相机触发模式为外触发
    m_camera->m_pAcq->SetParameter(CORACQ_PRM_FRAME_LENGTH, CORACQ_VAL_FRAME_LENGTH_VARIABLE, TRUE);
    m_camera->m_pAcq->SetParameter(CORACQ_PRM_EXT_FRAME_TRIGGER_ENABLE, TRUE, TRUE);


    FILE* fp;
    fp = fopen(WHITE_IMAGE_PATH, "rb");
    fread(m_camera->white_buf, 4, CORRECT_WIDTH*CORRECT_HEIGHT*3*4, fp);
    fclose(fp);
    fp = fopen(BLACK_IMAGE_PATH, "rb");
    fread(m_camera->black_buf, 4, CORRECT_WIDTH*CORRECT_HEIGHT*3*4, fp);
    fclose(fp);

    m_camera->white_mat = cv::Mat(CORRECT_HEIGHT, CORRECT_WIDTH, CV_32FC3, m_camera->white_buf);
    m_camera->black_mat = cv::Mat(CORRECT_HEIGHT, CORRECT_WIDTH, CV_32FC3, m_camera->black_buf);

    if(m_camera->m_buffer && *m_camera->m_buffer)
    {
        m_camera->m_buffer->Destroy();
        qDebug() << "aaaa";
    }

    if(m_camera->m_pAcq && *m_camera->m_pAcq)
    {
        m_camera->m_pAcq->Destroy();
        qDebug() << "bbbb";
    }

    if(m_camera->c_trans)
    {
        delete m_camera->c_trans;
        qDebug() << "cccc";
    }

    if(m_camera->m_buffer)
    {
        delete m_camera->m_buffer;
        qDebug() << "dddd";
    }

    if(m_camera->m_pAcq)
    {
        delete m_camera->m_pAcq;
        qDebug() << "eeee";
    }

    if(m_camera->m_loc)
    {
        delete m_camera->m_loc;
        qDebug() << "ffff";
    }

    restart_flag = 1;
    ui->btn_Tab4_return->setEnabled(true);

}

void Widget::on_btn_Tab2_start_2_clicked()
{
//    write_modbus(0x400001, 2);
//    write_modbus(0x400002, 2);
    char fileName[64];
    sprintf(fileName, "E:/autoLearning");
    int len = strlen(fileName);
    qDebug() << len;
}

void Widget::on_btn_Tab2_start_3_clicked()
{
//    write_modbus(0x400001, 1);
//    uint16_t tmp = 900;
//    int ret = modbus_write_registers(plc1, 0x400002, 1, &tmp);
//    if(ret == -1)
//        qDebug() << "write error";
//    else
//        qDebug() << "write success";

    uint16_t tmp = 0;
    uint16_t arr[9] = {0x400001, 0x400002, 0x400003, 0x400004, 0x400005, 0x400006, 0x400007, 0x400008, 0x400009};
    for(int i = 0; i < 9; i++)
    {
        tmp = i;
        int ret = modbus_write_registers(plc1, arr[i], 1, &tmp);
        if(ret == -1)
            qDebug() << "write error";
        else
            qDebug() << "write success";
        Sleep(100);
    }

}

void Widget::on_btn_Tab2_start_4_clicked()
{
//    write_modbus(0x400001, 0);
    uint16_t tmp = 2;
    int ret = modbus_read_registers(plc1, 0x400002, 1, &tmp);
    if(ret == -1)
        qDebug() << "write error";
    else
        qDebug() << "read success: " <<tmp;
}

//Tab5
void Widget::btn_Tab5_skip_click()
{
    ui->tabWidget->setCurrentIndex(0);
    skip->stop();
    disconnect(skip, SIGNAL(timeout()), this, SLOT(skip_Notime()));
    delete skip;
}

void Widget::skip_Notime()
{
    ui->tabWidget->setCurrentIndex(0);
    disconnect(skip, SIGNAL(timeout()), this, SLOT(skip_Notime()));
    delete skip;
}





