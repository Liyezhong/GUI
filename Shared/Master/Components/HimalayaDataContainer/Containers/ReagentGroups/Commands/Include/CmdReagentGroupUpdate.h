/****************************************************************************/
/*! \file CmdReagentGroupUpdate.h
 *
 *  \brief Update reagent group command definition.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-02-02
 *  $Author:    $ R. Wu
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

#ifndef MSGCLASSES_CMDREAGENTGROUPUPDATE_H
#define MSGCLASSES_CMDREAGENTGROUPUPDATE_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {


class CmdReagentGroupUpdate : public Global::Command
{
    friend QDataStream & operator << (QDataStream &, const CmdReagentGroupUpdate &);
    friend QDataStream & operator >> (QDataStream &, CmdReagentGroupUpdate &);
public:
    CmdReagentGroupUpdate(int TimeOut, const QString& GroupId, const QString& color);
    CmdReagentGroupUpdate(void);

    ~CmdReagentGroupUpdate(void);

    virtual QString GetName(void) const;

    static QString NAME;
    inline const QString& GroupId()const {return m_GroupId;}
    inline const QString& GroupColor()const {return m_Color;}
private:
    CmdReagentGroupUpdate(const CmdReagentGroupUpdate &);
    const CmdReagentGroupUpdate &operator = (const CmdReagentGroupUpdate &);
    QString m_Color;
    QString m_GroupId;
}; // end class CmdReagentGroupUpdate

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdReagentGroupUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_Color;
    Stream << Cmd.m_GroupId;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdReagentGroupUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_Color;
    Stream >> Cmd.m_GroupId;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDREAGENTGROUPUPDATE_H
