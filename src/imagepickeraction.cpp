/****************************************************************************
**
** This file is part of Wavr IM Application.
** 
** Copyright (c) 2013 Parikshit Agarwal.
** 
** Contact:  parikshit.ag@gmail.com
** 
** Wavr is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Wavr is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Wavr.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/



#include <QMenu>
#include "imagepickeraction.h"

wavrImagePickerAction::wavrImagePickerAction(QObject* parent, const QString source[], int sourceCount, int picSize, int columns, int* selected)
    : QWidgetAction(parent) {
        this->source = new QList<QString>();
        for(int index = 0; index < sourceCount; index++)
            this->source->append(source[index]);
        this->picSize = picSize;
        this->columns = columns;
        this->selected = selected;
}

wavrImagePickerAction::~wavrImagePickerAction(void) {
}

void wavrImagePickerAction::releaseWidget(QWidget* widget) {
    widget->deleteLater();
}

QWidget* wavrImagePickerAction::createWidget(QWidget* parent) {
    QMenu* menu = (QMenu*)parent;
    int index = 0;
    for(; index < menu->actions().count(); index++)
        if(menu->actions()[index] == this)
            break;
    wavrImagePicker* widget = new wavrImagePicker(parent, source, picSize, columns, selected, index);
    return widget;
}
