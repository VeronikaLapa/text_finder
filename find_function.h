#ifndef FIND_FUNCTION_H
#define FIND_FUNCTION_H

#include <QString>
#include <set>

void concat_sets( std::set<std::pair<QString, std::set<std::pair<size_t,QString>>>>& res, const std::set<std::pair<QString, std::set<std::pair<size_t,QString>>>> intermid);
struct find_function {
    QString text;

    find_function(const QString& text, const std::atomic_bool &run_st);

    std::set<std::pair<QString, std::set<std::pair<size_t,QString>>>> operator() (std::pair<QString, std::set<long int>> file);
    using result_type = std::set<std::pair<QString, std::set<std::pair<size_t,QString>>>>;
    const std::atomic_bool &run_st;
};
#endif // FIND_FUNCTION_H
