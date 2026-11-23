#ifndef RENAMECORE_H
#define RENAMECORE_H

#include <QList>
#include <QStringList>

namespace RenameCore {

enum class SkipReason {
    NoNumber,
    AlreadyExists,
    DuplicateOutput
};

struct PlannedCopy {
    QString sourceName;
    QString outputName;
};

struct SkippedFile {
    QString sourceName;
    SkipReason reason;
};

struct Plan {
    QList<PlannedCopy> copies;
    QList<SkippedFile> skipped;
};

struct DirectoryPaths {
    QString absolutePath;
    QString canonicalPath;
};

QString extractNumber(const QString &fileName);
// Writes a numéro on at least three digits: "1" -> "001", "0001" -> "001", "1234" stays.
QString formatNumber(const QString &digits);
QString validateNomenclature(const QString &nomenclature);
QString normalizeNomenclature(const QString &nomenclature);

// Source names must already be sorted and the nomenclature validated.
Plan plan(const QStringList &sourceNames, const QStringList &existingNames,
          const QString &nomenclature);

QString previewText(const Plan &plan);

// The caller resolves absolute and, where available, canonical paths.
bool sameDirectory(const DirectoryPaths &source, const DirectoryPaths &destination);

}

#endif
