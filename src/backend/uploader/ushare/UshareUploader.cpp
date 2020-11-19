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

#include "UshareUploader.h"
#include <QInputDialog>
#include <QApplication>
#include <QDesktopServices>
#include <QMessageBox>

UshareUploader::UshareUploader() :
	mConfig(KsnipConfigProvider::instance()),
    mUshareWrapper(new UshareWrapper(mConfig->ushareBaseUrl(), nullptr)),
    mUshareResponseLogger(new UshareResponseLogger)

{
    connect(mUshareWrapper, &UshareWrapper::uploadFinished, this, &UshareUploader::UshareUploadFinished);
    connect(mUshareWrapper, &UshareWrapper::error, this, &UshareUploader::UshareError);
    connect(mUshareWrapper, &UshareWrapper::tokenUpdated, this, &UshareUploader::UshareTokenUpdated);
    connect(mUshareWrapper, &UshareWrapper::tokenRefreshRequired, this, &UshareUploader::UshareTokenRefresh);
}

UploaderType UshareUploader::type() const
{
    return UploaderType::Ushare;
}

UshareUploader::~UshareUploader()
{
    delete mUshareWrapper;
    delete mUshareResponseLogger;
}

void UshareUploader::upload(const QImage &image)
{
    mImage = image;
    if (mConfig->ushareAccessToken().isEmpty())
    {
        auto ok = false;
        CustomInputDialog dialog (QApplication::activeWindow());
        dialog.exec();
        ok = dialog.getResult();
        auto text = dialog.getText();
        if (!QDialog::Accepted == ok || text.isEmpty())
        {
            return;
        }
        mConfig->setUshareAccessToken(text);
    }
    if (!mConfig->ushareAccessToken().isEmpty()) {
        mUshareWrapper->startUpload(mImage, mConfig->ushareAccessToken());
    } else {
        mUshareWrapper->startUpload(mImage);
    }
}

void UshareUploader::UshareUploadFinished(const UshareResponse &response)
{
    qInfo("%s", qPrintable(tr("Upload to Ushare.com finished!")));
    mUshareResponseLogger->log(response);

	auto url = formatResponseUrl(response);

    //emit finished(UploadResult(UploadStatus::NoError, type(), url));
    QDesktopServices::openUrl(url);
    qInfo("%s %s", qPrintable(tr("Try to open url - ")), qPrintable(url));

}

QString UshareUploader::formatResponseUrl(const UshareResponse &response) const
{
    if (!mConfig->ushareLinkDirectlyToImage()) {
		return response.link().remove(QStringLiteral(".png"));
	}
	return response.link();
}

void UshareUploader::UshareError(const QString &message)
{
    qCritical("MainWindow: Ushare uploader returned error: '%s'", qPrintable(message));
    auto ok = false;
    ok = QMessageBox::warning(QApplication::activeWindow(), "Error", message);
	emit finished(UploadResult(UploadStatus::NoError, type(), message));
}

void UshareUploader::UshareTokenUpdated(const QString &accessToken, const QString &refreshToken, const QString &username)
{
    mConfig->setUshareAccessToken(accessToken.toUtf8());

    mConfig->setUshareRefreshToken(refreshToken.toUtf8());
    mConfig->setUshareUsername(username);

    qInfo("%s", qPrintable(tr("Received new token, trying upload again…")));
    upload(mImage);
}

void UshareUploader::UshareTokenRefresh()
{
    mUshareWrapper->refreshToken(mConfig->ushareRefreshToken(), mConfig->ushareClientId(), mConfig->ushareClientSecret());
    qInfo("%s", qPrintable(tr("Ushare token has expired, requesting new token…")));
}
