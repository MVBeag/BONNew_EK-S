/*
 * main.cpp
 *
 * Copyright (C) 2015 NetModule AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

//--------------------------------------------------------------

#include <QApplication>
#include <QtQml>
#include <QMutex>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "updatedriver.h"

//--------------------------------------------------------------

void  extractFile(QString fileName, QString osPath, QString qrcPath)
{
  QString fileNameQrc = ":/";
  fileNameQrc.append(qrcPath);
  fileNameQrc.append(fileName);

  QString fileNameOs = fileName;
  fileNameOs.prepend(osPath);

  QFile file_R(fileNameQrc);
  QFile file_W(fileNameOs);

  if ( !file_R.open(QFile::ReadOnly) )
  {
    qDebug() << "file_R.open() failed!";
    return;
  }

  if ( !file_W.open(QFile::WriteOnly) )
  {
    qDebug() << "file_W.open() failed!";
    file_R.close();
    return;
  }

  while ( !file_R.atEnd() )
  {
    QByteArray line = file_R.readLine();
    file_W.write(line, line.length());
  }

  file_W.close();
  file_R.close();
}

//--------------------------------------------------------------

bool createFiles()
{
#ifdef __APPLE__
  extractFile("SirosUpdater.zip", "/tmp/", "");
#else
  extractFile("SirosUpdater.zip", "", "");

  if ( !QDir("QtQuick").exists() )
  {
    QDir().mkdir("QtQuick");

    QDir().mkdir("QtQuick/Controls");
    extractFile("QtQuick/Controls/qmldir", "", "imp/");

    QDir().mkdir("QtQuick/Layouts");
    extractFile("QtQuick/Layouts/qmldir", "", "imp/");

    QDir().mkdir("QtQuick/Window.2");
    extractFile("QtQuick/Window.2/qmldir", "", "imp/");
  }

  if ( !QDir("QtQuick.2").exists() )
  {
    QDir().mkdir("QtQuick.2");
    extractFile("QtQuick.2/qmldir", "", "imp/");
  }

  if ( !QFile("QtQuick/Controls/qmldir").exists() ||
       !QFile("QtQuick/Layouts/qmldir").exists() ||
       !QFile("QtQuick/Window.2/qmldir").exists() ||
       !QFile("QtQuick.2/qmldir").exists() )
  {
    return false;
  }

  extractFile("d3dcompiler_47.dll", "", "egl/");
  extractFile("libEGL.dll", "", "egl/");
  extractFile("libGLESv2.dll", "", "egl/");

  if ( !QFile("d3dcompiler_47.dll").exists() ||
       !QFile("libEGL.dll").exists() ||
       !QFile("libGLESv2.dll").exists() )
  {
    return false;
  }
#endif

  return true;
}

//--------------------------------------------------------------

int main(int argc, char *argv[])
{
  int nError = -1;
  QApplication app(argc, argv);

  if ( !createFiles() )
  {
    qDebug() << "createFiles() failed!";
    return (nError);
  }

  Q_INIT_RESOURCE(SirosUpdater);
  qmlRegisterType<UpdateDriver>("Drivers", 1, 0, "UpdateDriver");

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  nError = app.exec();

  return (nError);
} // main
