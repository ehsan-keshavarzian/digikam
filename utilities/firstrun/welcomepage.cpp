/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-28-04
 * Description : first run assistant dialog
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "welcomepage.h"

// Qt includes

#include <QLabel>

// KDE includes

#include <kvbox.h>
#include <klocale.h>

namespace Digikam
{

WelcomePage::WelcomePage(KAssistantDialog* dlg)
           : AssistantDlgPage(dlg, i18n("Welcome To digiKam"))
{
    KVBox *vbox   = new KVBox(this);
    QLabel *title = new QLabel(vbox);
    title->setWordWrap(true);
    title->setText(i18n("<qt>"
                        "<p><h1><b>Welcome To digiKam</b></h1></p>"
                        "<p>digiKam is an advanced digital photo management application published "
                        "in open-source.</p>"
                        "<p>This assistant will help you to configure first run settings to be able to "
                        "use digiKam quickly.</p>"
                        "</qt>"));

    setContentsWidget(vbox);
}

WelcomePage::~WelcomePage()
{
}

}   // namespace Digikam
