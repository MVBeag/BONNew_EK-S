#ifndef _BSL_INTERFACE_H_
#define _BSL_INTERFACE_H_

#include "BSL_Common.h"
#include <QObject>
#include <QByteArray>
#include <QSerialPortInfo>
#include <QTime>
#include <QTimer>


class BSL_IO_UART;

class BSL_Interface : public QObject
{
  Q_ENUMS( eBSL_RSP )
  Q_OBJECT

public:

  enum eBSL_RSP
  {
    BSL_RSP_HEADER_OFFSET,
    BSL_RSP_SIZE_LSB_OFFSET,
    BSL_RSP_SIZE_MSB_OFFSET,
    BSL_RSP_DATA_LSB_OFFSET,
    BSL_RSP_DATA_MSB_OFFSET,
    BSL_RSP_CKL_OFFSET,
    BSL_RSP_CKH_OFFSET,
    BSL_RSP_PACKET_SIZE
  };

  BSL_Interface( unsigned int nBufferSize, QObject *parent = 0 );
  ~BSL_Interface( );

  bool          openPort( const QSerialPortInfo &info );
  void          closePort();

  void          installFirmware( const QString &firmware , unsigned int size );
  bool          authenticate( );
  bool          erase( );
  unsigned int  getPercent( );

  bool          rxProgrammingMode( QString &version );

signals:

  void          progressChanged( const unsigned int &progress );
  void          firmwareInstallDone( );

private:

  unsigned char rxPassword( );
  unsigned char rxMassErase( );
  unsigned char rxDataBlock( unsigned long addr, const QByteArray &data );

  void          rxTxtFileBegin( const char* fileName );
  bool          rxTxtFileContinue( unsigned int imageSize );
  void          rxTxtFileEnd( );

  unsigned char txPacket( const QByteArray &data );

  bool          processRspData( const QByteArray &data, int expectedCoreRsp = SUCCESSFUL_OPERATION );
  void          updateProgress( unsigned int imageSize );

  QString       dataToHexString( const QByteArray &data );

  unsigned int  _bufferSize;
  BSL_IO_UART  *_serial;
  unsigned int  _percent;
  QTime         _rxTxtTime;
  unsigned int  _rxTxtBytes;
};

#endif // _BSL_INTERFACE_H_
