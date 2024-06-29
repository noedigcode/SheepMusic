#ifndef BREADCRUMBSWIDGET_H
#define BREADCRUMBSWIDGET_H

#include <QWidget>

class BreadcrumbsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BreadcrumbsWidget(QWidget *parent = nullptr);

    void setBounds(int count, int current);

signals:
    void breadcrumbClicked(int index);

private:
    int mCount = 0;
    int mCurrent = 0;

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // BREADCRUMBSWIDGET_H
