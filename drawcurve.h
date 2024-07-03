#ifndef DRAWCURVE_H
#define DRAWCURVE_H

#include <QPainterPath>
#include <QPen>
#include <QGraphicsPathItem>

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

private:
    QList<QLineF> mLines;
    bool initialised = false;
    QPainterPath mPainterPath;
    QGraphicsPathItem* mScenePath;
};

typedef QSharedPointer<DrawCurve> DrawCurvePtr;

#endif // DRAWCURVE_H
