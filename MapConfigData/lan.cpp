#include <QBuffer>
#include <QDataStream>
#include <QTCPSocket>
#include <QUDPSocket>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include "mapconfigdata.h"

#include <qtimer.h>

#include "lan.hpp"

Lan::Lan(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	setAttribute( Qt::WA_DeleteOnClose );

	m_Timer = new QTimer( this );
	m_MasterSocket = new QTcpSocket( this );
	m_UDPSocket = new QUdpSocket( this );

	m_Server = new QTcpServer( this );
	m_Server->listen( QHostAddress::AnyIPv4, 6110 );
	connect( m_Server, &QTcpServer::newConnection, this, &Lan::ProxyCanAccept );

	connect( m_MasterSocket, &QTcpSocket::connected, this, &Lan::onSocketConnected );
	connect( m_MasterSocket, &QTcpSocket::readyRead, this, &Lan::onSocketData );
	connect( m_MasterSocket, &QTcpSocket::disconnected, this, &Lan::onSocketDissconected );

	connect( m_Timer, &QTimer::timeout, this, &Lan::Ping );
	connect( ui.connect, &QPushButton::clicked, this, &Lan::Run );

	QSpacerItem *verticalSpacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );

	ui.scrollLayout->addItem( verticalSpacer, 1000, 0);

	m_NextRow = 0;
}

Lan::~Lan( ) {
	m_MasterSocket->disconnect( );

	for ( auto i = m_Games.begin( ); i != m_Games.end( ); ++i )
	{
		m_UDPSocket->writeDatagram( (*i)->GetDecreatePackage( ), ui.lanBroadcast->isChecked( ) ? QHostAddress::Broadcast : QHostAddress::LocalHost, 6113 );
		delete *i;
	}
		
}

void Lan::onSocketData( )
{
	m_Buffer.append( m_MasterSocket->readAll( ) );

	while ( m_Buffer.size( ) >= 4 )
	{
		QDataStream ds( m_Buffer );
		ds.setByteOrder( QDataStream::LittleEndian );

		quint8 pHeader;
		quint8 pType;
		quint16 pLength;

		ds >> pHeader;
		ds >> pType;
		ds >> pLength;

		if ( pHeader != W3GS_HEADER_CONSTANT && pHeader != 200 )
		{
			m_MasterSocket->disconnect( );
			return;
		}

		if ( m_Buffer.size( ) >= pLength )
		{
			onPackage( pType, QByteArray( m_Buffer.data( ), pLength ) );
			m_Buffer = m_Buffer.remove( 0, pLength );
		}
		else
			break;
	}
}

void Lan::onSocketConnected( )
{
	QByteArray b;
	b.push_back( 200 );
	b.push_back( 4 );
	b.push_back( 4 );
	b.push_back( (char)0 );
	m_MasterSocket->write( b );

	m_Timer->start( 5000 );

	ui.connect->setText( "Connected" );
}

void Lan::onSocketDissconected( )
{
	ui.connect->setEnabled( true );
	ui.connect->setText( "Connect" );

	for ( auto i = m_Games.begin( ); i != m_Games.end( ); ++i )
	{
		m_UDPSocket->writeDatagram( ( *i )->GetDecreatePackage( ), ui.lanBroadcast->isChecked( ) ? QHostAddress::Broadcast : QHostAddress::LocalHost, 6113 );
		delete *i;
	}

	m_Games.clear( );

}

void Lan::Ping( ) {

	if ( m_MasterSocket->isValid( ) )
	{
		QByteArray b;
		b.push_back( 1 );
		m_MasterSocket->write( b );
		m_Timer->start( 5000 );
	}
}

void Lan::ProxyConnected( )
{
	for ( auto i = m_Proxys.begin( ); i != m_Proxys.end( ); ++i )
	{
		if ( i->second == sender( ) )
		{
			i->second->write( i->first->readAll( ) );
		}
	}
}

void Lan::ProxyData( )
{
	for ( auto i = m_Proxys.begin( ); i != m_Proxys.end( ); ++i )
	{
		if ( i->second == sender( ) )
		{
			if ( i->first->isValid( ) )
			{
				i->first->write( i->second->readAll( ) );
			}
		}

		if ( i->first == sender( ) )
		{
			if ( i->second->isValid( ) )
			{
				i->second->write( i->first->readAll( ) );
			}
		}
	}
}

void Lan::ProxyDisconnected( )
{
	for ( auto i = m_Proxys.begin( ); i != m_Proxys.end( ); )
	{
		if ( i->second == sender( ) || i->first == sender( ) )
		{
			i->first->disconnect( );
			i->second->disconnect( );

			i->first->deleteLater( );
			i->second->deleteLater( );

			i = m_Proxys.erase( i );
		}
		else
			++i;
	}
}

void Lan::ProxyCanAccept( )
{
	QTcpSocket *NewSocket = m_Server->nextPendingConnection( );

	QTcpSocket *NewConnect = new QTcpSocket( this );
	NewConnect->connectToHost( "irinabot.ru", 6110 );

	connect( NewConnect, &QTcpSocket::connected, this, &Lan::ProxyConnected );
	connect( NewConnect, &QTcpSocket::readyRead, this, &Lan::ProxyData );
	connect( NewConnect, &QTcpSocket::disconnected, this, &Lan::ProxyDisconnected );

	connect( NewSocket, &QTcpSocket::readyRead, this, &Lan::ProxyData );
	connect( NewSocket, &QTcpSocket::disconnected, this, &Lan::ProxyDisconnected );

	m_Proxys.push_back( Proxy( NewSocket, NewConnect ) );
}

void Lan::GameEnabled( )
{
	CLanGame *game = dynamic_cast<CLanGame *> (sender( ));

	m_UDPSocket->writeDatagram( game->GetGameInfoPackage( ), ui.lanBroadcast->isChecked( ) ? QHostAddress::Broadcast : QHostAddress::LocalHost, 6113 );
}

void Lan::GameDisabled( )
{
	CLanGame *game = dynamic_cast<CLanGame *> ( sender( ) );

	m_UDPSocket->writeDatagram( game->GetDecreatePackage( ), ui.lanBroadcast->isChecked( ) ? QHostAddress::Broadcast : QHostAddress::LocalHost, 6113 );
}


void Lan::Run( ) {
	ui.connect->setEnabled( false );
	ui.connect->setText( "Connecting . . ." );
	m_MasterSocket->connectToHost( "irinabot.ru", 9874 );
}

void Lan::onPackage( quint8 pType, QByteArray pData )
{
	QByteArray b;
	QDataStream ds( &b, QIODevice::ReadWrite );

	QDataStream pds( pData );
	pds.setByteOrder( QDataStream::LittleEndian );

	switch ( pType )
	{
	case 48:
		if ( true )
		{
			pds.skipRawData( 12 );

			quint32 HCID;
			quint32 STotal;
			quint32 SOpen;
			pds >> HCID;
			pds.skipRawData( 4 );
			QString name = MapConfigData::readNullTremilanedString( pds );
			pds.skipRawData( 1 );
			MapConfigData::readNullTremilanedString( pds );
			pds >> STotal;
			pds.skipRawData( 8 );
			pds >> SOpen;

			CLanGame *Game = NULL;

			for ( auto i = m_Games.begin( ); i != m_Games.end( ); ++i )
			{
				if ( ( *i )->m_HCID == HCID )
					Game = *i;
			}

			if ( Game )
				Game->UpdateGameName( name );
			else
			{
				Game = new CLanGame( HCID, name, STotal, STotal - SOpen + 1, this );
				ui.scrollLayout->addWidget( Game, m_NextRow++, 0 );

				connect( Game, &CLanGame::disabledLAN, this, &Lan::GameDisabled );
				connect( Game, &CLanGame::enabledLAN, this, &Lan::GameEnabled );

				m_Games.push_back( Game );

				if ( m_NextRow > 950 )
					m_NextRow = 0;

			}

			Game->m_GameInfoPackage = pData;

			if ( Game->GetAllowLan( ) )
				m_UDPSocket->writeDatagram( pData, ui.lanBroadcast->isChecked( ) ? QHostAddress::Broadcast : QHostAddress::LocalHost, 6113 );
		}

		break;
	case 50:
		if ( true )
		{
			pds.skipRawData( 4 );
			quint32 HCID;
			quint32 Players;
			quint32 SlotsTotal;

			pds >> HCID;
			pds >> Players;
			pds >> SlotsTotal;

			for ( auto i = m_Games.begin( ); i != m_Games.end( ); ++i )
			{
				if ( ( *i )->m_HCID == HCID )
				{
					( *i )->UpdateSlots( SlotsTotal, Players );

					if ( ( *i )->GetAllowLan( ) )
					{
						m_UDPSocket->writeDatagram( pData, ui.lanBroadcast->isChecked( ) ? QHostAddress::Broadcast : QHostAddress::LocalHost, 6113 );
						m_UDPSocket->writeDatagram( ( *i )->GetGameInfoPackage( ), ui.lanBroadcast->isChecked( ) ? QHostAddress::Broadcast : QHostAddress::LocalHost, 6113 );
					}
						
				}
			}
		}
		break;

	case 51:
		if ( true )
		{
			pds.skipRawData( 4 );
			quint32 HCID;

			pds >> HCID;

			for ( auto i = m_Games.begin( ); i != m_Games.end( );  )
			{
				if ( ( *i )->m_HCID == HCID )
				{
					if ( ( *i )->GetAllowLan( ) )
						m_UDPSocket->writeDatagram( pData, ui.lanBroadcast->isChecked( ) ? QHostAddress::Broadcast : QHostAddress::LocalHost, 6113 );

					delete *i;
					i = m_Games.erase( i );
				}
					
				else
					++i;
			}
		}

		break;
	}
}
