#ifndef DASHBOARD_STACKEDLABEL_H
#define DASHBOARD_STACKEDLABEL_H

#include <QStackedWidget>
namespace Dashboard {

namespace Ui {
class StackedLabel;
}

/****************************************************************************/
/*!
 *  \brief  Definition of class CStackedLabel, it is used to show the favorite programs
 *
 */
/****************************************************************************/
class CStackedLabel : public QStackedWidget
{
    Q_OBJECT
    
public:
    /****************************************************************************/
    /*!
     *  \brief  Constructor of class CStackedLabel
     *
     *  \param parent = the parent of this widget
     *
     *  \return from CStackedLabel
     */
    /****************************************************************************/
    explicit CStackedLabel(QWidget *parent = 0);
    ~CStackedLabel();
   /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function setText
     *
     *  \param text =  text to show in this label
     *  \param highlight =  highlight or not
     *
     */
    /****************************************************************************/
    void setText(const QString &text, bool highlight = false);
    /****************************************************************************/
    /*!
     *  \brief  check this label is in highlight or not
     *  \return from IsTextHighlight
     *
     */
    /****************************************************************************/
    inline bool IsTextHighlight() {return m_TextHighlight; }    
public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function setHighlight
     *
     *  \param flag =  enable/disable highlight flag
     *
     */
    /****************************************************************************/
    void setHighlight(bool flag);    
private:
    Ui::StackedLabel *ui;
    QString m_Content;       ///<  Definition/Declaration of variable m_Text
    bool m_TextHighlight;    ///<  whether the text shall be highlighted or not
};

}//end namespace
#endif // DASHBOARD_STACKEDLABEL_H
