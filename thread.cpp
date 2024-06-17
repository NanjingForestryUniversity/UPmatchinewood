
#include "thread.h"
#include <QModbusRtuSerialMaster>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QVariant>
#include <QSerialPort>

int index = 0;
extern camera *m_camera;
extern correct *m_correct;

extern uint8_t learning_flag;
extern Process_img * process_img;
cv::Mat mat_1;
cv::Mat mat_2;
cv::Mat img_pop;
extern int save_flag;
int m_index1;
QSemaphore usebuff(0);
QSemaphore read_to_send(0);
//unsigned char *send_buf;
char *recv_buf;

extern uint8_t adjust_correct_flag;

#define WHITE_IMAGE_PATH "C:/Users/USER/Desktop/wood_aduiduiduiplus/build-wooden-Desktop_Qt_5_9_5_MSVC2017_64bit-Release/white"
#define BLACK_IMAGE_PATH "C:/Users/USER/Desktop/wood_aduiduiduiplus/build-wooden-Desktop_Qt_5_9_5_MSVC2017_64bit-Release/black"
#define correct_path "D:/correct.png"
int get_height(cv::Mat img)
{
    int b_temp = 0;
    float b_avg = 0.0;
    int i, j;
    for(i = 0; i < img.rows; i++)
    {
        for(j = 0; j < img.cols; j++)
        {
            b_temp += (int)img.at<cv::Vec3b>(i, j)[0];
        }
        b_avg = b_temp / 4096.0f;
        if(b_avg == 0)
        {
//            qDebug() << i;
            return i;
        }
        b_temp = 0;
    }
    return -1;
}

//采集
void onImageDataCallback(SapXferCallbackInfo *pInfo)
{
    qDebug() << "enter to calback" ;
    if(pInfo)
    {

        unsigned char *pData;
        int width = m_camera->m_buffer->GetWidth();
        int height = m_camera->m_buffer->GetHeight();
         qDebug() << "enter to calback: limian";

        m_camera->m_buffer->GetAddress((void**)&pData);
        cv::Mat img(height, width, CV_8UC3, pData);
        img.convertTo(img, CV_32FC3);
        m_camera->calibrated_img = (img - m_camera->black_mat) / (m_camera->white_mat - m_camera->black_mat + m_camera->eps);
        m_camera->calibrated_img *= 255;
        m_camera->calibrated_img.convertTo(m_camera->calibrated_img, CV_8UC3);

        m_camera->rgb_height = get_height(img);
//        qDebug() << "rgb_height: " << rgb_height;

        if(m_camera->rgb_height < 3000 && m_camera->rgb_height > 500 && !adjust_correct_flag)
        {

             m_camera->imgqueue.push(m_camera->calibrated_img);

//            m_camera->imgqueue.push(mat_2);
//            emit process_img->show_img(mat_1);
             //            usebuff.release();
             //            if(!learning_flag)
             //                read_to_send.release();
        }
        else if(adjust_correct_flag)
        {
            img.convertTo(img, CV_8UC3);
            mat_1 = img;
            usebuff.release();
        }
        else if(m_camera->rgb_height == 0 || m_camera->rgb_height == -1)
        {

            m_camera->m_buffer->Clear();
            return;
        }
    }
}

//校正
void onImageCorrectCallback(SapXferCallbackInfo *pInfo)
{
    qDebug() <<"11111111222222222333333333";
    qDebug() << " onImageCorrectCallback start ";
    //采集黑白帧
    if(pInfo)
    {
        qDebug() << "pInfo";
        if( m_camera->capture_black_flag || m_camera->capture_white_flag )
        {
            qDebug() << "flag";

            unsigned char pdata_rgb;
            void* pDataAddr_rgb = &pdata_rgb;
            m_camera->m_buffer->GetAddress(&pDataAddr_rgb);
            cv::Mat img(4096, 3000, CV_8UC3,pDataAddr_rgb);
            img.convertTo(img, CV_32FC3);
            if( m_camera->capture_black_flag )
            {
                qDebug() << "capture_black_flag";
                m_camera->capture_black_flag = false;
                m_camera->black_mat = img;
                qDebug() << "img";
                FILE* fp = fopen(BLACK_IMAGE_PATH, "wb");
                fwrite(m_camera->black_mat.data, 4096*3000*3*4, 1, fp);
                fclose(fp);
                qDebug() << "black frame acquisition OK!";
            }
            else if( m_camera->capture_white_flag )
            {
                m_camera->capture_white_flag = false;
                m_camera->white_mat = img;
                FILE* fp = fopen(WHITE_IMAGE_PATH, "wb");
                fwrite(m_camera->white_mat.data, 4096*3000*3*4, 1, fp);
                fclose(fp);
                qDebug() << " white frame acquisition OK!";
            }
        }
    }

    //    Widget::getInstance()->set();
}

Process_img::Process_img(QObject *parent) : QThread(parent), m_stop(false)
{

}

void Process_img::run()
{
    qDebug() << "process_imgID: " << currentThreadId();

    while(1)
    {       
        stop_mutex.lock();
        if(m_stop)
        {
            stop_mutex.unlock();
            qDebug() << "process thread quit______________________";
            return;
        }
        stop_mutex.unlock();

        if(m_camera->imgqueue.size()>1)
        {


            //            read_to_send.acquire();  //没有mat_2就阻塞

            m_camera->imgqueue.pop();
            qDebug()<<"the wood image's queue size: "<<m_camera->imgqueue.size();
            img_pop = m_camera->imgqueue.front();
            img_pop = m_camera->calibrated_img(cv::Rect(0, 0, 4096, m_camera->rgb_height)); //4096

            int save_count = 0;
            save_count++;

            if(save_flag && save_count == 1)
            {
                 qDebug() << "1111111111111";
        //        QDateTime time = QDateTime::currentDateTime();
        //        QString str = time.toString("yyyyMMddhhmmss");
        //        QString filepath = SAVE_IMAGE_PATH + str + ".png";
        //        qDebug() << filepath;
        //        cv::imwrite(filepath.toLatin1().data(), img);

                char fileName[64];
                sprintf(fileName, "D:/save_image/rgb%d.bmp", m_index1);

                m_index1++;
                qDebug() << fileName;
                cv::imwrite(fileName, img_pop);
                save_count = 0;

            }

            emit process_img->show_img(img_pop);
            usebuff.release();
            if(!learning_flag)
                read_to_send.release();

        }

//        if(!usebuff.tryAcquire())
//            continue;

    }

}

void Process_img::exitThread()
{
    stop_mutex.lock();
    m_stop = true;
    stop_mutex.unlock();
}


Adjust_para::Adjust_para(QObject *parent) : QThread(parent), is_stop(false), correct_flag(false)
{

}

void Adjust_para::run()
{
    while(1)
    {
        stop_mutex.lock();
        if(is_stop)
        {
            stop_mutex.unlock();
            qDebug() << "adjust thread qiut_______________________";
            return;
        }
        stop_mutex.unlock();
        if(!usebuff.tryAcquire())
            continue;

        qDebug() << "234";

        emit(send_image_debug(mat_1));

        correct_mutex.lock();

        qDebug() << "aabbbbbbbcccccc";

        if(correct_flag)
        {
            cv::imwrite(correct_path, mat_1);
            correct_flag = false;
        }
        correct_mutex.unlock();
    }
}

void Adjust_para::get_correct_signal()
{
    correct_mutex.lock();
    correct_flag = true;
//    qDebug() << "llllllllllll";
    correct_mutex.unlock();
}

void Adjust_para::exitThread()
{
    stop_mutex.lock();
    is_stop = true;
    stop_mutex.unlock();
}


SendThread::SendThread(QObject *parent) : QThread(parent)
{

}


void SendThread::run()
{
    bool is_timeout;
    cv::Mat temp_img;
    unsigned char* send_buf;

    send_server = new QTcpServer();
    send_server->listen(QHostAddress::Any, 21122);
    qDebug() << "sendthread waiting connect.....";
    send_server->waitForNewConnection(50000, &is_timeout);
    if(is_timeout)
    {
        qDebug() << "sendthread time out";
        return;
    }
    send_socket = send_server->nextPendingConnection();
    qDebug() << "sendsocket new connection";

    while(1)
    {
        if(!usebuff.tryAcquire())
            continue;
        read_to_send.acquire();
        temp_img = img_pop;
//        emit process_img->show_img(img_pop);



        send_buf = (uint8_t*)malloc(temp_img.cols * temp_img.rows * 3 + 16);
        memset(send_buf, 0, temp_img.cols * temp_img.rows * 3 + 16);

        uint16_t width = temp_img.cols;
        uint16_t height = temp_img.rows;
        uint32_t data_len = width * height * 3 + 8;

        //协议
        send_buf[0] = 0xAA;
        // 总体报文长度
        for(int i = 1; i<= 4; i++)
            send_buf[i] = (uint8_t)(data_len >> 8 * (4 - i));
        // 数据和指令部分
        send_buf[5] = ' ';
        send_buf[6] = ' ';
        send_buf[7] = 'I';
        send_buf[8] = 'M';
        send_buf[9] = (uint8_t)(height>>8) & 0xFF;
        send_buf[10] = (uint8_t)height;
        send_buf[11] = (uint8_t)(width>>8) & 0xFF;
        send_buf[12] = (uint8_t)width;
        memcpy(send_buf + 13, temp_img.data, temp_img.cols*temp_img.rows*3+4);
        // 末尾校验位、协议位
        send_buf[temp_img.cols * temp_img.rows*3 + 13] = 0xFF;
        send_buf[temp_img.cols * temp_img.rows*3 + 14] = 0xFF;
        send_buf[temp_img.cols * temp_img.rows*3 + 15] = 0xBB;

        send_socket->write((const char*)send_buf, (temp_img.cols * temp_img.rows * 3 + 16)*sizeof(unsigned char));
        send_socket->flush();

        qDebug() << "====send success====";


        free(send_buf);


    }

//
}



RecvThread::RecvThread(QObject *parent) : QThread(parent)
{

}


void RecvThread::run()
{
    bool is_timeout;
    char buf[1100] = {0};
    int len = 0;
    recv_server = new QTcpServer();
    recv_server->listen(QHostAddress::Any, 21123);
    qDebug() << "recvthread waiting connect.....";

    recv_server->waitForNewConnection(50000, &is_timeout);
    if(is_timeout == true)
    {
        qDebug() << "recvthread time out";
        return;
    }
    recv_socket = recv_server->nextPendingConnection();
    qDebug() << "recvthread new connection";
    while(1)
    {
        /***
//        memset(buf, 0, 10);
//        recv_socket->waitForReadyRead(-1); //没有东西就阻塞
//        len = recv_socket->read(buf, 10);

////        emit read_plc();

//        if(len < 0)
//        {
//            qDebug() << "receive data error";
//        }
//        else
//        {
//            qDebug() << "receive :" << len;
//            FILE* fp;
//            fp = fopen("C:/Users/USER/Desktop/1.txt", "a+");
//            fwrite(buf, 1, 1, fp);
//            fclose(fp);
//        }

//        emit send_sortingResult(buf);
***/
        memset(buf, 0, 1100);//初始化为0
        recv_socket->waitForReadyRead(-1);//阻塞
        qDebug() << "======receive success======";
        len = recv_socket->read(buf, 1100);
        qDebug() << "len" << len;
            char buf1[1100] = {0};
        for(int i = 0; i < 1080; i++)
        {
//            qDebug() << "buf" << buf[i] << i;
            buf1[i] = buf[i+6];
        }

        if(len < 0)
        {
//            qDebug() << "receiv data error";
        }
        else if(len < 10)
        {
//            qDebug() << "receive:" << len;
            emit send_sortingResult(buf);
        }
        else
        {
            qDebug() << "KM";
            //需要把获得的buf数据发送到showpic里
            emit SendPicInfo(buf1);
        }
    }


}

RestartThread::RestartThread(QObject *parent) : QThread(parent), restr_flag(false)
{

}

void RestartThread::run()
{
    while(1)
    {
        restr_mutex.lock();
        if(restr_flag)
        {
//            restr_mutex.unlock();
            m_camera->openCamera();
            qDebug() << "aa";
            restr_flag = false;
            qDebug() << "bb";
        }
        msleep(1);
        restr_mutex.unlock();
        msleep(1);
    }
}

void RestartThread::restr_fun()
{
    restr_mutex.lock();
    restr_flag = true;
    restr_mutex.unlock();
}
