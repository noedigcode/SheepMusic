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

#ifndef DRAWCURVE_H
#define DRAWCURVE_H

#include <QGraphicsPathItem>
#include <QJsonArray>
#include <QJsonObject>
#include <QPainterPath>
#include <QPen>

class DrawCurve
{
public:
    DrawCurve();
    ~DrawCurve();

    QPainterPath painterPath();
    QGraphicsPathItem* scenePathItem();
    void addPoint(QPointF point);
    bool intersects(DrawCurve* otherCurve);
    bool linesIntersect(const QLineF& line1, const QLineF& line2);
    QList<QLineF> lines() const;

    QJsonObject toJson();
    void fromJson(QJsonObject obj);

private:
    QList<QLineF> mLines;
    bool initialised = false;
    QPainterPath mPainterPath;
    QGraphicsPathItem* mScenePath = nullptr;
};

typedef QSharedPointer<DrawCurve> DrawCurvePtr;

#endif // DRAWCURVE_H
