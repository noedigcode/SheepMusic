#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
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
    QString filepath = QFileDialog::getOpenFileName(this);
    if (filepath.isEmpty()) { return; }

    ui->lineEdit_path->setText(filepath);

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

        QImage im = pdf.render(i, pdf.pageSize(0).toSize() * 2);
        PageScenePtr page(new PageScene());
        page->setImage(im);
        pages.append(page);
    }

    viewPage(1);
}

void MainWindow::print(QString msg)
{
    ui->plainTextEdit->appendPlainText(msg);
}

void MainWindow::setupGraphicsView()
{
    // Mouse event signals/slots
    connect(ui->graphicsView, &GraphicsView::leftMouseDragStart,
            this, &MainWindow::onGraphicsViewLeftMouseDragStart);
    connect(ui->graphicsView, &GraphicsView::leftMouseDrag,
            this, &MainWindow::onGraphicsViewLeftMouseDrag);
    connect(ui->graphicsView, &GraphicsView::leftMouseDragEnd,
            this, &MainWindow::onGraphicsViewLeftMouseDragEnd);
    connect(ui->graphicsView, &GraphicsView::resized,
            this, &MainWindow::onGraphicsViewResized);
}

void MainWindow::viewPage(int index)
{
    PageScenePtr page = pages.value(index);
    if (!page) { return; }

    ui->graphicsView->setScene(page.data());
    QMetaObject::invokeMethod(this, &MainWindow::scaleScene, Qt::QueuedConnection);
    currentPage = index;
}

void MainWindow::scaleScene()
{
    PageScenePtr page = pages.value(currentPage);
    if (!page) { return; }

    QRectF rect;
    if (mIsCropping) {
        rect = page->mPixmap->boundingRect();
    } else {
        rect = page->mSelrect->rect();
    }

    ui->graphicsView->fitInView(rect, Qt::KeepAspectRatio);
    ui->graphicsView->centerOn(rect.center());
}

void MainWindow::onGraphicsViewLeftMouseDragStart(QPointF pos)
{
    mGraphicsViewLeftMouseDown = true;

    PageScenePtr page = pages.value(currentPage);
    if (!page) { return; }

    int edge = 0; // left, right, top, bottom
    qreal dist = qAbs(pos.x() - page->mSelrect->rect().left());

    qreal d2 = qAbs(pos.x() - page->mSelrect->rect().right());
    if (d2 < dist) {
        dist = d2;
        edge = 1;
    }

    d2 = qAbs(pos.y() - page->mSelrect->rect().top());
    if (d2 < dist) {
        dist = d2;
        edge = 2;
    }

    d2 = qAbs(pos.y() - page->mSelrect->rect().bottom());
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

    PageScenePtr page = pages.value(currentPage);
    if (!page) { return; }

    qreal dist;
    if (mSelrectEdge <= 1) {
        // Left/right
        dist = pos.x() - mSelStart.x();
    } else {
        // Up/down
        dist = pos.y() - mSelStart.y();
    }

    QRectF rect = page->mSelrect->rect();
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
    page->mSelrect->setRect(rect);

    mSelStart = pos;
}

void MainWindow::onGraphicsViewLeftMouseDragEnd(QPointF /*pos*/)
{
    mGraphicsViewLeftMouseDown = false;
}

void MainWindow::onGraphicsViewResized()
{
    scaleScene();
}

void MainWindow::on_pushButton_crop_clicked()
{
    mIsCropping = ui->pushButton_crop->isChecked();

    PageScenePtr page = pages.value(currentPage);
    if (!page) { return; }

    page->mSelrect->setVisible(mIsCropping);

    scaleScene();
}

void MainWindow::on_action_Debug_Console_triggered()
{
    if (ui->action_Debug_Console->isChecked()) {
        ui->stackedWidget->setCurrentWidget(ui->page_console);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->page_main);
    }
}

void MainWindow::on_pushButton_next_clicked()
{
    viewPage(currentPage + 1);
}

void MainWindow::on_pushButton_prev_clicked()
{
    viewPage(currentPage - 1);
}


MainWindow::PageScene::PageScene()
{
    // Add selection box
    mSelrect = new QGraphicsRectItem(-50, -50, 100, 100);
    mSelrect->setPen(QPen(Qt::blue, 2)); // Set blue border
    QColor fillColor(Qt::blue);
    fillColor.setAlphaF(0.5);
    mSelrect->setBrush(fillColor); // Set semi-transparent blue fill

    // Add the item to the scene
    this->addItem(mSelrect);
    mSelrect->setZValue(1);
    mSelrect->hide();
}

void MainWindow::PageScene::setImage(QImage image)
{
    mPixmap = this->addPixmap(QPixmap::fromImage(image));
    mSelrect->setRect(mPixmap->boundingRect());
}
