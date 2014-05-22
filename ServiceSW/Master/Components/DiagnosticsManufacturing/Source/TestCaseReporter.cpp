#include <QList>
#include <QDateTime>
#include "DiagnosticsManufacturing/Include/TestCaseReporter.h"
#include "Global/Include/SystemPaths.h"
#include "Global/Include/AdjustedTime.h"
#include "ServiceDataManager/Include/TestCaseFactory.h"

namespace DiagnosticsManufacturing {

CTestCaseReporter::CTestCaseReporter(QString ModuleName, QString SerialNumber):
    m_ModuleName(ModuleName),
    m_SerialNumber(SerialNumber)
{
}

CTestCaseReporter::~CTestCaseReporter()
{

}

bool CTestCaseReporter::GenReportFile()
{
    QString TempFilePath = Global::SystemPaths::Instance().GetTempPath();
    QString ReportName = m_SerialNumber + "_" + m_ModuleName;
    QString FileName = TempFilePath + "/" + ReportName + ".txt";
    QFile File(FileName);

    if (!File.open(QFile::WriteOnly | QFile::Text )) {
        qDebug() << "CTestCaseReporter:Open file to write failed " << FileName;
        return false;
    }

    QTextStream TestStream(&File);
    FillReport(TestStream);

    m_TestReportFile = FileName;

    File.close();

    return true;
}

void CTestCaseReporter::FillReport(QTextStream& TextStream)
{
    QDateTime DateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QString CurrentDateTime = DateTime.toString("yyyy/MM/dd-hh:mm:ss");
    TextStream<<QString("Module Name: %1\n").arg(m_ModuleName);
    TextStream<<QString("Time Stamp: %1\n").arg(CurrentDateTime);
    TextStream<<QString("Serial Number: %1\n").arg(m_SerialNumber);

    QList<DataManager::CTestCase*> TestCases = DataManager::CTestCaseFactory::Instance().GetModuleTestCase(m_ModuleName);
    QList<DataManager::CTestCase*>::iterator itr = TestCases.begin();

    for (; itr != TestCases.end(); ++itr) {
        TextStream<<(*itr)->GenReport();
    }
}

}  // end of namespace DiagnosticsManufacturing
