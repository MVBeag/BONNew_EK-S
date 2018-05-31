#ifndef UPDATEDRIVER_H_
#define UPDATEDRIVER_H_

//--- includes -----------------------------------------------------------------------------------

#include <QObject>
#include <QMap>
#include <QThread>
#include "targetinfo.h"


//------------------------------------------------------------------------------------------------

class UpdateDriver : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool active READ active NOTIFY activeChanged )
    Q_PROPERTY( int progress READ progress NOTIFY progressChanged )
    Q_PROPERTY( int modelType READ modelType )
    Q_PROPERTY( QString modelName READ modelName )
    Q_PROPERTY( QString modelPicture READ modelPicture )
    Q_PROPERTY( QString versionCurrent READ versionCurrent )
    Q_PROPERTY( QString versionNew READ versionNew )

public:

    explicit UpdateDriver( QObject *parent = 0 );
    ~UpdateDriver( );

    bool    active( );
    int     progress( );
    int     modelType( );
    QString modelName( );
    QString modelPicture( );
    QString versionCurrent( );
    QString versionNew( );

signals:

    void activeChanged( );
    void progressChanged( );

    void searchDevice( int vendor, int product );
    void stopSearching( );
    void installFirmware( const QString &firmware, unsigned int size );

public slots:

    bool isDriverAvailable();
    bool isDriverLoaded();
    void search( bool on );
    void versionHandle( const QString &modelName, const QString &version );
    bool isUpdateAvailable( );
    void openFileNatively(const QString &resourceFile, const QString &outputFile);
    void openChangelogNatively( );
    void openDocumentationNatively( );
    void install( );
    void progressHandle( const unsigned int &progress );
    void resultHandle( const bool &result );
    bool result( );

private:

    static const int     _vendor;
    static const int     _product;
    static const QString _changelogLocal;
    static const QString _documentationLocal;

    bool                 _active;
    int                  _progress;
    bool                 _result;
    TargetInfo           _target;
    QThread              _thread;

    Q_DISABLE_COPY( UpdateDriver )
};

#endif // UPDATEDRIVER_H_

//--- eof ----------------------------------------------------------------------------------------
