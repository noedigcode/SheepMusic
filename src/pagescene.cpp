#include "pagescene.h"

PageScene::PageScene()
{
    setBackgroundBrush(QBrush(Qt::white));
}

void PageScene::setImage(QImage image)
{
    mPixmap = this->addPixmap(QPixmap::fromImage(image));
}

void PageScene::setSelRect(QRectF rect)
{
    if (!mSelrect) { initSelRect(); }
    mSelrect->setRect(rect);
}

QRectF PageScene::getSelRect()
{
    if (!mSelrect) { initSelRect(); }
    return mSelrect->rect();
}

void PageScene::showSelRect(bool show)
{
    if (show) {
        if (!mSelrect) { initSelRect(); }
        mSelrect->show();
    } else {
        if (mSelrect) { mSelrect->hide(); }
    }
}

void PageScene::setZoomRect(QRectF rect)
{
    if (!mZoomrect) { initZoomRect(); }
    mZoomrect->setRect(rect);
}

QRectF PageScene::getZoomRect()
{
    if (!mZoomrect) { initZoomRect(); }
    return mZoomrect->rect();
}

void PageScene::showZoomRect(bool show)
{
    if (show) {
        if (!mZoomrect) { initZoomRect(); }
        mZoomrect->show();
    } else {
        if (mZoomrect) { mZoomrect->hide(); }
    }
}

void PageScene::addDrawCurve(DrawCurvePtr drawCurve)
{
    mDrawCurves.append(drawCurve);
    this->addItem(drawCurve->scenePathItem());
}

QList<DrawCurvePtr> PageScene::drawCurves()
{
    return mDrawCurves;
}

void PageScene::removeDrawCurve(DrawCurvePtr drawCurve)
{
    this->removeItem(drawCurve->scenePathItem());
    mDrawCurves.removeAll(drawCurve);
}

void PageScene::initSelRect()
{
    QRectF rect;
    if (mPixmap) {
        rect = mPixmap->boundingRect();
    }
    mSelrect = new QGraphicsRectItem(rect);
    mSelrect->setPen(QPen(Qt::blue, 2)); // Set blue border
    QColor fillColor("#676cf5");
    fillColor.setAlphaF(0.5);
    mSelrect->setBrush(fillColor); // Set semi-transparent blue fill

    // Add the item to the scene
    this->addItem(mSelrect);
    mSelrect->setZValue(1);
    mSelrect->hide();
}

void PageScene::initZoomRect()
{
    mZoomrect = new QGraphicsRectItem();
    mZoomrect->setPen(QPen(Qt::blue, 2)); // Set blue border
    QColor fillColor("#676cf5");
    fillColor.setAlphaF(0.5);
    mZoomrect->setBrush(fillColor); // Set semi-transparent blue fill

    // Add the item to the scene
    this->addItem(mZoomrect);
    mZoomrect->setZValue(1);
    mZoomrect->hide();
}
