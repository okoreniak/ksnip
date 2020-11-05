/*
 * Copyright (C) 2017 Damir Porobic <damir.porobic@gmx.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "AbstractRectAreaImageGrabber.h"
#include <QDebug>

AbstractRectAreaImageGrabber::AbstractRectAreaImageGrabber(AbstractSnippingArea *snippingArea) :
    mSnippingArea(snippingArea),
    mFreezeImageWhileSnipping(mConfig->freezeImageWhileSnippingEnabled())
{
    Q_ASSERT(mSnippingArea != nullptr);

    connectSnippingAreaCancel();
}

AbstractRectAreaImageGrabber::~AbstractRectAreaImageGrabber()
{
    delete mSnippingArea;
}

void AbstractRectAreaImageGrabber::grabImage(CaptureModes captureMode, bool captureCursor, int delay)
{
    setIsCaptureCursorEnabled(captureCursor);
    setCaptureDelay(delay);
    setCaptureMode(captureMode);

    if (isRectAreaCaptureWithoutBackground()) {
        openSnippingAreaWithoutBackground();
    } else {
        QTimer::singleShot(captureDelay(), this, &AbstractRectAreaImageGrabber::prepareGrab);
    }
}

/*
 * Returns the rect of the screen where the mouse cursor is currently located
 */
QRect AbstractRectAreaImageGrabber::currentScreenRect() const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    auto screen = QGuiApplication::screenAt(QCursor::pos());
    if (screen == nullptr) {
        screen = QGuiApplication::primaryScreen();
    }
    return screen->geometry();
#else
    auto screen = QApplication::desktop()->screenNumber(QCursor::pos());
    return QApplication::desktop()->screenGeometry(screen);
#endif
}

QRect AbstractRectAreaImageGrabber::lastRectArea() const
{
	auto rectArea = mConfig->lastRectArea();
	if(rectArea.isNull()) {
		qWarning("ImageGrabber: No RectArea found, capturing full screen.");
		return fullScreenRect();
	}
	return rectArea;
}

void AbstractRectAreaImageGrabber::openSnippingAreaWithoutBackground()
{
	connectSnippingAreaFinish();
    mSnippingArea->showWithoutBackground();
}

void AbstractRectAreaImageGrabber::openSnippingAreaWithBackground(const QPixmap& background)
{
	connectSnippingAreaFinish();
    mSnippingArea->showWithBackground(background);
}

QRect AbstractRectAreaImageGrabber::selectedSnippingAreaRect() const
{
    qDebug() << __FUNCTION__ ;
    return mSnippingArea->selectedRectArea();
}

QPixmap AbstractRectAreaImageGrabber::snippingAreaBackground() const
{
    qDebug() << __FUNCTION__ ;

	return mSnippingArea->background();
}

QPixmap AbstractRectAreaImageGrabber::getScreenshotFromRect(const QRect &rect) const
{
    qDebug() << __FUNCTION__ ;

	auto screen = QGuiApplication::primaryScreen();
	auto windowId = QApplication::desktop()->winId();
	auto rectPosition = rect.topLeft();
	return screen->grabWindow(windowId, rectPosition.x(), rectPosition.y(), rect.width(), rect.height());
}

QPixmap AbstractRectAreaImageGrabber::getScreenshot() const
{
	if (isRectAreaCaptureWithBackground()) {
        qDebug() << __FUNCTION__ << " case 1";
		return snippingAreaBackground().copy(mCaptureRect);
	} else {
        qDebug() << __FUNCTION__ << " case 2";
        return getScreenshotFromRect(mCaptureRect);
	}
}

void AbstractRectAreaImageGrabber::setCaptureRectFromCorrectSource()
{
    qDebug() << __FUNCTION__ ;

	switch (captureMode()) {
		case CaptureModes::RectArea:
			mCaptureRect = selectedSnippingAreaRect();
			break;
		case CaptureModes::LastRectArea:
			mCaptureRect = lastRectArea();
			break;
		case CaptureModes::CurrentScreen:
			mCaptureRect = currentScreenRect();
			break;
		case CaptureModes::ActiveWindow:
			mCaptureRect = activeWindowRect();
			if (mCaptureRect.isNull()) {
				qWarning("ImageGrabber::getActiveWindow: Found no window with focus.");
				mCaptureRect = currentScreenRect();
			}
			break;
        default:
            mCaptureRect = fullScreenRect();
    }
}

bool AbstractRectAreaImageGrabber::isSnippingAreaBackgroundTransparent() const
{
	return !mFreezeImageWhileSnipping;
}

void AbstractRectAreaImageGrabber::prepareGrab()
{
    qDebug() << __FUNCTION__ ;

	if (isRectAreaCaptureWithBackground()) {
		openSnippingArea();
	} else {
		grab();
	}
}

void AbstractRectAreaImageGrabber::grab()
{
	setCaptureRectFromCorrectSource();
	CaptureDto capture(getScreenshot());

	if (shouldCaptureCursor()) {
		capture.cursor = getCursorRelativeToScreenshot();
	}
	emit finished(capture);
}

CursorDto AbstractRectAreaImageGrabber::getCursorRelativeToScreenshot() const
{
	auto cursor = getCursorImageWithPositionFromCorrectSource();
	if(mCaptureRect.contains(cursor.position)) {
		cursor.position -= mCaptureRect.topLeft();
		return cursor;
	}
	return {};
}

bool AbstractRectAreaImageGrabber::shouldCaptureCursor() const
{
	return isCaptureCursorEnabled() && !(captureMode() == CaptureModes::RectArea && isCaptureDelayBelowMin());
}

void AbstractRectAreaImageGrabber::openSnippingArea()
{    
	if (isSnippingAreaBackgroundTransparent()) {
        qDebug() << __FUNCTION__ << " 1";
		openSnippingAreaWithoutBackground();
	} else {
        qDebug() << __FUNCTION__ << " 2";

		auto screenRect = fullScreenRect();
		auto background = getScreenshotFromRect(screenRect);
		mStoredCursorImageWithPosition = getCursorWithPosition();
		openSnippingAreaWithBackground(background);
	}
}

void AbstractRectAreaImageGrabber::connectSnippingAreaCancel()
{
    connect(mSnippingArea, &AbstractSnippingArea::canceled, this, &AbstractRectAreaImageGrabber::canceled);
}

void AbstractRectAreaImageGrabber::connectSnippingAreaFinish()
{
	disconnectSnippingAreaFinish();

	if (isSnippingAreaBackgroundTransparent()) {
		connect(mSnippingArea, &AbstractSnippingArea::finished, [this]()
		{
			QTimer::singleShot(captureDelay(), this, &AbstractRectAreaImageGrabber::grab);
		});
	} else {
		connect(mSnippingArea, &AbstractSnippingArea::finished, this, &AbstractRectAreaImageGrabber::grab);
	}
}

void AbstractRectAreaImageGrabber::disconnectSnippingAreaFinish()
{
	disconnect(mSnippingArea, &AbstractSnippingArea::finished, nullptr, nullptr);
}

CursorDto AbstractRectAreaImageGrabber::getCursorImageWithPositionFromCorrectSource() const
{
	return isRectAreaCaptureWithBackground() ? mStoredCursorImageWithPosition : getCursorWithPosition();
}

bool AbstractRectAreaImageGrabber::isRectAreaCaptureWithBackground() const
{
	return captureMode() == CaptureModes::RectArea && !isSnippingAreaBackgroundTransparent();
}

bool AbstractRectAreaImageGrabber::isRectAreaCaptureWithoutBackground() const
{
	return captureMode() == CaptureModes::RectArea && isSnippingAreaBackgroundTransparent();
}
