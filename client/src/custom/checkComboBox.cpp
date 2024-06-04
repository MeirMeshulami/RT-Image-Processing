#include "CheckComboBox.h"
#include <QCoreApplication>
#include <QEvent>
#include <QMouseEvent>

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

	if (item->text() == "ALL") {
		bool isChecked = item->checkState() == Qt::Checked;
		for (int i = 0; i < listWidget->count(); ++i) {
			QListWidgetItem* listItem = listWidget->item(i);
			listItem->setCheckState(isChecked ? Qt::Unchecked : Qt::Checked);
			if (listItem->text() != "ALL") {
				emit checkedItemsChanged(listItem->text().toStdString(), isChecked);
			}
			QCoreApplication::processEvents();
		}
	}
	else {
		bool isChecked = item->checkState() == Qt::Checked;
		item->setCheckState(isChecked ? Qt::Unchecked : Qt::Checked);
		std::string classItem = item->text().toStdString();
		emit checkedItemsChanged(classItem, isChecked);
	}


}

bool CheckComboBox::eventFilter(QObject* obj, QEvent* event) {
	if (obj == listWidget && event->type() == QEvent::MouseButtonRelease) {
		return true;  // Filter out the mouse release event
	}

	// Pass the event on to the base class
	return QComboBox::eventFilter(obj, event);
}
