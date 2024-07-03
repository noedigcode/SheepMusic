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
    QGraphicsPathItem* mScenePath;
};

typedef QSharedPointer<DrawCurve> DrawCurvePtr;

#endif // DRAWCURVE_H
