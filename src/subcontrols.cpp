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


#include <QApplication>
#include "subcontrols.h"

wavrToolButton::wavrToolButton(QWidget* parent) : QToolButton(parent) {
}

void wavrToolButton::paintEvent(QPaintEvent*) {
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    opt.features &= (~QStyleOptionToolButton::HasMenu);
    p.drawComplexControl(QStyle::CC_ToolButton, opt);
}

wavrCmbBox::wavrCmbBox(QWidget* parent) : QComboBox(parent) {
//    QPalette p = QApplication::palette();
//    p.setColor(QPalette::Base, p.color(QPalette::Window));
//    p.setColor(QPalette::Text, GRAY_TEXT_COLOR);
//    setPalette(p);
}

void wavrCmbBox::focusInEvent(QFocusEvent* event) {
    QComboBox::focusInEvent(event);
    highlight();
}

void wavrCmbBox::focusOutEvent(QFocusEvent* event) {
    QComboBox::focusOutEvent(event);
    emit lostFocus();
}

void wavrCmbBox::mousePressEvent(QMouseEvent *event) {
    QComboBox::mousePressEvent(event);
    highlight();
}

void wavrCmbBox::highlight(void) {
    lineEdit()->setSelection(0, currentText().length());
    QPalette p = QApplication::palette();
    p.setColor(QPalette::Base, p.color(QPalette::Base));
    p.setColor(QPalette::Text, p.color(QPalette::Text));
    setPalette(p);
}

wavrLineEdit::wavrLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    clearButton = new QToolButton(this);
    QPixmap pixmap(IDR_SRCHCLOSE);
    clearButton->setIcon(QIcon(pixmap));
    clearButton->setIconSize(pixmap.size());
    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    clearButton->hide();
    connect(clearButton, SIGNAL(clicked()), this, SLOT(hideSearch()));
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateCloseButton(const QString&)));
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(clearButton->sizeHint().width() + frameWidth + 1));
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
                   qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));
}

void wavrLineEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = clearButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    clearButton->move(rect().right() - frameWidth - sz.width(),
                      (rect().bottom() + 1 - sz.height())/2);
}

void wavrLineEdit::keyPressEvent(QKeyEvent *pKeyEvent) {
    QLineEdit::keyPressEvent(pKeyEvent);
    if(pKeyEvent->key() == Qt::Key_Escape) {
        setVisible(false);
        clear();
    }
}

void wavrLineEdit::updateCloseButton(const QString& text)
{
    clearButton->setVisible(!text.isEmpty());
}

void wavrLineEdit::hideSearch(void) {
    clear();
    hide();
}
