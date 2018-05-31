/*==========================================================================*\
|                                                                            |
|                                                                            |
| PC-side Bootstrap Loader communication Application                         |
|                                                                            |
| See main.c for full version and legal information                          |
|                                                                            |
\*==========================================================================*/

#include "TextFileIO.h"
#include <stdio.h>
#include <string.h>
#include <QFile>
#include <QDebug>

QFile* pFile = Q_NULLPTR;
unsigned int currentAddr = NO_DATA_READ;

/*******************************************************************************
*Function:    openTI_TextForRead
*Description: Opens a TXT file for reading
*Parameters: 
*             char *filename        The string containing the file name
*Returns:
*             SUCCESSFUL_OPERATION  The file is open for reading
*             ERROR_OPENING_FILE    Some error occured during file open
*******************************************************************************/
int openTI_TextForRead(const char *filename)
{
  int nError = ERROR_OPENING_FILE;

  if (Q_NULLPTR == pFile)
  {
    currentAddr = NO_DATA_READ;

    pFile = new QFile(filename);
    if (Q_NULLPTR != pFile)
    {
      if (pFile->open(QIODevice::ReadOnly | QIODevice::Unbuffered))
      {
        nError = OPERATION_SUCCESSFUL;
      }
      else
      {
        qDebug() << "pFile->open failed: " << pFile->errorString();
        delete pFile;
        pFile = Q_NULLPTR;
      }
    }
  }
  return(nError);
}

/*******************************************************************************
*Function:    closeTI_Text
*Description: Closes a TXT file
*Parameters: 
*             none
*Returns:
*             none
*******************************************************************************/
void closeTI_Text()
{
  if (Q_NULLPTR != pFile)
  {
    pFile->close();
    delete pFile;
    pFile = Q_NULLPTR;
  }
}

/*******************************************************************************
*Function:    moreDataToRead
*Description: checks whether an end-of-file was hit during read
*Parameters: 
*             none
*Returns:
*             1                     if an EOF has not been hit
*             0                     if an EOF has been hit
*******************************************************************************/
int moreDataToRead()
{
  return !(currentAddr == TXT_EOF);
}

/*******************************************************************************
*Function:    readTI_TextFile
*Description: reads a certain amount of bytes from a TI TEXT file
*Parameters: 
*             int bytesToRead       The maximum number of bytes to read from the file
*             QByteArray &data      An array of bytes
*Returns:
*             The address of target's memory to write
*******************************************************************************/
unsigned long readTI_TextFile(int bytesToRead, QByteArray &data)
{
  char string[50];
  bool bEof = false;

  if ( currentAddr == NO_DATA_READ )
  {
    if ( Q_NULLPTR != pFile )
    {
      (void)pFile->readLine(string, sizeof(string));
    }
#ifdef __APPLE__
    (void)sscanf(&string[1], "%x\n", &currentAddr);
#else
    (void)sscanf_s(&string[1], "%x\n", &currentAddr);
#endif
  }

  unsigned long returnAddr = currentAddr;

  do
  {
    int stringLength = 0;
    int stringPosition = 0;

    if ( Q_NULLPTR != pFile )
    {
      stringLength = pFile->readLine(string, sizeof(string));
      bEof = pFile->atEnd();
    }

    if ( bEof )
    {
      currentAddr = EOF;
    }
    else if( string[0] == '@' )
    {
#ifdef __APPLE__
      (void)sscanf(&string[1], "%x\n", &currentAddr);
#else
      (void)sscanf_s(&string[1], "%x\n", &currentAddr);
#endif
      bEof = true;
    }
    else if ( string[0] == 'q' || string[0] == 'Q' )
    {
      bEof = true;
      currentAddr = EOF;
    }
    else
    {
      for ( stringPosition = 0; stringPosition < (stringLength-3); stringPosition += 3 )
      {
        unsigned int nValue = 0;

#ifdef __APPLE__
        (void)sscanf( &string[stringPosition], "%2x", &nValue );
#else
        (void)sscanf_s( &string[stringPosition], "%2x", &nValue );
#endif
        data.append((unsigned char)((nValue) & 0xFF));

        currentAddr++;
      }
    }
  }
  while( (!bEof) && (data.size() < bytesToRead) );

  return returnAddr;
}
