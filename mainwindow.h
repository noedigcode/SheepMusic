#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settings.h"
#include "version.h"

#include <QGraphicsScene>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMainWindow>
#include <QSharedPointer>

#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openSession(QString filepath);
    bool saveSession();
    void setFullscreen(bool fullscreen);

    bool msgBoxYesNo(QString title, QString text);

private:
    Ui::MainWindow *ui;

    void updateWindowTitle();

    void print(QString msg);

    Settings settings {"Noedigcode", "noedigcode.co.za", APP_NAME, APP_VERSION};
    void setupSettings();

    // -------------------------------------------------------------------------

    class PageScene : public QGraphicsScene
    {
    public:
        PageScene();

        QGraphicsPixmapItem* mPixmap = nullptr;
        void setImage(QImage image);

        void setSelRect(QRectF rect);
        QRectF getSelRect();
        void showSelRect(bool show);

    private:
        QGraphicsRectItem* mSelrect = nullptr;
        void initSelRect();
    };
    typedef QSharedPointer<PageScene> PageScenePtr;

    struct Document
    {
        QString name;
        QString filepath;
        QList<PageScenePtr> pages;
    };
    typedef QSharedPointer<Document> DocumentPtr;

    DocumentPtr currentDoc;
    int currentPage;

    QList<DocumentPtr> documents;

    void setupGraphicsView();
    bool mIsCropping = false;
    bool mGraphicsViewLeftMouseDown = false;
    int mSelrectEdge = 0;
    QPointF mSelStart;

    void viewPage(DocumentPtr doc, int pageIndex);
    void scaleScene();

    // -------------------------------------------------------------------------

    QJsonObject rectToJson(QRectF rect);
    QRectF jsonToRect(QJsonObject obj);

    void clearSession();
    void loadPdf(DocumentPtr doc);
    bool writeSession(QString filepath);
    bool canSessionBeClosed();

    QString mSessionFilepath;
    void setSessionFilepath(QString path);
    bool mSessionModified = false;
    void setSessionModified(bool modified);

    // -------------------------------------------------------------------------

    void setupBreadcrumbs();
    void updateBreadcrumbs();

private slots:
    void onGraphicsViewLeftClick(QPointF pos);
    void onGraphicsViewLeftMouseDragStart(QPointF pos);
    void onGraphicsViewLeftMouseDrag(QPointF pos);
    void onGraphicsViewLeftMouseDragEnd(QPointF pos);
    void onGraphicsViewResized();
    void on_action_Debug_Console_triggered();
    void on_action_Next_Page_triggered();
    void on_action_Previous_Page_triggered();
    void on_action_Crop_triggered();
    void on_action_Add_Document_triggered();
    void on_action_Save_Session_triggered();
    void on_action_Open_Session_triggered();
    void on_action_Fullscreen_triggered();
    void on_action_Quit_triggered();
    void on_action_Remove_Document_triggered();

protected:
    void closeEvent(QCloseEvent* event) override;
};
#endif // MAINWINDOW_H
