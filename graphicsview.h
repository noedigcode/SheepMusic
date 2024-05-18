#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWidget>
#include <QMouseEvent>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget *parent = nullptr);

signals:
    void leftMouseDragStart(QPointF pos);
    void leftMouseDrag(QPointF pos);
    void leftMouseDragEnd(QPointF pos);

protected:
    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (event->buttons() & Qt::LeftButton) {
            emit leftMouseDrag(mapToScene(event->pos()));
        }

        QGraphicsView::mouseMoveEvent(event);
    }

    void mousePressEvent(QMouseEvent *event)
    {
        if (event->buttons() & Qt::LeftButton) {
            emit leftMouseDragStart(mapToScene(event->pos()));
        }

        QGraphicsView::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event)
    {
        if (event->buttons() & Qt::LeftButton) {
            emit leftMouseDragEnd(mapToScene(event->pos()));
        }

        QGraphicsView::mouseReleaseEvent(event);
    }
};

#endif // GRAPHICSVIEW_H
