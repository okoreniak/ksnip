/*
 * Copyright (C) 2020 Damir Porobic <damir.porobic@gmx.com>
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

#include "SingleCaptureHandler.h"

SingleCaptureHandler::SingleCaptureHandler(IImageAnnotator *imageAnnotator, IToastService *toastService, IClipboard *clipboard, QWidget *parent) :
		mImageAnnotator(imageAnnotator),
		mToastService(toastService),
		mParent(parent),
		mCaptureChangeListener(nullptr),
		mIsSaved(true),
		mClipboard(clipboard)
{
	mImageAnnotator->setTabBarAutoHide(true);

	connect(mImageAnnotator, &IImageAnnotator::imageChanged, this, &SingleCaptureHandler::markUnsaved);
}

bool SingleCaptureHandler::canClose()
{
	return discardChanges();
}

bool SingleCaptureHandler::canTakeNew()
{
	return discardChanges();
}

bool SingleCaptureHandler::isSaved() const
{
	return mIsSaved;
}

QString SingleCaptureHandler::path() const
{
	return mPath;
}

void SingleCaptureHandler::saveAs()
{
	innerSave(false);
}

void SingleCaptureHandler::save()
{
	innerSave(true);
}

void SingleCaptureHandler::copy()
{
	auto image = mImageAnnotator->image();
	mClipboard->setImage(image);
}

void SingleCaptureHandler::innerSave(bool isInstant)
{
	auto image = mImageAnnotator->image();
	SaveOperation operation(mParent, image, isInstant, mPath, mToastService);
	auto saveResult = operation.execute();
	mPath =  saveResult.path;
	mIsSaved = saveResult.isSuccessful;
	captureChanged();
}

void SingleCaptureHandler::upload()
{
}

void SingleCaptureHandler::load(const CaptureDto &capture)
{
	resetStats();
	mImageAnnotator->loadImage(capture.screenshot);
	if (capture.isCursorValid()) {
		mImageAnnotator->insertImageItem(capture.cursor.position, capture.cursor.image);
	}
}

void SingleCaptureHandler::resetStats()
{
	mIsSaved = false;
	mPath = QString();
}

QImage SingleCaptureHandler::image() const
{
	return mImageAnnotator->image();
}

void SingleCaptureHandler::insertImageItem(const QPointF &pos, const QPixmap &pixmap)
{
	mImageAnnotator->insertImageItem(pos, pixmap);
}

void SingleCaptureHandler::addListener(ICaptureChangeListener *captureChangeListener)
{
	mCaptureChangeListener = captureChangeListener;
}

bool SingleCaptureHandler::discardChanges()
{
	auto image = mImageAnnotator->image();
	auto filename = PathHelper::extractFilename(mPath);
	CanDiscardOperation operation(mParent, image, !mIsSaved, mPath, filename, mToastService);
	return operation.execute();
}

void SingleCaptureHandler::captureChanged()
{
	if(mCaptureChangeListener != nullptr) {
		mCaptureChangeListener->captureChanged();
	}
}

void SingleCaptureHandler::markUnsaved()
{
	mIsSaved = false;
	captureChanged();
}
