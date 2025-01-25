#ifndef PAGESCENE_H
#define PAGESCENE_H

#include "drawcurve.h"

#include <QGraphicsScene>

class PageScene : public QGraphicsScene
{
public:
    PageScene();

    QGraphicsPixmapItem* mPixmap = nullptr;
    void setImage(QImage image);

    void setPageRectToCropRect();
    QRectF getPageRect();

    void setCropRect(QRectF rect);
    QRectF getCropRect();
    void showCropRect(bool show);

    void setZoomRect(QRectF rect);
    QRectF getZoomRect();
    void showZoomRect(bool show);

    void addDrawCurve(DrawCurvePtr drawCurve);
    QList<DrawCurvePtr> drawCurves();
    void removeDrawCurve(DrawCurvePtr drawCurve);

private:
    QGraphicsRectItem* mPagerect = nullptr;
    void initPageRect();

    QGraphicsRectItem* mCroprect = nullptr;
    void initCropRect();

    QGraphicsRectItem* mZoomrect = nullptr;
    void initZoomRect();

    QList<DrawCurvePtr> mDrawCurves;
};

typedef QSharedPointer<PageScene> PageScenePtr;

#endif // PAGESCENE_H
