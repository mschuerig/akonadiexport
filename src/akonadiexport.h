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

#ifndef AKONADIEXPORT_H
#define AKONADIEXPORT_H

#include <qobject.h>
#include <qstringlist.h>
#include <Akonadi/Collection>
#include <Akonadi/Item>

class QTimer;
class KJob;

class AkonadiExport : public QObject
{
    Q_OBJECT
public:
    AkonadiExport(const QString& collectionName, int timeout = 2000);

    virtual ~AkonadiExport();

public slots:
    void listCollections();
    void dump();

signals:
    void finished();
    void failed( const QString& error );
    
private slots:
    void handleResult( KJob* job );
    void displayCollections( const Akonadi::Collection::List& collections );
    KJob* resolveCollection();
    void collectionResolved( KJob* job );
    void fetchAndDumpItems();
    KJob* fetchItems();
    void itemsFetched( KJob* job );
    void dumpItems();
    void timedOut();
    void stopTimeout();
    
private:
    const QString collectionName;
    Akonadi::Collection collection;
    Akonadi::Item::List items;
    QTimer* timer;
    int timeout;
    
    void startTimeout();
};

#endif // _AKONADIEXPORT_H_
