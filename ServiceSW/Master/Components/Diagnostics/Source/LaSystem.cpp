/****************************************************************************/
/*! \file Diagnostics/Source/LaSystem.cpp
 *
 *  \brief Implementation of Liquid and air system test.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-05-27
 *   $Author:  $ R.Wu
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "Diagnostics/Include/LaSystem.h"

#include <QDebug>

#include "ui_LaSystem.h"

#include "Diagnostics/Include/LaSystem/AirHeatingTubeTest.h"
#include "Diagnostics/Include/LaSystem/LiquidHeatingTubeTest.h"
#include "Global/Include/Utils.h"
#include "Main/Include/HimalayaServiceEventCodes.h"
#include "Diagnostics/Include/DiagnosticMessageDlg.h"


namespace Diagnostics {

CLaSystem::CLaSystem(QWidget *p_Parent) :
    QWidget(p_Parent),
    ui(new Ui::CLaSystem)
{
    ui->setupUi(this);
    dlg = new CDiagnosticMessageDlg(this);
    (void)connect(ui->testLiquidHeatingTube,
                 SIGNAL(clicked()),
                 this,
                 SLOT(StartLiquidHeatingTubeTest()) );

    (void)connect(ui->testAirHeatingTube,
                  SIGNAL(clicked()),
                  this,
                  SLOT(StartAirHeatingTubeTest()) );
}

CLaSystem::~CLaSystem()
{
    delete dlg;
    delete ui;
}

void CLaSystem::StartLiquidHeatingTubeTest(void)
{
    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_DIAGNOSTICS_LASYSTEM_HEATING_LIQUID_TEST);
    qDebug() << "Start Liquid heating tube test";

    emit SetGUITabEnable(false);
    LaSystem::CLiquidHeatingTubeTest test(dlg);
    ErrorCode_t ret = (ErrorCode_t)test.Run();
    if (ret == RETURN_OK)
        Global::EventObject::Instance().RaiseEvent(EVENT_GUI_DIAGNOSTICS_LASYSTEM_HEATING_LIQUID_TEST_SUCCESS);
    else if (ret == RETURN_ABORT)
        Global::EventObject::Instance().RaiseEvent(EVENT_GUI_DIAGNOSTICS_TEST_ABORT, Global::tTranslatableStringList()<<" Liquid heating tube");
    else
        Global::EventObject::Instance().RaiseEvent(EVENT_GUI_DIAGNOSTICS_LASYSTEM_HEATING_LIQUID_TEST_FAILURE);

    emit SetGUITabEnable(true);
}

void CLaSystem::StartAirHeatingTubeTest(void)
{
    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_DIAGNOSTICS_LASYSTEM_AIR_SYSTEM_TEST);
    qDebug() << "Start Air heating tube test";

    emit SetGUITabEnable(false);
    LaSystem::CAirHeatingTubeTest test(dlg);
    ErrorCode_t ret = (ErrorCode_t)test.Run();
    if (ret == RETURN_OK)
        Global::EventObject::Instance().RaiseEvent(EVENT_GUI_DIAGNOSTICS_LASYSTEM_AIR_SYSTEM_TEST_SUCCESS);
    else if (ret == RETURN_ABORT)
        Global::EventObject::Instance().RaiseEvent(EVENT_GUI_DIAGNOSTICS_TEST_ABORT, Global::tTranslatableStringList()<<" Air heating tube");
    else
        Global::EventObject::Instance().RaiseEvent(EVENT_GUI_DIAGNOSTICS_LASYSTEM_AIR_SYSTEM_TEST_FAILURE);

    emit SetGUITabEnable(true);
}
/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CLaSystem::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

} // namespace Diagnostics
