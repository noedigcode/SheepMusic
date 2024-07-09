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

    ui->label_version->setText(QString("%1 %2").arg(APP_NAME).arg(APP_VERSION));
    ui->label_settingsLocation->setText(QSettings().fileName());

    // Set the icon size settings defaults to those specified in the GUI
    settings.iconsHorizontalSize.setDefaultValue(ui->toolBar_main->iconSize().width());
    settings.iconsVerticalSize.setDefaultValue(ui->toolBar_main->iconSize().height());
    updateToolbarIconSizeFromSettings();

    makeAllToolbarsCentered();

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

void MainWindow::makeAllToolbarsCentered()
{
    foreach (QToolBar* toolbar, allToolbars()) {
        // Insert expanding widgets at beginning and end
        QWidget* w1 = new QWidget(this);
        w1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        toolbar->insertWidget(toolbar->actions().value(0), w1);

        QWidget* w2 = new QWidget(this);
        w2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        toolbar->addWidget(w2);
    }
}

void MainWindow::showOnlyToolbar(QToolBar* toolbar)
{
    foreach (QToolBar* tb, allToolbars()) {
        tb->setVisible(tb == toolbar);
    }
}

QList<QToolBar*> MainWindow::allToolbars()
{
    return this->findChildren<QToolBar*>();
}

void MainWindow::updateToolbarIconSizeFromSettings()
{
    QSize size;
    size.setWidth(settings.iconsHorizontalSize.value().toInt());
    size.setHeight(settings.iconsVerticalSize.value().toInt());

    if (size.width() < minIconSize) {
        size.setWidth(minIconSize);
    }
    if (size.height() < minIconSize) {
        size.setHeight(minIconSize);
    }

    foreach (QToolBar* tb, allToolbars()) {
        tb->setIconSize(size);
    }
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
            QJsonArray jcurves = jpage.value("drawCurves").toArray();
            foreach (QJsonValue jvcurve, jcurves) {
                QJsonObject jcurve = jvcurve.toObject();
                DrawCurvePtr d(new DrawCurve());
                d->fromJson(jcurve);
                page->addDrawCurve(d);
            }

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
            if (!filepath.endsWith(mSessionExt)) { filepath.append(mSessionExt); }
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
    QString text;

    if (!mSessionFilepath.isEmpty()) {
        text = QFileInfo(mSessionFilepath).baseName();
    }
    if (currentDoc) {
        text += " - " + currentDoc->name;
    }
    if (mSessionModified) {
        text.append("*");
    }

    if (text.isEmpty()) {
        text = QString("%1 %2").arg(APP_NAME).arg(APP_VERSION);
    }

    setWindowTitle(text);
}

void MainWindow::showMainPagesView()
{
    ui->stackedWidget->setCurrentWidget(ui->page_main);
    showOnlyToolbar(ui->toolBar_main);
}

void MainWindow::showDocOrderView()
{
    ui->stackedWidget->setCurrentWidget(ui->page_orderDocs);
    showOnlyToolbar(ui->toolBar_order);

    // Select current document
    if (currentDoc) {
        int index = documents.indexOf(currentDoc);
        ui->listWidget_docs->setCurrentRow(index);
    }
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

void MainWindow::enableCropping(bool enable)
{
    mIsCropping = enable;

    if (!currentDoc) { return; }
    PageScenePtr page = currentDoc->pages.value(currentPage);
    if (!page) { return; }

    scaleScene();
}

void MainWindow::setDrawPen()
{
    mDrawMode = DrawMode::Pen;

    ui->action_Pen->setChecked(true);
    ui->action_Erase->setChecked(false);
}

void MainWindow::setDrawErase()
{
    mDrawMode = DrawMode::Erase;

    ui->action_Pen->setChecked(false);
    ui->action_Erase->setChecked(true);
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
    updateWindowTitle();
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

void MainWindow::removeDocAndShowOther(DocumentPtr doc)
{
    if (!doc) { return; }

    int docIndex = documents.indexOf(doc);
    documents.remove(doc);

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

            QJsonArray jcurves;
            foreach (DrawCurvePtr c, page->drawCurves()) {
                jcurves.append(c->toJson());
            }
            jpage.insert("drawCurves", jcurves);

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
    if (mIsCropping) { return; }
    if (mIsDrawing) { return; }

    QRectF rect = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->geometry()).boundingRect();
    if (pos.x() < rect.x() + rect.width()*0.5) {
        on_action_Previous_Page_triggered();
    } else if (pos.x() > rect.x() + rect.width()*0.5) {
        on_action_Next_Page_triggered();
    }
    return;
}

void MainWindow::onGraphicsViewLeftMouseDragStart(QPointF pos)
{
    mGraphicsViewLeftMouseDown = true;

    if (!currentDoc) { return; }
    PageScenePtr page = currentDoc->pages.value(currentPage);
    if (!page) { return; }

    if (mIsCropping) {

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

    } else if (mIsDrawing) {

        mDrawCurve.reset(new DrawCurve());
        mDrawCurve->addPoint(pos);

        if (mDrawMode == DrawMode::Pen) {
            page->addDrawCurve(mDrawCurve);
            setSessionModified(true);
        }

    }
}

void MainWindow::onGraphicsViewLeftMouseDrag(QPointF pos)
{
    if (!mGraphicsViewLeftMouseDown) { return; }

    if (!currentDoc) { return; }
    PageScenePtr page = currentDoc->pages.value(currentPage);
    if (!page) { return; }

    if (mIsCropping) {

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

    } else if (mIsDrawing) {

        mDrawCurve->addPoint(pos);

        if (mDrawMode == DrawMode::Pen) {
            setSessionModified(true);
        } else if (mDrawMode == DrawMode::Erase) {
            foreach (DrawCurvePtr c, page->drawCurves()) {
                if (mDrawCurve->intersects(c.data())) {
                    page->removeDrawCurve(c);
                    setSessionModified(true);
                }
            }
        }

    }
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
    enableCropping(true);
    showOnlyToolbar(ui->toolBar_crop);
    ui->stackedWidget->setCurrentWidget(ui->page_main);
}

void MainWindow::on_action_Add_Document_triggered()
{
    QStringList filepaths = QFileDialog::getOpenFileNames(this, "Add Document",
                                                     QString(), "PDF (*.pdf)");
    if (filepaths.isEmpty()) { return; }

    DocumentPtr docToView;

    foreach (QString filepath, filepaths) {
        DocumentPtr doc(new Document());
        doc->name = QFileInfo(filepath).baseName();
        doc->filepath = filepath;
        documents.append(doc);
        updateBreadcrumbs();
        loadPdf(doc);
        if (!docToView) { docToView = doc; }
    }

    viewPage(docToView, 0);
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

    removeDocAndShowOther(currentDoc);
}

void MainWindow::on_action_New_Session_triggered()
{
    if (!canSessionBeClosed()) { return; }

    clearSession();
}

void MainWindow::on_action_Order_Documents_triggered()
{
    showDocOrderView();
}

void MainWindow::on_stackedWidget_currentChanged(int /*arg1*/)
{
    ui->action_Settings->setChecked(
                ui->stackedWidget->currentWidget() == ui->page_settings);
}

void MainWindow::on_action_Draw_triggered()
{
    if (mIsCropping) {
        enableCropping(false);
    }

    mIsDrawing = true;
    showOnlyToolbar(ui->toolBar_draw);
    ui->stackedWidget->setCurrentWidget(ui->page_main);
    setDrawPen();
}

void MainWindow::on_action_Exit_Draw_Mode_triggered()
{
    mIsDrawing = false;
    showOnlyToolbar(ui->toolBar_main);
}

void MainWindow::on_action_Pen_triggered()
{
    setDrawPen();
}

void MainWindow::on_action_Erase_triggered()
{
    setDrawErase();
}

void MainWindow::Documents::append(DocumentPtr doc)
{
    documents.append(doc);
    mw->updateDocOrderList_appended(doc);
    mw->updateBreadcrumbs();
}

QList<MainWindow::DocumentPtr> MainWindow::Documents::all()
{
    return documents;
}

MainWindow::DocumentPtr MainWindow::Documents::value(int index)
{
    return documents.value(index);
}

void MainWindow::Documents::clear()
{
    documents.clear();
    mw->updateDocOrderList_cleared();
    mw->updateBreadcrumbs();
}

int MainWindow::Documents::count()
{
    return documents.count();
}

int MainWindow::Documents::indexOf(DocumentPtr doc)
{
    return documents.indexOf(doc);
}

void MainWindow::Documents::remove(DocumentPtr doc)
{
    int index = documents.indexOf(doc);
    if (index >= 0) {
        documents.removeAt(index);
        mw->updateDocOrderList_removed(index);
        mw->updateBreadcrumbs();
    }
}

void MainWindow::Documents::move(int from, int to)
{
    if ((from < 0) || (from >= documents.count())) { return; }
    // Wrap around to
    if (to < 0) { to = documents.count() - 1; }
    if (to >= documents.count()) { to = 0; }

    documents.move(from, to);
    mw->updateDocOrderList_moved(from, to);
    mw->updateBreadcrumbs();
}

void MainWindow::on_action_Exit_Order_Mode_triggered()
{
    // View the document currently selected in the order list
    int index = ui->listWidget_docs->currentRow();
    if (index >= 0) {
        DocumentPtr doc = documents.value(index);
        if (doc) {
            viewPage(doc, 0);
        }
    }

    showMainPagesView();
}

void MainWindow::on_action_Move_Doc_Up_triggered()
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

void MainWindow::on_action_Move_Doc_Down_triggered()
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

void MainWindow::on_action_Order_Remove_Document_triggered()
{
    int index = ui->listWidget_docs->currentRow();
    if (index < 0) { return; }

    DocumentPtr doc = documents.value(index);
    if (!doc) { return; }

    if (!msgBoxYesNo("Remove file", "Are you sure you want to remove the selected file?")) {
        return;
    }

    removeDocAndShowOther(doc);
}

void MainWindow::on_action_Exit_Crop_Mode_triggered()
{
    enableCropping(false);
    showOnlyToolbar(ui->toolBar_main);
}

void MainWindow::on_action_Settings_triggered()
{
    if (ui->action_Settings->isChecked()) {
        ui->stackedWidget->setCurrentWidget(ui->page_settings);
    } else {
        showMainPagesView();
    }
}

void MainWindow::on_toolButton_iconhsize_up_clicked()
{
    int s = qMax(settings.iconsHorizontalSize.value().toInt() + 2, minIconSize);
    settings.iconsHorizontalSize.set(s);
    updateToolbarIconSizeFromSettings();
}

void MainWindow::on_toolButton_ioconhsize_down_clicked()
{
    int s = qMax(settings.iconsHorizontalSize.value().toInt() - 2, minIconSize);
    settings.iconsHorizontalSize.set(s);
    updateToolbarIconSizeFromSettings();
}

void MainWindow::on_toolButton_iconvsize_up_clicked()
{
    int s = qMax(settings.iconsVerticalSize.value().toInt() + 2, minIconSize);
    settings.iconsVerticalSize.set(s);
    updateToolbarIconSizeFromSettings();
}

void MainWindow::on_toolButton_iconvsize_down_clicked()
{
    int s = qMax(settings.iconsVerticalSize.value().toInt() - 2, minIconSize);
    settings.iconsVerticalSize.set(s);
    updateToolbarIconSizeFromSettings();
}

