/*
 * Copyright (C) 2017 Damir Porobic <https://github.com/damirporobic>
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

#if  defined(_WIN32)  && defined(QT_NO_DEBUG)
// Prevent starting console in background under windows
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include <QApplication>

#include "BuildConfig.h"
#include "src/bootstrapper/BootstrapperFactory.h"
#include "src/backend/config/KsnipConfigProvider.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.setOrganizationName(QStringLiteral("Phonexa"));
    app.setOrganizationDomain(QStringLiteral("phonexa.com"));
    app.setApplicationName(QStringLiteral("Phonexa Screenshoter"));
    app.setApplicationVersion(QStringLiteral(KSNIP_VERSION));
    app.setDesktopFileName(QStringLiteral("com.phonexa.desktop"));


    app.setStyle(KsnipConfigProvider::instance()->applicationStyle());
    KsnipConfig* mConfig = KsnipConfigProvider::instance();
    UploaderType oldValue = mConfig->uploaderType();
    auto restoreOldUploader = false;

    if (oldValue != UploaderType::Ushare)
    {
        mConfig->setUploaderType(UploaderType::Ushare);
        restoreOldUploader = true;
    }
    mConfig->setUshareAlwaysCopyToClipboard(true);
    mConfig->setUshareOpenLinkInBrowser(true);

	BootstrapperFactory bootstrapperFactory;
    int retValue =  bootstrapperFactory.create()->start(app);
    if (restoreOldUploader)
        mConfig->setUploaderType(oldValue);
    return retValue;
}
