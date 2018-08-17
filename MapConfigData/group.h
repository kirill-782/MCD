#pragma once

#include "mapconfigdata.h"

class CGroup
{
public:
	CGroup( unsigned short nMask );
	virtual ~CGroup( );

	virtual QByteArray onSave( ) = 0;
	virtual void onLoad( QByteArray data ) = 0;

	unsigned short m_Mask;

};

