#ifndef FIND_FUNCTION_H
#define FIND_FUNCTION_H

#include <QString>
#include <set>


struct find_function {
    QString text;

    find_function(QString text);

    std::set<std::pair<QString, std::set<std::pair<size_t,QString>>>> operator() (std::pair<QString, std::set<long int>> file);
    using result_type = std::set<std::pair<QString, std::set<std::pair<size_t,QString>>>>;
};
#endif // FIND_FUNCTION_H
