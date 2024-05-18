#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPdfDocument>
#include <QGraphicsPixmapItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->page_main);

    setupGraphicsView();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_load_clicked()
{
    QString filepath = ui->lineEdit_path->text();

    QPdfDocument pdf;
    print("Loading " + filepath);
    QPdfDocument::DocumentError error = pdf.load(filepath);
    print(QString("Load result: %1").arg(QVariant::fromValue(error).toString()));

    print(QString("Pages: %1").arg(pdf.pageCount()));
    for (int i=0; i < pdf.pageCount(); i++) {
        QSizeF size = pdf.pageSize(i);
        print(QString("    %1: %2x%3")
              .arg(i)
              .arg(size.width()).arg(size.height()));
    }

    QImage im = pdf.render(0, pdf.pageSize(0).toSize() * 2);
    QGraphicsPixmapItem *p = mScene->addPixmap(QPixmap::fromImage(im));

    mSelrect->setRect(p->boundingRect());

    ui->graphicsView->fitInView(p, Qt::KeepAspectRatio);
}

void MainWindow::print(QString msg)
{
    ui->plainTextEdit->appendPlainText(msg);
}

void MainWindow::setupGraphicsView()
{
    mScene = new QGraphicsScene();
    ui->graphicsView->setScene(mScene);

    // Mouse event signals/slots
    connect(ui->graphicsView, &GraphicsView::leftMouseDragStart,
            this, &MainWindow::onGraphicsViewLeftMouseDragStart);
    connect(ui->graphicsView, &GraphicsView::leftMouseDrag,
            this, &MainWindow::onGraphicsViewLeftMouseDrag);
    connect(ui->graphicsView, &GraphicsView::leftMouseDragEnd,
            this, &MainWindow::onGraphicsViewLeftMouseDragEnd);

    // Add selection box
    mSelrect = new QGraphicsRectItem(-50, -50, 100, 100);
    mSelrect->setPen(QPen(Qt::blue, 2)); // Set blue border
    QColor fillColor(Qt::blue);
    fillColor.setAlphaF(0.5);
    mSelrect->setBrush(fillColor); // Set semi-transparent blue fill

    // Add the item to the scene
    mScene->addItem(mSelrect);
    mSelrect->hide();
}

void MainWindow::onGraphicsViewLeftMouseDragStart(QPointF pos)
{
    mGraphicsViewLeftMouseDown = true;

    int edge = 0; // left, right, top, bottom
    qreal dist = qAbs(pos.x() - mSelrect->rect().left());

    qreal d2 = qAbs(pos.x() - mSelrect->rect().right());
    if (d2 < dist) {
        dist = d2;
        edge = 1;
    }

    d2 = qAbs(pos.y() - mSelrect->rect().top());
    if (d2 < dist) {
        dist = d2;
        edge = 2;
    }

    d2 = qAbs(pos.y() - mSelrect->rect().bottom());
    if (d2 < dist) {
        dist = d2;
        edge = 3;
    }

    mSelrectEdge = edge;
    mSelStart = pos;
}

void MainWindow::onGraphicsViewLeftMouseDrag(QPointF pos)
{
    if (!mGraphicsViewLeftMouseDown) { return; }
    if (!mIsCropping) { return; }

    qreal dist;
    if (mSelrectEdge <= 1) {
        // Left/right
        dist = pos.x() - mSelStart.x();
    } else {
        // Up/down
        dist = pos.y() - mSelStart.y();
    }

    QRectF rect = mSelrect->rect();
    switch (mSelrectEdge) {
    case 0:
        rect.setLeft(rect.left() + dist);
        break;
    case 1:
        rect.setRight(rect.right() + dist);
        break;
    case 2:
        rect.setTop(rect.top() + dist);
        break;
    case 3:
        rect.setBottom(rect.bottom() + dist);
        break;
    }
    mSelrect->setRect(rect);

    mSelStart = pos;
}

void MainWindow::onGraphicsViewLeftMouseDragEnd(QPointF /*pos*/)
{
    mGraphicsViewLeftMouseDown = false;
}


void MainWindow::on_pushButton_crop_clicked()
{
    mIsCropping = ui->pushButton_crop->isChecked();

    mSelrect->setVisible(mIsCropping);

    if (mIsCropping) {
        ui->graphicsView->fitInView(mScene->sceneRect(), Qt::KeepAspectRatio);
    } else {
        ui->graphicsView->fitInView(mSelrect->rect(), Qt::KeepAspectRatio);
    }
}


void MainWindow::on_action_Debug_Console_triggered()
{
    if (ui->action_Debug_Console->isChecked()) {
        ui->stackedWidget->setCurrentWidget(ui->page_console);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->page_main);
    }
}

