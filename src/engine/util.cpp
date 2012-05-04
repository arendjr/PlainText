#include "util.h"


QString Util::joinFancy(const QStringList &list,
                        const QString &separator, const QString &last) {

    QString string;
    int length = list.length();
    for (int i = 0; i < length; i++) {
        string += list[i];
        if (i < length - 2) {
            string += separator;
        } else if (i == length - 2) {
            string += last;
        }
    }
    return string;
}

QString Util::fromIndex(const QStringList &list, int index, const QString &separator) {

    QString string;
    int length = list.length();
    for (int i = index; i < length; i++) {
        string += list[i];
        if (i < length - 1) {
            string += separator;
        }
    }
    return string;
}

QString Util::jsString(QString string) {

    return "\"" + string.replace('\\', "\\\\")
                        .replace('"', "\\\"")
                        .replace('\n', "\\n") + "\"";
}

static const char *colorMap[] = {
    "37;1", "37",
    "30;1", "30",
    "31;1", "31",
    "33;1", "33",
    "32;1", "32",
    "36;1", "36",
    "34;1", "34",
    "35;1", "35"
};

QString Util::colorize(const QString &string, Color color) {

    return "\\x1B[" + QString(colorMap[color]) + "m" + string + "\\x1B[0m";

}

Util::Util() {
}
