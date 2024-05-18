#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

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

    QGraphicsScene* mScene = nullptr;
    void setupGraphicsView();
    bool mIsCropping = false;
    bool mGraphicsViewLeftMouseDown = false;
    QGraphicsRectItem* mSelrect = nullptr;
    int mSelrectEdge = 0;
    QPointF mSelStart;

private slots:
    void onGraphicsViewLeftMouseDragStart(QPointF pos);
    void onGraphicsViewLeftMouseDrag(QPointF pos);
    void onGraphicsViewLeftMouseDragEnd(QPointF pos);
    void on_pushButton_crop_clicked();
    void on_action_Debug_Console_triggered();
};
#endif // MAINWINDOW_H
