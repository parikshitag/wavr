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



#ifndef SUBCONTROLS_H
#define SUBCONTROLS_H

#include <QComboBox>
#include <QStylePainter>
#include <QToolButton>
#include <QLineEdit>
#include <QKeyEvent>
#include "uidefinitions.h"

class wavrToolButton : public QToolButton {
public:
    wavrToolButton(QWidget* parent = 0);

protected:
    void paintEvent(QPaintEvent*);
};

class wavrCmbBox : public QComboBox {
        Q_OBJECT

    public:
        wavrCmbBox(QWidget* parent = 0);

    signals:
        void lostFocus(void);

    protected:
        void focusInEvent(QFocusEvent* event);
        void focusOutEvent(QFocusEvent* event);
        void mousePressEvent(QMouseEvent *event);

    private:
        void highlight(void);
};

class wavrLineEdit : public QLineEdit {
        Q_OBJECT

    public:
        wavrLineEdit(QWidget* parent = 0);

    protected:
           void resizeEvent(QResizeEvent *);
           void keyPressEvent(QKeyEvent *);

    private slots:
           void updateCloseButton(const QString &text);
            void hideSearch(void);

    private:
           QToolButton *clearButton;
};

#endif // SUBCONTROLS_H
