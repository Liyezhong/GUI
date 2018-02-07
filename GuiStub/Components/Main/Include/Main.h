/****************************************************************************/
/*! \file Components/Main/Include/Main.h
 *
 *  \brief Include file for main routine.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-12-26
 *  $Author:    $ BhanuPrasad Hulsoor
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

#ifndef MAIN_H
#define MAIN_H

/****************************************************************************/
/**
 * \brief Main function.
 *
 * Basicly the following is done: the main thread controller is created and
 * started in its own thread. Also the application object is started.
 * As soon as the main threads finishes, the application is also terminated.
 *
 * \param[in]   Argc      Argument count.
 * \param[in]   p_Argv    Argument list.
 * \return          Return code.
 */
/****************************************************************************/
int main(int Argc, char *p_Argv[]);

#endif // MAIN_H
