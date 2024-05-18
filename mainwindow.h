#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_load_clicked();

private:
    Ui::MainWindow *ui;

    void print(QString msg);

    class PageScene : public QGraphicsScene
    {
    public:
        PageScene();

        QGraphicsPixmapItem* mPixmap = nullptr;
        QGraphicsRectItem* mSelrect = nullptr;

        void setImage(QImage image);
    };
    typedef QSharedPointer<PageScene> PageScenePtr;

    QList<PageScenePtr> pages;
    int currentPage = -1;

    void setupGraphicsView();
    bool mIsCropping = false;
    bool mGraphicsViewLeftMouseDown = false;
    int mSelrectEdge = 0;
    QPointF mSelStart;

    void viewPage(int index);
    void scaleScene();

private slots:
    void onGraphicsViewLeftMouseDragStart(QPointF pos);
    void onGraphicsViewLeftMouseDrag(QPointF pos);
    void onGraphicsViewLeftMouseDragEnd(QPointF pos);
    void onGraphicsViewResized();
    void on_pushButton_crop_clicked();
    void on_action_Debug_Console_triggered();
    void on_pushButton_next_clicked();
    void on_pushButton_prev_clicked();
};
#endif // MAINWINDOW_H
