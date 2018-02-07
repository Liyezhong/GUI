#ifndef REAGENTSUBMENUWIDGET_H
#define REAGENTSUBMENUWIDGET_H


#include "Global/Include/Utils.h"
#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/MainWindow.h"


#include "KeyBoard/Include/KeyBoard.h"

namespace Reagents {

namespace Ui {
    class CReagentSubMenuWidget;
}

/****************************************************************************/
/**
 * \brief Main  reagents widget listing all reagents available
 */
/****************************************************************************/
class CReagentSubMenuWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT
    friend class CTestReagents;

private:
    Ui::CReagentSubMenuWidget *mp_Ui;                     //!< User interface
    void RetranslateUI();

protected:
void changeEvent(QEvent *p_Event);

public:
    explicit CReagentSubMenuWidget(QWidget *p_Parent = 0);
    virtual ~CReagentSubMenuWidget();

signals:
    /****************************************************************************/
    /*!
     *  \brief Changes the selected settings widget
     *
     *  \iparam CurrentRow = Number of the widget
     */
    /****************************************************************************/
    void CurrentRowChanged(int CurrentRow);
};
}
#endif // REAGENTSUBMENUWIDGET_H

