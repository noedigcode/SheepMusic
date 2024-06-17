#ifndef BREADCRUMBS_H
#define BREADCRUMBS_H

#include <QHBoxLayout>
#include <QObject>
#include <QPushButton>

class Breadcrumbs : public QObject
{
    Q_OBJECT
public:
    explicit Breadcrumbs(QObject *parent, QHBoxLayout* layout,
                         QWidget* placeholder, QString tag);

    void setBounds(int count, int current);

signals:
    void breadcrumbClicked(int index);

private:
    QHBoxLayout* mLayout = nullptr;
    QWidget* mPlaceholder = nullptr;
    QString mTag;
    QList<QPushButton*> buttons;
};

#endif // BREADCRUMBS_H
