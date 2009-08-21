/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-11-24
 * Description : Color management setup tab.
 *
 * Copyright (C) 2005-2007 by F.J. Cruz <fj.cruz@supercable.es>
 * Copyright (C) 2005-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "setupicc.h"
#include "setupicc.moc"

// Qt includes

#include <QButtonGroup>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QPushButton>
#include <QStringList>
#include <QMap>
#include <QDir>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QWhatsThis>

// KDE includes

#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpagedialog.h>
#include <kstandarddirs.h>
#include <ktabwidget.h>
#include <ktoolinvocation.h>
#include <kurllabel.h>
#include <kurlrequester.h>
#include <kvbox.h>

// LibKDcraw includes

#include <libkdcraw/squeezedcombobox.h>

// Local includes

#include "iccprofileinfodlg.h"
#include "iccprofilescombobox.h"
#include "iccsettings.h"
#include "iccsettingscontainer.h"
#include "albumsettings.h"

using namespace KDcrawIface;

namespace Digikam
{

class SetupICCPriv
{
public:

     SetupICCPriv()
     {
        enableColorManagement = 0;
        defaultSRGBConvert    = 0;
        bpcAlgorithm          = 0;
        managedView           = 0;
        defaultAskMismatch    = 0;
        defaultConvertMismatch= 0;
        defaultAskMissing     = 0;
        defaultSRGBMissing    = 0;
        defaultWSMissing      = 0;
        defaultInputMissing   = 0;
        defaultAskRaw         = 0;
        defaultInputRaw       = 0;
        defaultGuessRaw       = 0;
        infoWorkProfiles      = 0;
        infoMonitorProfiles   = 0;
        infoInProfiles        = 0;
        infoProofProfiles     = 0;
        workspaceGB           = 0;
        mismatchGB            = 0;
        missingGB             = 0;
        rawGB                 = 0;
        inputGB               = 0;
        viewGB                = 0;
        proofGB               = 0;
        advancedSettingsGB    = 0;
        defaultPathKU         = 0;
        renderingIntentKC     = 0;
        inProfilesKC          = 0;
        workProfilesKC        = 0;
        proofProfilesKC       = 0;
        monitorProfilesKC     = 0;
        iccFolderLabel        = 0;
        behaviorPanel         = 0;
        profilesPanel         = 0;
        advancedPanel         = 0;
     }

    QLabel                 *iccFolderLabel;

    QCheckBox              *enableColorManagement;
    QCheckBox              *defaultSRGBConvert;
    QCheckBox              *bpcAlgorithm;
    QCheckBox              *managedView;

    QRadioButton           *defaultAskMismatch;
    QRadioButton           *defaultConvertMismatch;
    QRadioButton           *defaultAskMissing;
    QRadioButton           *defaultSRGBMissing;
    QRadioButton           *defaultWSMissing;
    QRadioButton           *defaultInputMissing;
    QRadioButton           *defaultAskRaw;
    QRadioButton           *defaultInputRaw;
    QRadioButton           *defaultGuessRaw;

    QPushButton            *infoWorkProfiles;
    QPushButton            *infoMonitorProfiles;
    QPushButton            *infoInProfiles;
    QPushButton            *infoProofProfiles;

    QGroupBox              *workspaceGB;
    QGroupBox              *mismatchGB;
    QGroupBox              *missingGB;
    QGroupBox              *rawGB;
    QGroupBox              *inputGB;
    QGroupBox              *viewGB;
    QGroupBox              *proofGB;
    QGroupBox              *iccFolderGB;
    QGroupBox              *advancedSettingsGB;

    KUrlRequester          *defaultPathKU;

    KComboBox              *renderingIntentKC;

    QWidget                *behaviorPanel;
    QWidget                *profilesPanel;
    QWidget                *advancedPanel;
    KTabWidget             *tab;
    KPageDialog            *mainDialog;

    IccProfilesComboBox    *inProfilesKC;
    IccProfilesComboBox    *workProfilesKC;
    IccProfilesComboBox    *proofProfilesKC;
    IccProfilesComboBox    *monitorProfilesKC;
};

SetupICC::SetupICC(QWidget* parent, KPageDialog* dialog )
        : QScrollArea(parent), d(new SetupICCPriv)
{
    d->mainDialog = dialog;

    d->tab = new KTabWidget(viewport());
    setWidget(d->tab);
    setWidgetResizable(true);
    viewport()->setAutoFillBackground(false);

    d->behaviorPanel        = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(d->behaviorPanel);

    // --------------------------------------------------------

    QWidget *colorPolicy  = new QWidget;
    QGridLayout* gridHeader      = new QGridLayout(colorPolicy);

    d->enableColorManagement = new QCheckBox(colorPolicy);
    d->enableColorManagement->setText(i18n("Enable Color Management"));
    d->enableColorManagement->setWhatsThis( i18n("<ul><li>Checked: Color Management is enabled</li>"
                                                 "<li>Unchecked: Color Management is "
                                                 "disabled</li></ul>"));

    KUrlLabel *lcmsLogoLabel = new KUrlLabel(colorPolicy);
    lcmsLogoLabel->setText(QString());
    lcmsLogoLabel->setUrl("http://www.littlecms.com");
    lcmsLogoLabel->setPixmap( QPixmap( KStandardDirs::locate("data", "digikam/data/logo-lcms.png" ) ));
    lcmsLogoLabel->setToolTip(i18n("Visit Little CMS project website"));

    gridHeader->addWidget(d->enableColorManagement, 0, 0, 1, 1);
    gridHeader->addWidget(lcmsLogoLabel,            0, 2, 1, 1);
    gridHeader->setColumnStretch(1, 10);
    gridHeader->setMargin(KDialog::spacingHint());
    gridHeader->setSpacing(0);

    // --------------------------------------------------------

    d->workspaceGB       = new QGroupBox(i18n("Working Color Space"));
    QHBoxLayout *hboxWS  = new QHBoxLayout(d->workspaceGB);

    QLabel *workIcon     = new QLabel;
    workIcon->setPixmap(SmallIcon("input-tablet"));
    d->workProfilesKC    = new IccProfilesComboBox;
    d->workProfilesKC->setWhatsThis( i18n("<p>This is the color space all the images will be converted to when opened "
                                          "(if you choose to convert) and the profile that will be embedded when saving. "
                                          "Good and safe choices are <b>Adobe RGB (1998)</b> and <b>sRGB IEC61966-2.1</b>"));

    d->infoWorkProfiles = new QPushButton;
    d->infoWorkProfiles->setIcon(SmallIcon("documentinfo"));
    d->infoWorkProfiles->setWhatsThis( i18n("<p>You can use this button to get more detailed "
                     "information about the selected workspace profile.</p>"));

    hboxWS->addWidget(workIcon);
    hboxWS->addWidget(d->workProfilesKC, 10);
    hboxWS->addWidget(d->infoWorkProfiles);

    // --------------------------------------------------------

    d->mismatchGB                   = new QGroupBox;//(i18n("Behavior on Profile Mismatch");
    QVBoxLayout *vlayMismatch       = new QVBoxLayout(d->mismatchGB);

    QLabel *behaviorIcon = new QLabel;
    behaviorIcon->setPixmap(SmallIcon("image-loading", KIconLoader::SizeMedium));
    QLabel *behaviorLabel = new QLabel(i18n("When the profile of an image does not match the working color space"));
    behaviorLabel->setWordWrap(true);

    QHBoxLayout *hboxBL = new QHBoxLayout;
    hboxBL->addWidget(behaviorIcon);
    hboxBL->addWidget(behaviorLabel, 10);

    d->defaultAskMismatch = new QRadioButton(d->mismatchGB);
    d->defaultAskMismatch->setText(i18n("Ask when opening the image"));
    d->defaultAskMismatch->setWhatsThis( i18n("<p>If an image has an embedded color profile not matching the working "
                                         "space profile, digiKam will ask if you want to convert to the working space, "
                                         "keep the embedded profile or discard the embedded profile and assign "
                                         "a different one.</p>"));

    d->defaultConvertMismatch = new QRadioButton(d->mismatchGB);
    d->defaultConvertMismatch->setText(i18n("Convert the image to the working color space"));
    d->defaultConvertMismatch->setWhatsThis( i18n("<p>If an image has an embedded color profile not matching the working "
                                         "space profile, digiKam will convert the image's color information to "
                                         "the working color space. This changes the pixel data, but not the "
                                         "appearance of the image.</p>"));

    vlayMismatch->addLayout(hboxBL);
    vlayMismatch->addWidget(d->defaultAskMismatch);
    vlayMismatch->addWidget(d->defaultConvertMismatch);

    // --------------------------------------------------------

    d->missingGB                   = new QGroupBox;//(i18n("Missing Profile Behavior"));
    QVBoxLayout *vlayMissing       = new QVBoxLayout(d->missingGB);

    QLabel *missingIcon = new QLabel;
    missingIcon->setPixmap(SmallIcon("image-missing", KIconLoader::SizeMedium));
    QLabel *missingLabel = new QLabel(i18n("When an image has no color profile information"));
    missingLabel->setWordWrap(true);

    QHBoxLayout *hboxMP = new QHBoxLayout;
    hboxMP->addWidget(missingIcon);
    hboxMP->addWidget(missingLabel, 10);

    d->defaultAskMissing = new QRadioButton(i18n("Ask when opening the image"));
    d->defaultAskMissing->setWhatsThis( i18n("<p>If an image has no embedded color profile, "
                                         "digiKam will ask which color space shall be used to interpret the image "
                                         "and to which color space it shall be transformed for editing.</p>"));

    d->defaultSRGBMissing = new QRadioButton(i18n("Assume it is using the sRGB color space (Internet standard)"));
    //TODO d->defaultSRGBMissing->setWhatsThis( i18n("<p></p>"));

    d->defaultSRGBConvert = new QCheckBox(i18n("and convert it to the working color space"));
    //TODO d->defaultSRGBConvert->setWhatsThis( i18n("<p></p>"));
    d->defaultSRGBConvert->setChecked(true);

    QGridLayout *gridRgb = new QGridLayout;
    gridRgb->addWidget(d->defaultSRGBMissing, 0, 0, 1, 2);
    gridRgb->addWidget(d->defaultSRGBConvert, 1, 1);
    gridRgb->setColumnMinimumWidth(0, 10);

    d->defaultWSMissing = new QRadioButton(i18n("Assume it is using the working color space"));
    //TODO d->defaultWSMissing->setWhatsThis( i18n("<p></p>"));

    d->defaultInputMissing = new QRadioButton(i18n("Convert it from default input color space to working space"));
    //TODO d->defaultWSMissing->setWhatsThis( i18n("<p></p>"));

    vlayMissing->addLayout(hboxMP);
    vlayMissing->addWidget(d->defaultAskMissing);
    vlayMissing->addLayout(gridRgb);
    vlayMissing->addWidget(d->defaultWSMissing);
    vlayMissing->addWidget(d->defaultInputMissing);

    // --------------------------------------------------------

    d->rawGB                   = new QGroupBox;//(i18n("Raw File Behavior"));
    QVBoxLayout *vlayRaw       = new QVBoxLayout(d->rawGB);

    QLabel *rawBehaviorIcon = new QLabel;
    rawBehaviorIcon->setPixmap(SmallIcon("kdcraw", KIconLoader::SizeMedium));
    QLabel *rawBehaviorLabel = new QLabel(i18n("When loading a RAW file with uncalibrated colors"));
    rawBehaviorLabel->setWordWrap(true);

    QHBoxLayout *hboxRF = new QHBoxLayout;
    hboxRF->addWidget(rawBehaviorIcon);
    hboxRF->addWidget(rawBehaviorLabel, 10);

    d->defaultAskRaw = new QRadioButton(i18n("Ask for the input profile"));
    //TODO d->defaultAskRaw->setWhatsThis( i18n("<p></p>"));

    d->defaultGuessRaw = new QRadioButton(i18n("Automatic color correction"));
    //TODO d->defaultGuessRaw->setWhatsThis( i18n("<p></p>"));

    d->defaultInputRaw = new QRadioButton(i18n("Convert it from the default input profile"));
    //TODO d->defaultSRGBMissing->setWhatsThis( i18n("<p></p>"));

    d->defaultGuessRaw->setChecked(true);

    vlayRaw->addLayout(hboxRF);
    vlayRaw->addWidget(d->defaultAskRaw);
    vlayRaw->addWidget(d->defaultGuessRaw);
    vlayRaw->addWidget(d->defaultInputRaw);


    mainLayout->addWidget(colorPolicy);
    mainLayout->addWidget(d->workspaceGB);
    mainLayout->addWidget(d->mismatchGB);
    mainLayout->addWidget(d->missingGB);
    mainLayout->addWidget(d->rawGB);
    mainLayout->addStretch();

    // --------------------------------------------------------

    d->profilesPanel         = new QWidget;
    QVBoxLayout *vboxDisplay = new QVBoxLayout(d->profilesPanel);

    d->viewGB      = new QGroupBox(i18n("Color Managed View"));
    QGridLayout* gridView = new QGridLayout(d->viewGB);

    QLabel *monitorIcon       = new QLabel;
    monitorIcon->setPixmap(SmallIcon("video-display", KIconLoader::SizeMedium));
    QLabel *monitorProfiles   = new QLabel(i18n("Monitor profile:"));

    d->monitorProfilesKC = new IccProfilesComboBox;
    monitorProfiles->setBuddy(d->monitorProfilesKC);
    d->monitorProfilesKC->setWhatsThis( i18n("<p>Select the color profile for your monitor here.</p>"));

    d->infoMonitorProfiles = new QPushButton;
    d->infoMonitorProfiles->setIcon(SmallIcon("documentinfo"));
    d->infoMonitorProfiles->setWhatsThis( i18n("<p>You can use this button to get more detailed "
                     "information about the selected monitor profile.</p>"));

    d->managedView = new QCheckBox;
    d->managedView->setText(i18n("Use color managed view in editor"));
    d->managedView->setWhatsThis( i18n("<p>Turn on this option if "
                     "you want to use your <b>Monitor Color Profile</b> to show your pictures in "
                     "the Image Editor window with a color correction adapted to your monitor. "
                     "You can at any time toggle this option from the Editor window. "
                     "<i>Warning</i>: This can slow down rendering of the image, depending on the speed of your computer.</p>"));

    gridView->addWidget(monitorIcon, 0, 0);
    gridView->addWidget(monitorProfiles, 0, 1, 1, 2);
    gridView->addWidget(d->monitorProfilesKC, 1, 0, 1, 2);
    gridView->addWidget(d->infoMonitorProfiles, 1, 2);
    gridView->addWidget(d->managedView, 2, 0, 1, 3);
    gridView->setColumnStretch(1, 10);

    // --------------------------------------------------------

    d->inputGB = new QGroupBox(i18n("Camera and Scanner"));
    QGridLayout *gridIP  = new QGridLayout(d->inputGB);

    QLabel *inputIcon     = new QLabel;
    inputIcon->setPixmap(SmallIcon("input-tablet", KIconLoader::SizeMedium));
    QLabel *inputLabel    = new QLabel(i18n("Default input color profile:"));
    d->inProfilesKC    = new IccProfilesComboBox;
    d->inProfilesKC->setWhatsThis( i18n("<p>This is the default color profile for your input device "
                                          "(that is your camera - or your scanner). A camera input profile "
                                          "is recommended for correct conversion of RAW images in 16bit. "
                                          "Some of the options about loading behavior above refer to this profile.</b>"));

    d->infoInProfiles = new QPushButton;
    d->infoInProfiles->setIcon(SmallIcon("documentinfo"));
    d->infoInProfiles->setWhatsThis( i18n("<p>You can use this button to get more detailed "
                                            "information about the selected input profile.</p>"));

    gridIP->addWidget(inputIcon, 0, 0);
    gridIP->addWidget(inputLabel, 0, 1, 1, 2);
    gridIP->addWidget(d->inProfilesKC, 1, 0, 1, 2);
    gridIP->addWidget(d->infoInProfiles, 1, 2);
    gridIP->setColumnStretch(1, 10);

    // --------------------------------------------------------

    d->proofGB = new QGroupBox(i18n("Printing and Proofing"));
    QGridLayout *gridProof = new QGridLayout(d->proofGB);

    QLabel *proofIcon     = new QLabel;
    proofIcon->setPixmap(SmallIcon("printer", KIconLoader::SizeMedium));
    QLabel *proofLabel = new QLabel(i18n("Output device profile:"));
    d->proofProfilesKC    = new IccProfilesComboBox;
    proofLabel->setBuddy(d->proofProfilesKC);
    d->proofProfilesKC->setWhatsThis( i18n("<p>Select the profile for your output device "
                     "(usually, your printer). This profile will be used to do a soft proof, so you will "
                     "be able to preview how an image will be rendered via an output device.</p>"));

    d->infoProofProfiles = new QPushButton;
    d->infoProofProfiles->setIcon(SmallIcon("documentinfo"));
    d->infoProofProfiles->setWhatsThis( i18n("<p>You can use this button to get more detailed "
                     "information about the selected proofing profile.</p>"));

    gridProof->addWidget(proofIcon, 0, 0);
    gridProof->addWidget(proofLabel, 0, 1, 1, 2);
    gridProof->addWidget(d->proofProfilesKC, 1, 0, 1, 2);
    gridProof->addWidget(d->infoProofProfiles, 1, 2);
    gridProof->setColumnStretch(1, 10);

    // --------------------------------------------------------

    d->iccFolderGB = new QGroupBox(i18n("Color Profiles Repository"));
    QGridLayout *gridIccFolder = new QGridLayout(d->iccFolderGB);

    QLabel *iccFolderIcon     = new QLabel;
    iccFolderIcon->setPixmap(SmallIcon("folder-downloads", KIconLoader::SizeMedium));
    d->iccFolderLabel = new QLabel(i18n("DigiKam looks for ICC profiles in a number of <a href='default'>default locations</a>. "
                                        "You can specify an additional folder:"));
    d->iccFolderLabel->setWordWrap(true);

    d->defaultPathKU = new KUrlRequester;
    d->iccFolderLabel->setBuddy(d->defaultPathKU);
    d->defaultPathKU->lineEdit()->setReadOnly(true);
    d->defaultPathKU->setMode(KFile::Directory | KFile::LocalOnly | KFile::ExistingOnly);
    d->defaultPathKU->setWhatsThis( i18n("<p>DigiKam searches ICC profiles in default system folders "
                                         "and ships itself a few selected profiles. "
                                         "Store all your additional color profiles in the directory set here.</p>"));

    gridIccFolder->addWidget(iccFolderIcon, 0, 0);
    gridIccFolder->addWidget(d->iccFolderLabel, 0, 1);
    gridIccFolder->addWidget(d->defaultPathKU, 1, 0, 1, 2);
    gridIccFolder->setColumnStretch(1, 10);

    vboxDisplay->addWidget(d->viewGB);
    vboxDisplay->addWidget(d->inputGB);
    vboxDisplay->addWidget(d->proofGB);
    vboxDisplay->addWidget(d->iccFolderGB);
    vboxDisplay->addStretch(1);

    // --------------------------------------------------------

    d->advancedPanel             = new QWidget;
    QVBoxLayout *vboxAdvanced    = new QVBoxLayout(d->advancedPanel);
    d->advancedSettingsGB        = new QGroupBox(i18n("Advanced Settings"));
    QGridLayout* gridAdvanced    = new QGridLayout(d->advancedSettingsGB);

    d->bpcAlgorithm = new QCheckBox(d->advancedSettingsGB);
    d->bpcAlgorithm->setText(i18n("Use black point compensation"));
    d->bpcAlgorithm->setWhatsThis( i18n("<p><b>Black Point Compensation</b> is a way to make "
                     "adjustments between the maximum "
                     "black levels of digital files and the black capabilities of various "
                     "digital devices.</p>"));

    QLabel *lablel = new QLabel(d->advancedSettingsGB);
    lablel->setText(i18n("Rendering Intents:"));

    d->renderingIntentKC = new KComboBox(false, d->advancedSettingsGB);
    d->renderingIntentKC->addItem("Perceptual", IccTransform::Perceptual);
    d->renderingIntentKC->addItem("Relative Colorimetric", IccTransform::RelativeColorimetric);
    d->renderingIntentKC->addItem("Saturation", IccTransform::Saturation);
    d->renderingIntentKC->addItem("Absolute Colorimetric", IccTransform::AbsoluteColorimetric);
    d->renderingIntentKC->setWhatsThis( i18n("<ul><li><p><b>Perceptual intent</b> causes the full gamut of the image to be "
                     "compressed or expanded to fill the gamut of the destination device, so that gray balance is "
                     "preserved but colorimetric accuracy may not be preserved.</p>"
                     "<p>In other words, if certain colors in an image fall outside of the range of colors that the output "
                     "device can render, the image intent will cause all the colors in the image to be adjusted so that "
                     "the every color in the image falls within the range that can be rendered and so that the relationship "
                     "between colors is preserved as much as possible.</p>"
                     "<p>This intent is most suitable for display of photographs and images, and is the default intent.</p></li>"
                     "<li><p><b>Absolute Colorimetric intent</b> causes any colors that fall outside the range that the output device "
                     "can render to be adjusted to the closest color that can be rendered, while all other colors are "
                     "left unchanged.</p>"
                     "<p>This intent preserves the white point and is most suitable for spot colors (Pantone, TruMatch, "
                     "logo colors, ....)</p></li>"
                     "<li><p><b>Relative Colorimetric intent</b> is defined such that any colors that fall outside the range that the "
                     "output device can render are adjusted to the closest color that can be rendered, while all other colors "
                     "are left unchanged. Proof intent does not preserve the white point.</p></li>"
                     "<li><p><b>Saturation intent</b> preserves the saturation of colors in the image at the possible expense of "
                     "hue and lightness.</p>"
                     "<p>Implementation of this intent remains somewhat problematic, and the ICC is still working on methods to "
                     "achieve the desired effects.</p>"
                     "<p>This intent is most suitable for business graphics such as charts, where it is more important that the "
                     "colors be vivid and contrast well with each other rather than a specific color.</p></li></ul>"));

    gridAdvanced->addWidget(d->bpcAlgorithm,      0, 0, 1, 2);
    gridAdvanced->addWidget(lablel,               1, 0, 1, 1);
    gridAdvanced->addWidget(d->renderingIntentKC, 1, 1, 1, 1);
    gridAdvanced->setMargin(KDialog::spacingHint());
    gridAdvanced->setSpacing(0);

    vboxAdvanced->addWidget(d->advancedSettingsGB);
    vboxAdvanced->addStretch(1);

    // --------------------------------------------------------

    d->tab->addTab(d->behaviorPanel, i18n("Behavior"));
    d->tab->addTab(d->profilesPanel, i18n("Profiles"));
    d->tab->addTab(d->advancedPanel, i18n("Advanced"));

    // --------------------------------------------------------

    connect(d->enableColorManagement, SIGNAL(toggled(bool)),
            this, SLOT(slotToggledEnabled()));

    connect(lcmsLogoLabel, SIGNAL(leftClickedUrl(const QString&)),
            this, SLOT(processLcmsUrl(const QString&)));

    connect(d->infoProofProfiles, SIGNAL(clicked()),
            this, SLOT(slotClickedProof()) );

    connect(d->infoInProfiles, SIGNAL(clicked()),
            this, SLOT(slotClickedIn()) );

    connect(d->infoMonitorProfiles, SIGNAL(clicked()),
            this, SLOT(slotClickedMonitor()) );

    connect(d->infoWorkProfiles, SIGNAL(clicked()),
            this, SLOT(slotClickedWork()));

    connect(d->defaultPathKU, SIGNAL(urlSelected(const KUrl&)),
            this, SLOT(slotUrlChanged()));

    connect(d->iccFolderLabel, SIGNAL(linkActivated(const QString &)),
            this, SLOT(slotShowDefaultSearchPaths()));

    connect(d->defaultAskMissing, SIGNAL(toggled(bool)),
            this, SLOT(slotMissingToggled(bool)));

    connect(d->defaultSRGBMissing, SIGNAL(toggled(bool)),
            this, SLOT(slotMissingToggled(bool)));

    connect(d->defaultWSMissing, SIGNAL(toggled(bool)),
            this, SLOT(slotMissingToggled(bool)));

    connect(d->defaultInputMissing, SIGNAL(toggled(bool)),
            this, SLOT(slotMissingToggled(bool)));

    // --------------------------------------------------------

    adjustSize();

    readSettings();
    slotToggledEnabled();
}

SetupICC::~SetupICC()
{
    delete d;
}

void SetupICC::processLcmsUrl(const QString& url)
{
    KToolInvocation::self()->invokeBrowser(url);
}

void SetupICC::applySettings()
{
    ICCSettingsContainer settings;
    settings.enableCM = d->enableColorManagement->isChecked();

    if (d->defaultAskMismatch->isChecked())
        settings.defaultMismatchBehavior = ICCSettingsContainer::AskUser;
    else if (d->defaultConvertMismatch->isChecked())
        settings.defaultMismatchBehavior = ICCSettingsContainer::EmbeddedToWorkspace;

    if (d->defaultAskMissing->isChecked())
    {
        settings.defaultMissingProfileBehavior = ICCSettingsContainer::AskUser;
    }
    else if (d->defaultSRGBMissing->isChecked())
    {
        settings.defaultMissingProfileBehavior = ICCSettingsContainer::UseSRGB;

        if (d->defaultSRGBConvert->isChecked())
            settings.defaultMissingProfileBehavior |= ICCSettingsContainer::ConvertToWorkspace;
        else
            settings.defaultMissingProfileBehavior |= ICCSettingsContainer::KeepProfile;
    }
    else if (d->defaultWSMissing->isChecked())
    {
        settings.defaultMissingProfileBehavior = ICCSettingsContainer::UseWorkspace | ICCSettingsContainer::KeepProfile;
    }
    else if (d->defaultInputMissing->isChecked())
    {
        settings.defaultMissingProfileBehavior = ICCSettingsContainer::InputToWorkspace;
    }

    if (d->defaultAskRaw->isChecked())
        settings.defaultUncalibratedBehavior = ICCSettingsContainer::AskUser;
    else if (d->defaultInputRaw->isChecked())
        settings.defaultUncalibratedBehavior = ICCSettingsContainer::InputToWorkspace;
    else if (d->defaultGuessRaw->isChecked())
        settings.defaultUncalibratedBehavior = ICCSettingsContainer::AutomaticColors | ICCSettingsContainer::ConvertToWorkspace;

    settings.iccFolder = d->defaultPathKU->url().path();
    settings.useBPC =  d->bpcAlgorithm->isChecked();
    settings.renderingIntent = d->renderingIntentKC->itemData(d->renderingIntentKC->currentIndex()).toInt();
    settings.useManagedView = d->managedView->isChecked();

    settings.defaultInputProfile = d->inProfilesKC->currentProfile().filePath();
    settings.workspaceProfile = d->workProfilesKC->currentProfile().filePath();
    settings.monitorProfile = d->monitorProfilesKC->currentProfile().filePath();
    settings.defaultProofProfile = d->proofProfilesKC->currentProfile().filePath();

    IccSettings::instance()->setSettings(settings);
}

static void setCurrentIndexFromUserData(QComboBox *box, const QVariant& userData)
{
    if (userData.isNull())
    {
        box->setCurrentIndex(-1);
        return;
    }

    const int size = box->count();
    for (int i=0; i<size; i++)
    {
        if (box->itemData(i) == userData)
        {
            box->setCurrentIndex(i);
            return;
        }
    }
    box->setCurrentIndex(-1);
}

void SetupICC::readSettings(bool restore)
{
    ICCSettingsContainer settings = IccSettings::instance()->settings();

    if (!restore)
        d->enableColorManagement->setChecked(settings.enableCM);

    d->bpcAlgorithm->setChecked(settings.useBPC);
    setCurrentIndexFromUserData(d->renderingIntentKC, settings.renderingIntent);
    d->managedView->setChecked(settings.useManagedView);

    if (settings.defaultMismatchBehavior & ICCSettingsContainer::AskUser)
        d->defaultAskMismatch->setChecked(true);
    else if (settings.defaultMismatchBehavior & ICCSettingsContainer::ConvertToWorkspace)
        d->defaultConvertMismatch->setChecked(true);

    if (settings.defaultMissingProfileBehavior & ICCSettingsContainer::AskUser)
        d->defaultAskMissing->setChecked(true);
    else
    {
        if (settings.defaultMissingProfileBehavior & ICCSettingsContainer::UseSRGB)
        {
            d->defaultSRGBMissing->setChecked(true);
            d->defaultSRGBConvert->setChecked(settings.defaultMissingProfileBehavior & ICCSettingsContainer::ConvertToWorkspace);
        }
        else if (settings.defaultMissingProfileBehavior& ICCSettingsContainer::UseWorkspace)
            d->defaultWSMissing->setChecked(true);
        else if (settings.defaultMissingProfileBehavior & ICCSettingsContainer::UseDefaultInputProfile)
            d->defaultInputMissing->setChecked(true);
    }

    if (settings.defaultUncalibratedBehavior & ICCSettingsContainer::AskUser)
        d->defaultAskRaw->setChecked(true);
    else if (settings.defaultUncalibratedBehavior & ICCSettingsContainer::UseDefaultInputProfile)
        d->defaultInputRaw->setChecked(true);
    else if (settings.defaultUncalibratedBehavior & ICCSettingsContainer::AutomaticColors)
        d->defaultGuessRaw->setChecked(true);

    d->defaultPathKU->setUrl(settings.iccFolder);
    fillCombos(false);

    d->workProfilesKC->setCurrentProfile(settings.workspaceProfile);
    d->monitorProfilesKC->setCurrentProfile(settings.monitorProfile);
    d->inProfilesKC->setCurrentProfile(settings.defaultInputProfile);
    d->proofProfilesKC->setCurrentProfile(settings.defaultProofProfile);
}

void SetupICC::slotUrlChanged()
{
    IccSettings::instance()->setIccPath(d->defaultPathKU->url().path());
    fillCombos(true);
}

void SetupICC::fillCombos(bool report)
{
    if (!d->enableColorManagement->isChecked())
        return;

    QList<IccProfile> profiles = IccSettings::instance()->allProfiles();

    if ( profiles.isEmpty() )
    {
        if (report)
        {
            QString message = i18n("No ICC profiles files found.");
            KMessageBox::sorry(this, message);
        }

        kDebug(50003) << "No ICC profile files found!!!";
        d->mainDialog->enableButtonOk(false);
        return;
    }

    d->workProfilesKC->replaceProfilesSqueezed(IccSettings::instance()->workspaceProfiles());
    d->monitorProfilesKC->replaceProfilesSqueezed(IccSettings::instance()->displayProfiles());
    d->inProfilesKC->replaceProfilesSqueezed(IccSettings::instance()->inputProfiles());
    d->proofProfilesKC->replaceProfilesSqueezed(IccSettings::instance()->outputProfiles());

    d->workProfilesKC->setNoProfileIfEmpty(i18n("No Profile Available"));
    d->monitorProfilesKC->setNoProfileIfEmpty(i18n("No Display Profile Available"));
    d->inProfilesKC->setNoProfileIfEmpty(i18n("No Input Profile Available"));
    d->proofProfilesKC->setNoProfileIfEmpty(i18n("No Output Profile Available"));

    if (d->monitorProfilesKC->count() == 0)
    {
        d->managedView->setEnabled(false);
        d->managedView->setChecked(false);
    }
    else
    {
        d->managedView->setEnabled(true);
    }

    if (d->workProfilesKC->count() == 0)
    {
        // If there is no workspace ICC profiles available,
        // the CM is broken and cannot be used.
        d->mainDialog->enableButtonOk(false);
        return;
    }

    d->mainDialog->enableButtonOk(true);
}

void SetupICC::setWidgetsEnabled(bool enabled)
{
    d->workspaceGB->setEnabled(enabled);
    d->mismatchGB->setEnabled(enabled);
    d->missingGB->setEnabled(enabled);
    d->rawGB->setEnabled(enabled);
    d->tab->setTabEnabled(1, enabled);
    d->tab->setTabEnabled(2, enabled);
    //d->profilesPanel->setEnabled(enabled);
    //d->advancedPanel->setEnabled(enabled);
}

void SetupICC::slotToggledEnabled()
{
    bool enabled = d->enableColorManagement->isChecked();

    setWidgetsEnabled(enabled);

    if (enabled)
    {
        readSettings(true);
    }
    else
    {
        d->mainDialog->enableButtonOk(true);
    }
}

void SetupICC::slotClickedWork()
{
    IccProfile profile = d->workProfilesKC->currentProfile();
    if (!profile.isNull())
       profileInfo(profile);
}

void SetupICC::slotClickedIn()
{
    IccProfile profile = d->inProfilesKC->currentProfile();
    if (!profile.isNull())
       profileInfo(profile);
}

void SetupICC::slotClickedMonitor()
{
    IccProfile profile = d->monitorProfilesKC->currentProfile();
    if (!profile.isNull())
       profileInfo(profile);
}

void SetupICC::slotClickedProof()
{
    IccProfile profile = d->proofProfilesKC->currentProfile();
    if (!profile.isNull())
       profileInfo(profile);
}

void SetupICC::profileInfo(const IccProfile& profile)
{
    if (profile.isNull())
    {
        KMessageBox::error(this, i18n("No profile is selected."), i18n("Profile Error"));
        return;
    }

    ICCProfileInfoDlg infoDlg(this, profile.filePath(), profile);
    infoDlg.exec();
}

void SetupICC::slotMissingToggled(bool on)
{
    if (!on)
        return;
    d->defaultSRGBConvert->setEnabled(d->defaultSRGBMissing->isChecked());
}

void SetupICC::slotShowDefaultSearchPaths()
{
    QStringList defaultSearchPaths = IccProfile::defaultSearchPaths();
    QString existingPaths;
    if (defaultSearchPaths.isEmpty())
        existingPaths = i18nc("none of the paths", "none");
    else
        existingPaths = defaultSearchPaths.join("</li><li>");
    QString text = i18n("On Linux, the default search paths include "
                        "<ul>"
                        "<li>/usr/share/color/icc</li>"
                        "<li>/usr/local/share/color/icc</li>"
                        "<li>~/.local/share/color/icc/</li>"
                        "<li>~/.local/share/icc/</li>"
                        "<li>~/.color/icc/</li>"
                        "</ul>"
                        "On your system, currently these paths exist and are scanned:"
                        "<ul>"
                        "<li>%1</li>"
                        "</ul>",
                        existingPaths);
    QWhatsThis::showText(d->iccFolderLabel->mapToGlobal(QPoint(0,0)), text, d->iccFolderLabel);
}

bool SetupICC::iccRepositoryIsValid()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group = config->group(QString("Color Management"));

    // If color management is disable, no need to check anymore.
    if (!group.readEntry("EnableCM", false))
        return true;


    // can at least libkdcraw profiles be opened?
    if (IccProfile::sRGB().open())
        return true;

    // To be valid, the ICC profiles repository must exist and be readable.
    QString extraPath = group.readEntry("DefaultPath", QString());
    QFileInfo info(extraPath);
    if (info.isDir() && info.exists() && info.isReadable())
        return true;
    QStringList paths = IccProfile::defaultSearchPaths();
    return !paths.isEmpty();
}

}  // namespace Digikam
