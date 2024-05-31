#include "CheckComboBox.h"
#include <QEvent>
#include <QMouseEvent>
#include <QCoreApplication>

CheckComboBox::CheckComboBox(QWidget* parent) : QComboBox(parent), listWidget(new QListWidget(this)) {
    setModel(listWidget->model());
    setView(listWidget);

    connect(listWidget, &QListWidget::itemPressed, this, &CheckComboBox::onItemChanged);
    this->view()->installEventFilter(this);

}

void CheckComboBox::addItem(const QString& text, const QVariant& data) {
    QListWidgetItem* item = new QListWidgetItem(text, listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    item->setData(Qt::UserRole, data);
}

void CheckComboBox::onItemChanged(QListWidgetItem* item) {

    bool isChecked = item->checkState() == Qt::Checked;
    if (item->text() == "ALL") {
        for (int i = 0; i < listWidget->count(); ++i) {
            QListWidgetItem* listItem = listWidget->item(i);
            listItem->setCheckState(isChecked ? Qt::Unchecked : Qt::Checked);
            if (listItem->text() != "ALL") {
                emit checkedItemsChanged(listItem->text().toStdString(), isChecked);
            }
            QCoreApplication::processEvents();
        }
    } else {
        item->setCheckState(isChecked ? Qt::Unchecked : Qt::Checked);
        isChecked = !isChecked; // Toggle checked state
    }

    std::string classItem = item->text().toStdString();
    emit checkedItemsChanged(classItem, isChecked);
}

bool CheckComboBox::eventFilter(QObject* obj, QEvent* event) {
    if (obj == listWidget && event->type() == QEvent::MouseButtonRelease) {
        return true;  // Filter out the mouse release event
    }

    // Pass the event on to the base class
    return QComboBox::eventFilter(obj, event);
}
