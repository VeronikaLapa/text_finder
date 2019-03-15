#include "indexer.h"
#include <set>
#include <unordered_set>
#include <QFile>
#include <QDir>
#include <QDirIterator>
using namespace std;
int const BUFFER_SIZE = 1024;

long int chars_to_trigram(char a, char b,char c) {
    return  (a << 16) + (b << 8) + c;
}
bool check_utf8(unsigned char a, unsigned char b) {
    if (a < 0x80)
        return b < 0x80 || (0xc0 <= b && b < 0xf8);
    if (a < 0xc0)
        return b < 0xf8;
    if (a < 0xf8)
        return 0x80 <= b && b < 0xc0;

    return false;
}
std::pair<bool, std::set<long int>> index_file(QFile* file){ // return (status, trigrams)
    std::set<long int> trigrams;
    char buffer[BUFFER_SIZE + 10];
    long long buffLength;
    int last_buf_size = 0;
    while(!file->atEnd()) {
        buffLength = file->read(&buffer[last_buf_size], sizeof(buffer));
        for (int i = 0; i < buffLength - 2; ++i) {
            if (!check_utf8(static_cast<unsigned char>(buffer[i]), static_cast<unsigned char>(buffer[i + 1])) ||
                    !check_utf8(static_cast<unsigned char>(buffer[i+1]), static_cast<unsigned char>(buffer[i + 2]))) {
                return {false,{}};
            }
            long int trig = chars_to_trigram(buffer[i], buffer[i+1], buffer[i+2]);
            trigrams.insert(trig);
        }
        last_buf_size = 2;
        for (int i = 0; i < last_buf_size; ++i) {
            buffer[i] = buffer[buffLength - last_buf_size - 1 + i];
        }
    }
    return {true, trigrams};
}

std::set<long int> string_to_trigrams(const string& s) {
    std::set<long int> trigrams;
    for (size_t i = 0; i < s.size() - 2; ++i) {
        long int trigr = chars_to_trigram(s[i], s[i+1], s[i+2]);
        trigrams.insert(trigr);
    }
    return trigrams;
}

bool check_index(std::set<long int>& file_trigrams, const std::set<long int> all_trigrams) {
    for (long int trigram: file_trigrams) {
        if (all_trigrams.count(trigram) == 0) {
            return false;
        }
    }
    return true;
}
bool check_string(QString& s, std::set<long int>& all_trigrams) {
    std::set<long int> ft = string_to_trigrams(s.toStdString());
    return check_index(ft, all_trigrams);
}
std::vector<std::pair<QString, std::set<long int>>> index_dir(QString file_path) {
    std::pair<bool, std::set<long int>> res = {false, {}};
    QFile* file = new QFile(file_path);
    if (file->open(QIODevice::ReadOnly)) {
        res = index_file(file);

    }
    delete file;
    if (res.first) {
        std::vector<std::pair<QString, std::set<long int>>>  result;
        result.push_back(std::pair<QString, std::set<long int>>({file_path, res.second}));
        return result;
    }
    return {};
}

std::vector<QString> get_all_files(QDir dir) {
    dir.setFilter(QDir::Files |
                          QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
    QDirIterator it(dir.absolutePath(), QDirIterator::Subdirectories);
    std::vector<QString> files;
    while (it.hasNext()) {
        files.push_back(QFile(it.next()).fileName());
    }
    return files;
}
