#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QTreeWidget>
#include <set>

namespace Ui {
class MainWindow;
}

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_window(QWidget *parent = 0);
    ~main_window();

private slots:
    void select_directory();
    void scan_directory(QString const& dir);
    void show_about_dialog();
    void onTreeItemClicked(QTreeWidgetItem*);
    void index();
    void find();
    void openFileInNotepad(QTreeWidgetItem*);
private:

   std::vector<std::pair<QString, std::set<long int>>> indexed_files;
    QString crnt_dir;
    std::unique_ptr<Ui::MainWindow> ui;
};

#endif // MAINWINDOW_H
