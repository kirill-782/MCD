#ifndef UIAUTOHOST_H
#define UIAUTOHOST_H

#include <QWidget>
#include "ui_uiautohost.h"
#include "group.h"

class UIAutohost : public CGroup
{
	Q_OBJECT

public:
	UIAutohost( MapConfigData *parent = 0);
	~UIAutohost();

	virtual QByteArray onSave( );
	virtual void onLoad( QDataStream &data );

private:
	Ui::UIAutohost ui;
};

#endif // UIAUTOHOST_H
