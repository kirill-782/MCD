#include "mapconfigdata.h"
#include "uislots.h"
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qscrollarea.h>

#include <ghost\map.h>
#include <ghost\config.h>

#include <vector>

UISlots::UISlots( MapConfigData *parent)
	: CGroup(parent)
{
	ui.setupUi(this);

	CConfig MapCFG;
	MapCFG.Set( "map_path", "" );
	MapCFG.Set( "map_localpath", parent->m_CurrentFilePatch.toStdString( ) );

	CMap *map = new CMap( &MapCFG, "" );
	
	vector<CGameSlot> gameslots = map->GetSlots( );

	if ( gameslots.size( ) > 12 )
	{
		QMessageBox::critical( this, "Too many slots", "This map have 12+ slots." );
		return;
	}

	for ( auto i = gameslots.begin( ); i != gameslots.end( ); ++i )
	{
		m_Slots.push_back( new UISlot( this, *i, m_Slots.size( ) ) );
	}

	connect( ui.helpButton, &QPushButton::clicked, this, &UISlots::ShowHelp );
	connect( ui.addSlot, &QPushButton::clicked, this, &UISlots::AddSlot );

	if ( map->GetMapOptions( ) & MAPOPT_MELEE )
		ui.meleCheck->setChecked( true );
	if ( map->GetMapOptions( ) & MAPOPT_CUSTOMFORCES )
		ui.checkBox->setChecked( true );
	if ( map->GetMapOptions( ) & MAPOPT_FIXEDPLAYERSETTINGS )
		ui.fixsedPSCheck->setChecked( true );

	m_AddNum = 20;

	delete map;
	UpdateAddButtonState( );
}

UISlots::~UISlots()
{
	for ( auto i = m_Slots.begin( ); i != m_Slots.end( ); ++i )
		delete *i;
}

void UISlots::RemoveReqest( )
{
	for ( auto i = m_Slots.begin( ); i != m_Slots.end( ); ++i )
	{
		if ( ( *i )->isMeRemoveButton( sender( ) ) )
		{
			delete *i;
			m_Slots.erase( i );
			UpdateAddButtonState( );
			break;
			
		}
	}
}
QByteArray UISlots::onSave( )
{
	QByteArray resp;

	quint8 flags = ( ui.meleCheck->isChecked( ) ? MAPOPT_MELEE : 0 ) | ( ui.fixsedPSCheck->isChecked( ) ? MAPOPT_FIXEDPLAYERSETTINGS : 0 ) | ( ui.checkBox->isChecked( ) ? MAPOPT_CUSTOMFORCES : 0 );
	quint8 count = m_Slots.size( );

	resp.push_back( flags );
	resp.push_back( count );

	for ( auto i = m_Slots.begin( ); i != m_Slots.end( ); ++i )
		resp.append( ( *i )->getSlotInfo( ) );
	
	return resp;
}

void UISlots::onLoad( QDataStream & data )
{
	quint8 flags;
	quint8 count;

	data >> flags;
	data >> count;

	for ( auto i = m_Slots.begin( ); i != m_Slots.end( ); ++i )
		delete *i;

	m_Slots.clear( );

	for ( int i = 0; i < count; i++ )
	{
		quint8 controller;
		quint8 race;
		quint8 team;
		quint8 color;
		quint8 handicap;

		data >> controller;
		data >> race;
		data >> team;
		data >> color;
		data >> handicap;

		quint8 SLOTSTATUS = ( controller < 2 ) ? (int)controller : SLOTSTATUS_OPEN;
		quint8 COMPUTER = ( controller >= 2 ) ? 1 : 0;
		quint8 COMPUTER_TYPE = controller - 2;
		CGameSlot newSlot( 0, 0, SLOTSTATUS, COMPUTER, team, color, race, COMPUTER_TYPE, handicap );

		m_Slots.push_back( new UISlot( this, newSlot, i ) );

	}

	ui.meleCheck->setChecked( false );
	ui.checkBox->setChecked( false );
	ui.fixsedPSCheck->setChecked( false );

	if ( flags & MAPOPT_MELEE )
		ui.meleCheck->setChecked( true );
	if ( flags & MAPOPT_CUSTOMFORCES )
		ui.checkBox->setChecked( true );
	if ( flags & MAPOPT_FIXEDPLAYERSETTINGS )
		ui.fixsedPSCheck->setChecked( true );

	UpdateAddButtonState( );
}

void UISlots::UpdateAddButtonState( )
{
	if ( m_Slots.size( ) < 12 )
		ui.addSlot->setVisible( true );
	else
		ui.addSlot->setVisible( false );
}

void UISlots::ShowHelp( )
{
	QMessageBox::information( this, "", "" );
}

void UISlots::AddSlot( )
{
	m_Slots.push_back( new UISlot( this, CGameSlot( 0, 0, SLOTSTATUS_OPEN, 0, 12, m_Slots.size( ), SLOTRACE_NIGHTELF, SLOTCOMP_NORMAL, 100 ), m_AddNum++ ) );
	UpdateAddButtonState( );
}

UISlot::UISlot( UISlots * widget, CGameSlot & slot, quint8 SID )
{
	// Create Layout

	Layout = new QHBoxLayout( );
	Layout->setSpacing( 6 );

	// Create combobx for slot status

	UISlotStatus = new QComboBox( widget );

	UISlotStatus->addItem( "Open" );
	UISlotStatus->addItem( "Closed" );
	UISlotStatus->addItem( "Computer (easy)" );
	UISlotStatus->addItem( "Computer (normal)");
	UISlotStatus->addItem( "Computer (hard)" );

	switch ( slot.GetSlotStatus( ) )
	{
	case SLOTSTATUS_OPEN:
		UISlotStatus->setCurrentIndex( 0 );
		break;
	case SLOTSTATUS_CLOSED:
		UISlotStatus->setCurrentIndex( 1 );
		break;
	default:
		if ( !slot.GetComputer( ) )
		{
			UISlotStatus->setCurrentIndex( 0 );
			break;
		}
		switch ( slot.GetComputerType( ) )
		{
		case SLOTCOMP_EASY:
			UISlotStatus->setCurrentIndex( 2 );
			break;
		case SLOTCOMP_NORMAL:
			UISlotStatus->setCurrentIndex( 3 );
			break;
		case SLOTCOMP_HARD:
			UISlotStatus->setCurrentIndex( 4 );
			break;
		default:
			UISlotStatus->setCurrentIndex( 0 );
			break;
		}
	}

	Layout->addWidget( UISlotStatus, 10 );

	// Race

	UIRace = new QComboBox( widget );

	UIRace->addItem( "Numan" );
	UIRace->addItem( "Orc" );
	UIRace->addItem( "NightElf" );
	UIRace->addItem( "Undead" );
	UIRace->addItem( "Random" );

	if ( slot.GetRace( ) & SLOTRACE_HUMAN )
		UIRace->setCurrentIndex( 0 );
	else if ( slot.GetRace( ) & SLOTRACE_ORC )
		UIRace->setCurrentIndex( 1 );
	else if ( slot.GetRace( ) & SLOTRACE_NIGHTELF )
		UIRace->setCurrentIndex( 2 );
	else if ( slot.GetRace( ) & SLOTRACE_UNDEAD )
		UIRace->setCurrentIndex( 3 );
	else
		UIRace->setCurrentIndex( 4 );


	Layout->addWidget( UIRace, 5 );

	//Selectableraces

	UISelectableRaces = new QCheckBox( widget );
	UISelectableRaces->setMaximumWidth( 13 );
	UISelectableRaces->setMinimumHeight( 20 );

	if ( slot.GetRace( ) & SLOTRACE_SELECTABLE )
		UISelectableRaces->setChecked( true );
	else
		UISelectableRaces->setChecked( false );

	Layout->addWidget( UISelectableRaces, 1 );

	// Team

	UITeam = new QComboBox( widget );

	UITeam->addItem( "Clan 1" );
	UITeam->addItem( "Clan 2" );
	UITeam->addItem( "Clan 3" );
	UITeam->addItem( "Clan 4" );
	UITeam->addItem( "Clan 5" );
	UITeam->addItem( "Clan 6" );
	UITeam->addItem( "Clan 7" );
	UITeam->addItem( "Clan 8" );
	UITeam->addItem( "Clan 9" );
	UITeam->addItem( "Clan 10" );
	UITeam->addItem( "Clan 11" );
	UITeam->addItem( "Clan 12" );
	UITeam->addItem( "Obs/Ref" );

	if ( slot.GetTeam( ) <= 12 )
		UITeam->setCurrentIndex( slot.GetTeam( ) );

	Layout->addWidget( UITeam, 3 );

	//COLOR TODO

	UIColor = new QComboBox( widget );
	UIColor->addItem( "0" );
	UIColor->addItem( "1" );
	UIColor->addItem( "2" );
	UIColor->addItem( "3" );
	UIColor->addItem( "4" );
	UIColor->addItem( "5" );
	UIColor->addItem( "6" );
	UIColor->addItem( "7" );
	UIColor->addItem( "8" );
	UIColor->addItem( "9" );
	UIColor->addItem( "10" );
	UIColor->addItem( "11" );
	UIColor->addItem( "12" );

	if ( slot.GetColour( ) < 12 )
		UIColor->setCurrentIndex( slot.GetColour( ) );

	Layout->addWidget( UIColor, 3 );

	// Handicap

	UIHandicap = new QSpinBox( widget );

	UIHandicap->setMaximum( 255 );
	UIHandicap->setMinimum( 0 );

	UIHandicap->setValue( slot.GetHandicap( ) );

	Layout->addWidget( UIHandicap, 3 );

	// remove btn

	UIRemove = new QPushButton( "-" );
	UIRemove->setMaximumWidth( 25 );
	Layout->addWidget( UIRemove );

	widget->connect( UIRemove, &QPushButton::clicked, widget, &UISlots::RemoveReqest );
	widget->ui.gridLayout->addLayout( Layout, 6 + SID, 0 );

}

UISlot::~UISlot( )
{
	delete Layout;
	delete UIColor;
	delete UIHandicap;
	delete UISelectableRaces;
	delete UISlotStatus;
	delete UIRace;
	delete UITeam;
	delete UIRemove;
}

QByteArray UISlot::getSlotInfo( )
{
	QByteArray resp;

	quint8 controller = UISlotStatus->currentIndex( );
	quint8 race = 32;

	switch ( UIRace->currentIndex( ) )
	{
	case 0:
		race = SLOTRACE_HUMAN;
		break;
	case 1:
		race = SLOTRACE_ORC;
		break;
	case 2:
		race = SLOTRACE_NIGHTELF;
		break;
	case 3:
		race = SLOTRACE_UNDEAD;
		break;
	case 4:
		race = SLOTRACE_RANDOM;
		break;
	}

	if ( UISelectableRaces->isChecked( ) )
		race |= SLOTRACE_SELECTABLE;

	quint8 team = UITeam->currentIndex( );
	quint8 color = UIColor->currentIndex( );
	quint8 handicap = UIHandicap->value( );

	resp.push_back( controller );
	resp.push_back( race );
	resp.push_back( team );
	resp.push_back( color );
	resp.push_back( handicap );

	return resp;
}

bool UISlot::isMeRemoveButton( QObject * obj )
{
	return obj == UIRemove;
}
