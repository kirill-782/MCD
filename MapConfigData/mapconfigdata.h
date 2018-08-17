#ifndef MAPCONFIGDATA_H
#define MAPCONFIGDATA_H

#define MCD_BYTE_VERSION 1

#define MCD_ALLOW_SEND_METADATA 1
#define MCD_CUSTOM_SLOT 1 << 1
#define MCD_CUSTOM_MAPSTAT 1 << 2
#define MCD_CUSTOM_AUTOHOST 1 << 3

#include <QtWidgets/QMainWindow>
#include "ui_mapconfigdata.h"
#include <QVector>

class MapConfigData : public QMainWindow
{
	Q_OBJECT

public:
	MapConfigData(QWidget *parent = 0);
	~MapConfigData();

	static QString readNullTremilanedString( QDataStream &stream );
	static QByteArray readNullTremilanedByte( QDataStream &stream );
	static void escapeColorString( QString &str );

	QString m_CurrentFilePatch;
	QByteArray m_SourceMapData;

private slots:
	void menuOpen( );
	void menuSaveAs( );

private:
	Ui::MapConfigDataClass ui;
};

#endif // MAPCONFIGDATA_H
