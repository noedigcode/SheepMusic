#include "pagescene.h"

PageScene::PageScene()
{
    setBackgroundBrush(QBrush(Qt::white));
}

void PageScene::setImage(QImage image)
{
    mPixmap = this->addPixmap(QPixmap::fromImage(image));
    initPageRect();
}

void PageScene::setPageRectToCropRect()
{
    if (!mPagerect) { initPageRect(); }
    mPagerect->setRect(mCroprect->rect());
}

QRectF PageScene::getPageRect()
{
    if (!mPagerect) { initPageRect(); }
    return mPagerect->rect();
}

void PageScene::setCropRect(QRectF rect)
{
    if (!mCroprect) { initCropRect(); }
    mCroprect->setRect(rect);
}

QRectF PageScene::getCropRect()
{
    if (!mCroprect) { initCropRect(); }
    return mCroprect->rect();
}

void PageScene::showCropRect(bool show)
{
    if (show) {
        if (!mCroprect) { initCropRect(); }
        mCroprect->show();
    } else {
        if (mCroprect) { mCroprect->hide(); }
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

void PageScene::initPageRect()
{
    QRectF rect;
    if (mCroprect) {
        rect = mCroprect->rect();
    } else if (mPixmap) {
        rect = mPixmap->boundingRect();
    }

    if (!mPagerect) {
        mPagerect = new QGraphicsRectItem(rect);
        QPen pen(Qt::black, 1);
        pen.setCosmetic(true);
        mPagerect->setPen(pen);

        // Add the item to the scene
        this->addItem(mPagerect);
        mPagerect->setZValue(1);
    } else {
        mPagerect->setRect(rect);
    }
}

void PageScene::initCropRect()
{
    QRectF rect;
    if (mPixmap) {
        rect = mPixmap->boundingRect();
    }
    mCroprect = new QGraphicsRectItem(rect);
    QPen pen(Qt::blue, 2);
    pen.setCosmetic(true);
    mCroprect->setPen(pen);
    QColor fillColor("#676cf5");
    fillColor.setAlphaF(0.5);
    mCroprect->setBrush(fillColor); // Set semi-transparent blue fill

    // Add the item to the scene
    this->addItem(mCroprect);
    mCroprect->setZValue(1);
    mCroprect->hide();
}

void PageScene::initZoomRect()
{
    mZoomrect = new QGraphicsRectItem();
    QPen pen(Qt::blue, 2);
    pen.setCosmetic(true);
    mZoomrect->setPen(pen);

    // Add the item to the scene
    this->addItem(mZoomrect);
    mZoomrect->setZValue(1);
    mZoomrect->hide();
}
