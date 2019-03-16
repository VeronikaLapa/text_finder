#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QTreeWidget>
#include <set>
#include <QFileSystemWatcher>

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
    void dir_changed();
    void find_cancel();
    void index_cancel();
private:
    QFileSystemWatcher file_watcher;
    std::vector<std::pair<QString, std::set<long int>>> indexed_files;
    QString crnt_dir;
    std::unique_ptr<Ui::MainWindow> ui;
    std::atomic_bool find_st;
    std::atomic_bool index_st;
};

#endif // MAINWINDOW_H
