/******************************************************************************
 *
 * This file is part of SheepMusic.
 * Copyright (C) 2025 Gideon van der Kolf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include "breadcrumbswidget.h"

#include <QPainter>
#include <QMouseEvent>

BreadcrumbsWidget::BreadcrumbsWidget(QWidget *parent)
    : QWidget{parent}
{

}

void BreadcrumbsWidget::setBounds(int count, int current)
{
    mCount = count;
    mCurrent = current;

    update();
}

void BreadcrumbsWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    QBrush bg(Qt::white);
    QBrush current(QColor("#2f8ca3"));

    if (mCount <= 0) {
        painter.drawRect(this->rect());
    } else {
        float w = (float)width() / (float)mCount;
        for (int i = 0; i < mCount; i++) {
            if (i == mCurrent) {
                painter.setBrush(current);
            } else {
                painter.setBrush(bg);
            }
            painter.drawRect(w * i, 0, w, height());
        }
    }
}

void BreadcrumbsWidget::mousePressEvent(QMouseEvent* event)
{
    if (mCount <= 0) { return; }

    int index = ((float)event->pos().x() / (float)width()) * mCount;
    emit breadcrumbClicked(index);
}
