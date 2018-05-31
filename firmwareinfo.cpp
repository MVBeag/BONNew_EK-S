#include "firmwareinfo.h"


FirmwareInfo::FirmwareInfo( QString image, int size, QString version, QString changelog, QString logo, QObject *parent )
    : QObject( parent )
    , _image( image )
    , _size( size )
    , _version( version )
    , _changelog( changelog )
    , _logo( logo )
{

}

//-------------------------------------------------------------

QString FirmwareInfo::image( ) const
{
    return _image;
}

//-------------------------------------------------------------

unsigned int FirmwareInfo::size( ) const
{
    return _size;
}

//-------------------------------------------------------------

QString FirmwareInfo::version( ) const
{
    return _version;
}

//-------------------------------------------------------------

QString FirmwareInfo::changelog( ) const
{
    return _changelog;
}

//-------------------------------------------------------------

QString FirmwareInfo::logo( ) const
{
    return _logo;
}

//-------------------------------------------------------------
