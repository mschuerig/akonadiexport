/***************************************************************************
 *   Copyright (C) 2011 by Michael Schürig <michael@schuerig.de>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "akonadiexport.h"
#include <QTimer>
#include <KDE/KApplication>
#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>
#include <KDE/KLocale>

static const char description[] =
    I18N_NOOP("A KDE 4 Application");

static const char version[] = "0.1";


int main(int argc, char **argv)
{
    KAboutData about("akonadiexport", 0, ki18n("akonadiexport"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2011 Michael Schürig"), KLocalizedString(), 0, "michael@schuerig.de");
    about.addAuthor( ki18n("Michael Schürig"), KLocalizedString(), "michael@schuerig.de" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("list", ki18n("List top-level collections") );
    options.add("dump", ki18n("Dump the items of the given collection") );
    options.add("+[collection]", ki18n("name of a collection") );
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app(false);
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    QString collection;
    if ( args->count() > 0 ) {
        collection = args->arg(0);
    }
    AkonadiExport exp(collection);

    if ( args->isSet("list") ) {
      QTimer::singleShot(0, &exp, SLOT(listCollections()));
    } else {
        const char* op = 0;
        if ( args->isSet("dump") ) {
            op = SLOT(dump());
        }
        
        if (op) {
            QTimer::singleShot(0, &exp, op);
        }
    }

    QObject::connect( &exp, SIGNAL(finished()), &app, SLOT(quit()) );
    QObject::connect( &exp, SIGNAL(failed(const QString&)), &app, SLOT(quit()) );

    kDebug() << "app exec\n";
    return app.exec();
}
