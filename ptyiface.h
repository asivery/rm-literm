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

#ifndef PTYIFACE_H
#define PTYIFACE_H

#include <QByteArray>
#include <QObject>
#include <QSize>
#include <QSocketNotifier>
#include <QStringConverter>

class Terminal;

class PtyIFace : public QObject
{
    Q_OBJECT
public:
    explicit PtyIFace(Terminal* term, const QString& charset, const QByteArray& terminalEnv, const QString& commandOverride, QObject* parent);
    virtual ~PtyIFace();

    void writeTerm(const QString& chars);
    bool failed() { return iFailed; }

    QString takeData()
    {
        QString tmp = m_pendingData;
        m_pendingData = QString();
        return tmp;
    }

private slots:
    void resize(int rows, int columns);
    void readActivated();
    void processDied();

signals:
    void dataAvailable();
    void terminalQuit();

private:
    Q_DISABLE_COPY(PtyIFace)

    void writeTerm(const QByteArray& chars);

    Terminal* iTerm;
    int iPid;
    int iMasterFd;
    bool iFailed;
    bool m_childProcessQuit;
    int m_childProcessPid;

    QSocketNotifier* iReadNotifier;
    QSocketNotifier* iDeathNotifier;

    QStringDecoder iTextCodec;
    QStringEncoder oTextCodec;

    QString m_pendingData;
};

#endif // PTYIFACE_H
