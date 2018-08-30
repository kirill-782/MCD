#include "mapconfigdata.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QBuffer>
#include <QDataStream>

#define __STORMLIB_SELF__
#include <StormLib.h>

#include <ghost/config.h>
#include <ghost/gameslot.h>
#include <ghost/map.h>

#include "uiautohost.h"
#include "uislots.h"

MapConfigData::MapConfigData( QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect( ui.MOpen, &QAction::triggered, this, &MapConfigData::menuOpen );
	connect( ui.MSaveAs, &QAction::triggered, this, &MapConfigData::menuSaveAs );

	connect( ui.MAdd, &QMenu::triggered, this, &MapConfigData::onAdd );
	connect( ui.tabWidget, &QTabWidget::tabCloseRequested, this, &MapConfigData::onCloseTab );
}

MapConfigData::~MapConfigData()
{

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
	str = str.replace( "\\|r", "" );
	str = str.replace( "\\|n", "" );
	str = str.replace( "\\|R", "" );
	str = str.replace( "\\|N", "" );
}

void MapConfigData::menuSaveAs( )
{
	QString fileName = QFileDialog::getSaveFileName( this, "Сохранить как", m_CurrentFilePatch, "*.w3x *.w3m" );

	if ( fileName.size( ) > 0 )
	{
		QFile writeMap( fileName );

		if ( writeMap.open( QIODevice::ReadWrite | QIODevice::Truncate ) )
		{
			QDataStream write( &writeMap );
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

			read >> GrabInt;
			write << GrabInt;
			read >> GrabInt;
			write << GrabInt;

			// Write last 8 bytes

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
				write << (quint8)0;

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

		writeMap.close( );
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

				if ( version != MCD_BYTE_VERSION )
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

				QFileInfo fileInfo( file.fileName( ) );
				QString filename( fileInfo.fileName( ) );

			}
			else
				QMessageBox::critical( this, "Ошибка", "Не удалось открыть файл для чтения." );
		}	
		else
			QMessageBox::critical( this, "Ошибка", "Файл не является валидным MPQ архивом." );
	}
	
}


