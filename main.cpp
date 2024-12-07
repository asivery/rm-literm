/*
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

#include <QDir>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QScreen>
#include <QString>

#include "keyloader.h"
#include "textrender.h"
#include "utilities.h"
#include "version.h"
#include <dlfcn.h>

static void copyFileFromResources(QString from, QString to);

extern "C" void registerQMLTypes()
{
    qmlRegisterType<TextRender>("literm", 1, 0, "TextRender");
    qmlRegisterUncreatableType<Util>("literm", 1, 0, "Util", "Util is created by app");

    QString settings_path(QDir::homePath() + "/.config/literm");
    QDir dir;

    if (!dir.exists(settings_path)) {
        // Migrate FingerTerm settings if present
        QString old_settings_path(QDir::homePath() + "/.config/FingerTerm");
        if (dir.exists(old_settings_path)) {
            if (!dir.rename(old_settings_path, settings_path))
                qWarning() << "Could not migrate FingerTerm settings path" << old_settings_path << "to" << settings_path;
        } else if (!dir.mkdir(settings_path))
            qWarning() << "Could not create literm settings path" << settings_path;
    }

    QString settingsFile = settings_path + "/settings.ini";

    Util *util = new Util(settingsFile);
    qmlRegisterSingletonInstance("literm", 1, 0, "Util", util);

    QString startupErrorMsg;

    // copy the default config files to the config dir if they don't already exist
    copyFileFromResources(":/literm/data/menu.xml", util->configPath() + "/menu.xml");
    copyFileFromResources(":/literm/data/english.layout", util->configPath() + "/english.layout");
    copyFileFromResources(":/literm/data/finnish.layout", util->configPath() + "/finnish.layout");
    copyFileFromResources(":/literm/data/french.layout", util->configPath() + "/french.layout");
    copyFileFromResources(":/literm/data/german.layout", util->configPath() + "/german.layout");
    copyFileFromResources(":/literm/data/qwertz.layout", util->configPath() + "/qwertz.layout");

    KeyLoader *keyLoader = new KeyLoader();
    keyLoader->setUtil(util);
    qmlRegisterSingletonInstance("literm", 1, 0, "KeyLoader", keyLoader);
    bool ret = keyLoader->loadLayout(util->keyboardLayout());
    if (!ret) {
        // on failure, try to load the default one (english) directly from resources
        startupErrorMsg = "There was an error loading the keyboard layout.<br>\nUsing the default one instead.";
        util->setKeyboardLayout("english");
        ret = keyLoader->loadLayout(":/literm/data/english.layout");
        if (!ret)
            qFatal("failure loading keyboard layout");
    }
}

static void copyFileFromResources(QString from, QString to)
{
    // copy a file from resources to the config dir if it does not exist there
    QFileInfo toFile(to);
    if (!toFile.exists()) {
        QFile newToFile(toFile.absoluteFilePath());
        QResource res(from);
        if (newToFile.open(QIODevice::WriteOnly)) {
            newToFile.write(reinterpret_cast<const char*>(res.data()));
            newToFile.close();
        } else {
            qWarning() << "Failed to copy default config from resources to" << toFile.filePath();
        }
    }
}
