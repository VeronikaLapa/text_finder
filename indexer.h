#ifndef INDEXER_H
#define INDEXER_H
#include <set>
#include <QFile>
#include <QDir>
#include <vector>
std::pair<bool, std::set<int>> index_file(QFile file, const std::atomic_bool &run_st);
std::vector<QString> get_all_files(QDir dir);
//std::vector<std::pair<QString, std::set<long int>>> index_dir(QString file_path);
bool check_string(QString& s, std::set<long int>& all_trigrams);

void concat_list(std::vector<std::pair<QString, std::set<long int>>>& res, std::vector<std::pair<QString, std::set<long int>>> intermid);
struct index_function {
    index_function(const std::atomic_bool &run_st);

    std::vector<std::pair<QString, std::set<long int>>>  operator() (QString file_path);
    using result_type = std::vector<std::pair<QString, std::set<long int>>>;
    const std::atomic_bool &run_st;
};
#endif // INDEXER_H
