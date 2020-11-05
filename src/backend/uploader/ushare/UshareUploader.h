/*
 * Copyright (C) 2018 Damir Porobic <damir.porobic@gmx.com>
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

#ifndef KSNIP_UshareUPLOADER_H
#define KSNIP_UshareUPLOADER_H

#include <QImage>

#include "UshareWrapper.h"
#include "UshareResponseLogger.h"
#include "src/backend/uploader/IUploader.h"
#include "src/backend/uploader/UploadResult.h"
#include "src/backend/config/KsnipConfigProvider.h"
#include "src/common/constants/DefaultValues.h"
#include "src/gui/CustomInputDialog.h"


class UshareUploader : public QObject, public IUploader
{
Q_OBJECT
public:
    explicit UshareUploader();
    ~UshareUploader() override;

    void upload(const QImage &image) override;
	UploaderType type() const override;

signals:
    void finished(const UploadResult &result) override;

private:
	KsnipConfig *mConfig;
        UshareWrapper *mUshareWrapper;
    UshareResponseLogger *mUshareResponseLogger;
    QImage mImage;

private slots:
    void UshareUploadFinished(const UshareResponse &response);
    void UshareError(const QString &message);
    void UshareTokenUpdated(const QString &accessToken, const QString &refreshToken, const QString &username);
    void UshareTokenRefresh();
        QString formatResponseUrl(const UshareResponse &response) const;
};

#endif //KSNIP_UshareUPLOADER_H
