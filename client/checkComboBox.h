#ifndef CHECKCOMBOBOX_H
#define CHECKCOMBOBOX_H

#include <QComboBox>
#include <QListWidget>

class CheckComboBox : public QComboBox {
    Q_OBJECT

public:
    explicit CheckComboBox(QWidget *parent = nullptr);
    void addItem(const QString &text, const QVariant &data = QVariant());
    QListWidget *listWidget;
signals:
    void checkedItemsChanged(std::string className, bool isChecked);

public slots:
    void onItemChanged(QListWidgetItem *item);

};

#endif // CHECKCOMBOBOX_H
