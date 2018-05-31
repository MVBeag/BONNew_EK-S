#include "updateworker.h"
#include <QDebug>
#include <QSerialPortInfo>

#undef SIROS_MOCKUP

void UpdateWorker::doSearchDevice( int vendor, int product )
{
#ifdef SIROS_MOCKUP
  Q_UNUSED(vendor);
  Q_UNUSED(product);

  emit versionChanged( "Siros 400 S", "49.06" );
//  emit versionChanged( "Siros 400 S", "49.07" );
//  emit versionChanged( "Siros 400 S", "49.08" );
//  emit versionChanged( "Siros 400 L", "52.01" );
//  emit versionChanged( "Siros 400 L", "52.02" );
//  emit versionChanged( "Siros 400 L", "52.03" );

  emit result( true );
#else
  bool status = false;

  foreach ( const QSerialPortInfo &info, QSerialPortInfo::availablePorts( ) )
  {
    QMutexLocker locker( &_mutex );

    if ( _stopSearching )
    {
      break;
    }

    status = isDeviceAvailable( info, vendor, product );
    if ( status )
    {
      break;
    }
  }

  emit result( status );
#endif
}

//-------------------------------------------------------------

void UpdateWorker::doStopSearching( )
{
  QMutexLocker locker( &_mutex );

  _stopSearching = false;
}

//-------------------------------------------------------------

void UpdateWorker::doInstallFirmware( const QString &firmware, unsigned int size )
{
  qDebug("------------------------------------------------");

  _bslInterface->installFirmware( firmware, size );
  _bslInterface->closePort( );
}

//-------------------------------------------------------------

void UpdateWorker::doProgressUpdate( const unsigned int &progress )
{
  emit progressChanged( progress );
}

//-------------------------------------------------------------

void UpdateWorker::doInstallFirmwareResult( )
{
  emit result( _bslInterface->getPercent( ) == 100 ? ( true ) : ( false ) );
}

//-------------------------------------------------------------

void UpdateWorker::initSerialPort( )
{
  delete _bslInterface; _bslInterface = Q_NULLPTR;

  _bslInterface = new BSL_Interface( BSL_BUFFER_SIZE, this );

  Q_CHECK_PTR( _bslInterface );

  connect( _bslInterface, SIGNAL( progressChanged( uint ) ),
           this, SLOT( doProgressUpdate( uint ) ) );
  connect( _bslInterface, SIGNAL( firmwareInstallDone( ) ),
           this, SLOT( doInstallFirmwareResult( ) ) );
}

//-------------------------------------------------------------

bool UpdateWorker::isDeviceAvailable( const QSerialPortInfo &info, int vendor, int product )
{
  bool status = false;

  qDebug( ) << "------------------------------------------------";
  qDebug( ) << "Name         :" << info.portName( );
  qDebug( ) << "Description  :" << info.description( );
  qDebug( ) << "Manufacturer :" << info.manufacturer( );

  if ( info.hasVendorIdentifier( ) )
  {
    qDebug( ) << "Vendor ID    :" << info.vendorIdentifier( );
  }

  if ( info.hasProductIdentifier( ) )
  {
    qDebug( ) << "Product ID   :" << info.productIdentifier( );
  }

  if ( ( vendor == info.vendorIdentifier( ) ) &&
       ( product == info.productIdentifier( ) ) )
  {
    initSerialPort( );

    if ( _bslInterface->openPort( info ) )
    {
      status = getFirmwareVersion( );
      if ( !status )
      {
        _bslInterface->closePort( );
      }
    }
  }

  return ( status );
}

//-------------------------------------------------------------

bool UpdateWorker::setProgrammingMode( QString &version )
{
  return ( _bslInterface->rxProgrammingMode( version ) );
}

//-------------------------------------------------------------

bool UpdateWorker::getFirmwareVersion()
{
  QString receivedVersion;
  if ( setProgrammingMode(receivedVersion) )
  {
    QString model;
    QString versionNumberMajor;
    QString versionNumberMinor;
    QString versionCurrent;

    QStringList versionTokens = receivedVersion.split(QRegExp("\\b"));

     qDebug() << "Tokens       :" << versionTokens;
    qDebug() << "Version      :" << receivedVersion;

    // Siros S
    // Version 49.01 tokens e.g. ("", "49", ".", "01", "(", "1623", ")")
    // Version 49.02 tokens e.g. ("", "49", ".", "05", "(", "1799", "),", "Siros", " ", "400S", "")

    // Siros L
    // Version 52.02 tokens e.g. ("", "52", ".", "02", "(", "2316", "),", "Siros", " ", "400", " ", "L", "")

    versionNumberMajor = versionTokens.at(1);
    versionNumberMinor = versionTokens.at(3);

    if ( ( versionNumberMajor == "49" ) && ( versionNumberMinor == "01" ) )
    {
      if ( versionTokens.length() != 7 )
      {
        qDebug() << __FUNCTION__ << "49.01 found but incompatible number of tokens!";
        return (false);
      }

      model = "Siros";
    }
    else if ( ( versionNumberMajor == "52" ) && ( versionNumberMinor.toInt() >= 2 ) )
    {
      if ( versionTokens.length() != 13 )
      {
        qDebug() << __FUNCTION__ << "Incompatible number of tokens!";
        return (false);
      }

      model = versionTokens.at(7);
      model.append(" ");
      model.append(versionTokens.at(9));
      model.append(" ");
      model.append(versionTokens.at(11));
    }
    else
    {
      if ( versionTokens.length() != 11 )
      {
        qDebug() << __FUNCTION__ << "Incompatible number of tokens!";
        return (false);
      }

      model = versionTokens.at(7);
      model.append(" ");
      QString type = versionTokens.at(9);
      if ( type == "400S" )
      {
        type = "400 S";
      }
      else if ( type == "400L" )
      {
        type = "400 L";
      }
      else if ( type == " 800S" )
      {
        type = " S";
      }
      else if ( type == "800L" )
      {
        type = "800 L";
      }
      model.append(type);
    }

    if ( versionNumberMinor.compare("40") == 0 )
    {
      versionNumberMinor = "04";
    }

    versionCurrent = versionNumberMajor;
    versionCurrent += ".";
    versionCurrent += versionNumberMinor;

    emit versionChanged(model, versionCurrent);

    return (true);
  }
  else
  {
    qDebug() << __FUNCTION__ << "setProgrammingMode failed!";
    return (false);
  }
}

//-------------------------------------------------------------
