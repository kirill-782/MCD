#ifndef UISLOTS_H
#define UISLOTS_H

#include <QWidget>
#include "ui_uislots.h"
#include "group.h"

class UISlot;
class CGameSlot;
class QSpinBox;

//
// BYTEMAP
//
//  1 BYTE			-> FLAGS
//  1 BYTE			-> SLOTS COUNT
//	FOR 0 .. SLOTS COUNT
//		1 BYTE			-> CONTROLLER 0 - open, 1 - close, 2 - comp easy, 3 - comp normal, 4 - comp hard
//		1 BYTE			-> RACE ( LIKE gameslot.h )
//		1 BYTE			-> TEAM
//		1 BYTE			-> COLOR
//		1 BYTE			-> HCANDICAP

class UISlots : public CGroup
{
	Q_OBJECT

public:
	UISlots( MapConfigData *parent = 0);
	~UISlots();

	virtual QByteArray onSave( );
	virtual void onLoad( QDataStream &data );

	Ui::UISlots ui;

	QVector<UISlot *> m_Slots;
	void UpdateAddButtonState( );
	int m_AddNum;

public slots:
	void RemoveReqest( );
	void ShowHelp( );
	void AddSlot( );
};

class UISlot
{
public:
	UISlot( UISlots *widget, CGameSlot &slot, quint8 SID );
	~UISlot( );
	QByteArray getSlotInfo( );
	bool isMeRemoveButton( QObject *obj );

private:
	QPushButton *UIRemove;

	QHBoxLayout *Layout;
	QComboBox *UISlotStatus;
	QComboBox *UIRace;
	QComboBox *UITeam;
	QComboBox *UIColor;
	QSpinBox *UIHandicap;
	QCheckBox *UISelectableRaces;
};

#endif // UISLOTS_H
