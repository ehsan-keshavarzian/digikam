/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2016-08-14
 * Description : CLI tool to test to load metadata from images through multi-core threads.
 *
 * Copyright (C) 2016 by by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "metareaderthread.h"

// Qt includes

#include <QDirIterator>
#include <QDebug>
#include <QApplication>
#include <QElapsedTimer>

// Local includes

#include "dmetadata.h"
#include "metaengine.h"

class Mytask : public ActionJob
{
public:

    Mytask()
        : ActionJob()
    {
    }

    MetadataSettingsContainer settings;
    QUrl                      url;

protected:

    void run()
    {
        DMetadata meta;
        meta.setSettings(settings);

        if (!meta.load(url.toLocalFile()))
        {
            qDebug() << url.fileName() << " : cannot load metadata!";
        }
        else
        {
            qDebug() << url.fileName() << meta.getPhotographInformation();
        }

        emit signalDone();
    }
};

// ----------------------------------------------------------------------------------------------------

MetaReaderThread::MetaReaderThread(QObject* const parent)
    : ActionThreadBase(parent)
{
}

void MetaReaderThread::readMetadata(const QList<QUrl>& list, const MetadataSettingsContainer& settings)
{
    ActionJobCollection collection;

    foreach (const QUrl& url, list)
    {
        Mytask* const job = new Mytask();
        job->url          = url;
        job->settings     = settings;

        collection.insert(job, 0);

        qDebug() << "Appending file to process " << url;
    }

    appendJobs(collection);
}

void MetaReaderThread::slotJobFinished()
{
    ActionThreadBase::slotJobFinished();

    if (isEmpty())
        emit done();
}

// ----------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    if (argc < 3)
    {
        qDebug() << "metareaderthread - test to load metadata from images through multi-core threades";
        qDebug() << "Usage  : <images path> <image file filter> ... <image file filter>";
        qDebug() << "Example: /mnt/photos *.jpg *.png *.tif *.nef *.dng";
        return -1;
    }

    QString path = QString::fromLocal8Bit(argv[1]);
    qDebug() << "Images path : " << path;
    QStringList filters;

    for (int i = 2 ; i < argc ; i++)
        filters << QString::fromLocal8Bit(argv[i]);

    if (filters.isEmpty())
    {
        qDebug() << "Image filters list is empty!";
        return -1;
    }

    QList<QUrl> list;
    QDirIterator it(path, filters,
                    QDir::Files,
                    QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        QString path = it.next();
        list.append(QUrl::fromLocalFile(path));
    }

    if (list.isEmpty())
    {
        qDebug() << "Files list to process is empty!";
        return -1;
    }

    MetaEngine::initializeExiv2();
    MetadataSettingsContainer settings;

    MetaReaderThread* const thread = new MetaReaderThread(&app);
    thread->readMetadata(list, settings);

    QElapsedTimer timer;
    timer.start();

    thread->start();

    QObject::connect(thread, SIGNAL(done()),
                     &app, SLOT(quit()));

    app.exec();

    qDebug() << "Reading metadata from " << list.size() << " files took " << timer.elapsed()/1000.0 << " seconds";

    MetaEngine::cleanupExiv2();

    return 0;
}