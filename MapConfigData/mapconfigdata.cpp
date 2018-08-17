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

MapConfigData::MapConfigData(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect( ui.MOpen, &QAction::triggered, this, &MapConfigData::menuOpen );
	connect( ui.MSaveAs, &QAction::triggered, this, &MapConfigData::menuSaveAs );
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

			// TODO


			if ( ui.MSaveMetadata->isChecked( ) )
				write << (quint16)MCD_ALLOW_SEND_METADATA;

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

				if ( version > MCD_BYTE_VERSION )
					QMessageBox::critical( this, "Ошибка", "Эта карта запакована новой версией MCD. Редактирование невозможно." );
				else
				{
					data.setByteOrder( QDataStream::LittleEndian );
					quint16 editMask;

					data >> editMask;

					ui.MSaveAs->setEnabled( true );
					ui.MSaveMetadata->setEnabled( true );
					ui.MSaveMetadata->setChecked( false );

					m_CurrentFilePatch = fileName;

					if ( editMask & MCD_ALLOW_SEND_METADATA )
						ui.MSaveMetadata->setChecked( true );

				}

				file.close( );

				QFileInfo fileInfo( file.fileName( ) );
				QString filename( fileInfo.fileName( ) );

				CConfig cfg;
				cfg.Set( "map_path", "Maps\\Download\\" + filename.toStdString( ) );
				cfg.Set( "map_localpath", m_CurrentFilePatch.toStdString( ) );

				CMap *map = new CMap( &cfg, filename.toStdString( ) );

				qDebug( ) << map->GetSlots( ).size( );
				qDebug( ) << map->GetValid( );

				vector<string> *x = CONSOLE_Get( );

				for ( vector<string> :: iterator i = x->begin( ); i != x->end( ); i++ )
					qDebug( ) << QString::fromStdString(*i);
			}
			else
				QMessageBox::critical( this, "Ошибка", "Не удалось открыть файл для чтения." );
		}	
		else
			QMessageBox::critical( this, "Ошибка", "Файл не является валидным MPQ архивом." );
	}
	
}


