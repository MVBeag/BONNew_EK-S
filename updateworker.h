#ifndef UPDATEWORKER_H
#define UPDATEWORKER_H

#include <QThread>
#include "bsl/BSL_IO_UART.h"
#include "bsl/BSL_Interface.h"
#include <QMutex>


class UpdateWorker : public QObject
{
    Q_OBJECT

public:
    UpdateWorker( QObject *parent = 0 )
        : QObject( parent )
        , _bslInterface( Q_NULLPTR )
        , _initDone( false )
        , _stopSearching( false )
    {
    }
    ~UpdateWorker( )
    {
        delete _bslInterface; _bslInterface = Q_NULLPTR;
    }

signals:

    void versionChanged( const QString &model, const QString &version );
    void progressChanged( const unsigned int &progress );
    void result( const bool &status );

public slots:

    void doSearchDevice( int vendor, int product );
    void doStopSearching( );

    void doInstallFirmware( const QString &firmware , unsigned int size );
    void doProgressUpdate( const unsigned int &progress );
    void doInstallFirmwareResult( );

private:

    void initSerialPort( );
    bool isDeviceAvailable( const QSerialPortInfo &info, int vendor, int product );
    bool setProgrammingMode( QString &version );
    bool getFirmwareVersion( );

    QMutex         _mutex;
    BSL_Interface *_bslInterface;
    bool           _initDone;
    bool           _stopSearching;
};

#endif // UPDATEWORKER_H
