#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QBuffer>
#include <QTimer>
#include <QDataStream>
#include <QtWebSockets/QWebSocket>
#include <qabstractsocket.h>

#include "mapconfigdata.h"

#define __STORMLIB_SELF__
#include <StormLib.h>

#include <ghost/config.h>
#include <ghost/gameslot.h>
#include <ghost/map.h>

#include "uiautohost.h"
#include "uislots.h"

#include "lan.hpp"

MapConfigData::MapConfigData( QWidget *parent)
	: QMainWindow(parent)
{
	m_Uploading = false;
	m_UploaedPartsQueue = 0;
	m_UploaedPartsMaxQueue = 2;
	ui.setupUi(this);

	connect( ui.MOpen, &QAction::triggered, this, &MapConfigData::menuOpen );
	connect( ui.MSaveAs, &QAction::triggered, this, &MapConfigData::menuSaveAs );
	connect( ui.actionLAN, &QAction::triggered, this, &MapConfigData::openLANWindow );

	connect( ui.MAdd, &QMenu::triggered, this, &MapConfigData::onAdd );
	connect( ui.tabWidget, &QTabWidget::tabCloseRequested, this, &MapConfigData::onCloseTab );

	connect( ui.MUpload, &QAction::triggered, this, &MapConfigData::onUploadReqest );

	m_Timer = new QTimer( this );

	m_Timer->start( 25 );
	connect( m_Timer, &QTimer::timeout, this, &MapConfigData::uploadPart );

	m_LanWindow = NULL;
	m_WebSocket = NULL;
}

MapConfigData::~MapConfigData()
{
	QApplication::exit( );
}

QString MapConfigData::readNullTremilanedString( QDataStream &stream )
{
	QByteArray response;

	while ( !stream.atEnd( ) )
	{
		quint8 byte;
		stream >> byte;

		if ( byte != 0 )
			response.push_back( byte );
		else 
			break;
	}

	return QString( response );
}

QByteArray MapConfigData::readNullTremilanedByte( QDataStream & stream )
{
	QByteArray response;

	while ( !stream.atEnd( ) )
	{
		quint8 byte;
		stream >> byte;

		if ( byte != 0 )
			response.push_back( byte );
		else
			break;
	}

	return response;
}

void MapConfigData::escapeColorString( QString & str )
{
	str = str.replace( QRegExp( "\\|c[A-F0-9a-f]{8}" ), "" );
	str = str.replace( QRegExp( "\\|C[A-F0-9a-f]{8}" ), "" );
	str = str.replace( "\\|r", "" );
	str = str.replace( "\\|n", "" );
	str = str.replace( "\\|R", "" );
	str = str.replace( "\\|N", "" );
}

void MapConfigData::AppendLength( QByteArray & b )
{
	if ( b.size( ) < 4 || b.size( ) > 0xFFFF )
		return;
	QByteArray len;
	QDataStream ds( &len, QIODevice::ReadWrite );
	ds.setByteOrder( QDataStream::LittleEndian );
	ds << (quint16)b.size( );

	b[2] = len[0];
	b[3] = len[1];
}

void MapConfigData::menuSaveAs( )
{
	QString fileName = QFileDialog::getSaveFileName( this, "Сохранить как", m_CurrentFilePatch, "*.w3x *.w3m" );

	if ( fileName.size( ) > 0 )
	{
		QFile writeMap( fileName );

		if ( writeMap.open( QIODevice::ReadWrite | QIODevice::Truncate ) )
		{
			m_EditedMapData.clear( );

			QFileInfo fi( fileName );
			m_UplpadMapName = fi.fileName( );

			QDataStream write( &m_EditedMapData, QIODevice::WriteOnly );
			QDataStream read( m_SourceMapData );

			quint32 GrabInt = 0;

			// Write first 8 bytes
			read >> GrabInt;
			write << GrabInt;
			read >> GrabInt;
			write << GrabInt;

			QByteArray mapName = readNullTremilanedByte( read );
			const char *mapBytes = mapName.constData( );
			write.writeRawData( mapBytes, mapName.size( ) + 1 );

			// Write last 8 bytes

			read >> GrabInt;
			write << GrabInt;

			read >> GrabInt;
			write << GrabInt;

			int bytesUse = 17 + mapName.size( ); // MPQ START = 512 byte

			read.skipRawData( 512 - bytesUse );

			//
			// MCD BYTES
			//

			//
			// HEADER
			//

			write.setByteOrder( QDataStream::LittleEndian );

			bytesUse += 3;

			write << (quint8)MCD_BYTE_VERSION;

			quint16 mask = 0;

			if ( ui.MSaveMetadata->isChecked( ) )
				mask = mask | (quint16)MCD_ALLOW_SEND_METADATA;

			QByteArray tabsData;

			for ( auto i = m_Tabs.begin( ); i != m_Tabs.end( ); ++i )
			{
				mask = mask | i.key();
				tabsData.append( i.value()->onSave( ) );
			}

			write << mask;

			bytesUse += tabsData.size( );

			if ( bytesUse > 512 )
			{
				//data too long;
			}

			for ( auto i = tabsData.begin( ); i != tabsData.end( ); ++i )
				write << (quint8)*i;

			for ( int i = 512 - bytesUse; i > 0; i-- )
			{
				write << (quint8)0;
			}
				

			while ( !read.atEnd( ) )
			{
				quint8 b;
				read >> b;
				write << b;
			}
		}
		else
		{
			QMessageBox::critical( this, "Ошибка", "Не удалось записать карту." );
		}

		qDebug( ) << m_EditedMapData.size( );

		writeMap.write( m_EditedMapData );

		
	}
}

void MapConfigData::onAdd( QAction * action )
{
	int CustomIndex = action->property( "CustomIndex" ).toInt( );
	AddTab( CustomIndex );
	UpdateAllowAddTabs( );
}

void MapConfigData::onCloseTab( int index )
{
	if ( index == 0 )
	{
		QMessageBox::information( this, "Информация", "Справку закрыть нельзя" );
		return;
	}
	
	for ( auto i = m_Tabs.begin( ); i != m_Tabs.end( ); ++i )
	{
		if ( i.value( ) == ui.tabWidget->widget( index ) )
		{
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question( this, "Закрыть вкладку?", "Все данные вкладки будут уничтожены.",
				QMessageBox::Yes | QMessageBox::No );

			if ( reply == QMessageBox::Yes ) {
				m_Tabs.erase( i );
				delete ui.tabWidget->widget( index );
				UpdateAllowAddTabs( );
			}
			break;
		}
	}
}

void MapConfigData::uploadPart( )
{
	m_Timer->start( 250 );

	if ( m_Uploading && !m_MapData.isEmpty( ) && m_WebSocket )
	{
		if ( m_UploaedPartsQueue == 0 )
			m_UploaedPartsMaxQueue += 2;
		else
			m_UploaedPartsMaxQueue -= 1;

		if ( m_UploaedPartsMaxQueue < 2 )
			m_UploaedPartsMaxQueue = 2;

		while ( m_UploaedPartsQueue < m_UploaedPartsMaxQueue )
		{

			m_UploaedPartsQueue++;
			QByteArray answer;
			QDataStream answerstream( &answer, QIODevice::WriteOnly );
			answerstream.setByteOrder( QDataStream::LittleEndian );

			m_Uploading = true;

			QByteArray mappart;

			answerstream << (quint8)2;
			answerstream << (quint8)1;

			for ( ; m_UploadOffset < m_MapData.size( ); m_UploadOffset++ )
			{
				if ( mappart.size( ) >= 20000 )
					break;
				mappart.push_back( m_MapData[m_UploadOffset] );
			}

			answerstream.writeRawData( mappart.data( ), mappart.size( ) );

			m_WebSocket->sendBinaryMessage( answer );

			if ( m_UploadOffset >= m_MapData.size( ) )
			{
				m_WebSocket->sendBinaryMessage( { 2,2 } );
				m_Uploading = false;
			}
				
		}

	}
}

void MapConfigData::onUploadReqest( )
{
	if ( !m_WebSocket )
	{
		m_WebSocket = new QWebSocket( );
		m_WebSocket->open( QUrl( "ws://irinabot.ru/ghost/" ) );
		connect( m_WebSocket, &QWebSocket::connected, this, &MapConfigData::onSocketConnected );
		connect( m_WebSocket, &QWebSocket::disconnected, this, &MapConfigData::onSocketDisconnected );
		connect( m_WebSocket, &QWebSocket::binaryMessageReceived, this, &MapConfigData::onSocketData );
		ui.MUpload->setDisabled( true );
		ui.MUpload->setText( "Подключаюсь..." );
	}

	if ( m_EditedMapData.isEmpty( ) )
		m_MapData = m_SourceMapData;
	else
		m_MapData = m_EditedMapData;

	m_UploadOffset = 0;
}

void MapConfigData::onSocketData( QByteArray message )
{
	if ( message.size( ) < 2 )
		return;

	QDataStream pds( message );
	pds.setByteOrder( QDataStream::LittleEndian );

	quint8 context;
	quint8 type;

	pds >> context;
	pds >> type;


	if ( context == 0 )
	{
		if ( type == 0 )
		{
			pds.skipRawData( 1 );
			QMessageBox::critical( this, "Ошибка", readNullTremilanedString( pds ) );
			m_WebSocket->disconnect( );
			onSocketDisconnected( );
			return;
		}
	}

	if ( context == 2 )
	{
		if ( type == 0 ) // Upload accepted
		{
			m_Uploading = true;
			m_UploaedPartsQueue = 0;

			ui.MUpload->setText( "Выгрузка принята." );
		}

		if ( type == 1 ) // Mappart ok
		{
			m_UploaedPartsQueue--;

			quint32 uploaded;
			pds >> uploaded;

			ui.MUpload->setText( "Выгружено: " + QString::number( (int)( ( (float)uploaded / (float)m_MapData.size( ) ) * 100 ) ) + "%" );
		}
		
		if ( type == 2 ) // Complete
		{
			QMessageBox::warning( this, "Выгружено", "Все выгружено. Присвоенное имя карты: <b>" + readNullTremilanedString( pds ) + "</b>" );
			m_WebSocket->disconnect( );
			onSocketDisconnected( );
		}
	}
}

void MapConfigData::onSocketConnected( )
{
	// SEND MAP UPLOAD REQ

	QByteArray b;
	QDataStream stream( &b, QIODevice::WriteOnly );
	stream.setByteOrder( QDataStream::LittleEndian );
	stream << (quint8)0;
	stream << (quint8)1;
	stream << (quint8)2;

	stream << m_MapData.size( );
	stream.writeRawData( m_UplpadMapName.toStdString( ).c_str( ), m_UplpadMapName.toStdString( ).size( ) + 1 );

	m_WebSocket->sendBinaryMessage( b );

	ui.MUpload->setText( "Отправляю запрос на загрузку..." );
}

void MapConfigData::onSocketDisconnected( )
{
	ui.MUpload->setText( "Залить карту на хостбот" );
	ui.MUpload->setDisabled( false );
	m_WebSocket->deleteLater( );
	m_WebSocket = NULL;
	m_Uploading = false;
}

void MapConfigData::openLANWindow( )
{
	if ( m_LanWindow )
		m_LanWindow->activateWindow( );
	else
	{
		m_LanWindow = new Lan( );
		m_LanWindow->showNormal( );
		m_LanWindow->setFocus( );

		connect( m_LanWindow, &QWidget::destroyed, this, &MapConfigData::onLANWindowClose );
	}
}

void MapConfigData::onLANWindowClose( )
{
	m_LanWindow = NULL;
}

void MapConfigData::UpdateAllowAddTabs( )
{
	QList<QAction *> Actions = ui.MAdd->actions( );
	for ( auto j = Actions.begin( ); j != Actions.end( ); ++j )
	{
		( *j )->setDisabled( false );
		for ( auto i = m_Tabs.begin( ); i != m_Tabs.end( ); ++i )
		{
			if ( ( *j )->property( "CustomIndex" ).toInt( ) == i.key( ) )
			{
				( *j )->setDisabled( true );
			}
		}
	}

	if ( ui.tabWidget->count( ) > 1 )
	{
		ui.tabWidget->setTabsClosable( true );
	}
	else
	{
		ui.tabWidget->setTabsClosable( false );
	}
}

void MapConfigData::AddTab( int index )
{
	switch ( index )
	{
	case MCD_CUSTOM_SLOT:
		m_Tabs[index] = new UISlots( this );
		ui.tabWidget->addTab( m_Tabs[index], "Слоты" );
		break;
	case MCD_CUSTOM_AUTOHOST:
		m_Tabs[index] = new UIAutohost( this );
		ui.tabWidget->addTab( m_Tabs[index], "Автохост" );
		break;
	default:
		break;
	}
}

void MapConfigData::menuOpen( )
{
	QString fileName = QFileDialog::getOpenFileName( this, "Открыть", "", "*.w3x *.w3m" );

	if ( fileName.size( ) > 0 )
	{
		QFile file( fileName );

		HANDLE MapMPQ;

		if ( SFileOpenArchive( fileName.toStdWString( ).c_str( ), 0, MPQ_OPEN_FORCE_MPQ_V1, &MapMPQ ) )
		{
			SFileCloseArchive( MapMPQ );

			if ( file.open( QIODevice::ReadOnly ) )
			{
				m_SourceMapData = file.readAll( );
				m_EditedMapData.clear( );

				ui.MUpload->setEnabled( true );

				QFileInfo fi( fileName );
				m_UplpadMapName = fi.fileName( );

				QDataStream data( m_SourceMapData );
				data.skipRawData( 8 );

				QString MapName = readNullTremilanedString( data );
				escapeColorString( MapName );

				if ( !MapName.isEmpty( ) )
					this->setWindowTitle( MapName );

				data.skipRawData( 8 );

				// MCD Custom Bytes

				quint8 version;

				data >> version;

				if ( version > MCD_BYTE_VERSION )
					QMessageBox::critical( this, "Ошибка", "Эта версия MCD не поддерживает схему, по которой запакованы данные" );
				else
				{
					for ( auto i = m_Tabs.begin( ); i != m_Tabs.end( ); ++i )
					{
						delete i.value( );
					}

					m_Tabs.clear( );
					
					data.setByteOrder( QDataStream::LittleEndian );
					quint16 editMask;

					data >> editMask;

					ui.MSaveAs->setEnabled( true );
					ui.MSaveMetadata->setEnabled( true );
					ui.MSaveMetadata->setChecked( false );
					ui.MAdd->setEnabled( true );

					m_CurrentFilePatch = fileName;

					if ( editMask & MCD_ALLOW_SEND_METADATA )
						ui.MSaveMetadata->setChecked( true );

					for ( int i = 1; i < 16; i++ )
					{
						if ( editMask & ( 1 << i ) )
						{
							AddTab( 1 << i );
						}
					}

					UpdateAllowAddTabs( );

					for ( auto i = m_Tabs.begin( ); i != m_Tabs.end( ); ++i )
					{
						i.value()->onLoad( data );
					}
				}

				file.close( );
			}
			else
				QMessageBox::critical( this, "Ошибка", "Не удалось открыть файл для чтения." );
		}	
		else
			QMessageBox::critical( this, "Ошибка", "Файл не является валидным MPQ архивом." );
	}
	
}


