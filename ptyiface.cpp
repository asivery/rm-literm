/*
    Copyright (C) 2017 Robin Burchell <robin+git@viroteck.net>
    Copyright 2011-2012 Heikki Holstila <heikki.holstila@gmail.com>

    This work is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This work is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this work.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QAbstractEventDispatcher>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

extern "C" {
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#if defined(Q_OS_LINUX)
#    include <pty.h>
#elif defined(Q_OS_MAC)
#    include <util.h>
#endif
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
}

#include "ptyiface.h"
#include "terminal.h"


PtyIFace::PtyIFace(Terminal* term, const QString& charset, const QByteArray& termEnv, const QString& commandOverride, QObject* parent)
    : QObject(parent)
    , iTerm(term)
    , iFailed(false)
    , m_childProcessQuit(false)
    , m_childProcessPid(0)
    , iReadNotifier(0)
    , iDeathNotifier(0)
    , iTextCodec(QStringDecoder(QStringConverter::Utf8))
    , oTextCodec(QStringEncoder(QStringConverter::Utf8))
{
    // fork the child process before creating QGuiApplication
    int socketM;
    int pid = forkpty(&socketM, NULL, NULL, NULL);
    if (pid == -1) {
        qFatal("forkpty failed");
        exit(1);
    } else if (pid == 0) {
        setenv("TERM", termEnv, 1);

        QString execCmd;
        bool next = false;
        // ### this belongs elsewhere
        for (int i = 0; i < qApp->arguments().count(); ++i) {
            if (next) {
                execCmd = qApp->arguments().at(i);
                break;
            }
            if (qApp->arguments().at(i) == "-e") {
                next = true;
            }
        }
        if (execCmd.isEmpty()) {
            execCmd = commandOverride;
        }
        if (execCmd.isEmpty()) {
            // execute the user's default shell
            passwd* pwdstruct = getpwuid(getuid());
            execCmd = QString(pwdstruct->pw_shell);
            execCmd.append(" --login");
        }

        QStringList execParts = execCmd.split(' ', Qt::SkipEmptyParts);
        if (execParts.length() == 0)
            exit(0);
        char* ptrs[execParts.length() + 1];
        for (int i = 0; i < execParts.length(); i++) {
            ptrs[i] = new char[execParts.at(i).toLatin1().length() + 1];
            memcpy(ptrs[i], execParts.at(i).toLatin1().data(), execParts.at(i).toLatin1().length());
            ptrs[i][execParts.at(i).toLatin1().length()] = 0;
        }
        ptrs[execParts.length()] = 0;

        execvp(execParts.first().toLatin1(), ptrs);
        exit(0);
    }

    iPid = pid;
    iMasterFd = socketM;

    m_childProcessPid = iPid;

    if (!iTerm || m_childProcessQuit) {
        iFailed = true;
        qFatal("PtyIFace: null Terminal pointer");
    }

    resize(iTerm->rows(), iTerm->columns());
    connect(iTerm, SIGNAL(termSizeChanged(int, int)), this, SLOT(resize(int, int)));

    iReadNotifier = new QSocketNotifier(iMasterFd, QSocketNotifier::Read, this);
    connect(iReadNotifier, SIGNAL(activated(int)), this, SLOT(readActivated()));

    iDeathNotifier = new QSocketNotifier(iMasterFd, QSocketNotifier::Exception, this);
    connect(iDeathNotifier, SIGNAL(activated(int)), this, SLOT(processDied()));

    fcntl(iMasterFd, F_SETFL, O_NONBLOCK); // reads from the descriptor should be non-blocking

    if (!charset.isEmpty()) {
        auto encoding = QStringConverter::encodingForName(charset.toLatin1());
        if(auto e = encoding) {
            iTextCodec = QStringDecoder(*e);
            oTextCodec = QStringEncoder(*e);
        }
    }
}

PtyIFace::~PtyIFace()
{
    if (!m_childProcessQuit) {
        // make the process quit
        kill(iPid, SIGHUP);
        kill(iPid, SIGTERM);
    }
}

void PtyIFace::processDied(){
    if (m_childProcessQuit) {
        return;
    }
    m_childProcessQuit = true;
    qDebug() << "Process died!!";
    emit terminalQuit();
}

void PtyIFace::readActivated()
{
    if (m_childProcessQuit)
        return;

    int ret = 0;
    char ch[4096];
    ret = read(iMasterFd, ch, sizeof(ch));
    if (iTerm && ret > 0) {
        m_pendingData += iTextCodec(QByteArray::fromRawData(ch, ret));
        emit dataAvailable();
    }
}

void PtyIFace::resize(int rows, int columns)
{
    if (m_childProcessQuit)
        return;

    winsize winp;
    winp.ws_col = columns;
    winp.ws_row = rows;

    ioctl(iMasterFd, TIOCSWINSZ, &winp);
}

void PtyIFace::writeTerm(const QString& chars)
{
    writeTerm(oTextCodec(chars));
}

void PtyIFace::writeTerm(const QByteArray& chars)
{
    if (m_childProcessQuit)
        return;

    int ret = write(iMasterFd, chars, chars.size());
    if (ret != chars.size())
        qDebug() << "write error!";
}
