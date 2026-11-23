#include "renamecore.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QPair>
#include <QRegularExpression>
#include <QSet>

#include <algorithm>

namespace RenameCore {
namespace {

QRegularExpressionMatch lastNumberMatch(const QString &baseName)
{
    static const QRegularExpression digits(QStringLiteral("[0-9]+"));
    auto matches = digits.globalMatch(baseName);
    QRegularExpressionMatch number;
    while (matches.hasNext()) {
        number = matches.next();
    }
    return number;
}

struct SourceNumber {
    QString digits;
    qsizetype seriesIndex = -1;
};

qsizetype selectSeries(const QList<Series> &series, Plan &result)
{
    if (series.isEmpty()) {
        return -1;
    }

    const auto largest = std::max_element(series.cbegin(), series.cend(),
                                          [](const Series &left, const Series &right) {
        return left.size < right.size;
    });
    const qsizetype largestSize = largest->size;
    const bool tied = std::count_if(series.cbegin(), series.cend(), [largestSize](const Series &entry) {
        return entry.size == largestSize;
    }) > 1;

    qsizetype selected = -1;
    if (tied) {
        result.seriesStatus = SeriesStatus::Tie;
    } else {
        selected = largest - series.cbegin();
        result.seriesStatus = series.size() == 1 ? SeriesStatus::Single : SeriesStatus::Dominant;
        result.series = *largest;
    }

    for (qsizetype i = 0; i < series.size(); ++i) {
        const Series &entry = series.at(i);
        if (i != selected && (!tied || entry.size == largestSize)) {
            result.otherSeries.append(entry);
        }
    }
    std::sort(result.otherSeries.begin(), result.otherSeries.end(), [](const Series &left, const Series &right) {
        return left.size == right.size ? left.pattern < right.pattern : left.size > right.size;
    });
    return selected;
}

}

QString extractNumber(const QString &fileName)
{
    return lastNumberMatch(QFileInfo(fileName).completeBaseName()).captured();
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
          const QString &nomenclature, const QString &imposedPrefix)
{
    Plan result;
    const bool prefixImposed = !imposedPrefix.trimmed().isEmpty();
    QList<SourceNumber> sourceNumbers;
    qsizetype selectedSeries = -1;
    if (prefixImposed) {
        result.seriesStatus = SeriesStatus::PrefixImposed;
    } else {
        QList<Series> series;
        QHash<QPair<QString, QString>, qsizetype> seriesIndexes;
        sourceNumbers.reserve(sourceNames.size());
        for (const QString &sourceName : sourceNames) {
            const QString baseName = QFileInfo(sourceName).completeBaseName();
            const auto match = lastNumberMatch(baseName);
            if (!match.hasMatch()) {
                sourceNumbers.append(SourceNumber{});
                continue;
            }

            const QString before = baseName.left(match.capturedStart());
            const QString after = baseName.mid(match.capturedEnd());
            const auto key = qMakePair(before.toCaseFolded(), after.toCaseFolded());
            qsizetype index = seriesIndexes.value(key, -1);
            if (index == -1) {
                index = series.size();
                seriesIndexes.insert(key, index);
                series.append({before + QStringLiteral("…") + after, 0});
            }
            ++series[index].size;
            sourceNumbers.append({match.captured(), index});
        }
        selectedSeries = selectSeries(series, result);
    }

    const QString nameTemplate = normalizeNomenclature(nomenclature);
    QSet<QString> existing;
    for (const QString &name : existingNames) {
        existing.insert(name.toCaseFolded());
    }

    QSet<QString> planned;
    for (qsizetype i = 0; i < sourceNames.size(); ++i) {
        const QString &sourceName = sourceNames.at(i);
        if (prefixImposed && !sourceName.startsWith(imposedPrefix, Qt::CaseInsensitive)) {
            result.skipped.append({sourceName, SkipReason::PrefixMismatch});
            continue;
        }

        const QString number = prefixImposed ? extractNumber(sourceName) : sourceNumbers.at(i).digits;
        if (number.isEmpty()) {
            result.skipped.append({sourceName, SkipReason::NoNumber});
            continue;
        }
        if (!prefixImposed && sourceNumbers.at(i).seriesIndex != selectedSeries) {
            result.skipped.append({sourceName, SkipReason::OutsideSeries});
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
