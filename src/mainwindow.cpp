/*
 * Tiled Map Editor (Qt port)
 * Copyright 2008 Tiled (Qt port) developers (see AUTHORS file)
 *
 * This file is part of Tiled (Qt port).
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#include "mainwindow.h"

#include "layer.h"
#include "map.h"
#include "mapscene.h"
#include "resizedialog.h"
#include "xmlmapreader.h"

#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

using namespace Tiled::Internal;

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    mUi.setupUi(this);
    mUi.splitter->setCollapsible(0, false);

    mUi.actionOpen->setShortcut(QKeySequence::Open);
    mUi.actionSave->setShortcut(QKeySequence::Save);
    mUi.actionCopy->setShortcut(QKeySequence::Copy);
    mUi.actionPaste->setShortcut(QKeySequence::Paste);

    connect(mUi.actionOpen, SIGNAL(triggered()), SLOT(openFile()));
    connect(mUi.actionSave, SIGNAL(triggered()), SLOT(saveFile()));
    connect(mUi.actionQuit, SIGNAL(triggered()), SLOT(close()));
    connect(mUi.actionResize, SIGNAL(triggered()), SLOT(resizeMap()));
    connect(mUi.actionAbout, SIGNAL(triggered()), SLOT(aboutTiled()));
    connect(mUi.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


    QMenu* menu = new QMenu(this);
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
         mRecentFiles[i] = new QAction(this);
         menu->addAction(mRecentFiles[i]);
         mRecentFiles[i]->setVisible(false);
         connect(mRecentFiles[i], SIGNAL(triggered()),
                 this, SLOT(openRecentFile()));
    }
    mUi.actionRecent_Files->setMenu(menu);

    mScene = new MapScene(this);
    mUi.graphicsView->setScene(mScene);
    mUi.graphicsView->centerOn(0, 0);
    mUi.actionResize->setEnabled(false);
    readSettings();
}

MainWindow::~MainWindow()
{
    writeSettings();
}

void MainWindow::openFile(const QString &fileName)
{
    // Use the XML map reader to read the map (assuming it's a .tmx file)
    if (!fileName.isEmpty()) {
        qDebug() << "Loading map:" << fileName;
        XmlMapReader mapReader;
        Map *map = mapReader.read(fileName);
        Map *previousMap = mScene->map();
        mScene->setMap(map);
        mUi.graphicsView->centerOn(0, 0);
        mUi.actionResize->setEnabled(map != 0);
        delete previousMap;

        if (map) {
            setWindowFilePath(fileName);
            setRecentFile(fileName);
        } else {
            setWindowFilePath(QString());
        }
    }
}

void MainWindow::openFile()
{
    QStringList recentFiles = mSettings.value("recentFiles",
                                              QStringList()).toStringList();
    QString start;
    if (!recentFiles.isEmpty())
        start = recentFiles.first();

    openFile(QFileDialog::getOpenFileName(this, tr("Open Map"), start));
}

void MainWindow::saveFile()
{
    const QString fileName = QFileDialog::getSaveFileName(this);
    qDebug() << fileName;
}

void MainWindow::resizeMap()
{
    ResizeDialog resizeDialog(this);
    resizeDialog.setOldSize(QSize(mScene->map()->width(),
                                  mScene->map()->height()));

    if (resizeDialog.exec()) {
        mScene->map()->setWidth(resizeDialog.newSize().width());
        mScene->map()->setHeight(resizeDialog.newSize().height());
    }
    // TODO: Actually implement map resizing
}

void MainWindow::readSettings()
{
    mSettings.beginGroup("mainwindow");
    resize(mSettings.value("size", QSize(553, 367)).toSize());
    mSettings.endGroup();
    updateRecentFiles();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        openFile(action->data().toString());
}

void MainWindow::setRecentFile(const QString &fileName)
{
    QStringList files = mSettings.value("recentFiles").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    mSettings.setValue("recentFiles", files);
    updateRecentFiles();
}

void MainWindow::updateRecentFiles()
{
    QStringList recentFiles = mSettings.value("recentFiles").toStringList();
    const int numRecentFiles = qMin(recentFiles.size(), (int) MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        mRecentFiles[i]->setText(QFileInfo(recentFiles[i]).fileName());
        mRecentFiles[i]->setData(recentFiles[i]);
        mRecentFiles[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    {
        mRecentFiles[j]->setVisible(false);
    }
}

void MainWindow::writeSettings()
{
    mSettings.beginGroup("mainwindow");
    mSettings.setValue("size", size());
    mSettings.endGroup();
}

void MainWindow::aboutTiled()
{
    // TODO: Implement about dialog
}