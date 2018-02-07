/****************************************************************************/
/*! \file StackedLabel.cpp
 *
 *  \brief Implementation of file for class CStackedLabel.
 *
 *  \b Description:
 *          This is a customized label stacked widget for Himalaya Project.
 *
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2016-02-05
 *   $Author:  $ A.Yang
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2016 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#include "Dashboard/Include/StackedLabel.h"
#include "ui_StackedLabel.h"

namespace Dashboard {
    
CStackedLabel::CStackedLabel(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::StackedLabel),
    m_TextHighlight(false)
{
    ui->setupUi(this);
}

CStackedLabel::~CStackedLabel()
{
    delete ui;
}

void CStackedLabel::setHighlight(bool flag)
{
    m_TextHighlight = flag;
    if (flag == false) {
        this->setCurrentIndex(0);//normal
    }
    else {
        this->setCurrentIndex(1);//glow
    }
}

void CStackedLabel::setText(const QString &txt, bool highlight)
{
    m_Content = txt;
    m_TextHighlight = highlight;
    ui->labelNormal->setText(txt);
    ui->labelGlow->setText(txt, true);
}

}//end namespace
