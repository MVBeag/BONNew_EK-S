#include "targetinfo.h"


TargetInfo::TargetInfo( QObject *parent )
    : QObject( parent )
    , _type( TARGET_UNKNOWN )
{
    _sirosModelsS << "Siros" << "Siros 400" << "Siros 800" << "Siros 400S" << "Siros 800S" << "Siros 400 S" << "Siros 800 S";
    _sirosModelsL << "Siros 400L" << "Siros 800L" << "Siros 400 L" << "Siros 800 L";

    /**/
    _firmware.insert( TARGET_SIROS_S, new FirmwareInfo( ":/fmw/s/image.txt", 126820UL, "49.10", ":/fmw/s/changelog.txt", "/fmw/s/logo.jpg" ) );
    _firmware.insert( TARGET_SIROS_L, new FirmwareInfo( ":/fmw/l/image.txt", 124254UL, "52.04", ":/fmw/l/changelog.txt", "/fmw/l/logo.jpg" ) );
}

//-------------------------------------------------------------

TargetInfo::~TargetInfo()
{
    qDeleteAll( _firmware );
    _firmware.clear( );
}

//-------------------------------------------------------------

TargetInfo::eTARGET_TYPE TargetInfo::type() const
{
    return _type;
}

//-------------------------------------------------------------

void TargetInfo::setName( const QString &name )
{
    _name = name;

    if ( _sirosModelsS.contains( _name ) )
    {
      _type = TARGET_SIROS_S;
    }
    else if ( _sirosModelsL.contains( _name ) )
    {
      _type = TARGET_SIROS_S;
    }
    else
    {
      _type = TARGET_UNKNOWN;
    }
}

//-------------------------------------------------------------

QString TargetInfo::name( ) const
{
    return _name;
}

//-------------------------------------------------------------

void TargetInfo::setVersion( const QString &version )
{
    _version = version;
}

//-------------------------------------------------------------

QString TargetInfo::version() const
{
    return _version;
}

//-------------------------------------------------------------

QString TargetInfo::firmwareImage( ) const
{
    Q_ASSERT( _type != TARGET_UNKNOWN );

    return _firmware.value( _type )->image( );
}

//-------------------------------------------------------------

unsigned int TargetInfo::firmwareSize() const
{
    Q_ASSERT( _type != TARGET_UNKNOWN );

    return _firmware.value( _type )->size( );
}

//-------------------------------------------------------------

QString TargetInfo::firmwareVersion() const
{
    Q_ASSERT( _type != TARGET_UNKNOWN );

    return _firmware.value( _type )->version( );
}

//-------------------------------------------------------------

QString TargetInfo::firmwareChangelog() const
{
    Q_ASSERT( _type != TARGET_UNKNOWN );

    return _firmware.value( _type )->changelog( );
}

//-------------------------------------------------------------

QString TargetInfo::firmwareLogo() const
{
    Q_ASSERT( _type != TARGET_UNKNOWN );

    return _firmware.value( _type )->logo( );
}

//-------------------------------------------------------------
