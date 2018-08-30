#include "uiautohost.h"

UIAutohost::UIAutohost( MapConfigData *parent )
	: CGroup( parent)
{
	ui.setupUi(this);

}

UIAutohost::~UIAutohost()
{

}

QByteArray UIAutohost::onSave( )
{
	QByteArray b;
	b.push_back( (char)ui.spinBox->value( ) );
	b.push_back( (char)ui.spinBox_2->value( ) );

	QByteArray gameName =  ui.lineEdit->text( ).toUtf8( );
	
	if ( gameName.size( ) > 29 )
	{
		// size too long
		// fail
	}

	b.append( gameName );
	b.push_back( (char)0 );

	return b;
}

void UIAutohost::onLoad( QDataStream & data )
{
	if ( !data.atEnd( ) )
	{
		quint8 countGames;
		quint8 startPlayers;

		data >> countGames;
		data >> startPlayers;

		ui.spinBox->setValue( (unsigned char)countGames );
		ui.spinBox_2->setValue( (unsigned char)startPlayers );

		ui.lineEdit->setText( MapConfigData::readNullTremilanedString( data ) );
		return;
	}

	// fail
}

