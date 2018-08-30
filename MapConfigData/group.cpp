#include "group.h"

CGroup::CGroup( QWidget *parent ) : QWidget( parent )
{
}


CGroup::~CGroup( )
{
}

void CGroup::onCFGRead( CConfig * CFG )
{
}

QVector<QString> CGroup::onCFGGenerate( )
{
	return QVector<QString>( );
}
