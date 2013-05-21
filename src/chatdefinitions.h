#ifndef CHATDEFINITIONS_H
#define CHATDEFINITIONS_H

#include "uidefinitions.h"

const int HTMLESC_COUNT = 5;
const QString htmlSymbol[] = {"&", "\"", "<", ">", " "};
const QString htmlEscape[] = {"&amp;", "&quot;", "&lt;", "&gt;", "&nbsp;"};

enum InfoType {
    IT_Ok			= 0x00,
    IT_Busy			= 0x01,
    IT_Offline		= 0x02,
    IT_Disconnected	= 0x04,
    IT_Away			= 0x08
};

/****************************************************************************
**	Chat state definitions
**	The enum and the string array should always be synced
****************************************************************************/
enum ChatState {
    CS_Blank = 0,
    CS_Active,
    CS_Composing,
    CS_Paused,
    CS_Inactive,
    CS_Gone,
    CS_Max
};

const QString ChatStateNames[] = {
    "",
    "active",
    "composing",
    "paused",
    "inactive",
    "gone"
};

#endif // CHATDEFINITIONS_H
