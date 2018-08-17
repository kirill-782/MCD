#ifndef UISLOTS_H
#define UISLOTS_H

#include <QWidget>
#include "ui_uislots.h"
#include "group.h"

class UISlots : public CGroup
{
	Q_OBJECT

public:
	UISlots(QWidget *parent = 0);
	~UISlots();

private:
	Ui::UISlots ui;
};

#endif // UISLOTS_H
