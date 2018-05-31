#ifndef _TEXTFILEIO_H_
#define _TEXTFILEIO_H_

#include "BSL_Common.h"
#include <QByteArray>

#define OPERATION_SUCCESSFUL 0
#define TXT_EOF ((unsigned int)-1)
#define ERROR_OPENING_FILE -2
#define NO_DATA_READ ((unsigned int)-3)

int openTI_TextForRead(const char *filename);
void closeTI_Text();

int moreDataToRead();
unsigned long readTI_TextFile(int bytesToRead, QByteArray &data);

#endif // _TEXTFILEIO_H_
