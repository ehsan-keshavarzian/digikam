/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date  : 2006-06-13
 * Description : a widget to display an image preview
 *
 * Copyright 2006 Gilles Caulier
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

// Qt includes.

#include <qpainter.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qrect.h>
#include <qtimer.h>
#include <qguardedptr.h>

// KDE include.

#include <kcursor.h>
#include <kprocess.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kfilemetainfo.h>

// Local includes.

#include "themeengine.h"
#include "albumsettings.h"
#include "imagepreviewjob.h"
#include "imagepreviewwidget.h"

namespace Digikam
{

class ImagePreviewWidgetPriv
{
public:

    ImagePreviewWidgetPriv()
    {
        previewBlink      = false;
        blinkPreviewTimer = 0;
        previewJob        = 0;
    }

    bool                          previewBlink;
    
    QString                       path;

    QPixmap                       pixmap;

    QImage                        preview;
    
    QGuardedPtr<ImagePreviewJob>  previewJob;
    
    QTimer                       *blinkPreviewTimer;
};

ImagePreviewWidget::ImagePreviewWidget(QWidget *parent)
                  : QFrame(parent, 0, Qt::WDestructiveClose)
{
    d = new ImagePreviewWidgetPriv;
    d->blinkPreviewTimer = new QTimer(this);
    setBackgroundMode(Qt::NoBackground);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(QWidget::StrongFocus);
    setFrameStyle(QFrame::NoFrame);
    setMargin(0);
    setLineWidth(0);
    
    // ---------------------------------------------------------------
    
    connect(d->blinkPreviewTimer, SIGNAL(timeout()),
            this, SLOT(slotPreviewBlinkTimerDone()));
            
    connect(ThemeEngine::instance(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotThemeChanged()));
}

ImagePreviewWidget::~ImagePreviewWidget()
{
    if (!d->previewJob.isNull())
        d->previewJob->kill();
    
    d->blinkPreviewTimer->stop();
            
    delete d;
}

void ImagePreviewWidget::setImagePath( const QString& path )
{
    if (path == d->path) return;

    kapp->setOverrideCursor( KCursor::waitCursor() );
    d->path              = path;
    d->previewBlink      = false;
    d->blinkPreviewTimer->start(200);

    d->previewJob = new ImagePreviewJob(KURL(path), 1024, AlbumSettings::instance()->getExifRotate());

    connect(d->previewJob, SIGNAL(signalImagePreview(const KURL&, const QImage&)),
            this, SLOT(slotGotImagePreview(const KURL&, const QImage&)));

    connect(d->previewJob, SIGNAL(signalFailed(const KURL&)),
            this, SLOT(slotFailedImagePreview(const KURL&)));   

    emit previewUnderProgress(true);
}

void ImagePreviewWidget::slotPreviewBlinkTimerDone()
{
    d->previewBlink = !d->previewBlink;
    updatePixmap();
    repaint(false);
    d->blinkPreviewTimer->start(200);
}
                
void ImagePreviewWidget::slotGotImagePreview(const KURL&, const QImage& preview)
{
    d->blinkPreviewTimer->stop();
    d->previewJob = 0;    
    d->preview    = preview;
    d->pixmap     = QPixmap(contentsRect().size());
    updatePixmap();
    repaint(false);
    kapp->restoreOverrideCursor();
    emit previewUnderProgress(false);
}

void ImagePreviewWidget::slotFailedImagePreview(const KURL&)
{
    d->blinkPreviewTimer->stop();
    d->previewJob = 0;    
    d->preview    = QImage();
    d->pixmap     = QPixmap(contentsRect().size());
    updatePixmap();
    repaint(false);
    kapp->restoreOverrideCursor();
    emit previewUnderProgress(false);
}

void ImagePreviewWidget::updatePixmap( void )
{
    d->pixmap.fill(ThemeEngine::instance()->baseColor());

    if (!d->path.isEmpty())
    {
        QPainter p(&(d->pixmap));

        if (!d->previewJob)
        {
            // Preview extraction is complete
            
            if (!d->preview.isNull())
            {
                QPixmap pix(d->preview.scale(contentsRect().size(), QImage::ScaleMin));
                p.drawPixmap((contentsRect().width()-pix.width())/2,
                             (contentsRect().height()-pix.height())/2, pix,
                             0, 0, pix.width(), pix.height());
            }
            else
            {
                p.setPen(QPen(Qt::red));
                p.drawText(0, 0, d->pixmap.width(), d->pixmap.height(),
                           Qt::AlignCenter|Qt::WordBreak, 
                           i18n("Cannot display image preview!"));
            }
        }
        else
        {
            // Preview extraction under progress
            
            p.setPen(QPen(d->previewBlink ? Qt::green : Qt::darkGreen));
            p.drawText(0, 0, d->pixmap.width(), d->pixmap.height(),
                       Qt::AlignCenter|Qt::WordBreak, 
                       i18n("Preview extraction in progress..."));
        }
    
        p.end();
    }
}

void ImagePreviewWidget::drawContents(QPainter *)
{
    bitBlt(this, contentsRect().topLeft(), &(d->pixmap), contentsRect(), Qt::CopyROP);
}

void ImagePreviewWidget::slotThemeChanged()
{
    updatePixmap();
    repaint(false);
}

void ImagePreviewWidget::resizeEvent(QResizeEvent *)
{
    blockSignals(true);
    d->pixmap = QPixmap(contentsRect().size());
    updatePixmap();
    repaint(false);
    blockSignals(false);
}

void ImagePreviewWidget::wheelEvent( QWheelEvent * e )
{
    if (e->delta() > 0)
        emit prevItem();

    if (e->delta() < 0)
        emit nextItem();
}

void ImagePreviewWidget::keyPressEvent( QKeyEvent * e )
{
    if ( e->state() == 0 )
    {
        switch ( e->key() )
        {
            case Key_PageUp:
                emit prevItem();
            break;

            case Key_PageDown:
                emit nextItem();
            break;

            case Key_Home:
                emit firstItem();
            break;

            case Key_End:
                emit lastItem();
            break;

            case Key_Escape:
                emit escapeSignal();
            break;

            default:
                return;
        }
    }
}

}  // NameSpace Digikam

#include "imagepreviewwidget.moc"
