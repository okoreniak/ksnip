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

#ifndef KSNIP_MULTICAPTUREHANDLER_H
#define KSNIP_MULTICAPTUREHANDLER_H

#include "src/gui/captureHandler/ICaptureHandler.h"
#include "src/gui/captureHandler/ICaptureTabStateHandler.h"
#include "src/gui/captureHandler/TabContextMenuAction.h"
#include "src/gui/operations/CanDiscardOperation.h"
#include "src/gui/operations/SaveOperation.h"
#include "src/gui/IToastService.h"
#include "src/gui/clipboard/IClipboard.h"
#include "src/gui/imageAnnotator/IImageAnnotator.h"
#include "src/gui/desktopService/IDesktopService.h"
#include "src/common/provider/NewCaptureNameProvider.h"
#include "src/common/provider/PathFromCaptureProvider.h"
#include "src/common/loader/IconLoader.h"
#include "src/backend/config/KsnipConfigProvider.h"

class MultiCaptureHandler : public QObject, public ICaptureHandler
{
Q_OBJECT
public:
	explicit MultiCaptureHandler(IImageAnnotator *imageAnnotator, IToastService *toastService, IClipboard *clipboard, IDesktopService *desktopService,
								 ICaptureTabStateHandler *captureTabStateHandler, QWidget *parent);
	~MultiCaptureHandler() override;
	bool canClose() override;
	bool canTakeNew() override;
	bool isSaved() const override;
	QString path() const override;
	void saveAs() override;
	void save() override;
	void copy() override;
    void upload() override;
	void load(const CaptureDto &capture) override;
	QImage image() const override;
	void insertImageItem(const QPointF &pos, const QPixmap &pixmap) override;
	void addListener(ICaptureChangeListener *captureChangeListener) override;

private:
	IImageAnnotator *mImageAnnotator;
	ICaptureTabStateHandler *mTabStateHandler;
	IToastService *mToastService;
	QWidget *mParent;
	ICaptureChangeListener *mCaptureChangeListener;
	NewCaptureNameProvider mNewCaptureNameProvider;
	PathFromCaptureProvider mPathFromCaptureProvider;
	KsnipConfig *mConfig;
	IClipboard *mClipboard;
	IDesktopService *mDesktopService;
	TabContextMenuAction *mSaveContextMenuAction;
	TabContextMenuAction *mSaveAsContextMenuAction;
	TabContextMenuAction *mOpenDirectoryContextMenuAction;
	TabContextMenuAction *mCopyPathToClipboardContextMenuAction;
	TabContextMenuAction *mCopyToClipboardContextMenuAction;

	bool discardChanges(int index);
	void removeTab(int currentTabIndex);
	void saveAt(int index, bool isInstant);

private slots:
	void tabCloseRequested(int index);
	void captureChanged();
	void captureEmpty();
	void annotatorConfigChanged();
	void addTabContextMenuActions();
	void saveAsTab(int index);
	void saveTab(int index);
	void openDirectoryTab(int index);
	void updateContextMenuActions(int index);
	void copyToClipboardTab(int index);
	void copyPathToClipboardTab(int index);
};

#endif //KSNIP_MULTICAPTUREHANDLER_H
