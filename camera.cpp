#include "camera.h"

#define FIRST_DEVICE_INDEX 1
//#define SETTING_PATH "C:/Program Files/Teledyne DALSA/Sapera/CamFiles/User/202298_first.ccf"
#define SETTING_PATH "C:/Program Files/Teledyne DALSA/Sapera/CamFiles/User/abc.ccf"

extern void onImageDataCallback(SapXferCallbackInfo *pInfo);
extern void onImageCorrectCallback(SapXferCallbackInfo *pInfo);

camera::camera()
{

}

bool camera::enumDevic(char *serverName)
{
    int devicecount = SapManager::GetServerCount();
    if(devicecount < 1)
    {
        qDebug() << "no device found";
        return false;
    }

    SapManager::GetServerName(FIRST_DEVICE_INDEX, serverName, CORSERVER_MAX_STRLEN);
    qDebug() << serverName;

    return true;
}

bool camera::creatObjects(char *serverName)
{
    m_loc = new SapLocation(serverName, 1);
    m_pAcq = new SapAcquisition(*m_loc, SETTING_PATH);
    m_buffer = new SapBuffer(1, m_pAcq);
//    m_trans = new SapAcqToBuf(m_pAcq, m_buffer, onImageDataCallback, NULL);

    if(false == m_pAcq->Create())
    {
        destroyObjects();
        qDebug() << "create pAcq device failed";
        return false;
    }

    if(false == m_buffer->Create())
    {
        destroyObjects();
        qDebug() << "create buff failed";
        return false;
    }

//    if(false == m_trans->Create())
//    {
//        destroyObjects();
//        qDebug() << "create transport failed";
//        return false;
//    }

    qDebug() << "create object success";
    return true;

}

void camera::destroyObjects()
{
    if(m_trans && *m_trans)
    {
        m_trans->Destroy();
        qDebug() << "1";
    }
    if(c_trans && *c_trans)
    {
        c_trans->Destroy();
        qDebug() << "2";
    }
    if(m_buffer && *m_buffer)
    {
        m_buffer->Destroy();
        qDebug() << "3";
    }
    if(m_pAcq && *m_pAcq)
    {
        m_pAcq->Destroy();
        qDebug() << "4";
    }

}

bool camera::openCamera()
{
    char serverName[CORSERVER_MAX_STRLEN];
    if(!enumDevic(serverName))
    {
        qDebug() << "enum device failed";
        return false;
    }

    if(!creatObjects(serverName))
        return false;

    qDebug() << "open device success";

    return true;
}

void camera::closeCamera()
{
    destroyObjects();
    if(m_trans)
    {
        delete m_trans;
        qDebug() << "5";
    }
    if(c_trans)
    {
        delete c_trans;
        qDebug() << "6";
    }
    if(m_buffer)
    {
        delete m_buffer;
        qDebug() << "7";
    }
    if(m_pAcq)
    {
        delete m_pAcq;
        qDebug() << "8";
    }

}

bool camera::startCapture()
{
    bool success;
    m_trans = new SapAcqToBuf(m_pAcq, m_buffer, onImageDataCallback, NULL);
    if(false == m_trans->Create())
    {
        destroyObjects();
        if(m_trans && *m_trans)
        {
            m_trans->Destroy();
        }
        qDebug() << "create transport failed";
        return false;
    }
    success = m_trans->Grab();
    if(success == false)
    {
        qDebug() << "grab failed";
        return false;
    }
    return true;
}

bool camera::stopCapture()
{
    bool success;
    success = m_trans->Freeze();
    if(success == false)
    {
        qDebug() << "stop capture failed";
        return false;
    }
    m_trans->Wait(5000);
    if(m_trans && *m_trans)
    {
        m_trans->Destroy();
    }
    return true;
}

bool camera::startCorrect()
{
    bool success;
    qDebug() << "start correct";
    c_trans = new SapAcqToBuf(m_pAcq, m_buffer, onImageCorrectCallback, NULL);
    if(false == c_trans->Create())
    {
        destroyObjects();
        if(c_trans && *c_trans)
        {
           c_trans->Destroy();
        }
        qDebug() << "create c_transport failed";
        return false;
    }
    qDebug() << "create c_trans";
    success = c_trans->Grab();
    if(success == false)
    {
        qDebug() << "start correct failed";
        return false;
    }
    qDebug() << "c_trans grab";
    return true;
}

bool camera::stopCorrect()
{
    bool success;
    success = c_trans->Freeze();
    if(success == false)
    {
        qDebug() << "stop correct failed";
        return false;
    }
    c_trans->Wait(5000);

    if(c_trans && *c_trans)
    {
        c_trans->Destroy();
    }

    return true;
}

//没用
bool camera::interTricapture()
{
    m_pAcq->SetParameter(CORACQ_PRM_INT_LINE_TRIGGER_FREQ, 8, TRUE);//内触发信号频率
    m_pAcq->SetParameter(CORACQ_PRM_INT_LINE_TRIGGER_ENABLE, TRUE, TRUE);//内触发

    bool success;
    success = m_trans->Grab();
    if(success == false)
    {
        qDebug() << "inter trigger capture failed";
        return false;
    }
    m_trans->Wait(5000);
    return true;
}





