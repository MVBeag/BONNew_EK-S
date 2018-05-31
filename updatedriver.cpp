#include "updatedriver.h"
#include "updateworker.h"
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QUrl>
#include <QDir>
#include <QStringList>

#ifndef __APPLE__
#include <windows.h>
#include <psapi.h>
#include <tchar.h>
#endif

const int UpdateDriver::_vendor  = 1240;
const int UpdateDriver::_product = 223;

#ifdef __APPLE__
const QString UpdateDriver::_changelogLocal = "/tmp/changelog.txt";
const QString UpdateDriver::_documentationLocal = "/tmp/missing_documentation_siros_osx.pdf";
#else
const QString UpdateDriver::_changelogLocal = "changelog.txt";
const QString UpdateDriver::_documentationLocal = "Instructions Windows - Follow those steps.pdf";
#endif


UpdateDriver::UpdateDriver( QObject *parent )
  : QObject( parent )
  , _active( false )
  , _progress( 0 )
  , _result( false )
{
  UpdateWorker *updateWorker = new UpdateWorker;
  updateWorker->moveToThread( &_thread );

  connect( &_thread, SIGNAL( finished( ) ),
           updateWorker, SLOT( deleteLater( ) ) );

  connect( this, SIGNAL( searchDevice( int, int ) ),
           updateWorker, SLOT( doSearchDevice( int, int ) ) );
  connect( this, SIGNAL( stopSearching( ) ),
           updateWorker, SLOT( doStopSearching( ) ) );
  connect( this, SIGNAL( installFirmware( const QString &, unsigned int ) ),
           updateWorker, SLOT( doInstallFirmware( const QString &, unsigned int ) ) );

  connect( updateWorker, SIGNAL( versionChanged( const QString &, const QString & ) ),
           this, SLOT( versionHandle( const QString &, const QString & ) ) );
  connect( updateWorker, SIGNAL( progressChanged( const unsigned int & ) ),
           this, SLOT( progressHandle( const unsigned int & ) ) );
  connect( updateWorker, SIGNAL( result( const bool & ) ),
           this, SLOT( resultHandle( const bool & ) ) );

  _thread.start( );
  _thread.setPriority( QThread::TimeCriticalPriority );
}

//-------------------------------------------------------------

UpdateDriver::~UpdateDriver( )
{
  _thread.quit( );
  _thread.wait( );
}

//-------------------------------------------------------------

bool UpdateDriver::active( )
{
  return _active;
}

//-------------------------------------------------------------

int UpdateDriver::progress( )
{
  return _progress;
}

//-------------------------------------------------------------

int UpdateDriver::modelType( )
{
  return _target.type( );
}

//-------------------------------------------------------------

QString UpdateDriver::modelName( )
{
    return _target.name( );
}

//-------------------------------------------------------------

QString UpdateDriver::modelPicture( )
{
  QString logo = _target.firmwareLogo( );

  if ( logo.isEmpty( ) )
  {
    qDebug( ) << "Firmware is missing for model" << _target.name( );
  }

  return logo;
}

//-------------------------------------------------------------

QString UpdateDriver::versionCurrent( )
{
  return _target.version( );
}

//-------------------------------------------------------------

QString UpdateDriver::versionNew( )
{
    return _target.firmwareVersion( );
}

//-------------------------------------------------------------

bool UpdateDriver::isDriverAvailable( )
{
#ifdef __APPLE__
  return true;
#else
  bool driverFound = false;

  QDir dir("C:\\Windows\\System32\\DriverStore\\FileRepository");
  QStringList list = dir.entryList();

  for ( int i = 0; i < list.size(); i++ )
  {
    if ( list.at(i).startsWith("mchpcdc.inf") )
    {
      driverFound = true;
      break;
    }
  }

  return driverFound;
#endif
}

//-------------------------------------------------------------

bool UpdateDriver::isDriverLoaded( )
{
#ifdef __APPLE__
  return true;
#else
  bool driverFound = false;

  const int arraySize = 1024;
  LPVOID drivers[arraySize];
  DWORD cbNeeded;
  int cDrivers, i;

  if ( EnumDeviceDrivers( drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers ))
  {
      TCHAR szDriver[arraySize];
      cDrivers = cbNeeded / sizeof(drivers[0]);

      for ( i = 0; i < cDrivers; i++ )
      {
          if ( GetDeviceDriverBaseName( drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0]) ) )
          {
              QString driverName = QString::fromWCharArray( szDriver );

              if ( driverName == "usbser.sys" )
              {
                  driverFound = true;
              }
          }
      }
  }
  else
  {
      qDebug() << QString( "Can't enumerate drivers!" );
  }

  return driverFound;
#endif
}

//-------------------------------------------------------------

void UpdateDriver::search( bool on )
{
  _active = on;

  if ( on )
  {
    emit searchDevice( _vendor, _product );
  }
  else
  {
    emit stopSearching( );
  }
}

//-------------------------------------------------------------

void UpdateDriver::versionHandle( const QString &model, const QString &version )
{
  _target.setName( model );
  _target. setVersion( version );
}

//-------------------------------------------------------------

bool UpdateDriver::isUpdateAvailable( )
{
  QStringList verCur = _target.version( ).split( "." );
  QStringList verNew = versionNew( ).split( "." );

  if ( ( verCur.length( ) != 2 ) || ( verNew.length( ) != 2 ) )
  {
    return false;
  }

  int mjCur = verCur.at( 0 ).toInt( );
  int mnCur = verCur.at( 1 ).toInt( );
  int mjNew = verNew.at( 0 ).toInt( );
  int mnNew = verNew.at( 1 ).toInt( );

  if ( ( ( mjNew == mjCur ) && ( mnNew > mnCur ) ) || ( mjNew > mjCur ) )
  {
    return true;
  }

  return false;
}

//-------------------------------------------------------------

void UpdateDriver::openFileNatively(const QString &resourceFile, const QString &outputFile)
{
  Q_ASSERT(!resourceFile.isEmpty());
  Q_ASSERT(!outputFile.isEmpty());

  QFile file_R( resourceFile );
  QFile file_W( outputFile );

  if ( !file_R.open( QFile::ReadOnly ) )
  {
    qDebug( ) << "file_R.open() failed!";
    return;
  }

  if ( !file_W.open( QFile::WriteOnly ) )
  {
    qDebug( ) << "file_W.open() failed!";
    file_R.close( );
    return;
  }

  while ( !file_R.atEnd( ) )
  {
    QByteArray line = file_R.readLine( );
    file_W.write( line, line.length( ) );
  }

  file_W.close( );
  file_R.close( );

  QDesktopServices::openUrl( QUrl::fromLocalFile( outputFile ) );
}

//-------------------------------------------------------------

void UpdateDriver::openChangelogNatively( )
{
  QString changelog = _target.firmwareChangelog( );

  if ( !changelog.isEmpty( ) )
  {
    openFileNatively( changelog, _changelogLocal );
  }
  else
  {
    qDebug( ) << "Changelog is missing for model" << _target.name( );
  }
}

//-------------------------------------------------------------

void UpdateDriver::openDocumentationNatively()
{
#ifdef __APPLE__
  qDebug( ) << "Documentation is missing OSX";
#else
  openFileNatively( ":/doc/" + _documentationLocal, _documentationLocal );
#endif
}

//-------------------------------------------------------------

void UpdateDriver::install( )
{
  QString firmware = _target.firmwareImage( );
  unsigned int size = _target.firmwareSize( );

  if ( !firmware.isEmpty( ) && ( size > 0 ) )
  {
    _active   = true;
    _progress = 0;

    emit installFirmware( firmware, size );
  }
  else
  {
    qDebug( ) << "Firmware is missing for model" << _target.name( );
  }
}

//-------------------------------------------------------------

void UpdateDriver::progressHandle( const unsigned int &progress )
{
  _progress = progress;

  emit progressChanged( );
}

//-------------------------------------------------------------

void UpdateDriver::resultHandle( const bool &result )
{
  _active = false;
  _result = result;

  qDebug( "Status       : %s", result ? "OK" : "NOK" );

  emit activeChanged( );
}

//-------------------------------------------------------------

bool  UpdateDriver::result( )
{
  return _result;
}

//-------------------------------------------------------------
