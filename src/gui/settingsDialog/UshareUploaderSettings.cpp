/*
 * Copyright (C) 2019 Damir Porobic <damir.porobic@gmx.com>
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

#include "UshareUploaderSettings.h"
#include <QDebug>

UshareUploaderSettings::UshareUploaderSettings(KsnipConfig *ksnipConfig) :
	mConfig(ksnipConfig),
    mTokenLineEdit(new QLineEdit(this)),
    mTokenLabel(new QLabel(this)),
	mHistoryButton(new QPushButton(this)),
    mUshareWrapper(new UshareWrapper(mConfig->ushareBaseUrl(), this)),
	mLayout(new QGridLayout(this))
{
	Q_ASSERT(mConfig != nullptr);

	initGui();
	loadConfig();
}

UshareUploaderSettings::~UshareUploaderSettings()
{
    delete mTokenLineEdit;
    delete mTokenLabel;
	delete mHistoryButton;
    delete mUshareWrapper;
	delete mLayout;
}

void UshareUploaderSettings::saveSettings()
{
    // should be both true
    qDebug() << mConfig->ushareAlwaysCopyToClipboard();
    qDebug() << mConfig->ushareOpenLinkInBrowser();
}

void UshareUploaderSettings::initGui()
{
    connect(mUshareWrapper, &UshareWrapper::tokenUpdated, this, &UshareUploaderSettings::ushareTokenUpdated);
    connect(mUshareWrapper, &UshareWrapper::error, this, &UshareUploaderSettings::ushareTokenError);

    mTokenLineEdit->setPlaceholderText(tr("Token"));
    mTokenLineEdit->setText(mConfig->ushareAccessToken());
    connect(mTokenLineEdit, &QLineEdit::textChanged, this, &UshareUploaderSettings::ushareClientEntered);

    mHistoryButton->setText(tr("Ushare History"));
    connect(mHistoryButton, &QPushButton::clicked, this, &UshareUploaderSettings::showUshareHistoryDialog);

	mLayout->setAlignment(Qt::AlignTop);
	mLayout->setColumnMinimumWidth(0, 10);
	mLayout->setRowMinimumHeight(4, 15);
	mLayout->setRowMinimumHeight(6, 15);
    mLayout->addWidget(mTokenLabel, 5, 0, 1, 3);
    mLayout->addWidget(mHistoryButton, 5, 3, 1, 1);
    mLayout->addWidget(mTokenLineEdit, 6, 0, 1, 3);

    setTitle(tr("Ushare Uploader"));
	setLayout(mLayout);
}

void UshareUploaderSettings::loadConfig()
{
    qDebug() << mConfig->ushareOpenLinkInBrowser();
    qDebug() << mConfig->ushareAlwaysCopyToClipboard();
    //mTokenLabel->setText(tr("Token") + ": " + mConfig->ushareUsername());
    QString link =( "<a href=\"https://ushare.phonexa.com/\">link</a> ");
    QString ll =  QString("To login please, use the following %1, copy token and paste in field below").arg(link);
    mTokenLabel->setText(ll);
    mTokenLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    mTokenLabel->setOpenExternalLinks(true);
    if(!mConfig->ushareAccessToken().isEmpty()) {
        mTokenLineEdit->setPlaceholderText(mConfig->ushareAccessToken());
	}
}

/*
 * Based on the entered client id and client secret we create a pin request and open it up in the
 * default browser.
 */
void UshareUploaderSettings::requestUsharePin()
{
	// Save client ID and Secret to config file
    mConfig->setUshareClientId(mTokenLineEdit->text().toUtf8());

	// Open the pin request in the default browser
    QDesktopServices::openUrl(mUshareWrapper->pinRequestUrl(mTokenLineEdit->text()));

	// Cleanup line edits
    mTokenLineEdit->setPlaceholderText(mTokenLineEdit->text());
    mTokenLineEdit->clear();
}

/*
 * Request a new token from imgur.com when clicked.
 */
void UshareUploaderSettings::getUshareToken()
{
//    mUshareWrapper->getAccessToken(mPinLineEdit->text().toUtf8(),
//                                  mConfig->ushareClientId(),
//                                  mConfig->ushareClientSecret());
    qInfo("%s", qPrintable(tr("Waiting for ushare.com…")));
}

void UshareUploaderSettings::ushareClientEntered(const QString& t)
{
    //mGetPinButton->setEnabled(!mTokenLineEdit->text().isEmpty() && !mClientSecretLineEdit->text().isEmpty());
    mConfig->setUshareAccessToken(t);
}

/*
 * We have received a new token from imgur.com, now we save it to config for
 * later use and inform the user about it.
 */
void UshareUploaderSettings::ushareTokenUpdated(const QString& accessToken, const QString& refreshToken, const QString& username)
{
    mConfig->setUshareAccessToken(accessToken.toUtf8());
    mConfig->setUshareRefreshToken(refreshToken.toUtf8());
    mConfig->setUshareUsername(username);

    QString link =( "<a href=\"https://ushare.phonexa.com/\">link</a> ");
    QString ll =  QString("To login please, use the following %1, copy token and paste in field below").arg(link);


    mTokenLabel->setText(ll);
    qInfo("%s", qPrintable(tr("Ushare.com token successfully updated.")));
}

/*
 * Something went wrong while requesting a new token, we write the message to
 * shell.
 */
void UshareUploaderSettings::ushareTokenError(const QString& message)
{
	qCritical("SettingsDialog returned error: '%s'", qPrintable(message));
    qInfo("%s", qPrintable(tr("Ushare.com token update error.")));
}

void UshareUploaderSettings::showUshareHistoryDialog()
{
    UshareHistoryDialog dialog;
	dialog.exec();
}

