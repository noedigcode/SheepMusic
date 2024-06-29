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
