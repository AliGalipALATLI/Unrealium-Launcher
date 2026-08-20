#pragma once
#include <QDialog>
#include "SettingsManager.h"

class ArrowComboBox;
class QSlider;
class QLabel;
class QPushButton;
class QVBoxLayout;

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(const AppSettings& initial, QWidget* parent = nullptr);

    AppSettings currentSettings() const { return m_settings; }

signals:
    void settingsApplied(const AppSettings& settings);

private slots:
    void onFontChanged(int value);
    void onUiChanged(int value);
    void onLanguageChanged(const QString& lang);
    void onReset();

private:
    void buildUi();
    void setControlsFromSettings();

    AppSettings  m_settings;

    ArrowComboBox* m_languageCombo = nullptr;
    QSlider*     m_fontSlider    = nullptr;
    QSlider*     m_uiSlider      = nullptr;
    QLabel*      m_fontValueLbl  = nullptr;
    QLabel*      m_uiValueLbl    = nullptr;
    QPushButton* m_resetBtn      = nullptr;
};