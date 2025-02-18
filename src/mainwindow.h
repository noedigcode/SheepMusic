#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "drawcurve.h"
#include "gidfile.h"
#include "pagescene.h"
#include "settings.h"
#include "version.h"

#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMainWindow>
#include <QPainterPath>
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

    void makeAllToolbarsCentered();
    void showOnlyToolbar(QToolBar* toolbar);
    QList<QToolBar*> allToolbars();

    const int minIconSize = 24;
    void updateToolbarIconSizeFromSettings();

    void openSession(QString filepath);
    bool saveSession();
    void setFullscreen(bool fullscreen);

    bool msgBoxYesNo(QString title, QString text);

private:
    Ui::MainWindow *ui;

    void updateWindowTitle();
    void updateAboutPage();
    QFont getMonospaceFont();

    void showMainPagesView();
    void showDocOrderView();

    void print(QString msg);

    Settings settings {"Noedigcode", "noedigcode.co.za", APP_NAME, APP_VERSION};
    void setupSettings();

    // -------------------------------------------------------------------------

    struct Document
    {
        QString name;
        QString filepath;
        QList<PageScenePtr> pages;
    };
    typedef QSharedPointer<Document> DocumentPtr;

    DocumentPtr currentDoc;
    int currentPage;

    class Documents
    {
    public:
        Documents(MainWindow* mw) : mw(mw) {}
        void append(DocumentPtr doc);
        QList<DocumentPtr> all();
        DocumentPtr value(int index);
        void clear();
        int count();
        int indexOf(DocumentPtr doc);
        void remove(DocumentPtr doc);
        void move(int from, int to);
    private:
        MainWindow* mw;
        QList<DocumentPtr> documents;
    };
    friend class Documents;

    Documents documents {this};

    void setupGraphicsView();
    bool mIsCropping = false;
    void enableCropping(bool enable);
    bool mGraphicsViewLeftMouseDown = false;
    int mSelrectEdge = 0;
    QPointF mSelStart;

    bool mIsDrawing = false;
    DrawCurvePtr mDrawCurve;
    enum class DrawMode { Pen, Erase} mDrawMode;
    void setDrawPen();
    void setDrawErase();

    bool mIsZooming = false;
    bool mIsZoomed = false;
    QPointF mZoomStart;
    void unZoom();
    void cancelZooming();

    void viewPage(DocumentPtr doc, int pageIndex);
    void scaleScene();

    void removeDocAndShowOther(DocumentPtr doc);

    // -------------------------------------------------------------------------

    QJsonObject rectToJson(QRectF rect);
    QRectF jsonToRect(QJsonObject obj);

    const QString mSessionExt = ".sheets";
    const QString mSessionFileFilter = "Sheet Sessions (*.sheets)";

    void clearSession();
    void loadPdf(DocumentPtr doc);
    bool writeSession(QString filepath);
    bool canSessionBeClosed();

    QString mSessionFilepath;
    void setSessionFilepath(QString path);
    bool mSessionModified = false;
    void setSessionModified(bool modified);

    // -------------------------------------------------------------------------

    void updateDocOrderList_appended(DocumentPtr doc);
    void updateDocOrderList_cleared();
    void updateDocOrderList_removed(int index);
    void updateDocOrderList_moved(int from, int to);
    void updateDocOrderListIndexes();

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
    void on_action_New_Session_triggered();
    void on_action_Order_Documents_triggered();
    void on_stackedWidget_currentChanged(int arg1);
    void on_action_Draw_triggered();
    void on_action_Exit_Draw_Mode_triggered();
    void on_action_Pen_triggered();
    void on_action_Erase_triggered();
    void on_action_Exit_Order_Mode_triggered();
    void on_action_Move_Doc_Up_triggered();
    void on_action_Move_Doc_Down_triggered();
    void on_action_Order_Remove_Document_triggered();
    void on_action_Exit_Crop_Mode_triggered();
    void on_action_Settings_triggered();
    void on_toolButton_iconhsize_up_clicked();
    void on_toolButton_ioconhsize_down_clicked();
    void on_toolButton_iconvsize_up_clicked();
    void on_toolButton_iconvsize_down_clicked();
    void on_action_Zoom_triggered();
    void on_pushButton_console_clicked();
    void on_pushButton_about_clicked();

protected:
    void closeEvent(QCloseEvent* event) override;
};
#endif // MAINWINDOW_H
