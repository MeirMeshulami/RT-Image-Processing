#include "CheckComboBox.h"

CheckComboBox::CheckComboBox(QWidget *parent) : QComboBox(parent), listWidget(new QListWidget(this)) {
    setModel(listWidget->model());
    setView(listWidget);

    connect(listWidget, &QListWidget::itemPressed, this, &CheckComboBox::onItemChanged);
}

void CheckComboBox::addItem(const QString &text, const QVariant &data) {
    QListWidgetItem *item = new QListWidgetItem(text, listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    item->setData(Qt::UserRole, data);
}

void CheckComboBox::onItemChanged(QListWidgetItem *item) {

    bool isChecked = item->checkState() == Qt::Checked;
    item->setCheckState(isChecked ? Qt::Unchecked : Qt::Checked);
    this->setPlaceholderText("Choose objects..");
    std::string classItem = item->text().toStdString();

    emit checkedItemsChanged(classItem, isChecked);
}



