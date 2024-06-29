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
    void leftClick(QPointF pos);
    void leftMouseDragStart(QPointF pos);
    void leftMouseDrag(QPointF pos);
    void leftMouseDragEnd(QPointF pos);
    void resized();

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
            emit leftClick(mapToScene(event->pos()));
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

    void resizeEvent(QResizeEvent* event) override
    {
        emit resized();
        QGraphicsView::resizeEvent(event);
    }
};

#endif // GRAPHICSVIEW_H
