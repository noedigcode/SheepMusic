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
