/*==========================================================================*\
|                                                                            |
|                                                                            |
| PC-side Bootstrap Loader communication Application                         |
|                                                                            |
| See main.c for full version and legal information                          |
|                                                                            |
\*==========================================================================*/
#include "BSL_IO_UART.h"
#include "BSL_Common.h"
#include "CRC.h"
#include <QDebug>
#include <QThread>
#include <QTimer>


//------------------------------------------------------------------------------

BSL_IO_UART::BSL_IO_UART(QObject *parent)
  : QObject( parent )
#ifdef __APPLE__
#else
  , _hComPort( INVALID_HANDLE_VALUE )
#endif
{
}

//------------------------------------------------------------------------------

BSL_IO_UART::~BSL_IO_UART()
{
  (void)close( );
}

//------------------------------------------------------------------------------

#ifdef __APPLE__
bool BSL_IO_UART::open( const QSerialPortInfo &portInfo )
{
  _serialPort.setPort( portInfo );

  if ( !_serialPort.setBaudRate( QSerialPort::Baud115200 ) )
  {
    qDebug( "Failed to set baud rate!" );
    return ( false );
  }

  if ( !_serialPort.setDataBits( QSerialPort::Data8 ) )
  {
    qDebug("Failed to set data bits!");
    return (false);
  }

  if ( !_serialPort.setParity( QSerialPort::NoParity ) )
  {
    qDebug("Failed to set parity!");
    return (false);
  }

  if ( !_serialPort.setStopBits( QSerialPort::OneStop ) )
  {
    qDebug("Failed to set stop bits!");
    return (false);
  }

  if ( !_serialPort.setFlowControl( QSerialPort::NoFlowControl ) )
  {
    qDebug( "Failed to set flow control!" );
    return ( false );
  }

  if ( !_serialPort.open( QIODevice::ReadWrite ) )
  {
    qDebug( "Failed to open serial port!" );
    return ( false );
  }

  return ( true );
}
#else
bool BSL_IO_UART::open( const wchar_t *comPort, long rate, unsigned char parity )
{
  Q_CHECK_PTR( comPort );

  _nakDelay = (DWORD)( (11 * MAX_UART_FRAME_SIZE) / (float)(rate / 1000) );

  _hComPort = CreateFile( comPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0 );
  if ( _hComPort == INVALID_HANDLE_VALUE )
  {
    qDebug() << __FUNCTION__ << "CreateFile failed!";
    return ( false );
  }

  if ( !SetupComm( _hComPort, QUEUE_SIZE, QUEUE_SIZE ) )
  {
    qDebug() << __FUNCTION__ << "SetupComm failed!";
    return ( false );
  }

  if ( !setTimeouts( ) )
  {
    qDebug() << __FUNCTION__ << "setTimeouts failed!";
    return ( false );
  }

  if ( !setState( rate, parity ) )
  {
    qDebug() << __FUNCTION__ << "setTimeouts failed!";
    return ( false );
  }

  clearBuffers( );

  return ( true );
}
#endif

//------------------------------------------------------------------------------

bool BSL_IO_UART::close( )
{
#ifdef __APPLE__
  _serialPort.close( );
#else
  if ( _hComPort != INVALID_HANDLE_VALUE )
  {
    if ( !restoreTimeouts( ) )
    {
      qDebug() << __FUNCTION__ << "restoreTimeouts failed!";
      return ( false );
    }

    if ( !CloseHandle( _hComPort ) )
    {
      qDebug() << __FUNCTION__ << "CloseHandle failed!";
      return ( false );
    }

    _hComPort = INVALID_HANDLE_VALUE;
  }
#endif

  return ( true );
}

//------------------------------------------------------------------------------

void BSL_IO_UART::clearBuffers( )
{
#ifdef __APPLE__
  (void)_serialPort.clear( );
#else
  (void)PurgeComm( _hComPort, PURGE_TXCLEAR | PURGE_TXABORT );
  (void)PurgeComm( _hComPort, PURGE_RXCLEAR | PURGE_RXABORT );
#endif
}

//------------------------------------------------------------------------------

unsigned char BSL_IO_UART::readByte( )
{
#ifdef __APPLE__
  if ( ( _serialPort.bytesAvailable( ) > 0 ) || _serialPort.waitForReadyRead( TIMEOUT_MS ) )
  {
    return ( _serialPort.read( 1 ).at( 0 ));
  }
  else
  {
    return ( TIMEOUT_ERROR );
  }
#else
  unsigned char byte = TIMEOUT_ERROR;

  unsigned long dwRead;
  unsigned long errors;

  QTimer timer;
  timer.setSingleShot( true );
  timer.setInterval( TIMEOUT_MS );
  timer.start( );

  do
  {
    if ( !ClearCommError( _hComPort, &errors, &_comState ) )
    {
      qDebug() << __FUNCTION__ << "ClearCommError failed!";
    }
  }
  while ( ( _comState.cbInQue == 0 ) && ( timer.remainingTime() > 0 ) );

  if ( timer.remainingTime() > 0 )
  {
    if ( !ReadFile( _hComPort, &byte, 1, &dwRead, NULL ) )
    {
      qDebug() << __FUNCTION__ << "ReadFile failed!";
    }
  }

  return byte;
#endif
}

//------------------------------------------------------------------------------

void BSL_IO_UART::readUntilTimeout( QByteArray &data )
{
  forever
  {
    unsigned char byte = readByte( );
    if ( byte == TIMEOUT_ERROR )
    {
      break;
    }

    data += byte;
  }
}

//------------------------------------------------------------------------------

void BSL_IO_UART::readGarbageUntilTimeout( )
{
  forever
  {
    unsigned char byte = readByte( );
    if ( byte == TIMEOUT_ERROR )
    {
      break;
    }
  }
}

//------------------------------------------------------------------------------

int BSL_IO_UART::write( const QByteArray &data )
{
#ifdef __APPLE__
  return ( _serialPort.write( data ) );
#else
  unsigned long dwWrite;

  PurgeComm( _hComPort, PURGE_RXCLEAR | PURGE_RXABORT );
  if ( !WriteFile( _hComPort, data.data(), data.size(), &dwWrite, NULL ) )
  {
      qDebug() << __FUNCTION__ << "WriteFile failed!";
  }

  return ( dwWrite );
#endif
}

//------------------------------------------------------------------------------

bool BSL_IO_UART::txProgrammingMode( )
{
  QByteArray mode;
  mode.append(QS_ESC);
  mode.append(QS_ESC);
  mode.append(BSL_MODE);

  if ( mode.size() != write(mode) )
  {
    qDebug("%s: set mode failed!", __FUNCTION__);
    return ( false );
  }

  return ( true );
}

//------------------------------------------------------------------------------

bool BSL_IO_UART::rxProgrammingMode( QString &version )
{
  bool result = false;
  QByteArray data;

  for ( int i = 0; i < 10; i++ )
  {
    readUntilTimeout( data );

    if ( data.size() < 1 )
    {
      continue;
    }

    if ( (BYTE)data.at( data.size( ) - 1 ) == QS_FRAME )
    {
      if ( ( (BYTE)data.at( 0 ) == QS_ESC ) &&
           ( (BYTE)data.at( 1 ) == QS_ESC ) &&
           ( (BYTE)data.at( 2 ) == BSL_MODE ) )
      {
        version = "";
        version.append( data.mid( PROG_MODE_VER_OFF, data.size( ) - 1 - PROG_MODE_VER_OFF ) );

        result = true;
      }
      else
      {
        qDebug() << "Wrong header!" << data.size( ) << dataToHexString( data );
      }

      break;
    }
  }

  if ( data.size( ) < PROG_MODE_MIN_RESP )
  {
    qDebug() << "Not enought data!" << data.size( ) << dataToHexString( data );
  }

  return ( result );
}

//------------------------------------------------------------------------------

bool BSL_IO_UART::setTimeouts( )
{
#ifdef __APPLE__
#else
  if ( !GetCommTimeouts( _hComPort, &_orgTimeouts ) )
  {
    qDebug() << __FUNCTION__ << "GetCommTimeouts failed!";
    return ( false );
  }

  COMMTIMEOUTS timeouts;
  timeouts.ReadIntervalTimeout         = 0;
  timeouts.ReadTotalTimeoutMultiplier  = 0;
  timeouts.ReadTotalTimeoutConstant    = 0;
  timeouts.WriteTotalTimeoutMultiplier = 0;
  timeouts.WriteTotalTimeoutConstant   = 0;

  if ( !SetCommTimeouts( _hComPort, &timeouts ) )
  {
    qDebug() << __FUNCTION__ << "SetCommTimeouts failed!";
    return ( false );
  }
#endif

  return ( true );
}

//------------------------------------------------------------------------------

bool BSL_IO_UART::restoreTimeouts( )
{
#ifdef __APPLE__
#else
  if ( !SetCommTimeouts( _hComPort, &_orgTimeouts ) )
  {
    qDebug() << __FUNCTION__ << "SetCommTimeouts failed!";
    return ( false );
  }
#endif

  return ( true );
}

//------------------------------------------------------------------------------

bool BSL_IO_UART::setState( long rate, unsigned char parity )
{
#ifdef __APPLE__
  Q_UNUSED( rate );
  Q_UNUSED( parity );
#else
  if ( !GetCommState( _hComPort, &_comDCB ) )
  {
    qDebug() << __FUNCTION__ << "GetCommState failed!";
    return ( false );
  }

  _comDCB.BaudRate     = rate;
  _comDCB.ByteSize     = 8;
  _comDCB.Parity       = ( parity > 0 ) ? EVENPARITY : NOPARITY;
  _comDCB.StopBits     = ONESTOPBIT;
  _comDCB.fBinary      = TRUE;
  _comDCB.fParity      = TRUE;
  _comDCB.ErrorChar    = (char)0xEF;
  _comDCB.fRtsControl  = RTS_CONTROL_ENABLE;
  _comDCB.fDtrControl  = DTR_CONTROL_ENABLE;
  _comDCB.fOutxCtsFlow = FALSE;
  _comDCB.fOutxDsrFlow = FALSE;
  _comDCB.fOutX        = FALSE;
  _comDCB.fInX         = FALSE;
  _comDCB.fNull        = FALSE;
  _comDCB.fErrorChar   = FALSE;

  if ( !SetCommState( _hComPort, &_comDCB ) )
  {
    qDebug() << __FUNCTION__ << "SetCommState failed!";
    return ( false );
  }
#endif

  return ( true );
}

//------------------------------------------------------------------------------

QString BSL_IO_UART::dataToHexString( const QByteArray &data )
{
  QString hexString;

  for ( int i = 0; i < data.size(); i++ )
  {
    hexString.append(QString("%1").arg((BYTE)data.at(i), 2, 16, QChar('0')));
  }

  return (hexString);
}

//------------------------------------------------------------------------------
