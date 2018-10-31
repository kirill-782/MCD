#pragma once
#include <QWidget>
#include "ui_lan.h"
#include <QTCPSocket>
#include <QTCPServer>
#include <QUDPSocket>
#include <qpair.h>
#include "clangame.h"

#define W3GS_HEADER_CONSTANT		247

typedef QPair<QTcpSocket *, QTcpSocket *> Proxy;

class Lan : public QWidget {
	Q_OBJECT

public:
	Lan(QWidget * parent = Q_NULLPTR);
	~Lan();

private:
	Ui::Lan ui;

	QTcpSocket *m_MasterSocket;
	QUdpSocket *m_UDPSocket;
	QByteArray m_Buffer;
	QTcpServer *m_Server;

	QVector<CLanGame *> m_Games;

	int m_NextRow;

	void onPackage( quint8 pType, QByteArray pData );

	QTimer *m_Timer;

	QVector<Proxy> m_Proxys;


private slots:
	void Run( );
	void onSocketData( );
	void onSocketConnected( );
	void onSocketDissconected( );
	void Ping( );

	void ProxyConnected( );
	void ProxyData( );
	void ProxyDisconnected( );
	void ProxyCanAccept( );

	void GameEnabled( );
	void GameDisabled( );
};


