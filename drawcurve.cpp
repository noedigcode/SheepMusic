#include "drawcurve.h"


DrawCurve::DrawCurve()
{
    mScenePath = new QGraphicsPathItem();
    QPen pen;
    pen.setCosmetic(true);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    mScenePath->setPen(pen);
}

DrawCurve::~DrawCurve()
{
    delete mScenePath;
}

QPainterPath DrawCurve::painterPath()
{
    return mPainterPath;
}

QGraphicsPathItem* DrawCurve::scenePathItem()
{
    return mScenePath;
}

void DrawCurve::addPoint(QPointF point)
{
    if (!initialised) {
        mPainterPath = QPainterPath(point);
        initialised = true;
    } else {
        mPainterPath.lineTo(point);

        int n = mPainterPath.elementCount();
        QPainterPath::Element elem1 = mPainterPath.elementAt(n - 2);
        QPainterPath::Element elem2 = mPainterPath.elementAt(n - 1);
        mLines.append(QLineF(elem1.x, elem1.y, elem2.x, elem2.y));

        mScenePath->setPath(mPainterPath);
    }
}

bool DrawCurve::intersects(DrawCurve* otherCurve)
{
    foreach (const QLineF& line1, mLines) {
        foreach (const QLineF& line2, otherCurve->lines()) {
            if (linesIntersect(line1, line2)) {
                return true;
            }
        }
    }
    return false;
}

bool DrawCurve::linesIntersect(const QLineF& line1, const QLineF& line2)
{
    return line1.intersects(line2, nullptr) == QLineF::BoundedIntersection;
}

QList<QLineF> DrawCurve::lines() const
{
    return mLines;
}

QJsonObject DrawCurve::toJson()
{
    QJsonObject obj;

    QJsonArray points;
    for (int i = 0; i < mPainterPath.elementCount(); i++) {
        QPainterPath::Element e = mPainterPath.elementAt(i);
        QJsonObject point;
        point.insert("x", e.x);
        point.insert("y", e.y);
        points.append(point);
    }
    obj.insert("points", points);

    return obj;
}

void DrawCurve::fromJson(QJsonObject obj)
{
    QJsonArray points = obj.value("points").toArray();
    foreach (QJsonValue v, points) {
        QJsonObject point = v.toObject();
        this->addPoint(QPointF(point.value("x").toDouble(),
                               point.value("y").toDouble()));
    }
}
