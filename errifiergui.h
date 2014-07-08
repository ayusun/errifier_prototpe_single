#define QT_NO_KEYWORDS
#ifndef ERRIFIERGUI_H
#define ERRIFIERGUI_H

#include <QMainWindow>
#include <QFileDialog>
#include "common.h"

#include <QtGui/QStandardItemModel>
#include <QPlainTextEdit>
namespace Ui {
class ErrifierGUI;
}

class ErrifierGUI : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ErrifierGUI(QWidget *parent = 0);
    ~ErrifierGUI();
    
private Q_SLOTS:
    void on_browse_clicked();

    void on_watch_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::ErrifierGUI *ui;
    QStandardItemModel *model;
    ThreadList threads;

};

#endif // ERRIFIERGUI_H
