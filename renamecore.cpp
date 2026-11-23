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

QString formatNumber(const QString &digits)
{
    qsizetype firstSignificant = 0;
    while (firstSignificant < digits.size() - 1 && digits.at(firstSignificant) == QLatin1Char('0')) {
        ++firstSignificant;
    }
    const QString significant = digits.mid(firstSignificant);
    return significant.rightJustified(3, QLatin1Char('0'));
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
        outputName.replace(QStringLiteral("{n}"), formatNumber(number));
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

QString previewText(const Plan &plan)
{
    const qsizetype copyCount = plan.copies.size();
    QString count;
    if (copyCount == 0) {
        count = QStringLiteral("Aucun fichier à copier");
    } else if (copyCount == 1) {
        count = QStringLiteral("1 fichier à copier");
    } else {
        count = QStringLiteral("%1 fichiers à copier").arg(copyCount);
    }

    const qsizetype skippedCount = plan.skipped.size();
    if (skippedCount > 0) {
        const QString skipped = skippedCount == 1 ? QStringLiteral("1 ignoré")
                                                  : QStringLiteral("%1 ignorés").arg(skippedCount);
        count += QStringLiteral(", ") + skipped;
    }
    if (plan.copies.isEmpty()) {
        return count;
    }

    const auto &copy = plan.copies.constFirst();
    QString example = QStringLiteral("Exemple : %1 → %2").arg(copy.sourceName, copy.outputName);
    example.replace(QLatin1Char('\n'), QStringLiteral("\\n"))
           .replace(QLatin1Char('\r'), QStringLiteral("\\r"))
           .replace(QChar::LineSeparator, QStringLiteral("\\u2028"))
           .replace(QChar::ParagraphSeparator, QStringLiteral("\\u2029"));
    return example + QLatin1Char('\n') + count;
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
