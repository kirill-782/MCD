#ifndef MAPCONFIGDATA_H
#define MAPCONFIGDATA_H

#define MCD_BYTE_VERSION 1

#define MCD_ALLOW_SEND_METADATA 1
#define MCD_CUSTOM_SLOT 1 << 1
#define MCD_CUSTOM_MAPSTAT 1 << 2
#define MCD_CUSTOM_AUTOHOST 1 << 3

#define MU_HEADER_CONSTANT 200

#include <QtWidgets/QMainWindow>
#include "ui_mapconfigdata.h"
#include <QVector>
#include <qsslerror.h>
#include <QTime>

class CGroup;
class QTcpSocket;
class QWebSocket;

class MapConfigData : public QMainWindow
{
	Q_OBJECT

public:
	MapConfigData(QWidget *parent = 0);
	~MapConfigData();

	static QString readNullTremilanedString( QDataStream &stream );
	static QByteArray readNullTremilanedByte( QDataStream &stream );
	static void escapeColorString( QString &str );
	static void AppendLength( QByteArray &b );

	QString m_CurrentFilePatch;
	QByteArray m_SourceMapData;
	QByteArray m_EditedMapData;

	QWidget *m_LanWindow;

	bool m_Uploading;
	QTimer *m_Timer;
	quint32 m_UploaedPartsQueue;
	int m_UploaedPartsMaxQueue;

private:
	QWebSocket *m_WebSocket;
	QString m_UplpadMapName;
	QByteArray m_MapData;
	quint32 m_UploadOffset;

	QByteArray m_Buffer;

private slots:
	void menuOpen( );
	void menuSaveAs( );
	void onAdd( QAction *action );
	void onCloseTab( int index );

	void uploadPart( );
	void onUploadReqest( );
	void onSocketData( QByteArray message );
	void onSocketConnected( );
	void onSocketDisconnected( );

	void openLANWindow( );
	void onLANWindowClose( );

private:
	Ui::MapConfigDataClass ui;

	QMap<unsigned short, CGroup *> m_Tabs;
	void UpdateAllowAddTabs( );

	void AddTab( int index );

};

#endif // MAPCONFIGDATA_H
