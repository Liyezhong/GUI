/****************************************************************************/
/*! \file ManufacturingTestHandler.cpp
 *
 *  \brief Implementation file for class ManufacturingTestHandler.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2014-05-20
 *  $Author:    $ Sunny Qu
 *
 *  \b Company:
 *
 *       Leica Biosystems Shanghai.
 *
 *  (C) Copyright 2010 by Leica Biosystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <ServiceDeviceController/Include/DeviceProcessor/ManufacturingTestHandler.h>

#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
#include <ServiceDeviceController/Include/ServiceDeviceController.h>
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <ServiceDeviceController/Include/DeviceProcessor/Helper/WrapperFmStepperMotor.h>
#include <ServiceDeviceController/Include/DeviceProcessor/Helper/WrapperFmTempControl.h>
#include <ServiceDeviceController/Include/DeviceProcessor/Helper/WrapperFmPressureControl.h>
#include <ServiceDeviceController/Include/DeviceProcessor/Helper/WrapperUtils.h>
#include "Core/Include/CMessageString.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl {

/****************************************************************************/
ManufacturingTestHandler::ManufacturingTestHandler(IDeviceProcessing &iDevProc)
    : m_IsConfigured(false)
    , m_rIdevProc(iDevProc)
    , m_CurrentAction(TEST_OTHER_RUNS)
    , m_ExhaustInit(false)
    , m_HoodDeviceInit(false)
    , m_WaterDeviceInit(false)
    , m_DeviceLightInit(false)
    , mp_PressPump(NULL)
{
#if 0
    qRegisterMetaType<Global::LiquidLevelState>("Global::LiquidLevelState");
#endif

    m_UserAbort = false;
    mp_Utils = NULL;

    mp_TempRV = NULL;
    mp_TempLSensor = NULL;
    mp_TempRetortSide = NULL;
    mp_TempRetortBottom = NULL;
    mp_TempTube1 = NULL;
    mp_TempTube2 = NULL;
    mp_TempOvenTop = NULL;
    mp_TempOvenBottom = NULL;

    mp_MotorRV = NULL;
}

/****************************************************************************/
void ManufacturingTestHandler::Connect()
{
}

/****************************************************************************/
void ManufacturingTestHandler::CreateWrappers()
{
    qDebug()<<"ManufacturingTestHandler::CreateWrappers" ;

    // Temperature control
    CTemperatureControl *pTemperature;

    pTemperature = NULL;
    pTemperature = static_cast<CTemperatureControl *>(m_rIdevProc.GetFunctionModuleRef(DEVICE_INSTANCE_ID_ROTARY_VALVE, CANObjectKeyLUT::m_RVTempCtrlKey));
    if (NULL != pTemperature)
    {
        mp_TempRV = new WrapperFmTempControl("temp_rv", pTemperature, this);
    }

    pTemperature = NULL;
    pTemperature = static_cast<CTemperatureControl *>(m_rIdevProc.GetFunctionModuleRef(DEVICE_INSTANCE_ID_AIR_LIQUID, CANObjectKeyLUT::m_ALLevelSensorTempCtrlKey));
    if (NULL != pTemperature)
    {
        mp_TempLSensor = new WrapperFmTempControl("temp_lsensor", pTemperature, this);
    }

    pTemperature = NULL;
    pTemperature = static_cast<CTemperatureControl *>(m_rIdevProc.GetFunctionModuleRef(DEVICE_INSTANCE_ID_RETORT, CANObjectKeyLUT::m_RetortSideTempCtrlKey));
    if (NULL != pTemperature)
    {
        mp_TempRetortSide = new WrapperFmTempControl("temp_retort_side", pTemperature, this);
    }

    pTemperature = NULL;
    pTemperature = static_cast<CTemperatureControl *>(m_rIdevProc.GetFunctionModuleRef(DEVICE_INSTANCE_ID_RETORT, CANObjectKeyLUT::m_RetortBottomTempCtrlKey));
    if (NULL != pTemperature)
    {
        mp_TempRetortBottom = new WrapperFmTempControl("temp_retort_bottom", pTemperature, this);
    }

    pTemperature = NULL;
    pTemperature = static_cast<CTemperatureControl *>(m_rIdevProc.GetFunctionModuleRef(DEVICE_INSTANCE_ID_AIR_LIQUID, CANObjectKeyLUT::m_ALTube1TempCtrlKey));
    if (NULL != pTemperature)
    {
        mp_TempTube1 = new WrapperFmTempControl("temp_tube1", pTemperature, this);
    }

    pTemperature = NULL;
    pTemperature = static_cast<CTemperatureControl *>(m_rIdevProc.GetFunctionModuleRef(DEVICE_INSTANCE_ID_AIR_LIQUID, CANObjectKeyLUT::m_ALTube2TempCtrlKey));
    if (NULL != pTemperature)
    {
        mp_TempTube2 = new WrapperFmTempControl("temp_tube2", pTemperature, this);
    }

    pTemperature = NULL;
    pTemperature = static_cast<CTemperatureControl *>(m_rIdevProc.GetFunctionModuleRef(DEVICE_INSTANCE_ID_OVEN, CANObjectKeyLUT::m_OvenTopTempCtrlKey));
    if (NULL != pTemperature)
    {
        mp_TempOvenTop = new WrapperFmTempControl("temp_oven_top", pTemperature, this);
    }

    pTemperature = NULL;
    pTemperature = static_cast<CTemperatureControl *>(m_rIdevProc.GetFunctionModuleRef(DEVICE_INSTANCE_ID_OVEN, CANObjectKeyLUT::m_OvenBottomTempCtrlKey));
    if (NULL != pTemperature)
    {
        mp_TempOvenBottom = new WrapperFmTempControl("temp_oven_bottom", pTemperature, this);
    }

    // Stepper motor control
   /* CStepperMotor *pMotor;

    pMotor = NULL;
    pMotor = static_cast<CStepperMotor *>(m_rIdevProc.GetFunctionModuleRef(DEVICE_INSTANCE_ID_ROTARY_VALVE, CANObjectKeyLUT::m_RVMotorKey));
    if (NULL != pMotor)
    {
        mp_MotorRV = new WrapperFmStepperMotor("motor_rv", pMotor, this);
    }
*/
}

/****************************************************************************/
void ManufacturingTestHandler::Initialize()
{
    (void) CreateWrappers();
    m_IsConfigured = true;

    mp_Utils = new WrapperUtils(this);
}

/****************************************************************************/
bool ManufacturingTestHandler::IsInitialized()
{
    return m_IsConfigured;
}


#if 0
/****************************************************************************/
QString ManufacturingTestHandler::ReturnStringForID(ReturnCode_t HdlInfo)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == HdlInfo)
    {
        return "false";
    }
    return "true";
}

/****************************************************************************/
bool ManufacturingTestHandler::CheckHardwareInfo(ReturnCode_t HdlInfo)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == HdlInfo)
    {
        return true;
    }
    return false;
}
#endif

/****************************************************************************/
void ManufacturingTestHandler::OnAbortTest(Global::tRefType Ref, quint32 id)
{
    qDebug()<<"ManufacturingTestHandler::OnAbortTest";
    if(!IsInitialized()){
        Initialize();
    }

    if(NULL == mp_Utils)
    {
        emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
        return;
    }

    mp_Utils->AbortPause();

    m_UserAbort = true;

}


/****************************************************************************/
void ManufacturingTestHandler::OnLSensorDetectingTest(Global::tRefType Ref, quint32 id,
                                      qint32 Position)
{

    qDebug()<<"ManufacturingTestHandler::OnLSensorDetectingTest";
    if(!IsInitialized()){
        Initialize();
    }

    qint32  Ret(0);
    QString ReturnMsg("");

    qDebug("Device ID:%X, Position:%d", id, Position);

    if ( NULL == mp_MotorRV )
    {
        emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
        return;
    }

    if ( NULL == mp_PressPump )
    {
        emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
        return;
    }

    Ret = TestLSensorDetecting(id, Position);
    if ( 1 == Ret )
    {
        emit ReturnMessagetoMain(Service::MSG_LSENSOR_DETECTING_SUCCESS);
    }
    else
    {
        switch(Ret)  //-1=err, -2=time out, -3=overflow
        {
            case -1:
                emit ReturnErrorMessagetoMain(Service::MSG_LSENSOR_DETECTING_FAILURE);
                break;
            case -2:
                emit ReturnErrorMessagetoMain(Service::MSG_LSENSOR_DETECTING_TIMEOUT);
                break;
            case -3:
                emit ReturnErrorMessagetoMain(Service::MSG_LSENSOR_DETECTING_OVERFLOW);
                break;
            case -4:
                emit ReturnErrorMessagetoMain(Service::MSG_LSENSOR_DETECTING_INSUFFICIENT);
                break;
            default:
                break;
        }
    }

}

/****************************************************************************/
void ManufacturingTestHandler::OnHeatingTest(Global::tRefType Ref, quint32 id,
                                      quint8 HeaterIndex, quint8 CmdType)
{
    m_UserAbort = false;

    qDebug()<<"ManufacturingTestHandler::OnHeatingTest";
    if(!IsInitialized()){
        Initialize();
    }

    qint32  Ret(0);
    QString ReturnMsg("");

    qDebug("Device ID:%X, Index:%d, CmdType:%d", id, HeaterIndex, CmdType);


/*
    Ret= mp_Utils->Pause(10000);
    if (Ret == 1)
    {
        emit ReturnErrorMessagetoMain(Service::MSG_TEST_ABORT_FAILUE);
        return;
    }
*/

    if(DEVICE_INSTANCE_ID_ROTARY_VALVE == id)
    {
        if ( NULL == mp_TempRV )
        {
            emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
            return;
        }

        if ( 0 == TestRVHeating(id) )
        {
            emit ReturnMessagetoMain(Service::MSG_RV_HEATING_SUCCESS);
        }
        else
        {
            emit ReturnErrorMessagetoMain(Service::MSG_RV_HEATING_FAILURE);
        }
    }
    else
    if(DEVICE_INSTANCE_ID_RETORT == id)
    {
        if ( NULL == mp_TempRetortSide || NULL == mp_TempRetortBottom )
        {
            emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
            return;
        }

        if ( 0 == TestRVHeating(id) )
        {
            emit ReturnMessagetoMain(Service::MSG_RV_HEATING_SUCCESS);
        }
        else
        {
            emit ReturnErrorMessagetoMain(Service::MSG_RV_HEATING_FAILURE);
        }
    }
    else
    if(DEVICE_INSTANCE_ID_OVEN == id)
    {
        if ( NULL == mp_TempOvenTop || NULL == mp_TempOvenBottom )
        {
            emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
            return;
        }

        if ( 0 == TestOvenHeating(id) )
        {
            emit ReturnMessagetoMain(Service::MSG_OVEN_HEATING_SUCCESS);
        }
        else
        {
            emit ReturnErrorMessagetoMain(Service::MSG_OVEN_HEATING_FAILURE);
        }
    }
    else
    if(DEVICE_INSTANCE_ID_AIR_LIQUID == id)
    {
        switch(HeaterIndex)
        {
        case Service::HEATER_LEVEL_SENSOR:
        {
            if ( NULL == mp_TempLSensor )
            {
                emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
                return;
            }

            if ( 0 == TestLSensorHeating(id) )
            {
                emit ReturnMessagetoMain(Service::MSG_LSENSOR_HEATING_SUCCESS);
            }
            else
            {
                emit ReturnErrorMessagetoMain(Service::MSG_LSENSOR_HEATING_FAILURE);
            }
        }
            break;
        case Service::HEATER_TUBE1:
        {
            if ( NULL == mp_TempTube1 )
            {
                emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
                return;
            }

            if ( 0 == TestTubeHeating(id, 0) )
            {
                emit ReturnMessagetoMain(Service::MSG_TUBE1_HEATING_SUCCESS);
            }
            else
            {
                emit ReturnErrorMessagetoMain(Service::MSG_TUBE1_HEATING_FAILURE);
            }

        }
            break;
        case Service::HEATER_TUBE2:
        {
            if ( NULL == mp_TempTube2 )
            {
                emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
                return;
            }

            if ( 0 == TestTubeHeating(id, 1) )
            {
                emit ReturnMessagetoMain(Service::MSG_TUBE2_HEATING_SUCCESS);
            }
            else
            {
                emit ReturnErrorMessagetoMain(Service::MSG_TUBE2_HEATING_FAILURE);
            }
        }
            break;
        }

    }

}

/****************************************************************************/
void ManufacturingTestHandler::OnRotaryValveTest(Global::tRefType Ref, quint32 id,
                     qint32 Position, quint8 CmdType)
{
    m_UserAbort = false;

    qDebug()<<"ManufacturingTestHandler::OnRotaryValveTest";
    if(!IsInitialized()){
        Initialize();
    }

    qint32  Ret(0);
    QString ReturnMsg("");

    qDebug("Device ID:%X, Postion:%d, CmdType:%d", id, Position, CmdType);

    if ( NULL == mp_MotorRV )
    {
        emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
        return;
    }

    if ( NULL == mp_PressPump )
    {
        emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
        return;
    }

    switch(CmdType)
    {
    case Service::RV_MOVE_TO_INIT_POS:
        if ( 0 == MoveRVToInitPos(id) )
        {
            emit ReturnMessagetoMain(Service::MSG_RV_MOVETO_INITPOS_SUCCESS);
        }
        else
        {
            emit ReturnErrorMessagetoMain(Service::MSG_RV_MOVETO_INITPOS_FAILURE);
        }
        break;
    case Service::RV_MOVE_TO_TUBE_POS:
        if ( 0 == MoveRVToTubePos(id, Position) )
        {
            emit ReturnMessagetoMain(Service::MSG_RV_MOVETO_TUBEPOS_SUCCESS);
        }
        else
        {
            emit ReturnErrorMessagetoMain(Service::MSG_RV_MOVETO_TUBEPOS_FAILURE);
        }
        break;
    case Service::RV_MOVE_TO_SEAL_POS:
        if ( 0 == MoveRVToSealPos(id, Position) )
        {
            emit ReturnMessagetoMain(Service::MSG_RV_MOVETO_SEALPOS_SUCCESS);
        }
        else
        {
            emit ReturnErrorMessagetoMain(Service::MSG_RV_MOVETO_SEALPOS_FAILURE);
        }
        break;

    }
}

/****************************************************************************/
qint32 ManufacturingTestHandler::TestRVHeating(quint32 DeviceId)
{

    quint32 WaitSec(0);
    qint32  RVStatus(-1);
    qreal   CurrentTemp(0);

    WaitSec = Service::TEST_RV_HEATING_TIME;

    mp_TempRV->StartTemperatureControl(Service::TEST_RV_HEATING_TEMP);

    while (!m_UserAbort && WaitSec)
    {
        CurrentTemp = mp_TempRV->GetTemperature(0);
        qDebug() << "Current Temperature (RV): " << CurrentTemp;

        if ( CurrentTemp > Service::TEST_RV_HEATING_MIN_TEMP &&
             CurrentTemp < Service::TEST_RV_HEATING_MAX_TEMP )
        {
            RVStatus = 1;
            break;
        }

        mp_Utils->Pause(1000);
        WaitSec--;
    }

    mp_TempRV->StopTemperatureControl();

    if ( RVStatus == -1 )
    {
        if (m_UserAbort)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return 0;
    }
}

/****************************************************************************/
qint32 ManufacturingTestHandler::TestLSensorHeating(quint32 DeviceId)
{
    quint32 WaitSec(0);
    qint32  LSensorStatus(-1);
    qreal   CurrentTemp(0);

    WaitSec = Service::TEST_LS_HEATING_TIME;

    mp_TempLSensor->StartTemperatureControl(Service::TEST_LS_HEATING_TEMP);

    while (!m_UserAbort && WaitSec)
    {
        CurrentTemp = mp_TempLSensor->GetTemperature(0);
        qDebug() << "Current Temperature (LS): " << CurrentTemp;

        if ( CurrentTemp > Service::TEST_LS_HEATING_MIN_TEMP &&
             CurrentTemp < Service::TEST_LS_HEATING_MAX_TEMP )
        {
            LSensorStatus = 1;
            break;
        }

        mp_Utils->Pause(1000);
        WaitSec--;
    }

    mp_TempLSensor->StopTemperatureControl();

    if ( LSensorStatus == -1 )
    {
        if (m_UserAbort)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return 0;
    }
}


/****************************************************************************/
qint32 ManufacturingTestHandler::TestOvenHeating(quint32 DeviceId)
{
    quint32 WaitSec(0);
    qint32  OvenStatus(-1);
    qreal   CurrentTempTop(0), CurrentTempBottom(0);

    WaitSec = Service::TEST_OVEN_HEATING_TIME;

    mp_TempOvenTop->StartTemperatureControl(Service::TEST_OVEN_HEATING_TEMP);
    mp_TempOvenBottom->StartTemperatureControl(Service::TEST_OVEN_HEATING_TEMP);

    while (!m_UserAbort && WaitSec)
    {
        CurrentTempTop = mp_TempOvenTop->GetTemperature(0);
        CurrentTempBottom = mp_TempOvenBottom->GetTemperature(0);

        qDebug() << "Current Temperature: [Oven Top] " << CurrentTempTop << ", [Oven Bottom] " << CurrentTempBottom;

        if ( CurrentTempTop > Service::TEST_OVEN_HEATING_MIN_TEMP &&
             CurrentTempTop < Service::TEST_OVEN_HEATING_MAX_TEMP &&
             CurrentTempBottom > Service::TEST_OVEN_HEATING_MIN_TEMP &&
             CurrentTempBottom < Service::TEST_OVEN_HEATING_MAX_TEMP )
        {
            OvenStatus = 1;
            break;
        }

        mp_Utils->Pause(1000);
        WaitSec--;
    }

    mp_TempOvenTop->StopTemperatureControl();
    mp_TempOvenBottom->StopTemperatureControl();

    if ( OvenStatus == -1 )
    {
        if (m_UserAbort)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return 0;
    }
}


qint32 ManufacturingTestHandler::TestTubeHeating(quint32 DeviceId, quint8 TubeIndex)
{

    quint32 WaitSec(0);
    qint32  TubeStatus(-1);
    qreal   CurrentTemp(0);
    WrapperFmTempControl* p_TempCtrl;

    WaitSec = Service::TEST_TUBE_HEATING_TIME;

    switch(TubeIndex)
    {
    case 0:
    default:
        p_TempCtrl = mp_TempTube1;
        break;
    case 1:
        p_TempCtrl = mp_TempTube2;
        break;
    }

    p_TempCtrl->StartTemperatureControl(Service::TEST_TUBE_HEATING_TEMP);

    while (!m_UserAbort && WaitSec)
    {
        CurrentTemp = p_TempCtrl->GetTemperature(0);
        qDebug() << "Current Temperature (Tube " << TubeIndex << "):" << CurrentTemp;

        if ( CurrentTemp > Service::TEST_TUBE_HEATING_MIN_TEMP &&
             CurrentTemp < Service::TEST_TUBE_HEATING_MAX_TEMP )
        {
            TubeStatus = 1;
            break;
        }

        mp_Utils->Pause(1000);
        WaitSec--;
    }

    p_TempCtrl->StopTemperatureControl();

    if ( TubeStatus == -1 )
    {
        if (m_UserAbort)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return 0;
    }

}

qint32 ManufacturingTestHandler::MoveRVToInitPos(quint32 DeviceId)
{
    if ( !mp_MotorRV->MoveToInitialPosition() )
    {
        return -1;
    }

    return 0;
}

qint32 ManufacturingTestHandler::MoveRVToTubePos(quint32 DeviceId, qint32 Pos)
{
    if ( !mp_MotorRV->MoveToTubePosition(Pos) )
    {
        return -1;
    }

    mp_PressPump->SetTargetPressure(1, Service::TEST_RV_TUBEPOS_PRESS);

    mp_Utils->Pause(Service::TEST_RV_TUBEPOS_PRESS_DURATION);

    mp_PressPump->ReleasePressure();

    return 0;
}

qint32 ManufacturingTestHandler::MoveRVToSealPos(quint32 DeviceId, qint32 Pos)
{
    if ( !mp_MotorRV->MoveToSealPosition(Pos) )
    {
        return -1;
    }

    mp_PressPump->SetTargetPressure(1, Service::TEST_RV_SEALPOS_PRESS);

    mp_Utils->Pause(Service::TEST_RV_SEALPOS_PRESS_DURATION);

    if ( mp_PressPump->GetPressure(0) > Service::TEST_RV_SEALPOS_PRESS - Service::TEST_RV_SEALPOS_PRESS_DROP )
    {
        mp_PressPump->ReleasePressure();
        return 0;
    }
    else
    {
        mp_PressPump->ReleasePressure();
        return -1;
    }

}

qint32 ManufacturingTestHandler::TestLSensorDetecting(quint32 DeviceId, qint32 Pos)
{
    qint32 Ret;

    if ( !mp_MotorRV->MoveToTubePosition(Pos) )
    {
        return -1;
    }

    Ret = mp_PressPump->Sucking(0, Pos, false);
    switch(Ret)  //1=ok, -1=err, -2=time out, -3=overflow
    {
        case -1:
            qDebug() << "Sucking return: [General]";
            break;
        case -2:
            qDebug() << "Sucking return: [TimeOut]";
            break;
        case -3:
            qDebug() << "Sucking return: [Overflow]";
            mp_PressPump->Draining(1000, Pos);
            break;
        case -4:
            qDebug() << "Sucking return: [Insufficient]";
            break;
        default:
            break;
    }

    return Ret;
}

void ManufacturingTestHandler::OnCalibrateDevice(Service::DeviceCalibrationCmdType CmdType)
{
    Q_UNUSED(CmdType);
    qDebug()<<"ManufacturingTestHandler::OnCalibrateDevice";
    if(!IsInitialized()){
        Initialize();
    }
    if (!mp_PressPump)
        return;

    if(!mp_PressPump)
    {
        ReturnErrorMessagetoMain(Service::CMessageString::MSG_DEVICE_NOTYET_READY);
        emit ReturnCalibrationInitMessagetoMain("", false);
        return;
    }

    if(!mp_PressPump->ReleasePressure())
    {
       emit ReturnErrorMessagetoMain(Service::CMessageString::MSG_DEVICE_RELEASE_PRESSURE_FAILED);
       return;
    }

    float CurrentPressure = mp_PressPump->GetPressureFromSensor(0);
    if (CurrentPressure < 2 && CurrentPressure > -2)
    {
        //Calibration to 0 (+/-0.2Kpa) and save to the config file;
        mp_PressPump->WritePressureDrift(CurrentPressure);
        QString strDriftTip = QString("%1").arg(CurrentPressure);
        emit ReturnMessagetoMain(Service::CMessageString::MSG_DEVICE_PRESSURE_SENSOR_CALIBRATION_SUCCESS + strDriftTip);
        return;
    }
    else
    {
        emit ReturnErrorMessagetoMain(Service::CMessageString::MSG_DEVICE_PRESSURE_SENSOR_CALIBRATION_FAILED);
        return;
    }

}

void ManufacturingTestHandler::PerformModuleManufacturingTest(Service::ModuleTestNames TestName)
{
    qDebug()<<"ManufacturingTestHandler::PerformModuleManufacturingTest  test="<<TestName;
    if(!IsInitialized()){
        Initialize();
    }

    switch (TestName) {
    case Service::OVEN_COVER_SENSOR :
        TestOvenCoverSensor();
        //break;
    case Service::OVEN_HEATING_EMPTY:{
        int id = DEVICE_INSTANCE_ID_OVEN;
        if(DEVICE_INSTANCE_ID_OVEN == id)
        {
            if ( NULL == mp_TempOvenTop || NULL == mp_TempOvenBottom )
            {
                emit ReturnErrorMessagetoMain(Service::MSG_DEVICE_NOT_INITIALIZED);
                return;
            }

            if ( 0 == TestOvenHeating(id) )
            {
                emit ReturnMessagetoMain(Service::MSG_OVEN_HEATING_SUCCESS);
            }
            else
            {
                emit ReturnErrorMessagetoMain(Service::MSG_OVEN_HEATING_FAILURE);
            }
        }
        break;
    }
    }
}

qint32 ManufacturingTestHandler::TestOvenCoverSensor()
{

}
} // end namespace DeviceControl