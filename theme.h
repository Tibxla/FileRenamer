#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QIcon>
#include <QString>

// Thème inspiré de Windows 11 : police, palette, feuille de style (gabarit :/theme.qss dont les
// jetons @nom prennent la valeur claire ou sombre) et icônes SVG teintées.
namespace Theme {

enum class Role {
    Text,
    Secondary,
    Accent,
    Success,
    Caution,
    Critical
};

// Applique le thème clair ou sombre à toute l'application ; peut être rappelé à chaud.
void apply(bool dark);
bool isDark();
QColor color(Role role);

// Icône :/icons/<name>.svg, dont « currentColor » prend la couleur du rôle au moment de la peindre :
// elle suit donc les changements de thème sans être recréée.
QIcon icon(const QString &name, Role role = Role::Secondary);

}

#endif
