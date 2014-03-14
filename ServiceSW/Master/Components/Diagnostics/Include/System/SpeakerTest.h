/****************************************************************************/
/*! \file SpeakerTest.h
 *
 *  \brief Declaration of System speaker test.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-05-28
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

#ifndef DIAGNOSTICS_SYSTEM_SPEAKERTEST_H
#define DIAGNOSTICS_SYSTEM_SPEAKERTEST_H

#include "Diagnostics/Include/TestBase.h"

namespace Diagnostics {

namespace System {

class CSpeakerTest : public Diagnostics::CTestBase
{
    Q_OBJECT

public:
    CSpeakerTest(void);
    ~CSpeakerTest(void);

    void Run(void);

public Q_SLOTS:
    void FirstOpenDialog(void);

private Q_SLOTS:
    void TestSpeaker(int Sound, int Volume);

    void SecondConfirmResult(void);

    void Succeed(void);

    void Fail(void);

    void Cancel(void);
};

} // namespace System

} // namespace Diagnostics

#endif // DIAGNOSTICS_SYSTEM_SPEAKERTEST_H
