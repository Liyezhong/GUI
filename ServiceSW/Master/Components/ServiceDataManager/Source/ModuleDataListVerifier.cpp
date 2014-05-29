/****************************************************************************/
/*! \file ModuleDataListVerifier.cpp
 *
 *  \brief CModuleDataListVerifier class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-08
 *   $Author:  $ Soumya D
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "ServiceDataManager/Include/ModuleDataListVerifier.h"

//lint -e613

namespace ServiceDataManager
{

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CModuleDataListVerifier::CModuleDataListVerifier(): mp_MDL(NULL)
{
}

/****************************************************************************/
/*!
 *  \brief Verifies the data present in the module list against constraints.
 *  \iparam p_ModuleDataList
 *  \return true - verify success , false - verify failure
 */
/****************************************************************************/
bool CModuleDataListVerifier::VerifyData(DataManager::CDataContainerBase *p_ModuleDataList)
{
    bool VerifiedData = true;

    mp_MDL = static_cast<CModuleDataList*>(p_ModuleDataList);

    if(mp_MDL == NULL) {
        qDebug() << "Module List Data container is empty" << endl;
        VerifiedData = false;
    }

    QString InstrumentName = mp_MDL->GetInstrumentName();
    if(InstrumentName != "ST8200") {
        qDebug() << "Instrument Does not Exists" << endl;
        VerifiedData = false;
    }

    //!< Check the content of each Module
    CModule *p_Module = new CModule();
    QStringList ModuleList;
    ModuleList << "Drawer Left" << "Drawer Right" << "Oven" << "YZ_Arm_Left" << "YZ_Arm_Right"
               << "X-Arm" << "Agitator" << "Transfer" << "E Box" << "SlideID" << "HeatedCuvetts"
               << "Water" << "Exhaust" << "Display" << "Cover" << "JoyStic" << "RFID Consumables";


    for(qint32 i=0; i<mp_MDL->GetNumberofModules(); i++)
    {
        *p_Module = *mp_MDL->GetModule(i);

        if (p_Module) {

            QString ModuleName = p_Module->GetModuleName();

            if(!ModuleList.contains(ModuleName, Qt::CaseSensitive)) {
                qDebug() << "Unkown Module Name" << endl;
                VerifiedData = false;
            }
        }
    }

    CSubModule *p_SubModule = new CSubModule();
    for(qint32 j=0; j<p_Module->GetNumberofSubModules(); j++)
    {
        *p_SubModule = *p_Module->GetSubModuleInfo(j);

        if(!p_SubModule) {
            qDebug() << "Sub Module Data does not Exist" << endl;
            VerifiedData = false;
        }
    }

    delete p_SubModule;
    delete p_Module;

    return VerifiedData;
}

/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorMap_t& CModuleDataListVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorMap;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CModuleDataListVerifier::ResetErrors()
{
    m_ErrorMap.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 *  \return true
 */
/****************************************************************************/
bool CModuleDataListVerifier::IsLocalVerifier()
{
    return true;
}

}