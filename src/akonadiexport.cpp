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

#include <iostream>

#include <QTimer>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <akonadi/private/collectionpathresolver_p.h>
#include <KUrl>
#include "akonadiexport.h"

using namespace Akonadi;

AkonadiExport::AkonadiExport(const QString& collectionName, int timeout)
    : collectionName(collectionName), timeout(timeout)
{
    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(timedOut()) );
    timer->setSingleShot(true);
}

AkonadiExport::~AkonadiExport()
{
}

void
AkonadiExport::listCollections()
{
    kDebug() << "listCollections\n";

    CollectionFetchJob* job = new CollectionFetchJob( Collection::root(),
        CollectionFetchJob::Recursive );
    //###job->fetchScope().setContentMimeTypes( QStringList() << "message/rfc822" );
    connect( job, SIGNAL(result(KJob*)), SLOT(stopTimeout()) );
    connect( job, SIGNAL(result(KJob*)), SLOT(handleResult(KJob*)) );
    connect( job, SIGNAL(collectionsReceived(const Akonadi::Collection::List&)),
            SLOT(displayCollections(const Akonadi::Collection::List&)) );
    startTimeout();
}

void
AkonadiExport::displayCollections(const Collection::List& collections)
{
    foreach ( const Collection& coll, collections ) {
        std::cout << "Name: " << qPrintable(coll.name());
        std::cout << ", ID: " << coll.id() << std::endl;
    }
    
    emit finished();
}

void
AkonadiExport::dump()
{
    kDebug() << "dump\n";
    
    KJob* job = resolveCollection();
    connect( job, SIGNAL(result(KJob*)), SLOT(fetchAndDumpItems()) );
    
    startTimeout();
}

KJob*
AkonadiExport::resolveCollection()
{
    CollectionPathResolver* resolver = new CollectionPathResolver(collectionName);
    connect( resolver, SIGNAL(result(KJob*)), SLOT(handleResult(KJob*)) );
    connect( resolver, SIGNAL(result(KJob*)), SLOT(collectionResolved(KJob*)) );
    
    return resolver;
}

void
AkonadiExport::collectionResolved( KJob* job )
{
    kDebug() << "collectionResolved\n";
    
    CollectionPathResolver* resolver = qobject_cast<CollectionPathResolver*>(job);
    kDebug() << "path: " << resolver->path()       << "\n";
    kDebug() << "id: "   << resolver->collection() << "\n";
    collection = Collection(resolver->collection());
}

void
AkonadiExport::fetchAndDumpItems()
{
    KJob* job = fetchItems();
    connect( job, SIGNAL(result(KJob*)), SLOT(stopTimeout()) );
    connect( job, SIGNAL(result(KJob*)), SLOT(dumpItems()) );
}

KJob*
AkonadiExport::fetchItems()
{
    kDebug() << "fetchItems\n";
    
    ItemFetchJob* job = new ItemFetchJob(collection);
    job->fetchScope().fetchFullPayload();
    connect( job, SIGNAL(result(KJob*)), SLOT(handleResult(KJob*)) );
    connect( job, SIGNAL(result(KJob*)), SLOT(itemsFetched(KJob*)) );
    return job;
}

void
AkonadiExport::itemsFetched( KJob* job )
{
    kDebug() << "itemsFetched\n";
    
    items = qobject_cast<ItemFetchJob*>(job)->items();
}

void
AkonadiExport::dumpItems()
{
    kDebug() << "dumpItems\n";
    
    foreach ( const Akonadi::Item &item, items ) {
      kDebug() << "Item ID:" << item.id() << "\n";
      kDebug() << item.payloadData() << "\n";
    }
    
    emit finished();
}


void
AkonadiExport::handleResult( KJob* job )
{
    kDebug() << "handleResult\n";
    
    if ( job->error() ) {
        kDebug() << "failure: " << job->errorString() << "\n";
        
        // this does not work as intended
        // other slots are still called
        emit failed( job->errorString() );
    }
}

void
AkonadiExport::startTimeout()
{
    timer->start( timeout );
}

void
AkonadiExport::stopTimeout()
{
    timer->stop();
}

void
AkonadiExport::timedOut()
{
    kDebug() << "timed out\n";
    emit failed("timeout");
}

#include "akonadiexport.moc"
