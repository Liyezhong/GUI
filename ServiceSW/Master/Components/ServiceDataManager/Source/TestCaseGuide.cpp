/****************************************************************************/
/*! \file TestCaseGuide.cpp
 *
 *  \brief Implementation file for class CTestCaseGuide.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-5-21
 *   $Author:  $ Dixiong.li
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
#include <QDebug>
#include <QFile>
#include <QtXml/QXmlStreamReader>
#include "DataManager/Helper/Include/Helper.h"
#include "ServiceDataManager/Include/TestCaseGuide.h"

namespace DataManager {

CTestCaseGuide CTestCaseGuide::m_TestCaseGuide;

CTestCaseGuide::CTestCaseGuide()
{
}

CTestCaseGuide::~CTestCaseGuide()
{
    m_GuideHash.clear();
    m_TestCaseDescriptionHash.clear();
    m_TestCaseIDHash.clear();
}

GuideSteps CTestCaseGuide::GetGuideSteps(const QString& CaseName, int index)
{
    GuideSteps        Steps;
    QList<GuideSteps> Guides;
    QHash<QString, QList<GuideSteps> >::iterator itr = m_GuideHash.find(CaseName);

    if (itr != m_GuideHash.end() && (Guides = itr.value()).length() > index) {
        Steps = Guides[index];
    }
    else {
        qDebug()<<"CTestCaseGuide::Get guide string failed.";
    }

    return Steps;
}

QString CTestCaseGuide::GetTestCaseName(Service::ModuleTestCaseID Id)
{
    return m_TestCaseIDHash.value(Id);
}

Service::ModuleTestCaseID CTestCaseGuide::GetTestCaseId(const QString &TestCaseName)
{
    return m_TestCaseIDHash.key(TestCaseName);
}

QString CTestCaseGuide::GetTestCaseDescription(Service::ModuleTestCaseID Id)
{
    QString TestCaseName = m_TestCaseIDHash.value(Id);
    return m_TestCaseDescriptionHash.value(TestCaseName);
}

bool CTestCaseGuide::InitData(QString FileName)
{
    bool Result = true;
    QFile File(FileName);
    if (!QFile::exists(FileName)) {
        return false;
    }
    if (!File.open(QFile::ReadOnly | QFile::Text )) {
        qDebug() << "Open file to read failed " << FileName;
        return false;
    }

    if (!DeserializeContent(File, false)) {
        qDebug() << " CTestCaseGuide::Read failed for file during deserializing: " << FileName;
        return false;
    }

    File.close();

    return Result;
}

bool CTestCaseGuide::DeserializeContent(QIODevice& IODevice, bool CompleteData)
{
    bool Result = true;
    GuideSteps        Steps;
    QList<GuideSteps> Guides;
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(&IODevice);

    // look for node <TestCaseGuide>
    if (!Helper::ReadNode(XmlStreamReader, "TestCaseGuide")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: TestCaseGuide";
        return false;
    }

    // Read attribute Name
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << " attribute ServiceParameters <Version> is missing => abort reading";
        return false;
    }

    while ((!XmlStreamReader.atEnd()))
    {
        (void) XmlStreamReader.readNext();
        QString ElementName = XmlStreamReader.name().toString();
        QXmlStreamAttributes Attributes = XmlStreamReader.attributes();

        if (XmlStreamReader.isStartElement()) {
            if (ElementName == "Step") {
                Steps<<XmlStreamReader.readElementText();
            }
            else if (ElementName != "TestCaseGuide" && ElementName != "Guide") {
                if (Attributes.size()>0) {
                    m_TestCaseDescriptionHash.insert(ElementName, Attributes[0].value().toString());
                }
                SavetoIDHash(ElementName);
            }
        }
        else if (XmlStreamReader.isEndElement()) {
            if (ElementName == "Guide") {
                Guides<<Steps;
                Steps.clear();
            }
            else if (ElementName != "Step" && ElementName != "TestCaseGuide") {
                m_GuideHash.insert(ElementName, Guides);
                Guides.clear();
            }
        }
    }

    if(CompleteData) {

    }
    return Result;
}

void CTestCaseGuide::SavetoIDHash(const QString &TestCaseName)
{
    Service::ModuleTestCaseID Id;

    qDebug()<<"SavetoIdHash name="<<TestCaseName;
    if (TestCaseName == "OvenHeatingEmpty") {
        Id = Service::OVEN_HEATING_EMPTY;
    }
    else if (TestCaseName == "OvenHeatingWater") {
        Id = Service::OVEN_HEATING_WITH_WATER;
    }
    else if (TestCaseName == "OvenCoverSensor") {
        Id = Service::OVEN_COVER_SENSOR;
    }
    else if (TestCaseName == "EBoxASB3") {
        Id = Service::EBOX_ASB3;
    }
    else if (TestCaseName == "EBoxASB5") {
        Id = Service::EBOX_ASB5;
    }
    else if (TestCaseName == "EBoxASB15") {
        Id = Service::EBOX_ASB15;
    }

    m_TestCaseIDHash.insert(Id, TestCaseName);
}

} //end of namespace Datamanager
