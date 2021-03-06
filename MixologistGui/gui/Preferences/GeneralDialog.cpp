/****************************************************************
 *  Copyright 2010, Fair Use, Inc.
 *  Copyright 2006-2007, crypton
 *  Copyright 2006, Matt Edman, Justin Hipple
 *
 *  This file is part of The Mixologist.
 *
 *  The Mixologist is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  The Mixologist is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Mixologist; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

#include <QSettings>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <gui/Preferences/GeneralDialog.h>
#include <gui/MainWindow.h> //for settings files
#include <gui/LibraryDialog.h>
#include "gui/Util/OSHelpers.h"
#include <interface/init.h>
#include <interface/iface.h>
#include <interface/files.h>
#include <interface/notifyqt.h>
#include <interface/settings.h>

GeneralDialog::GeneralDialog(QWidget *parent)
    : ConfigPage(parent) {
    /* Invoke the Qt Designer generated object setup routine */
    ui.setupUi(this);

    QSettings startSettings(*startupSettings, QSettings::IniFormat, this);
    QString email(startSettings.value("DefaultEmail", "").toString());

    if (email.isEmpty()) {
        enableClearLogin(false);
    } else {
        connect(ui.clearLogin, SIGNAL(clicked()), this, SLOT(clearLogin()));
        enableClearLogin(true);
    }

    if (canHandleLinkAssociation()) {
        if (getMixologyLinksAssociated()) {
            enableAssociateLinks(false);
        } else {
            connect(ui.associateLinks, SIGNAL(clicked()), this, SLOT(AssociateMixologyLinks()));
            enableAssociateLinks(true);
        }
    } else ui.associateLinks->setVisible(false);

    connect(ui.showAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced(bool)));

    /* Setup display */
    if (canHandleRunOnBoot()) {
        ui.runOnBoot->setChecked(getRunOnBoot());
    } else ui.runOnBoot->setVisible(false);

    QSettings settings(*mainSettings, QSettings::IniFormat, this);
    if (settings.value("Gui/ShowAdvanced", DEFAULT_SHOW_ADVANCED).toBool()) {
        ui.showAdvanced->setChecked(true);
        ui.showAdvanced->setText("Advanced features enabled");
    } else {
        ui.showAdvanced->setChecked(false);
        ui.showAdvanced->setText("Advanced features disabled");
    }
}

bool GeneralDialog::save() {
    if (canHandleRunOnBoot()) setRunOnBoot(ui.runOnBoot->isChecked());
    return true;
}

void GeneralDialog::AssociateMixologyLinks() {
    if (setMixologyLinksAssociated()) {
        enableAssociateLinks(false);
    } else {
        QMessageBox::warning (this,
                              "Problem writing to registry",
                              "There was a problem setting mixology: links to open with the Mixologist.\n\nThis is usually because you do not have administrative rights.\n\nRestart by right clicking and choosing \"Run as administrator\" to set the links.",
                              QMessageBox::Ok);
    }
}

void GeneralDialog::toggleShowOnStartup(bool checked) {
    QSettings settings(*mainSettings, QSettings::IniFormat, this);
    settings.setValue("Gui/ShowMainWindowAtStart", checked);
}

void GeneralDialog::enableClearLogin(bool enable) {
    if (enable) {
        ui.clearLogin->setEnabled(true);
        ui.clearLogin->setToolTip("Click to clear all saved login information.\nThis means you will have to enter both your email and password next time you startup.");
    } else {
        ui.clearLogin->setEnabled(false);
        ui.clearLogin->setToolTip("No auto login information to clear");
    }
}

void GeneralDialog::enableAssociateLinks(bool enable) {
    if (enable) {
        ui.associateLinks->setEnabled(true);
        ui.associateLinks->setToolTip("If associated, then everytime you click a mixology: link,\nit will automatically be handled by the Mixologist.");
    } else {
        ui.associateLinks->setEnabled(false);
        ui.associateLinks->setToolTip("mixology: links already associated.");
    }
}

void GeneralDialog::clearLogin() {
    enableClearLogin(false);
    QSettings settings(*startupSettings, QSettings::IniFormat, this);
    settings.remove("DefaultEmail");
    settings.remove("DefaultPassword");
}

void GeneralDialog::showAdvanced(bool show) {
    QSettings settings(*mainSettings, QSettings::IniFormat, this);
    settings.setValue("Gui/ShowAdvanced", show);
    mainwindow->showAdvanced(show);
    mainwindow->libraryDialog->showAdvanced(show);
    mainwindow->preferencesWindow->connectionDialog->showAdvanced(show);
    if (show) ui.showAdvanced->setText("Advanced features enabled");
    else ui.showAdvanced->setText("Advanced features disabled");
}
