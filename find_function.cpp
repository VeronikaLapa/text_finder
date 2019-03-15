#include "find_function.h"
#include "indexer.h"

#include <QTextStream>
find_function::find_function(QString text) {
    this->text = text;
}

std::set<std::pair<QString, std::set<std::pair<size_t,QString>>>> find_function::operator() (std::pair<QString, std::set<long int>> file) {
    std::string t = text.toStdString();
    if (check_string(text, file.second)) {
        std::set<std::pair<size_t,QString>> result = {};
        QFile inputFile(file.first);
        if (inputFile.open(QIODevice::ReadOnly))
        {
           QTextStream in(&inputFile);
           size_t line_num = 0;
           while (!in.atEnd()) {
              ++line_num;
              QString line = in.readLine();
              if (line.toStdString().find(t) != std::string::npos) {
                  result.insert({line_num, line});
              }
           }
           inputFile.close();
           if (result.size() > 0) {
               std::set<std::pair<QString, std::set<std::pair<size_t,QString>>>> ans;
               ans.insert(std::pair<QString, std::set<std::pair<size_t,QString>>>(file.first, result));
               return ans;
           }
        }
    }
    return {};
}
