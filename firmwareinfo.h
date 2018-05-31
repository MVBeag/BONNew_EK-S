#ifndef FIRMWAREINFO_H_
#define FIRMWAREINFO_H_

//--- includes -----------------------------------------------------------------------------------

#include <QObject>


//------------------------------------------------------------------------------------------------

class FirmwareInfo : public QObject
{
    Q_OBJECT

public:

    explicit FirmwareInfo( QString image, int size, QString version, QString changelog, QString logo, QObject *parent = 0 );

    QString      image( ) const;
    unsigned int size( ) const;
    QString      version( ) const;
    QString      changelog( ) const;
    QString      logo( ) const;

private:

    QString      _image;
    unsigned int _size;
    QString      _version;
    QString      _changelog;
    QString      _logo;

    Q_DISABLE_COPY( FirmwareInfo )
};

#endif // FIRMWAREINFO_H_

//--- eof ----------------------------------------------------------------------------------------
