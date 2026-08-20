#include "SettingsDialog.h"
#include "ArrowComboBox.h"

#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace {
    QFrame* makeSectionCard(QWidget* parent) {
        auto* card = new QFrame(parent);
        card->setObjectName("settingsCard");
        return card;
    }

    QLabel* makeSectionTitle(const QString& text, QWidget* parent) {
        auto* lbl = new QLabel(text, parent);
        lbl->setObjectName("settingsSectionTitle");
        return lbl;
    }
}

SettingsDialog::SettingsDialog(const AppSettings& initial, QWidget* parent) : QDialog(parent),
    m_settings(initial) {
    setObjectName("settingsDialog");
    setWindowTitle(tr("Settings"));
    setModal(true);
    setMinimumWidth(420);
    setFixedWidth(460);

    buildUi();
    setControlsFromSettings();
}

void SettingsDialog::buildUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(22, 20, 22, 20);
    root->setSpacing(14);

    // Header
    auto* headerRow = new QHBoxLayout();
    headerRow->setContentsMargins(0, 0, 0, 0);
    headerRow->setSpacing(8);

    auto* titleIcon = new QLabel(QStringLiteral("\u2699"), this);
    titleIcon->setObjectName("settingsTitleIcon");
    headerRow->addWidget(titleIcon);

    auto* titleLbl = new QLabel(tr("Settings"), this);
    titleLbl->setObjectName("settingsTitle");
    headerRow->addWidget(titleLbl);
    headerRow->addStretch(1);

    auto* closeBtn = new QPushButton(QStringLiteral("\u2715"), this);
    closeBtn->setObjectName("settingsCloseBtn");
    closeBtn->setFixedSize(28, 28);
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    headerRow->addWidget(closeBtn);

    root->addLayout(headerRow);

    auto* subtitle = new QLabel(tr("Customize the look and language of the launcher."), this);
    subtitle->setObjectName("settingsSubtitle");
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    root->addSpacing(4);

    // ---------- Language ----------
    auto* langCard = makeSectionCard(this);
    auto* langLay  = new QVBoxLayout(langCard);
    langLay->setContentsMargins(16, 14, 16, 14);
    langLay->setSpacing(10);

    langLay->addWidget(makeSectionTitle(tr("Language"), langCard));

    m_languageCombo = new ArrowComboBox(langCard);
    m_languageCombo->setObjectName("settingsLangCombo");
    m_languageCombo->addItem(QStringLiteral("English"));
    m_languageCombo->setCursor(Qt::PointingHandCursor);
    langLay->addWidget(m_languageCombo);

    auto* langHint = new QLabel(tr("More languages coming soon."), langCard);
    langHint->setObjectName("settingsHint");
    langLay->addWidget(langHint);

    root->addWidget(langCard);

    // ---------- Font size ----------
    auto* fontCard = makeSectionCard(this);
    auto* fontLay  = new QVBoxLayout(fontCard);
    fontLay->setContentsMargins(16, 14, 16, 14);
    fontLay->setSpacing(10);

    auto* fontHeader = new QHBoxLayout();
    fontHeader->setContentsMargins(0, 0, 0, 0);
    fontHeader->setSpacing(8);
    fontHeader->addWidget(makeSectionTitle(tr("Text size"), fontCard));
    fontHeader->addStretch(1);
    m_fontValueLbl = new QLabel(fontCard);
    m_fontValueLbl->setObjectName("settingsValueLbl");
    fontHeader->addWidget(m_fontValueLbl);
    fontLay->addLayout(fontHeader);

    m_fontSlider = new QSlider(Qt::Horizontal, fontCard);
    m_fontSlider->setObjectName("settingsSlider");
    m_fontSlider->setRange(70, 150);
    m_fontSlider->setSingleStep(5);
    m_fontSlider->setPageStep(10);
    m_fontSlider->setCursor(Qt::PointingHandCursor);
    fontLay->addWidget(m_fontSlider);

    auto* fontHint = new QLabel(tr("Scales all text in the launcher."), fontCard);
    fontHint->setObjectName("settingsHint");
    fontLay->addWidget(fontHint);

    root->addWidget(fontCard);

    // ---------- UI size ----------
    auto* uiCard = makeSectionCard(this);
    auto* uiLay  = new QVBoxLayout(uiCard);
    uiLay->setContentsMargins(16, 14, 16, 14);
    uiLay->setSpacing(10);

    auto* uiHeader = new QHBoxLayout();
    uiHeader->setContentsMargins(0, 0, 0, 0);
    uiHeader->setSpacing(8);
    uiHeader->addWidget(makeSectionTitle(tr("UI size"), uiCard));
    uiHeader->addStretch(1);
    m_uiValueLbl = new QLabel(uiCard);
    m_uiValueLbl->setObjectName("settingsValueLbl");
    uiHeader->addWidget(m_uiValueLbl);
    uiLay->addLayout(uiHeader);

    m_uiSlider = new QSlider(Qt::Horizontal, uiCard);
    m_uiSlider->setObjectName("settingsSlider");
    m_uiSlider->setRange(70, 150);
    m_uiSlider->setSingleStep(5);
    m_uiSlider->setPageStep(10);
    m_uiSlider->setCursor(Qt::PointingHandCursor);
    uiLay->addWidget(m_uiSlider);

    auto* uiHint = new QLabel(tr("Scales spacing, sidebar and button sizes."), uiCard);
    uiHint->setObjectName("settingsHint");
    uiLay->addWidget(uiHint);

    root->addWidget(uiCard);

    root->addStretch(1);

    // ---------- Footer buttons ----------
    auto* footer = new QHBoxLayout();
    footer->setContentsMargins(0, 4, 0, 0);
    footer->setSpacing(8);

    m_resetBtn = new QPushButton(tr("Reset"), this);
    m_resetBtn->setObjectName("settingsResetBtn");
    m_resetBtn->setCursor(Qt::PointingHandCursor);
    footer->addWidget(m_resetBtn);
    footer->addStretch(1);

    auto* cancelBtn = new QPushButton(tr("Cancel"), this);
    cancelBtn->setObjectName("settingsCancelBtn");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    footer->addWidget(cancelBtn);

    auto* applyBtn = new QPushButton(tr("Apply"), this);
    applyBtn->setObjectName("settingsApplyBtn");
    applyBtn->setCursor(Qt::PointingHandCursor);
    applyBtn->setDefault(true);
    footer->addWidget(applyBtn);

    root->addLayout(footer);

    // Connections
    connect(m_fontSlider,    &QSlider::valueChanged, this, &SettingsDialog::onFontChanged);
    connect(m_uiSlider,      &QSlider::valueChanged, this, &SettingsDialog::onUiChanged);
    connect(m_languageCombo, &QComboBox::currentTextChanged, this, &SettingsDialog::onLanguageChanged);
    connect(m_resetBtn,      &QPushButton::clicked, this, &SettingsDialog::onReset);
    connect(cancelBtn,       &QPushButton::clicked, this, &QDialog::reject);
    connect(applyBtn,        &QPushButton::clicked, this, [this](){
        emit settingsApplied(m_settings);
        accept();
    });
}

void SettingsDialog::setControlsFromSettings() {
    QSignalBlocker fb(m_fontSlider);
    QSignalBlocker ub(m_uiSlider);
    QSignalBlocker lb(m_languageCombo);

    m_fontSlider->setValue(m_settings.fontScale);
    m_uiSlider->setValue(m_settings.uiScale);

    int idx = m_languageCombo->findText(m_settings.language);
    if (idx < 0) idx = 0;
    m_languageCombo->setCurrentIndex(idx);

    m_fontValueLbl->setText(QStringLiteral("%1%").arg(m_settings.fontScale));
    m_uiValueLbl->setText(QStringLiteral("%1%").arg(m_settings.uiScale));
}

void SettingsDialog::onFontChanged(int value) {
    m_settings.fontScale = value;
    m_fontValueLbl->setText(QStringLiteral("%1%").arg(value));
}

void SettingsDialog::onUiChanged(int value) {
    m_settings.uiScale = value;
    m_uiValueLbl->setText(QStringLiteral("%1%").arg(value));
}

void SettingsDialog::onLanguageChanged(const QString& lang) {
    m_settings.language = lang;
}

void SettingsDialog::onReset() {
    m_settings = AppSettings{};
    setControlsFromSettings();
}