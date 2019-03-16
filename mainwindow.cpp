#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTreeWidgetItem>
#include <QtConcurrent/QtConcurrent>
#include "indexer.h"
#include "find_function.h"
#include <unordered_map>
#include <unordered_set>
#include <experimental/algorithm>
#include <experimental/functional>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem*)));
    connect(ui->findButton, SIGNAL(clicked()), this, SLOT(find()));
    connect(ui->chooseButton, SIGNAL(clicked()), this, SLOT(index()));
    connect(ui->stringView, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(openFileInNotepad(QTreeWidgetItem*)));
    connect(&file_watcher, &QFileSystemWatcher::directoryChanged, this, &main_window::dir_changed);

    ui->status->setText("Choose directory");
    scan_directory(QDir::homePath());
}

void main_window::dir_changed() {
    indexed_files.clear();
    ui->status->setText("Chosen directory was changed. Please, rescan it");
}
void main_window::openFileInNotepad(QTreeWidgetItem* item) {
    if (item->childCount() > 0) {
        QProcess *proc = new QProcess(this);
        proc->start("notepad.exe " +  (item->data(0, Qt::UserRole)).toString());
    }
}



void main_window::index() {
    file_watcher.removePaths(file_watcher.files());
    QList<QTreeWidgetItem *> items = ui->treeWidget->selectedItems();
    QString fullFilePath;
    if (items.size() == 1) {
        QTreeWidgetItem * item = items[0];
        fullFilePath = (item->data(0,Qt::UserRole)).toString();
    } else {
        fullFilePath = crnt_dir;
    }
    if(QFileInfo(fullFilePath).isDir()) {
        std::vector<QString> files = get_all_files(QDir(fullFilePath));
        file_watcher.addPath(fullFilePath);
        for (QString str : files) {
            file_watcher.addPath(str);
        }
        QFutureWatcher<std::vector<std::pair<QString, std::set<long int>>>> watcher;
        QProgressDialog dialog;
        dialog.setLabelText(QString("Indexing..."));
        connect(&watcher, SIGNAL(finished()), &dialog, SLOT(reset()));
        connect(&dialog, SIGNAL(canceled()), this, SLOT(index_cancel()));
        connect(&watcher, SIGNAL(progressRangeChanged(int, int)), &dialog, SLOT(setRange(int, int)));
        connect(&watcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));
        index_st = true;
        index_function indf(std::ref(index_st));
        watcher.setFuture(QtConcurrent::mappedReduced(files, indf, concat_list));
        dialog.exec();
        watcher.waitForFinished();
        if (index_st) {
            index_st = false;
            indexed_files = watcher.result();
            ui->status->setText("Indexing done");
        } else {
            indexed_files = {};
            ui->status->setText("Indexing was canceled");
        }

    }
}

void main_window::index_cancel() {
    if (index_st) {
        index_st = false;
    }
}
/*
int boyerMoore(std::string& text, std::string& pattern) {
    int ans = 0;
    auto it = std::experimental::search(text.begin(), text.end(),std::experimental::boyer_moore_searcher(pattern.begin(), pattern.end()));
    while (it != text.end()) {
        ans++;
        it = std::experimental::search(it + 1, text.end(),std::experimental::boyer_moore_searcher(pattern.begin(), pattern.end()));
    }

    return ans;
}
*/




void main_window::find() {
    QString text = ui->lineEdit->text();
    ui->stringView->clear();
    if(text.size() < 3) {
        QMessageBox msgBox;
        msgBox.setText("Text should be longer than 3 symbols");
        msgBox.exec();
    } else if (indexed_files.size() == 0) {
        QMessageBox msgBox;
        msgBox.setText("You need to choose the directory");
        msgBox.exec();
    } else {
        QFutureWatcher<std::set<std::pair<QString, std::set<std::pair<size_t,QString>>>>> watcher;
        QProgressDialog dialog;
        dialog.setLabelText(QString("Finding..."));
        connect(&watcher, SIGNAL(finished()), &dialog, SLOT(reset()));
        connect(&dialog, SIGNAL(canceled()), this, SLOT(find_cancel()));
        connect(&watcher, SIGNAL(progressRangeChanged(int, int)), &dialog, SLOT(setRange(int, int)));
        connect(&watcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));
        find_st = true;
        find_function ff(text, std::ref(find_st));
        watcher.setFuture(QtConcurrent::mappedReduced(indexed_files, ff, concat_sets));
        dialog.exec();
        watcher.waitForFinished();
        if (find_st) {
            find_st = false;
            for (std::pair<QString, std::set<std::pair<size_t,QString>>> file_info : watcher.result()) {
                QFileInfo crnt_file(file_info.first);
                QTreeWidgetItem* parent_item = new QTreeWidgetItem(ui->stringView);
                parent_item->setText(0, crnt_file.fileName());
                parent_item->setData(0,  Qt::UserRole, file_info.first);
                parent_item->setText(1, crnt_file.filePath());
                for (std::pair<size_t,QString> line : file_info.second) {
                    QTreeWidgetItem* item = new QTreeWidgetItem(parent_item);
                    item->setText(1, line.second);
                    item->setText(0, QString::number(line.first));
                    parent_item->addChild(item);
                }
            }
        }
    }
}

void main_window::find_cancel() {
    if (find_st) {
        find_st = false;
    }
}

void main_window::onTreeItemClicked(QTreeWidgetItem* item) {
    QVariant file = item->data(0,Qt::UserRole);
    QString fullFilePath = file.toString();
    if(QFileInfo(fullFilePath).isDir()) {
        scan_directory(fullFilePath);
    }
}
main_window::~main_window()
{}

void main_window::select_directory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    scan_directory(dir);
}

void main_window::scan_directory(QString const& dir)
{
    crnt_dir = dir;
    ui->treeWidget->clear();

    setWindowTitle(QString("Directory Content - %1").arg(dir));
    QDir d(dir);
    QFileInfoList list = d.entryInfoList();
    for (QFileInfo file_info : list)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, file_info.fileName());
        item->setText(1, QString::number(file_info.size()));
        item->setData(0, Qt::UserRole, file_info.path()+"/"+file_info.fileName());
        ui->treeWidget->addTopLevelItem(item);
    }
}

void main_window::show_about_dialog()
{
    QMessageBox::aboutQt(this);
}
