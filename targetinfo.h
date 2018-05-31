#ifndef TARGETINFO_H_
#define TARGETINFO_H_

//--- includes -----------------------------------------------------------------------------------

#include <QObject>
#include <QMap>
#include <QStringList>
#include "firmwareinfo.h"


//------------------------------------------------------------------------------------------------

class TargetInfo : public QObject
{
    Q_OBJECT

    Q_ENUMS( eTARGET_TYPE )

public:

    enum eTARGET_TYPE
    {
        TARGET_UNKNOWN = 0,
        TARGET_SIROS_S = 1,
        TARGET_SIROS_L = 2,
    };

    explicit TargetInfo( QObject *parent = 0 );
    ~TargetInfo( );

    void          setType( eTARGET_TYPE type );
    eTARGET_TYPE  type( ) const;

    void          setName( const QString &name );
    QString       name( ) const;

    void          setVersion( const QString &version );
    QString       version( ) const;

    QString       firmwareImage( ) const;
    unsigned int  firmwareSize( ) const;
    QString       firmwareVersion( ) const;
    QString       firmwareChangelog( ) const;
    QString       firmwareLogo( ) const;

private:

    eTARGET_TYPE                      _type;
    QString                           _name;
    QString                           _version;
    QStringList                       _sirosModelsS;
    QStringList                       _sirosModelsL;
    QMap<eTARGET_TYPE, FirmwareInfo*> _firmware;

    Q_DISABLE_COPY( TargetInfo )
};

#endif // TARGETINFO_H_

//--- eof ----------------------------------------------------------------------------------------
