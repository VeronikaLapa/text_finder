#ifndef INDEXER_H
#define INDEXER_H
#include <set>
#include <QFile>
#include <QDir>
#include <vector>
std::pair<bool, std::set<int>> index_file(QFile file);
std::vector<QString> get_all_files(QDir dir);
std::vector<std::pair<QString, std::set<long int>>> index_dir(QString file_path);
bool check_string(QString& s, std::set<long int>& all_trigrams);
#endif // INDEXER_H
