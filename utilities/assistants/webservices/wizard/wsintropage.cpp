/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2017-06-27
 * Description : a tool to export items to web services.
 *
 * Copyright (C) 2017-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "wsintropage.h"

// Qt includes

#include <QLabel>
#include <QPixmap>
#include <QComboBox>
#include <QIcon>
#include <QGroupBox>
#include <QGridLayout>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dlayoutbox.h"
#include "wswizard.h"
#include "wssettings.h"
#include "dbinarysearch.h"
#include "balsabinary.h"
#include "clawsmailbinary.h"
#include "kmailbinary.h"
#include "evolutionbinary.h"
#include "netscapebinary.h"
#include "sylpheedbinary.h"
#include "thunderbirdbinary.h"

namespace Digikam
{

class WSIntroPage::Private
{
public:

    Private(QWizard* const dialog)
      : imageGetOption(0),
        hbox(0),
        wizard(0),
        iface(0),
        binSearch(0)
    {
        wizard = dynamic_cast<WSWizard*>(dialog);

        if (wizard)
        {
            iface = wizard->iface();
        }
    }

    QComboBox*        imageGetOption;
    DHBox*            hbox;
    WSWizard*       wizard;
    DInfoInterface*   iface;
    DBinarySearch*    binSearch;
    BalsaBinary       balsaBin;
    ClawsMailBinary   clawsBin;
    EvolutionBinary   evoluBin;
    KmailBinary       kmailBin;
    NetscapeBinary    netscBin;
    SylpheedBinary    sylphBin;
    ThunderbirdBinary thundBin;
};

WSIntroPage::WSIntroPage(QWizard* const dialog, const QString& title)
    : DWizardPage(dialog, title),
      d(new Private(dialog))
{
    DVBox* const vbox  = new DVBox(this);
    QLabel* const desc = new QLabel(vbox);

    desc->setWordWrap(true);
    desc->setOpenExternalLinks(true);
    desc->setText(i18n("<qt>"
                       "<p><h1><b>Welcome to Email Tool</b></h1></p>"
                       "<p>This assistant will guide you to send "
                       "your items with a mail client application.</p>"
                       "<p>Before to export contents, you will be able to adjust attachments "
                       "properties accordingly with your mail service capabilities.</p>"
                       "</qt>"));

    // ComboBox for image selection method

    d->hbox                     = new DHBox(vbox);
    QLabel* const getImageLabel = new QLabel(i18n("&Choose image selection method:"), d->hbox);
    d->imageGetOption           = new QComboBox(d->hbox);
    d->imageGetOption->insertItem(WSSettings::ALBUMS, i18n("Albums"));
    d->imageGetOption->insertItem(WSSettings::IMAGES, i18n("Images"));
    getImageLabel->setBuddy(d->imageGetOption);

    // --------------------

    QGroupBox* const binaryBox      = new QGroupBox(vbox);
    QGridLayout* const binaryLayout = new QGridLayout;
    binaryBox->setLayout(binaryLayout);
    binaryBox->setTitle(i18nc("@title:group", "Mail client application Binaries"));
    d->binSearch = new DBinarySearch(binaryBox);
    d->binSearch->addBinary(d->balsaBin);
    d->binSearch->addBinary(d->clawsBin);
    d->binSearch->addBinary(d->kmailBin);
    d->binSearch->addBinary(d->evoluBin);
    d->binSearch->addBinary(d->netscBin);
    d->binSearch->addBinary(d->sylphBin);
    d->binSearch->addBinary(d->thundBin);

#ifdef Q_OS_OSX
    // Std Macports install
    d->binSearch->addDirectory(QLatin1String("/opt/local/bin"));

    // digiKam Bundle PKG install
    d->binSearch->addDirectory(QLatin1String("/opt/digikam/bin"));
#endif

#ifdef Q_OS_WIN
    // FIXME : adjust paths
    d->binSearch->addDirectory(QLatin1String("C:/Program Files/"));

    d->binSearch->addDirectory(QLatin1String("C:/Program Files (x86)/"));
#endif

    vbox->setStretchFactor(desc,      2);
    vbox->setStretchFactor(d->hbox,   1);
    vbox->setStretchFactor(binaryBox, 3);

    setPageWidget(vbox);
    setLeftBottomPix(QIcon::fromTheme(QLatin1String("mail-client")));

    connect(d->binSearch, SIGNAL(signalBinariesFound(bool)),
            this, SLOT(slotBinariesFound()));
}

WSIntroPage::~WSIntroPage()
{
    delete d;
}

void WSIntroPage::initializePage()
{
    bool albumSupport = (d->iface && d->iface->supportAlbums());

    if (!albumSupport)
    {
        d->imageGetOption->setCurrentIndex(WSSettings::IMAGES);
        d->hbox->setEnabled(false);
    }
    else
    {
        d->imageGetOption->setCurrentIndex(d->wizard->settings()->selMode);
    }

    d->binSearch->allBinariesFound();
    slotBinariesFound();
}

bool WSIntroPage::validatePage()
{
    d->wizard->settings()->selMode = (WSSettings::Selection)d->imageGetOption->currentIndex();

    return true;
}

void WSIntroPage::slotBinariesFound()
{
    d->wizard->settings()->binPaths.insert(WSSettings::BALSA, d->balsaBin.isValid() ?
                                           d->balsaBin.path() : QString());

    d->wizard->settings()->binPaths.insert(WSSettings::CLAWSMAIL, d->clawsBin.isValid() ?
                                           d->clawsBin.path() : QString());

    d->wizard->settings()->binPaths.insert(WSSettings::EVOLUTION, d->evoluBin.isValid() ?
                                           d->evoluBin.path() : QString());

    d->wizard->settings()->binPaths.insert(WSSettings::KMAIL, d->kmailBin.isValid() ?
                                           d->kmailBin.path() : QString());

    d->wizard->settings()->binPaths.insert(WSSettings::NETSCAPE, d->netscBin.isValid() ?
                                           d->netscBin.path() : QString());

    d->wizard->settings()->binPaths.insert(WSSettings::SYLPHEED, d->sylphBin.isValid() ?
                                           d->sylphBin.path() : QString());

    d->wizard->settings()->binPaths.insert(WSSettings::THUNDERBIRD, d->thundBin.isValid() ?
                                           d->thundBin.path() : QString());

    emit completeChanged();
}

bool WSIntroPage::isComplete() const
{
    QString val = d->wizard->settings()->binPaths.values().join(QString());
    qCDebug(DIGIKAM_GENERAL_LOG) << val;

    return (!val.isEmpty());
}

} // namespace Digikam
