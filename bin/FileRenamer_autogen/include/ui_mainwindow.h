/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_1;
    QGroupBox *groupBox;
    QFormLayout *renameFormLayout;
    QLabel *label;
    QHBoxLayout *sourceDirLayout;
    QLineEdit *sourceDirLineEdit;
    QPushButton *browseButton;
    QLabel *label_2;
    QLineEdit *baseNameLineEdit;
    QLabel *label_3;
    QHBoxLayout *folderNameLayout;
    QLineEdit *folderNameLineEdit;
    QPushButton *browseDestButton;
    QHBoxLayout *renameButtonLayout;
    QPushButton *renameButton;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox_2;
    QFormLayout *createPdfFormLayout;
    QLabel *label_4;
    QLineEdit *nomFichierLineEdit;
    QLabel *label_5;
    QHBoxLayout *dossierDestinationLayout;
    QLineEdit *dossierDestinationLineEdit;
    QPushButton *browseDossierDestinationButton;
    QLabel *label_6;
    QSpinBox *nombreFichiersSpinBox;
    QHBoxLayout *createPdfButtonLayout;
    QPushButton *createPdfButton;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_7;
    QSpinBox *digitCountSpinBox;
    QGroupBox *motifGroupBox;
    QVBoxLayout *verticalLayout_5;
    QRadioButton *noLetterRadioButton;
    QRadioButton *letterBeforeRadioButton;
    QLineEdit *letterBeforeLineEdit;
    QRadioButton *letterAfterRadioButton;
    QLineEdit *letterAfterLineEdit;
    QRadioButton *customRegexRadioButton;
    QLineEdit *customRegexLineEdit;
    QGroupBox *themeGroupBox;
    QVBoxLayout *verticalLayout_6;
    QRadioButton *darkThemeRadioButton;
    QRadioButton *lightThemeRadioButton;
    QCheckBox *virusSimulationCheckBox;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(600, 573);
        MainWindow->setMinimumSize(QSize(0, 0));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tab = new QWidget();
        tab->setObjectName("tab");
        verticalLayout_1 = new QVBoxLayout(tab);
        verticalLayout_1->setObjectName("verticalLayout_1");
        groupBox = new QGroupBox(tab);
        groupBox->setObjectName("groupBox");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        renameFormLayout = new QFormLayout(groupBox);
        renameFormLayout->setSpacing(2);
        renameFormLayout->setObjectName("renameFormLayout");
        renameFormLayout->setSizeConstraint(QLayout::SetMinimumSize);
        label = new QLabel(groupBox);
        label->setObjectName("label");

        renameFormLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        sourceDirLayout = new QHBoxLayout();
        sourceDirLayout->setObjectName("sourceDirLayout");
        sourceDirLineEdit = new QLineEdit(groupBox);
        sourceDirLineEdit->setObjectName("sourceDirLineEdit");
        sourceDirLineEdit->setMinimumSize(QSize(0, 0));

        sourceDirLayout->addWidget(sourceDirLineEdit);

        browseButton = new QPushButton(groupBox);
        browseButton->setObjectName("browseButton");

        sourceDirLayout->addWidget(browseButton);


        renameFormLayout->setLayout(0, QFormLayout::ItemRole::FieldRole, sourceDirLayout);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        renameFormLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label_2);

        baseNameLineEdit = new QLineEdit(groupBox);
        baseNameLineEdit->setObjectName("baseNameLineEdit");
        baseNameLineEdit->setMinimumSize(QSize(0, 0));

        renameFormLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, baseNameLineEdit);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");

        renameFormLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label_3);

        folderNameLayout = new QHBoxLayout();
        folderNameLayout->setObjectName("folderNameLayout");
        folderNameLineEdit = new QLineEdit(groupBox);
        folderNameLineEdit->setObjectName("folderNameLineEdit");
        folderNameLineEdit->setMinimumSize(QSize(0, 0));

        folderNameLayout->addWidget(folderNameLineEdit);

        browseDestButton = new QPushButton(groupBox);
        browseDestButton->setObjectName("browseDestButton");

        folderNameLayout->addWidget(browseDestButton);


        renameFormLayout->setLayout(2, QFormLayout::ItemRole::FieldRole, folderNameLayout);


        verticalLayout_1->addWidget(groupBox);

        renameButtonLayout = new QHBoxLayout();
        renameButtonLayout->setObjectName("renameButtonLayout");
        renameButton = new QPushButton(tab);
        renameButton->setObjectName("renameButton");

        renameButtonLayout->addWidget(renameButton);


        verticalLayout_1->addLayout(renameButtonLayout);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        verticalLayout_2 = new QVBoxLayout(tab_2);
        verticalLayout_2->setObjectName("verticalLayout_2");
        groupBox_2 = new QGroupBox(tab_2);
        groupBox_2->setObjectName("groupBox_2");
        createPdfFormLayout = new QFormLayout(groupBox_2);
        createPdfFormLayout->setSpacing(2);
        createPdfFormLayout->setObjectName("createPdfFormLayout");
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName("label_4");

        createPdfFormLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label_4);

        nomFichierLineEdit = new QLineEdit(groupBox_2);
        nomFichierLineEdit->setObjectName("nomFichierLineEdit");
        nomFichierLineEdit->setMinimumSize(QSize(0, 0));

        createPdfFormLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, nomFichierLineEdit);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName("label_5");

        createPdfFormLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label_5);

        dossierDestinationLayout = new QHBoxLayout();
        dossierDestinationLayout->setObjectName("dossierDestinationLayout");
        dossierDestinationLineEdit = new QLineEdit(groupBox_2);
        dossierDestinationLineEdit->setObjectName("dossierDestinationLineEdit");
        dossierDestinationLineEdit->setMinimumSize(QSize(0, 0));

        dossierDestinationLayout->addWidget(dossierDestinationLineEdit);

        browseDossierDestinationButton = new QPushButton(groupBox_2);
        browseDossierDestinationButton->setObjectName("browseDossierDestinationButton");

        dossierDestinationLayout->addWidget(browseDossierDestinationButton);


        createPdfFormLayout->setLayout(1, QFormLayout::ItemRole::FieldRole, dossierDestinationLayout);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName("label_6");

        createPdfFormLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label_6);

        nombreFichiersSpinBox = new QSpinBox(groupBox_2);
        nombreFichiersSpinBox->setObjectName("nombreFichiersSpinBox");
        nombreFichiersSpinBox->setMinimumSize(QSize(0, 0));
        nombreFichiersSpinBox->setMinimum(1);
        nombreFichiersSpinBox->setMaximum(10000);
        nombreFichiersSpinBox->setValue(1);

        createPdfFormLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, nombreFichiersSpinBox);


        verticalLayout_2->addWidget(groupBox_2);

        createPdfButtonLayout = new QHBoxLayout();
        createPdfButtonLayout->setObjectName("createPdfButtonLayout");
        createPdfButton = new QPushButton(tab_2);
        createPdfButton->setObjectName("createPdfButton");

        createPdfButtonLayout->addWidget(createPdfButton);


        verticalLayout_2->addLayout(createPdfButtonLayout);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName("tab_3");
        verticalLayout_3 = new QVBoxLayout(tab_3);
        verticalLayout_3->setObjectName("verticalLayout_3");
        groupBox_3 = new QGroupBox(tab_3);
        groupBox_3->setObjectName("groupBox_3");
        verticalLayout_4 = new QVBoxLayout(groupBox_3);
        verticalLayout_4->setObjectName("verticalLayout_4");
        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName("label_7");

        verticalLayout_4->addWidget(label_7);

        digitCountSpinBox = new QSpinBox(groupBox_3);
        digitCountSpinBox->setObjectName("digitCountSpinBox");
        digitCountSpinBox->setMinimumSize(QSize(0, 0));
        digitCountSpinBox->setMinimum(1);
        digitCountSpinBox->setValue(4);

        verticalLayout_4->addWidget(digitCountSpinBox);

        motifGroupBox = new QGroupBox(groupBox_3);
        motifGroupBox->setObjectName("motifGroupBox");
        verticalLayout_5 = new QVBoxLayout(motifGroupBox);
        verticalLayout_5->setObjectName("verticalLayout_5");
        noLetterRadioButton = new QRadioButton(motifGroupBox);
        noLetterRadioButton->setObjectName("noLetterRadioButton");
        noLetterRadioButton->setChecked(true);

        verticalLayout_5->addWidget(noLetterRadioButton);

        letterBeforeRadioButton = new QRadioButton(motifGroupBox);
        letterBeforeRadioButton->setObjectName("letterBeforeRadioButton");

        verticalLayout_5->addWidget(letterBeforeRadioButton);

        letterBeforeLineEdit = new QLineEdit(motifGroupBox);
        letterBeforeLineEdit->setObjectName("letterBeforeLineEdit");
        letterBeforeLineEdit->setEnabled(false);
        letterBeforeLineEdit->setFixedHeight(25);
        letterBeforeLineEdit->setMinimumSize(QSize(0, 0));

        verticalLayout_5->addWidget(letterBeforeLineEdit);

        letterAfterRadioButton = new QRadioButton(motifGroupBox);
        letterAfterRadioButton->setObjectName("letterAfterRadioButton");

        verticalLayout_5->addWidget(letterAfterRadioButton);

        letterAfterLineEdit = new QLineEdit(motifGroupBox);
        letterAfterLineEdit->setObjectName("letterAfterLineEdit");
        letterAfterLineEdit->setEnabled(false);
        letterAfterLineEdit->setFixedHeight(25);
        letterAfterLineEdit->setMinimumSize(QSize(0, 0));

        verticalLayout_5->addWidget(letterAfterLineEdit);

        customRegexRadioButton = new QRadioButton(motifGroupBox);
        customRegexRadioButton->setObjectName("customRegexRadioButton");

        verticalLayout_5->addWidget(customRegexRadioButton);

        customRegexLineEdit = new QLineEdit(motifGroupBox);
        customRegexLineEdit->setObjectName("customRegexLineEdit");
        customRegexLineEdit->setEnabled(false);
        customRegexLineEdit->setFixedHeight(25);
        customRegexLineEdit->setMinimumSize(QSize(0, 0));

        verticalLayout_5->addWidget(customRegexLineEdit);


        verticalLayout_4->addWidget(motifGroupBox);

        themeGroupBox = new QGroupBox(groupBox_3);
        themeGroupBox->setObjectName("themeGroupBox");
        verticalLayout_6 = new QVBoxLayout(themeGroupBox);
        verticalLayout_6->setObjectName("verticalLayout_6");
        darkThemeRadioButton = new QRadioButton(themeGroupBox);
        darkThemeRadioButton->setObjectName("darkThemeRadioButton");
        darkThemeRadioButton->setChecked(true);

        verticalLayout_6->addWidget(darkThemeRadioButton);

        lightThemeRadioButton = new QRadioButton(themeGroupBox);
        lightThemeRadioButton->setObjectName("lightThemeRadioButton");

        verticalLayout_6->addWidget(lightThemeRadioButton);


        verticalLayout_4->addWidget(themeGroupBox);

        virusSimulationCheckBox = new QCheckBox(groupBox_3);
        virusSimulationCheckBox->setObjectName("virusSimulationCheckBox");
        virusSimulationCheckBox->setChecked(true);

        verticalLayout_4->addWidget(virusSimulationCheckBox);


        verticalLayout_3->addWidget(groupBox_3);

        tabWidget->addTab(tab_3, QString());

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 600, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Renommer Fichiers", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Dossier Source", nullptr));
        browseButton->setText(QCoreApplication::translate("MainWindow", "Parcourir", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Nouveau Nom de Base", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Nouveau Dossier", nullptr));
        browseDestButton->setText(QCoreApplication::translate("MainWindow", "Parcourir", nullptr));
        renameButton->setText(QCoreApplication::translate("MainWindow", "Renommer", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "Renommer", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "Cr\303\251er Fichiers PDF", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Nom de base des fichiers", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Dossier de destination", nullptr));
        browseDossierDestinationButton->setText(QCoreApplication::translate("MainWindow", "Parcourir", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Nombre de fichiers (1-10000)", nullptr));
        createPdfButton->setText(QCoreApplication::translate("MainWindow", "Cr\303\251er Fichiers PDF", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "Cr\303\251er PDF", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "Param\303\250tres", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Nombre de chiffres pour la num\303\251rotation :", nullptr));
        motifGroupBox->setTitle(QCoreApplication::translate("MainWindow", "Configuration du motif", nullptr));
        noLetterRadioButton->setText(QCoreApplication::translate("MainWindow", "Pas de lettre (ex: paris_004.pdf)", nullptr));
        letterBeforeRadioButton->setText(QCoreApplication::translate("MainWindow", "Lettre avant le nombre (ex: paris_P004.pdf)", nullptr));
        letterBeforeLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "ex: P", nullptr));
        letterAfterRadioButton->setText(QCoreApplication::translate("MainWindow", "Lettre apr\303\250s le nombre (ex: paris_004P.pdf)", nullptr));
        letterAfterLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "ex: P", nullptr));
        customRegexRadioButton->setText(QCoreApplication::translate("MainWindow", "Regex personnalis\303\251", nullptr));
        themeGroupBox->setTitle(QCoreApplication::translate("MainWindow", "Th\303\250me", nullptr));
        darkThemeRadioButton->setText(QCoreApplication::translate("MainWindow", "Sombre", nullptr));
        lightThemeRadioButton->setText(QCoreApplication::translate("MainWindow", "Clair", nullptr));
        virusSimulationCheckBox->setText(QCoreApplication::translate("MainWindow", "Activer simulation virus", nullptr));
        virusSimulationCheckBox->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 6px;", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("MainWindow", "Param\303\250tres", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
