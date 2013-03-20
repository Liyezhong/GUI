/****************************************************************************/
/*! \file Components/Main/Include/Main.h
 *
 *  \brief Include file for main routine.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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
 * \param[in]   argc    Argument count.
 * \param[in]   argv    Argument list.
 * \return          Return code.
 */
/****************************************************************************/
int main(int argc, char *argv[]);

#endif // MAIN_H
