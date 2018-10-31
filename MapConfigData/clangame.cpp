
#include "clangame.h"
#include "lan.hpp"
#include "mapconfigdata.h"
CLanGame::CLanGame( quint32 nHCID, QString nGameName, quint32 nSlots, quint32 nPlayerSlots, QWidget *parent )
	: QWidget(parent)
{
	ui = new Ui::CLanGame();
	ui->setupUi(this);

	m_HCID = nHCID;
	m_GameName = nGameName;
	m_Slots = nSlots;
	m_PlayerSlots = nPlayerSlots;

	ui->m_UIGameName->setText( "<span style=\" font-size:10pt; color:#008200;\">" + m_GameName + "</span>" );
	ui->m_UIPlayerStat->setText( "<span style=\" font-size:10pt; color:#b40000;\">(" + QString::number( m_PlayerSlots ) + "/" + ( QString::number( m_Slots ) ) + ")</span>" );

	connect( ui->m_UILanBroadcast, &QCheckBox::clicked, this, &CLanGame::LanStateChanged );
}

CLanGame::~CLanGame()
{
	delete ui;
}

void CLanGame::UpdateSlots( quint32 nSlots, quint32 nPlayerSlots )
{
	m_Slots = nSlots;
	m_PlayerSlots = nPlayerSlots;

	ui->m_UIGameName->setText( "<span style=\" font-size:10pt; color:#008200;\">" + m_GameName + "</span>" );
	ui->m_UIPlayerStat->setText( "<span style=\" font-size:10pt; color:#b40000;\">(" + QString::number( m_PlayerSlots ) + "/" + ( QString::number( m_Slots ) ) + ")</span>" );
}

void CLanGame::UpdateGameName( QString nGameName )
{
	m_GameName = nGameName;

	ui->m_UIGameName->setText( "<span style=\" font-size:10pt; color:#008200;\">" + m_GameName + "</span>" );
	ui->m_UIPlayerStat->setText( "<span style=\" font-size:10pt; color:#b40000;\">(" + QString::number( m_PlayerSlots ) + "/" + ( QString::number( m_Slots ) ) + ")</span>" );
}

QByteArray CLanGame::GetDecreatePackage( )
{
	QByteArray resp;
	QDataStream ds( &resp, QIODevice::ReadWrite );
	ds.setByteOrder( QDataStream::LittleEndian );

	ds << (quint8)W3GS_HEADER_CONSTANT;
	ds << (quint8)51;
	ds << (quint8)8;
	ds << (quint8)0;
	ds << m_HCID;

	return resp;
}

QByteArray CLanGame::GetSlotUpdatePackage( )
{
	QByteArray resp;
	QDataStream ds( &resp, QIODevice::ReadWrite );
	ds.setByteOrder( QDataStream::LittleEndian );

	ds << (quint8)W3GS_HEADER_CONSTANT;
	ds << (quint8)50;
	ds << (quint8)8;
	ds << (quint8)0;
	ds << m_HCID;
	ds << m_PlayerSlots;
	ds << m_Slots;

	return resp;
}

QByteArray CLanGame::GetGameInfoPackage( )
{
	QByteArray resp = m_GameInfoPackage;
	QDataStream ds( &resp, QIODevice::ReadWrite );
	ds.setByteOrder( QDataStream::LittleEndian );

	ds.skipRawData( 4 );
	MapConfigData::readNullTremilanedString( ds );
	ds.skipRawData( 1 );
	MapConfigData::readNullTremilanedString( ds );
	ds << m_Slots;
	ds.skipRawData( 8 );
	ds << m_Slots - m_PlayerSlots - 1;

	return resp;
}

void CLanGame::LanStateChanged( )
{
	if ( ui->m_UILanBroadcast->isChecked( ) )
		emit enabledLAN( );
	else
		emit disabledLAN( );
}