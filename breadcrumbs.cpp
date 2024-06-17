#include "breadcrumbs.h"

Breadcrumbs::Breadcrumbs(QObject *parent, QHBoxLayout* layout,
                         QWidget* placeholder, QString tag)
    : QObject{parent}, mLayout(layout), mPlaceholder(placeholder), mTag(tag)
{

}

void Breadcrumbs::setBounds(int count, int current)
{
    if (count) {
        if (mLayout->indexOf(mPlaceholder) != -1) {
            mPlaceholder->hide();
            mLayout->removeWidget(mPlaceholder);
        }
    } else {
        if (mLayout->indexOf(mPlaceholder) == -1) {
            mLayout->addWidget(mPlaceholder);
            mPlaceholder->show();
        }
    }

    // Remove excessive widgets
    for (int i = count; i < mLayout->count(); i++) {
        QPushButton* pb = buttons.value(i);
        if (!pb) { continue; }
        pb->hide();
        mLayout->removeWidget(pb);
    }

    // Add widgets
    for (int i = mLayout->count(); i < count; i++) {
        QPushButton* pb = buttons.value(i);
        if (!pb) {
            pb = new QPushButton();
            pb->setText(QString("%1 %2").arg(mTag).arg(i + 1));
            pb->setCheckable(true);
            buttons.append(pb);
            connect(pb, &QPushButton::clicked, this, [=]()
            {
                emit breadcrumbClicked(i);
            });
        }
        mLayout->addWidget(pb);
        pb->show();
    }

    for (int i=0; i < buttons.count(); i++) {
        QPushButton* pb = buttons.value(i);
        pb->setChecked(i == current);
    }
}
