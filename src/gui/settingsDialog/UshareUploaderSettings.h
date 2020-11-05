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

#ifndef KSNIP_USHAREUPLOADERSETTINGS_H
#define KSNIP_USHAREUPLOADERSETTINGS_H

#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QDesktopServices>

#include "src/backend/config/KsnipConfig.h"
#include "src/backend/uploader/ushare/UshareWrapper.h"
#include "src/gui/UshareHistoryDialog.h"
#include "src/widgets/CustomLineEdit.h"
#include "src/common/constants/DefaultValues.h"

class UshareUploaderSettings : public QGroupBox
{
	Q_OBJECT
public:
        explicit UshareUploaderSettings(KsnipConfig *ksnipConfig);
        ~UshareUploaderSettings() override;
	void saveSettings();

private:
	KsnipConfig *mConfig;
    QLineEdit *mTokenLineEdit;
    QLabel *mTokenLabel;
	QPushButton *mHistoryButton;
    UshareWrapper *mUshareWrapper;
	QGridLayout *mLayout;

	void initGui();
	void loadConfig();

private slots:
    void requestUsharePin();
    void getUshareToken();
    void ushareClientEntered(const QString &text);
    void ushareTokenUpdated(const QString &accessToken, const QString &refreshToken, const QString &username);
    void ushareTokenError(const QString &message);
    void showUshareHistoryDialog();
};

#endif //KSNIP_USHAREUPLOADERSETTINGS_H
