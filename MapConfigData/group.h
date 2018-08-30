#pragma once

#include "mapconfigdata.h"

class CConfig;

class CGroup : public QWidget
{
public:
	CGroup( QWidget *parent );
	virtual ~CGroup( );

	virtual QByteArray onSave( ) = 0;
	virtual void onLoad( QDataStream &data ) = 0;
	virtual void onCFGRead( CConfig *CFG );
	virtual QVector<QString> onCFGGenerate( );

};

