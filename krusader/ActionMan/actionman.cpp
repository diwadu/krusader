/*****************************************************************************
 * Copyright (C) 2006 Jonas Bähr <jonas.baehr@web.de>                        *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This package is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this package; if not, write to the Free Software               *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA *
 *****************************************************************************/

#include "actionman.h"

#include <KI18n/KLocalizedString>
#include <KWidgetsAddons/KMessageBox>

#include "useractionpage.h"
#include "../krusader.h"
#include "../UserAction/useraction.h"

ActionMan::ActionMan(QWidget * parent)
        : KDialog(parent)
{
    setWindowModality(Qt::WindowModal);
    setWindowTitle(i18n("ActionMan - Manage Your Useractions"));
    setButtons(KDialog::Apply | KDialog::Close);

    userActionPage = new UserActionPage(this);
    setMainWidget(userActionPage);

    connect(this, SIGNAL(applyClicked()), this, SLOT(slotApply()));
    connect(this, SIGNAL(closeClicked()), this, SLOT(slotClose()));
    connect(userActionPage, SIGNAL(changed()), SLOT(slotEnableApplyButton()));
    connect(userActionPage, SIGNAL(applied()), SLOT(slotDisableApplyButton()));
    enableButtonApply(false);

    exec();

    krApp->updateUserActions();
}

ActionMan::~ActionMan()
{
}

void ActionMan::slotClose()
{
    if (userActionPage->readyToQuit())
        reject();
}

void ActionMan::slotApply()
{
    userActionPage->applyChanges();
}

void ActionMan::slotEnableApplyButton()
{
    enableButtonApply(true);
}

void ActionMan::slotDisableApplyButton()
{
    enableButtonApply(false);
}



#include "actionman.moc"
