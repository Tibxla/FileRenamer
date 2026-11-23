#include "renamecore.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSet>

namespace RenameCore {

QString extractNumber(const QString &fileName)
{
    static const QRegularExpression digits(QStringLiteral("[0-9]+"));
    auto matches = digits.globalMatch(QFileInfo(fileName).completeBaseName());
    QString number;
    while (matches.hasNext()) {
        number = matches.next().captured();
    }
    return number;
}

QString validateNomenclature(const QString &nomenclature)
{
    if (nomenclature.isEmpty()) {
        return QCoreApplication::translate("RenameCore", "Saisissez une nomenclature.");
    }

    const QString forbidden = QStringLiteral("/\\:*?\"<>|");
    for (const QChar character : forbidden) {
        if (nomenclature.contains(character)) {
            return QCoreApplication::translate("RenameCore",
                                               "Le caractère « %1 » est interdit dans une nomenclature.")
                .arg(character);
        }
    }

    if (nomenclature.contains(QStringLiteral(".."))) {
        return QCoreApplication::translate("RenameCore",
                                           "Une nomenclature ne peut pas contenir deux points consécutifs.");
    }
    if (nomenclature.endsWith(QLatin1Char(' ')) || nomenclature.endsWith(QLatin1Char('.'))) {
        return QCoreApplication::translate("RenameCore",
                                           "Une nomenclature ne peut pas se terminer par un espace ou un point.");
    }
    return {};
}

QString normalizeNomenclature(const QString &nomenclature)
{
    return nomenclature.contains(QStringLiteral("{n}"))
        ? nomenclature : nomenclature + QStringLiteral("_{n}");
}

Plan plan(const QStringList &sourceNames, const QStringList &existingNames,
          const QString &nomenclature)
{
    Plan result;
    const QString nameTemplate = normalizeNomenclature(nomenclature);
    QSet<QString> existing;
    for (const QString &name : existingNames) {
        existing.insert(name.toCaseFolded());
    }

    QSet<QString> planned;
    for (const QString &sourceName : sourceNames) {
        const QString number = extractNumber(sourceName);
        if (number.isEmpty()) {
            result.skipped.append({sourceName, SkipReason::NoNumber});
            continue;
        }

        QString outputName = nameTemplate;
        outputName.replace(QStringLiteral("{n}"), number);
        const QString extension = QFileInfo(sourceName).suffix();
        if (!extension.isEmpty()) {
            outputName += QLatin1Char('.') + extension;
        }

        const QString outputKey = outputName.toCaseFolded();
        if (existing.contains(outputKey)) {
            result.skipped.append({sourceName, SkipReason::AlreadyExists});
        } else if (planned.contains(outputKey)) {
            result.skipped.append({sourceName, SkipReason::DuplicateOutput});
        } else {
            planned.insert(outputKey);
            result.copies.append({sourceName, outputName});
        }
    }
    return result;
}

bool sameDirectory(const DirectoryPaths &source, const DirectoryPaths &destination)
{
    const bool bothCanonical = !source.canonicalPath.isEmpty() && !destination.canonicalPath.isEmpty();
    const QString sourcePath = QDir::cleanPath(bothCanonical ? source.canonicalPath : source.absolutePath);
    const QString destinationPath = QDir::cleanPath(bothCanonical ? destination.canonicalPath
                                                                 : destination.absolutePath);
#ifdef Q_OS_WIN
    return sourcePath.compare(destinationPath, Qt::CaseInsensitive) == 0;
#else
    return sourcePath == destinationPath;
#endif
}

}
