#include "BSL_Interface.h"
#include "BSL_IO_UART.h"
#include "TextFileIO.h"
#include "CRC.h"
#include <QDebug>
#include <QThread>


//------------------------------------------------------------------------------

BSL_Interface::BSL_Interface( unsigned int nBufferSize, QObject *parent )
  : QObject( parent )
  , _bufferSize( nBufferSize )
  , _serial( new BSL_IO_UART( ) )
  , _percent( 0 )
  , _rxTxtBytes( 0 )
{
  Q_CHECK_PTR( _serial );
}

//------------------------------------------------------------------------------

BSL_Interface::~BSL_Interface( )
{
  delete _serial; _serial = Q_NULLPTR;
}

//------------------------------------------------------------------------------

bool BSL_Interface::openPort( const QSerialPortInfo &info )
{
#ifdef __APPLE__
  if ( !_serial->open( info ) )
  {
    qDebug() << __FUNCTION__ << "open failed!";
    return ( false );
  }
#else
  QString com = "\\\\.\\" + info.portName( );
  wchar_t* cCom = new wchar_t[com.length( ) + 1];
  com.toWCharArray( cCom );
  cCom[com.length( )] = 0;

  if ( !_serial->open( cCom, UART_BAUDRATE, UART_PARITY ) )
  {
    qDebug() << __FUNCTION__ << "open failed!";
    return ( false );
  }
#endif

  return ( true );
}

//------------------------------------------------------------------------------

void BSL_Interface::closePort( )
{
  if ( !_serial->close( ) )
  {
    qDebug() << __FUNCTION__ << "close failed!";
  }
}

//------------------------------------------------------------------------------

void BSL_Interface::installFirmware( const QString &firmware, unsigned int size )
{
  Q_ASSERT( !firmware.isEmpty( ) );

  if ( !authenticate( ) )
  {
    qDebug() << "Authentication failed!";
    goto exit;
  }

  if ( !erase( ) )
  {
    qDebug() << "Erase failed!";
    goto exit;
  }

  rxTxtFileBegin( firmware.toLocal8Bit( ).data( ) );

  forever
  {
    if ( !rxTxtFileContinue( size ) )
    {
      break;
    }
  }

exit:

  rxTxtFileEnd();
}

//------------------------------------------------------------------------------

bool BSL_Interface::authenticate( )
{
  qDebug() << "Authenticating...";

  bool expectPasswordErr = true;

  for ( int i = 0; i < 10; i++ )
  {
    if ( rxPassword( ) == FLASH_ACK )
    {
      QByteArray data;
      _serial->readUntilTimeout( data );

      if ( expectPasswordErr )
      {
        if ( processRspData( data, BSL_PASSWORD_ERROR ) )
        {
          expectPasswordErr = false;
        }
      }
      else
      {
        if ( processRspData( data ) )
        {
          return ( true );
        }
      }
    }

    if ( expectPasswordErr )
    {
      qDebug() << "Resending wrong password...";
    }
    else
    {
      qDebug() << "Sending password...";
    }
  }

  return ( false );
}

//------------------------------------------------------------------------------

bool BSL_Interface::erase( )
{
  qDebug() << "Erasing...";

  for ( int i = 0; i < 10; i++ )
  {
    if ( rxMassErase( ) == FLASH_ACK )
    {
      QByteArray data;
      _serial->readUntilTimeout( data );

      if ( processRspData( data ) )
      {
        return ( true );
      }
    }

    qDebug() << __FUNCTION__ << " repeat " + QString::number(i+1);
  }

  return ( false );
}

//------------------------------------------------------------------------------

bool BSL_Interface::rxProgrammingMode( QString &version )
{
  _serial->readGarbageUntilTimeout( );

  if ( _serial->txProgrammingMode( ) )
  {
    return ( _serial->rxProgrammingMode( version ) );
  }
  else
  {
    return ( false );
  }
}

//------------------------------------------------------------------------------

void BSL_Interface::rxTxtFileBegin( const char* fileName )
{
  qDebug( "Writing %s...", fileName );

  _rxTxtBytes = 0;

  openTI_TextForRead( fileName );
  _rxTxtTime.start( );
}

//------------------------------------------------------------------------------

bool BSL_Interface::rxTxtFileContinue( unsigned int imageSize )
{
  updateProgress( imageSize );

  if ( moreDataToRead( ) )
  {
    QByteArray sndData;
    unsigned long addr = readTI_TextFile( _bufferSize, sndData );
    _rxTxtBytes += sndData.size( );

    for ( int i = 0; i < 10; i++ )
    {
      if ( rxDataBlock( addr, sndData ) == FLASH_ACK )
      {
        QByteArray rspData;
        _serial->readUntilTimeout( rspData );

        if ( processRspData( rspData ) )
        {
          return ( true );
        }
      }

      qDebug() << __FUNCTION__ << " repeat " + QString::number(i+1);
    }
  }

  return ( false );
}

//------------------------------------------------------------------------------

void BSL_Interface::rxTxtFileEnd( )
{
  float seconds = ( (float)_rxTxtTime.elapsed( ) ) / (float)1000;
  float kbs = ( (float)_rxTxtBytes/(float)1024 ) / (float)seconds;

  qDebug( "Wrote %i bytes in %.2f seconds [%.2f Kbytes/s]", _rxTxtBytes, seconds, kbs );

  closeTI_Text( );

  emit firmwareInstallDone( );
}

/*******************************************************************************
*Function:    rxMassEraseAsn
*Description: sends the mass erase command to the device
*Parameters:
*             none
*Returns:
*             none
*******************************************************************************/
unsigned char BSL_Interface::rxMassErase( )
{
  QByteArray txData;
  txData.append( MASS_ERASE );

  return ( txPacket( txData ) );
}

/*******************************************************************************
*Function:    rxPasswordAsn
*Description: Sends a 32 byte password, only for the 5438 device
*Parameters:
*             none
*Returns:
*             none
*******************************************************************************/
unsigned char BSL_Interface::rxPassword( )
{
  QByteArray txData;
  txData.append( RX_PASSWORD );

  for ( int i = 0; i < 32; i++ )
  {
    txData.append( (BYTE)0xFF );
  }

  return ( txPacket( txData ) );
}

/*******************************************************************************
*Function:    rxDataBlockAsn
*Description: Programs a block of data to the MSP430 memory
*Parameters:
*             int addr              The address to begin the data block in the 430
*             QByteArray &data      An array of bytes
*Returns:
*             none
*******************************************************************************/
unsigned char BSL_Interface::rxDataBlock( unsigned long addr, const QByteArray &data )
{
  QByteArray txData;
  txData.append( RX_DATA_BLOCK );
  txData.append( (unsigned char)((addr)       & 0xFF) );
  txData.append( (unsigned char)((addr >> 8)  & 0xFF) );
  txData.append( (unsigned char)((addr >> 16) & 0xFF) );
  txData.append( data );

  return ( txPacket( txData ) );
}

//------------------------------------------------------------------------------

bool BSL_Interface::processRspData( const QByteArray &data , int expectedCoreRsp )
{
  bool result = false;

  if ( data.size( ) >= BSL_RSP_PACKET_SIZE )
  {
    if ( ( (BYTE)data.at( BSL_RSP_HEADER_OFFSET ) == BSL_HEADER ) &&
         ( (BYTE)data.at( BSL_RSP_SIZE_LSB_OFFSET ) == 0x02 ) &&
         ( (BYTE)data.at( BSL_RSP_SIZE_MSB_OFFSET ) == 0x00 ) &&
         ( (BYTE)data.at( BSL_RSP_DATA_LSB_OFFSET ) == 0x3B ) )
    {
      if ( expectedCoreRsp == SUCCESSFUL_OPERATION )
      {
        if ( ( (BYTE)data.at( BSL_RSP_DATA_MSB_OFFSET ) == 0x00 ) &&
             ( (BYTE)data.at( BSL_RSP_CKL_OFFSET ) == 0x60 ) &&
             ( (BYTE)data.at( BSL_RSP_CKH_OFFSET ) == 0xC4 ) )
        {
          result = true;
        }
        else
        {
          qDebug() << "Wrong Core RSP!" << expectedCoreRsp << data.size( ) << dataToHexString( data );
        }
      }
      else if ( expectedCoreRsp == BSL_PASSWORD_ERROR )
      {
        if ( ( (BYTE)data.at( BSL_RSP_DATA_MSB_OFFSET ) == 0x05 ) &&
             ( (BYTE)data.at( BSL_RSP_CKL_OFFSET ) == 0xC5 ) &&
             ( (BYTE)data.at( BSL_RSP_CKH_OFFSET ) == 0x94 ) )
        {
          result = true;
        }
        else
        {
          qDebug() << "Wrong Core RSP!" << expectedCoreRsp << data.size( ) << dataToHexString( data );
        }
      }
      else
      {
        qDebug() << "Unknown Core RSP!" << expectedCoreRsp;
      }
    }
    else
    {
      qDebug() << "Wrong RSP!" << data.size( ) << dataToHexString( data );
    }
  }
  else
  {
    qDebug() << "Too short RSP!" << data.size( ) << dataToHexString( data );
  }

  return result;
}

//------------------------------------------------------------------------------

unsigned char BSL_Interface::txPacket( const QByteArray &data )
{
  Flash_crcInit(0xFFFF);
  for ( int i = 0; i < data.size(); i++ )
  {
    Flash_crcInput(data.at(i));
  }

  QByteArray txData;
  txData.append((unsigned char)BSL_HEADER);
  txData.append((unsigned char)(data.size() & 0xFF));
  txData.append((unsigned char)((data.size() >> 8) & 0xFF));
  txData.append(data);
  txData.append(Flash_getLowByte());
  txData.append(Flash_getHighByte());

  _serial->write(txData);

  unsigned char ack = _serial->readByte( );
  if ( ack != FLASH_ACK )
  {
    _serial->clearBuffers( );
  }

  return ack;
}

//------------------------------------------------------------------------------

void BSL_Interface::updateProgress( unsigned int imageSize )
{
  Q_ASSERT( imageSize != 0 );

  unsigned int nPercent = 100UL * _rxTxtBytes / imageSize;
  qDebug("_rxTxtBytes: %i", _rxTxtBytes);
  qDebug("imageSize: %i", imageSize);
  qDebug("nPercent: %i", nPercent);
  qDebug("_Percent: %i", _percent);

  if ( _percent != nPercent )
  {
    qDebug("_percent != nPercent **********");
    _percent = nPercent;
    emit progressChanged(nPercent);
  }
}

//------------------------------------------------------------------------------

QString BSL_Interface::dataToHexString(const QByteArray &data)
{
  QString hexString;

  for ( int i = 0; i < data.size(); i++ )
  {
    hexString.append(QString("%1").arg((BYTE)data.at(i), 2, 16, QChar('0')));
  }

  return (hexString);
}

//------------------------------------------------------------------------------

unsigned int BSL_Interface::getPercent()
{
  return (_percent);
}

//------------------------------------------------------------------------------
