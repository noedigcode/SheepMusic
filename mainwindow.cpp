#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QPdfDocument>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFullscreen(settings.fullscreen.value().toBool());

    updateWindowTitle();

    showMainPagesView();
    setupBreadcrumbs();
    updateBreadcrumbs();
    setupGraphicsView();

    QString lastSession = settings.lastSession.string();
    if (!lastSession.isEmpty()) {
        openSession(lastSession);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openSession(QString filepath)
{
    // Clear current session
    clearSession();

    // Read file
    QByteArray data;
    QFile f(filepath);
    if (f.open(QIODevice::ReadOnly)) {
        data = f.readAll();
        f.close();
        print("Read session file " + filepath);
    } else {
        print(QString("Error opening file for reading: %1: %2")
              .arg(filepath)
              .arg(f.errorString()));
        QMessageBox::critical(this, "Open Error",
                "An error occurred and the session could not be opened.");
        return;
    }

    // Create documents from JSON
    QJsonDocument jin = QJsonDocument::fromJson(data);
    QJsonArray jdocs = jin.array();
    foreach (QJsonValue jval, jdocs) {
        QJsonObject jdoc = jval.toObject();
        DocumentPtr doc(new Document());
        doc->name = jdoc.value("name").toString();
        doc->filepath = jdoc.value("filepath").toString();
        QJsonArray jpages = jdoc.value("pages").toArray();
        foreach (QJsonValue jvpage, jpages) {
            QJsonObject jpage = jvpage.toObject();
            PageScenePtr page(new PageScene());
            page->setSelRect(jsonToRect(jpage.value("rect").toObject()));
            doc->pages.append(page);
            updateBreadcrumbs();
        }
        documents.append(doc);
    }

    // Load PDFs
    foreach (DocumentPtr doc, documents.all()) {
        loadPdf(doc);
    }

    viewPage(documents.value(0), 0);

    setSessionFilepath(filepath);
    settings.lastSession.set(filepath);
    setSessionModified(false);
}

bool MainWindow::saveSession()
{
    bool saved = false;
    if (!mSessionFilepath.isEmpty()) {
        // Try to save to existing file
        saved = writeSession(mSessionFilepath);
        if (!saved) {
            QMessageBox::critical(this, "Save Error",
                    "An error occurred and the session could not be saved to its current file path.");
        }
    }

    if (!saved) {
        // Try save-as
        QString filepath = QFileDialog::getSaveFileName(this, "Save Session",
                                                QString(), mSessionFileFilter);
        if (!filepath.isEmpty()) {
            saved = writeSession(filepath);
            if (saved) {
                setSessionFilepath(filepath);
            } else {
                QMessageBox::critical(this, "Save Error",
                        "An error occurred and the session could not be saved to the chosen file path.");
            }
        }
    }

    if (saved) {
        setSessionModified(false);
        settings.lastSession.set(mSessionFilepath);
    }

    return saved;
}

void MainWindow::setFullscreen(bool fullscreen)
{
    if (fullscreen) {
        setWindowState(Qt::WindowFullScreen);
    } else {
        setWindowState(Qt::WindowMaximized);
    }

    settings.fullscreen.set(fullscreen);

    ui->action_Fullscreen->setChecked(fullscreen);
}

bool MainWindow::msgBoxYesNo(QString title, QString text)
{
    return QMessageBox::question(this, title, text) == QMessageBox::Yes;
}

void MainWindow::updateWindowTitle()
{
    QString sessionText;
    if (!mSessionFilepath.isEmpty()) {
        sessionText = QFileInfo(mSessionFilepath).baseName();
        if (mSessionModified) {
            sessionText.append("*");
        }
        sessionText += " - ";
    }
    setWindowTitle(QString("%1%2 %3").arg(sessionText).arg(APP_NAME).arg(APP_VERSION));
}

void MainWindow::showMainPagesView()
{
    ui->stackedWidget->setCurrentWidget(ui->page_main);
}

void MainWindow::showDocOrderView()
{
    ui->stackedWidget->setCurrentWidget(ui->page_orderDocs);
}

void MainWindow::print(QString msg)
{
    ui->plainTextEdit->appendPlainText(msg);
}

void MainWindow::setupGraphicsView()
{
    //ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
    //ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Mouse event signals/slots
    connect(ui->graphicsView, &GraphicsView::leftClick,
            this, &MainWindow::onGraphicsViewLeftClick);
    connect(ui->graphicsView, &GraphicsView::leftMouseDragStart,
            this, &MainWindow::onGraphicsViewLeftMouseDragStart);
    connect(ui->graphicsView, &GraphicsView::leftMouseDrag,
            this, &MainWindow::onGraphicsViewLeftMouseDrag);
    connect(ui->graphicsView, &GraphicsView::leftMouseDragEnd,
            this, &MainWindow::onGraphicsViewLeftMouseDragEnd);
    connect(ui->graphicsView, &GraphicsView::resized,
            this, &MainWindow::onGraphicsViewResized);
}

void MainWindow::viewPage(DocumentPtr doc, int pageIndex)
{
    if (!doc) { return; }

    PageScenePtr page = doc->pages.value(pageIndex);
    if (!page) { return; }

    ui->graphicsView->setScene(page.data());
    QMetaObject::invokeMethod(this, &MainWindow::scaleScene, Qt::QueuedConnection);

    currentDoc = doc;
    currentPage = pageIndex;
    updateBreadcrumbs();
}

void MainWindow::scaleScene()
{
    if (!currentDoc) { return; }
    PageScenePtr page = currentDoc->pages.value(currentPage);
    if (!page) { return; }

    QRectF rect;
    if (mIsCropping) {
        rect = page->mPixmap->boundingRect();
    } else {
        rect = page->getSelRect();
    }

    page->showSelRect(mIsCropping);
    ui->graphicsView->fitInView(rect, Qt::KeepAspectRatio);
    ui->graphicsView->centerOn(rect.center());
}

QJsonObject MainWindow::rectToJson(QRectF rect)
{
    QJsonObject obj;
    obj.insert("xTopLeft", rect.topLeft().x());
    obj.insert("yTopLeft", rect.topLeft().y());
    obj.insert("xBotRight", rect.bottomRight().x());
    obj.insert("yBotRight", rect.bottomRight().y());
    return obj;
}

QRectF MainWindow::jsonToRect(QJsonObject obj)
{
    QRectF rect(
                QPointF(obj.value("xTopLeft").toDouble(),
                        obj.value("yTopLeft").toDouble()),
                QPointF(obj.value("xBotRight").toDouble(),
                        obj.value("yBotRight").toDouble()));
    return rect;
}

void MainWindow::clearSession()
{
    currentDoc.reset();
    currentPage = 0;
    documents.clear();
    ui->graphicsView->setScene(nullptr);
    updateBreadcrumbs();
    setSessionModified(false);
    setSessionFilepath("");
}

void MainWindow::loadPdf(DocumentPtr doc)
{
    if (!doc) { return; }

    QPdfDocument pdf;
    print("Loading " + doc->filepath);
    QPdfDocument::DocumentError error = pdf.load(doc->filepath);
    print(QString("Load result: %1").arg(QVariant::fromValue(error).toString()));

    print(QString("Pages: %1").arg(pdf.pageCount()));
    for (int i=0; i < pdf.pageCount(); i++) {
        QSizeF size = pdf.pageSize(i);
        print(QString("    %1: %2x%3")
              .arg(i)
              .arg(size.width()).arg(size.height()));

        QImage im = pdf.render(i, size.toSize() * 2);
        PageScenePtr page = doc->pages.value(i);
        if (!page) {
            print("Page doesn't exist, creating new");
            page.reset(new PageScene());
            doc->pages.append(page);
        }
        page->setImage(im);
    }
}

bool MainWindow::writeSession(QString filepath)
{
    QJsonArray jdocs;
    foreach (DocumentPtr doc, documents.all()) {
        QJsonObject jdoc;
        jdoc.insert("name", doc->name);
        jdoc.insert("filepath", doc->filepath);
        QJsonArray jpages;
        foreach (PageScenePtr page, doc->pages) {
            QJsonObject jpage;
            jpage.insert("rect", rectToJson(page->getSelRect()));
            jpages.append(jpage);
        }
        jdoc.insert("pages", jpages);
        jdocs.append(jdoc);
    }

    QJsonDocument jout;
    jout.setArray(jdocs);
    QByteArray json = jout.toJson();

    QFile f(filepath);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(json);
        f.close();
        print("Wrote session to file " + filepath);
        return true;
    } else {
        print(QString("Error opening file for writing: %1: %2")
              .arg(filepath)
              .arg(f.errorString()));
        return false;
    }
}

bool MainWindow::canSessionBeClosed()
{
    if (mSessionModified) {
        int button = QMessageBox::question(this, "Close Session",
                    "This session has been modified. Do you want to save changes?",
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (button == QMessageBox::Yes) {
            return saveSession();
        } else if (button == QMessageBox::No) {
            return true;
        } else {
            // Cancel
            return false;
        }
    }
    return true;
}

void MainWindow::setSessionFilepath(QString path)
{
    mSessionFilepath = path;
    updateWindowTitle();
}

void MainWindow::setSessionModified(bool modified)
{
    mSessionModified = modified;
    updateWindowTitle();
}

void MainWindow::updateDocOrderList_appended(DocumentPtr doc)
{
    ui->listWidget_docs->addItem(doc->name);
}

void MainWindow::updateDocOrderList_cleared()
{
    ui->listWidget_docs->clear();
}

void MainWindow::updateDocOrderList_removed(int index)
{
    QListWidgetItem* item = ui->listWidget_docs->item(index);
    if (item) { delete item; }
}

void MainWindow::updateDocOrderList_moved(int from, int to)
{
    QListWidgetItem* item = ui->listWidget_docs->takeItem(from);
    if (!item) { return; }

    ui->listWidget_docs->insertItem(to, item);
}

void MainWindow::setupBreadcrumbs()
{
    connect(ui->widget_docsBreadcrumbs, &BreadcrumbsWidget::breadcrumbClicked,
            this, [=](int index)
    {
        DocumentPtr doc = documents.value(index);
        if (!doc) { return; }
        viewPage(doc, 0);
    });

    connect(ui->widget_pagesBreadcrumbs, &BreadcrumbsWidget::breadcrumbClicked,
            this, [=](int index)
    {
        if (!currentDoc) { return; }
        viewPage(currentDoc, index);
    });
}

void MainWindow::updateBreadcrumbs()
{
    int pageCount = 0;
    if (currentDoc) {
        pageCount = currentDoc->pages.count();
    }

    ui->widget_docsBreadcrumbs->setBounds(
                documents.count(), documents.indexOf(currentDoc));

    ui->widget_pagesBreadcrumbs->setBounds(pageCount, currentPage);
}

void MainWindow::onGraphicsViewLeftClick(QPointF pos)
{
    if (!mIsCropping) {
        QRectF rect = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->geometry()).boundingRect();
        if (pos.x() < rect.x() + rect.width()*0.5) {
            on_action_Previous_Page_triggered();
        } else if (pos.x() > rect.x() + rect.width()*0.5) {
            on_action_Next_Page_triggered();
        }
        return;
    }
}

void MainWindow::onGraphicsViewLeftMouseDragStart(QPointF pos)
{
    mGraphicsViewLeftMouseDown = true;

    if (!mIsCropping) { return; }

    if (!currentDoc) { return; }
    PageScenePtr page = currentDoc->pages.value(currentPage);
    if (!page) { return; }

    QRectF selrect = page->getSelRect();

    int edge = 0; // left, right, top, bottom
    qreal dist = qAbs(pos.x() - selrect.left());

    qreal d2 = qAbs(pos.x() - selrect.right());
    if (d2 < dist) {
        dist = d2;
        edge = 1;
    }

    d2 = qAbs(pos.y() - selrect.top());
    if (d2 < dist) {
        dist = d2;
        edge = 2;
    }

    d2 = qAbs(pos.y() - selrect.bottom());
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

    if (!currentDoc) { return; }
    PageScenePtr page = currentDoc->pages.value(currentPage);
    if (!page) { return; }

    qreal dist;
    if (mSelrectEdge <= 1) {
        // Left/right
        dist = pos.x() - mSelStart.x();
    } else {
        // Up/down
        dist = pos.y() - mSelStart.y();
    }

    QRectF rect = page->getSelRect();
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
    page->setSelRect(rect);
    setSessionModified(true);

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

void MainWindow::on_action_Debug_Console_triggered()
{
    if (ui->action_Debug_Console->isChecked()) {
        ui->stackedWidget->setCurrentWidget(ui->page_console);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->page_main);
    }
}

MainWindow::PageScene::PageScene()
{
    setBackgroundBrush(QBrush(Qt::white));
}

void MainWindow::PageScene::setImage(QImage image)
{
    mPixmap = this->addPixmap(QPixmap::fromImage(image));
}

void MainWindow::PageScene::setSelRect(QRectF rect)
{
    if (!mSelrect) { initSelRect(); }
    mSelrect->setRect(rect);
}

QRectF MainWindow::PageScene::getSelRect()
{
    if (!mSelrect) { initSelRect(); }
    return mSelrect->rect();
}

void MainWindow::PageScene::showSelRect(bool show)
{
    if (show) {
        if (!mSelrect) { initSelRect(); }
        mSelrect->show();
    } else {
        if (mSelrect) { mSelrect->hide(); }
    }
}

void MainWindow::PageScene::initSelRect()
{
    QRectF rect;
    if (mPixmap) {
        rect = mPixmap->boundingRect();
    }
    mSelrect = new QGraphicsRectItem(rect);
    mSelrect->setPen(QPen(Qt::blue, 2)); // Set blue border
    QColor fillColor("#676cf5");
    fillColor.setAlphaF(0.5);
    mSelrect->setBrush(fillColor); // Set semi-transparent blue fill

    // Add the item to the scene
    this->addItem(mSelrect);
    mSelrect->setZValue(1);
    mSelrect->hide();
}

void MainWindow::on_action_Next_Page_triggered()
{
    if (!currentDoc) { return; }

    int ipage = currentPage + 1;
    if (ipage >= currentDoc->pages.count()) {
        // End of document. Go to next.
        DocumentPtr doc = documents.value(documents.indexOf(currentDoc) + 1);
        if (!doc) { return; }
        viewPage(doc, 0);
    } else {
        viewPage(currentDoc, ipage);
    }
}

void MainWindow::on_action_Previous_Page_triggered()
{
    if (!currentDoc) { return; }

    int ipage = currentPage - 1;
    if (ipage < 0) {
        // Start of document. Go to previous.
        DocumentPtr doc = documents.value(documents.indexOf(currentDoc) - 1);
        if (!doc) { return; }
        viewPage(doc, doc->pages.count() - 1);
    } else {
        viewPage(currentDoc, ipage);
    }
}

void MainWindow::on_action_Crop_triggered()
{
    mIsCropping = ui->action_Crop->isChecked();

    if (!currentDoc) { return; }
    PageScenePtr page = currentDoc->pages.value(currentPage);
    if (!page) { return; }

    scaleScene();
}

void MainWindow::on_action_Add_Document_triggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, "Add Document",
                                                    QString(), "PDF (*.pdf)");
    if (filepath.isEmpty()) { return; }

    DocumentPtr doc(new Document());
    doc->name = QFileInfo(filepath).baseName();
    doc->filepath = filepath;
    documents.append(doc);
    updateBreadcrumbs();

    loadPdf(doc);

    viewPage(doc, 0);
    setSessionModified(true);
}

void MainWindow::on_action_Save_Session_triggered()
{
    saveSession();
}

void MainWindow::on_action_Open_Session_triggered()
{
    if (!canSessionBeClosed()) { return; }

    QString filepath = QFileDialog::getOpenFileName(this, "Open Session",
                                                QString(), mSessionFileFilter);
    if (filepath.isEmpty()) { return; }

    openSession(filepath);
}

void MainWindow::on_action_Fullscreen_triggered()
{
    setFullscreen(ui->action_Fullscreen->isChecked());
}

void MainWindow::on_action_Quit_triggered()
{
    this->close();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (msgBoxYesNo("Quit", "Are you sure you want to quit?")) {
        if (canSessionBeClosed()) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->ignore();
    }
}

void MainWindow::on_action_Remove_Document_triggered()
{
    if (!currentDoc) { return; }

    if (!msgBoxYesNo("Remove file", "Are you sure you want to remove this file?")) {
        return;
    }

    int docIndex = documents.indexOf(currentDoc);
    documents.remove(currentDoc);

    if (documents.count()) {
        // Show previous doc
        if (docIndex > 0) { docIndex -= 1; }
        viewPage(documents.value(docIndex), 0);
    } else {
        currentDoc.reset();
        ui->graphicsView->setScene(nullptr);
        updateBreadcrumbs();
    }

    setSessionModified(true);
}

void MainWindow::on_action_New_Session_triggered()
{
    if (!canSessionBeClosed()) { return; }

    clearSession();
}

void MainWindow::on_action_Order_Documents_triggered()
{
    if (ui->action_Order_Documents->isChecked()) {
        showDocOrderView();
    } else {
        showMainPagesView();
    }
}

void MainWindow::on_stackedWidget_currentChanged(int /*arg1*/)
{
    QWidget* screen = ui->stackedWidget->currentWidget();

    ui->action_Debug_Console->setChecked(screen == ui->page_console);
    ui->action_Order_Documents->setChecked(screen == ui->page_orderDocs);

    bool enable = (screen == ui->page_main);
    ui->action_Crop->setEnabled(enable);
    ui->action_Next_Page->setEnabled(enable);
    ui->action_Previous_Page->setEnabled(enable);
    ui->action_Remove_Document->setEnabled(enable);
}

void MainWindow::on_toolButton_doc_down_clicked()
{
    QListWidgetItem* item = ui->listWidget_docs->currentItem();

    int from = ui->listWidget_docs->currentRow();
    if (from < 0) { return; }
    int to = from + 1;
    // Wrap around to
    if (to < 0) { to = documents.count() - 1; }
    if (to >= documents.count()) { to = 0; }

    documents.move(from, to);
    setSessionModified(true);

    // Keep moved item selected
    ui->listWidget_docs->setCurrentItem(item);
}

void MainWindow::on_toolButton_doc_up_clicked()
{
    QListWidgetItem* item = ui->listWidget_docs->currentItem();

    int from = ui->listWidget_docs->currentRow();
    if (from < 0) { return; }
    int to = from - 1;
    // Wrap around to
    if (to < 0) { to = documents.count() - 1; }
    if (to >= documents.count()) { to = 0; }

    documents.move(from, to);
    setSessionModified(true);

    // Keep moved item selected
    ui->listWidget_docs->setCurrentItem(item);
}

void MainWindow::on_toolButton_doc_view_clicked()
{
    int index = ui->listWidget_docs->currentRow();
    if (index < 0) { return; }

    DocumentPtr doc = documents.value(index);
    if (!doc) { return; }

    viewPage(doc, 0);
    showMainPagesView();
}

