//=============================================================================================================
/**
* @file     noiseestimate.cpp
* @author   Limin Sun <liminsun@nmr.mgh.harvard.edu>;
*           Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     July, 2014
*
* @section  LICENSE
*
* Copyright (C) 2014, Limin Sun, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Contains the implementation of the NoiseEstimate class.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "noiseestimate.h"
#include "FormFiles/noiseestimatesetupwidget.h"

//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QtCore/QtPlugin>
#include <QDebug>

//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace NoiseEstimatePlugin;
using namespace MNEX;
using namespace XMEASLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

NoiseEstimate::NoiseEstimate()
: m_bIsRunning(false)
, m_bProcessData(false)
, m_pRTMSAInput(NULL)
, m_pFSOutput(NULL)
, m_pBuffer(CircularMatrixBuffer<double>::SPtr())
, m_Fs(600)
, m_iFFTlength(16384)
{
}


//*************************************************************************************************************

NoiseEstimate::~NoiseEstimate()
{
    if(this->isRunning())
        stop();
}


//*************************************************************************************************************

QSharedPointer<IPlugin> NoiseEstimate::clone() const
{
    QSharedPointer<NoiseEstimate> pNoiseEstimateClone(new NoiseEstimate);
    return pNoiseEstimateClone;
}


//*************************************************************************************************************
//=============================================================================================================
// Creating required display instances and set configurations
//=============================================================================================================

void NoiseEstimate::init()
{    
    //
    // Load Settings
    //
    QSettings settings;
    m_iFFTlength = settings.value(QString("Plugin/%1/FFTLength").arg(this->getName()), 16384).toInt();

    // Input
    m_pRTMSAInput = PluginInputData<NewRealTimeMultiSampleArray>::create(this, "Noise Estimatge In", "Noise Estimate input data");
    connect(m_pRTMSAInput.data(), &PluginInputConnector::notify, this, &NoiseEstimate::update, Qt::DirectConnection);
    m_inputConnectors.append(m_pRTMSAInput);

    // Output
    m_pFSOutput = PluginOutputData<FrequencySpectrum>::create(this, "Noise Estimate Out", "Noise Estimate output data");
    m_pFSOutput->data()->setName(this->getName());//Provide name to auto store widget settings
    m_outputConnectors.append(m_pFSOutput);

//    m_pRTMSAOutput->data()->setMultiArraySize(100);
//    m_pRTMSAOutput->data()->setVisibility(true);

    //init channels when fiff info is available
    connect(this, &NoiseEstimate::fiffInfoAvailable, this, &NoiseEstimate::initConnector);

    //Delete Buffer - will be initailzed with first incoming data
    if(!m_pBuffer.isNull())
        m_pBuffer = CircularMatrixBuffer<double>::SPtr();
}


//*************************************************************************************************************

void NoiseEstimate::unload()
{
    //
    // Store Settings
    //
    QSettings settings;
    settings.setValue(QString("Plugin/%1/FFTLength").arg(this->getName()), m_iFFTlength);
}


//*************************************************************************************************************

void NoiseEstimate::initConnector()
{
    qDebug() << "void NoiseEstimate::initConnector()";
    if(m_pFiffInfo)
        m_pFSOutput->data()->initFromFiffInfo(m_pFiffInfo);
}


//*************************************************************************************************************

bool NoiseEstimate::start()
{
    //Check if the thread is already or still running. This can happen if the start button is pressed immediately after the stop button was pressed. In this case the stopping process is not finished yet but the start process is initiated.
    if(this->isRunning())
        QThread::wait();

    m_bIsRunning = true;

    // Start threads
    QThread::start();

    return true;
}


//*************************************************************************************************************

bool NoiseEstimate::stop()
{
    //Wait until this thread is stopped
    m_bIsRunning = false;

    m_pRtNoise->stop();

    if(m_bProcessData)
    {

        //In case the semaphore blocks the thread -> Release the QSemaphore and let it exit from the pop function (acquire statement)
        m_pBuffer->releaseFromPop();
        m_pBuffer->releaseFromPush();

        m_pBuffer->clear();

//        m_pNEOutput->data()->clear();
    }

    return true;
}


//*************************************************************************************************************

IPlugin::PluginType NoiseEstimate::getType() const
{
    return _IAlgorithm;
}


//*************************************************************************************************************

QString NoiseEstimate::getName() const
{
    return "Noise Estimation";
}


//*************************************************************************************************************

QWidget* NoiseEstimate::setupWidget()
{
    NoiseEstimateSetupWidget* setupWidget = new NoiseEstimateSetupWidget(this);//widget is later distroyed by CentralWidget - so it has to be created everytime new

    connect(this,&NoiseEstimate::SetNoisePara,setupWidget,&NoiseEstimateSetupWidget::init);
    //connect(this,&NoiseEstimate::RePlot,setupWidget,&NoiseEstimateSetupWidget::Update);

    return setupWidget;

}


//*************************************************************************************************************

void NoiseEstimate::update(XMEASLIB::NewMeasurement::SPtr pMeasurement)
{
    QSharedPointer<NewRealTimeMultiSampleArray> pRTMSA = pMeasurement.dynamicCast<NewRealTimeMultiSampleArray>();

    if(pRTMSA)
    {
        //Check if buffer initialized
        if(!m_pBuffer)
            m_pBuffer = CircularMatrixBuffer<double>::SPtr(new CircularMatrixBuffer<double>(64, pRTMSA->getNumChannels(), pRTMSA->getMultiArraySize()));

        //Fiff information
        if(!m_pFiffInfo)
        {
            m_pFiffInfo = pRTMSA->info();
            emit fiffInfoAvailable();
        }

        if(m_bProcessData)
        {
            MatrixXd t_mat(pRTMSA->getNumChannels(), pRTMSA->getMultiArraySize());

            for(qint32 i = 0; i < pRTMSA->getMultiArraySize(); ++i)
                t_mat.col(i) = pRTMSA->getMultiSampleArray()[i];

            m_pBuffer->push(&t_mat);
        }
    }
}

//*************************************************************************************************************

void NoiseEstimate::appendNoiseSpectrum(MatrixXd t_send)
{ 
    qDebug()<<"Spectrum"<<t_send(0,1)<<t_send(0,2)<<t_send(0,3);
    mutex.lock();
    m_qVecSpecData.push_back(t_send);
    mutex.unlock();
    //m_pFSOutput->data()->setValue(t_send);
    qDebug()<<"---------------------------------appendNoiseSpectrum--------------------------------";
}


//*************************************************************************************************************

void NoiseEstimate::run()
{
    //
    // Read Fiff Info
    //
    while(!m_pFiffInfo)
        msleep(10);// Wait for fiff Info

    //
    // Init Real-Time Noise Spectrum estimator
    //
    m_pRtNoise = RtNoise::SPtr(new RtNoise(m_iFFTlength, m_pFiffInfo));
    connect(m_pRtNoise.data(), &RtNoise::SpecCalculated, this, &NoiseEstimate::appendNoiseSpectrum);
    //connect(m_pRtNoise.data(), &RtNoise::SpecCalculated, this, &NoiseEstimate::appendNoiseSpectrum);

//    m_pRtNoise = new RtNoise(m_iFFTlength, m_pFiffInfo);
//    connect(m_pRtNoise, &RtNoise::SpecCalculated, this, &NoiseEstimate::appendNoiseSpectrum);


    // Start the rt helpers

    m_pRtNoise->start();


    m_bProcessData = true;

    while (m_bIsRunning)
    {
        if(m_bProcessData)
        {
            /* Dispatch the inputs */
            MatrixXd t_mat = m_pBuffer->pop();

            //ToDo: Implement your algorithm here
            m_pRtNoise->append(t_mat);

           if(m_qVecSpecData.size() > 0)
           {

               mutex.lock();
               qDebug()<<"%%%%%%%%%%%%%%%% send spectrum for display %%%%%%%%%%%%%%%%%%%";
                //send spectrum to the output data
               m_pFSOutput->data()->setValue(m_qVecSpecData[0]);
               m_qVecSpecData.pop_front();
               mutex.unlock();

            }
        }//m_bProcessData
    }//m_bIsRunning
    m_pRtNoise->stop();
//    delete m_pRtNoise;
}

