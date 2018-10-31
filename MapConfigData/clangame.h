#ifndef CLANGAME_H
#define CLANGAME_H

#include "ui_clangame.h"
#include <QWidget>
#include <qdatastream.h>
#include <QByteArray>

namespace Ui {class CLanGame;};

class CLanGame : public QWidget
{
	Q_OBJECT

public:
	CLanGame(quint32 nHCID, QString nGameName, quint32 nSlots, quint32 nPlayerSlots, QWidget *parent = 0 );
	~CLanGame();

	quint32 m_HCID;
	QString m_GameName;
	quint32 m_Slots;
	quint32 m_PlayerSlots;

	QByteArray m_GameInfoPackage;

	void UpdateSlots( quint32 nSlots, quint32 nPlayerSlots );
	void UpdateGameName( QString nGameName );

	bool GetAllowLan( ) { return ui->m_UILanBroadcast->isChecked( ); }

	QByteArray GetDecreatePackage( );
	QByteArray GetSlotUpdatePackage( );
	QByteArray GetGameInfoPackage( );

signals:
	void disabledLAN( );
	void enabledLAN( );

private:
	Ui::CLanGame *ui;

private slots:
	void LanStateChanged( );
};

#endif // CLANGAME_H
