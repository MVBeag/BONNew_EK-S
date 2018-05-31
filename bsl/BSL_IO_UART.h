#ifndef _BSL_IO_UART_H_
#define _BSL_IO_UART_H_

#include <QObject>
#ifdef __APPLE__
#include <QtSerialPort>
#else
#include <windows.h>
#endif


class BSL_IO_UART : public QObject
{
  Q_OBJECT

public:
  explicit BSL_IO_UART( QObject *parent = Q_NULLPTR );
  virtual ~BSL_IO_UART( );

#ifdef __APPLE__
  bool          open( const QSerialPortInfo &portInfo );
#else
  bool          open( const wchar_t *comPort, long rate, unsigned char parity );
#endif
  bool          close( );
  void          clearBuffers( );

  unsigned char readByte( );
  void          readUntilTimeout( QByteArray &data );
  void          readGarbageUntilTimeout( );

  int           write( const QByteArray &data );

  bool          txProgrammingMode( );
  bool          rxProgrammingMode( QString &version );

private:
  bool          setTimeouts( );
  bool          restoreTimeouts( );
  bool          setState( long rate, unsigned char parity );

  QString       dataToHexString( const QByteArray &data );

#ifdef __APPLE__
  QSerialPort   _serialPort;
#else
  HANDLE        _hComPort;    /* COM-Port Handle             */
  DCB           _comDCB;      /* COM-Port Control-Settings   */
  COMSTAT       _comState;    /* COM-Port Status-Information */
  COMMTIMEOUTS  _orgTimeouts; /* Original COM-Port Time-out  */
#endif
  unsigned int  _nakDelay;    /* Delay before DATA_NAK will be send */
};

#endif // _BSL_IO_UART_H_
