#include "StyleManager.h"
#include <QApplication>
#include <cmath>

namespace {
    int scaleFont(int px, int percent) {
        return qRound(px * percent / 100.0);
    }
    int scaleUi(int px, int percent) {
        return qRound(px * percent / 100.0);
    }
    QString px(int v) { return QString::number(v) + "px"; }
}

void StyleManager::apply(int fontScale, int uiScale) {
    if (fontScale < 70)  fontScale = 70;
    if (fontScale > 150) fontScale = 150;
    if (uiScale < 70)    uiScale = 70;
    if (uiScale > 150)   uiScale = 150;

    const int fsBase   = scaleFont(13, fontScale);
    const int fsTag    = scaleFont(11, fontScale);
    const int fsSmall  = scaleFont(12, fontScale);
    const int fsMed    = scaleFont(14, fontScale);
    const int fsCard   = scaleFont(15, fontScale);
    const int fsResult = scaleFont(16, fontScale);
    const int fsTitle  = scaleFont(22, fontScale);

    const int rad8   = scaleUi(8,  uiScale);
    const int rad6   = scaleUi(6,  uiScale);
    const int rad4   = scaleUi(4,  uiScale);
    const int rad22  = scaleUi(22, uiScale);
    const int navH   = scaleUi(38, uiScale);
    const int addH   = scaleUi(40, uiScale);
    const int padBtn = scaleUi(8,  uiScale);
    const int padN   = scaleUi(14, uiScale);

    const QString qss = QStringLiteral(R"(
        /* ---------- Base ---------- */
        QWidget        { background: #1e1e1e; color: #e8e8e8; font-size: %1; }
        QMainWindow    { background: #1e1e1e; }
        QDialog        { background: #1e1e1e; color: #e8e8e8; }
        QLabel         { color: #e8e8e8; background: transparent; }

        /* ---------- Sidebar ---------- */
        QWidget#sidebar {
            background: #171717;
            border-right: 1px solid #2a2a2a;
        }

        QFrame#navCard {
            background: #232323;
            border: 1px solid #2c2c2c;
            border-radius: %2;
        }
        QFrame#navCard:hover { background: #262626; }

        QPushButton#navBtn {
            background: transparent;
            border: none;
            border-radius: %2;
            padding: 9px 14px;
            text-align: left;
            color: #cccccc;
            font-size: %1;
            font-weight: 500;
            min-height: %3;
        }
        QPushButton#navBtn:hover   { color: #ffffff; }

        QFrame#navCard[active="true"] {
            background: #2e4663;
            border: 1px solid #3b6ea5;
        }
        QPushButton#navBtn:checked {
            color: #ffffff;
            font-weight: 600;
        }

        QPushButton#addEditorBtn {
            background: #4A9EFF;
            border: 1px solid #4A9EFF;
            border-radius: %2;
            color: #ffffff;
            font-size: %4;
            font-weight: 600;
            padding: %5 %6;
            text-align: center;
            min-height: %7;
        }
        QPushButton#addEditorBtn:hover {
            background: #5DABFF;
            border: 1px solid #5DABFF;
        }
        QPushButton#addEditorBtn:pressed {
            background: #3A8AEB;
            border: 1px solid #3A8AEB;
        }

        /* ---------- Page headers ---------- */
        QLabel#pageTitle    { font-size: %8; font-weight: 700; color: #ffffff; }
        QLabel#pageSubtitle { font-size: %9; color: #9a9a9a; }
        QLabel#cardHeader   { font-size: %9; font-weight: 700; color: #b0b0b0; letter-spacing: 1px; }

        /* ---------- News cards ---------- */
        QFrame#newsCard {
            background: #232323;
            border: 1px solid #2c2c2c;
            border-radius: %2;
        }
        QLabel#newsTag   { color: #4A9EFF; font-size: %10; font-weight: 700; letter-spacing: 1px; }
        QLabel#newsTitle { font-size: %11; font-weight: 600; color: #ffffff; }
        QLabel#newsBody  { color: #b8b8b8; }

        /* ---------- Add Editor page ---------- */
        QFrame#addEditorCard {
            background: #232323;
            border: 1px solid #2c2c2c;
            border-radius: %2;
        }
        QLineEdit#addEditorPath, QLineEdit#addEditorName, QLineEdit#addEditorArgs {
            background: #1c1c1c;
            border: 1px solid #333333;
            border-radius: %12;
            padding: 6px 10px;
            color: #ffffff;
        }
        QLineEdit#addEditorPath:focus,
        QLineEdit#addEditorName:focus,
        QLineEdit#addEditorArgs:focus {
            border: 1px solid #4A9EFF;
        }
        QPushButton#browseBtn {
            background: #2d2d2d;
            border: 1px solid #444;
            border-radius: %12;
            padding: 6px 14px;
        }
        QPushButton#browseBtn:hover { background: #3a3a3a; }
        QPushButton#primaryBtn {
            background: #4A9EFF;
            border: 1px solid #4A9EFF;
            border-radius: %12;
            color: #ffffff;
            font-weight: 600;
            padding: %5 18px;
        }
        QPushButton#primaryBtn:hover { background: #5DABFF; border: 1px solid #5DABFF; }
        QPushButton#primaryBtn:disabled {
            background: #2a3a52;
            border: 1px solid #2a3a52;
            color: #6c7d8e;
        }
        QPushButton#secondaryBtn {
            background: #2d2d2d;
            border: 1px solid #444;
            border-radius: %12;
            color: #cccccc;
            padding: %5 16px;
        }
        QPushButton#secondaryBtn:hover { background: #3a3a3a; color: #ffffff; }

        QFrame#discoveredRow {
            background: #1c1c1c;
            border: 1px solid #2c2c2c;
            border-radius: %12;
        }
        QLabel#discoveredName { font-weight: 600; color: #ffffff; }
        QLabel#discoveredPath { color: #8a8a8a; font-size: %9; font-family: monospace; }
        QLabel#discoveredEmpty { color: #6c6c6c; font-style: italic; padding: 12px; }

        /* ---------- Node search (AI-style) ---------- */
        QLineEdit#nodeSearch {
            background: #242424;
            border: 1px solid #333333;
            border-radius: %13;
            padding: 10px 18px;
            color: #ffffff;
            font-size: %4;
            selection-background-color: #4A9EFF;
        }
        QLineEdit#nodeSearch:focus {
            border: 1px solid #4A9EFF;
            background: #1f1f1f;
        }

        QFrame#nodeResultCard {
            background: #232323;
            border: 1px solid #2c2c2c;
            border-radius: %2;
        }
        QFrame#nodeResultCard:hover {
            background: #262626;
            border: 1px solid #353535;
        }
        QFrame#nodeResultCard[selected="true"] {
            background: #232323;
            border: 1px solid #353535;
        }
        QLabel#nodeResultTitle { font-size: %14; font-weight: 600; color: #ffffff; }
        QFrame#nodeResultDivider { background: #2e2e2e; max-height: 1px; border: none; }
        QLabel#nodeResultBody { color: #d8d8d8; font-size: %1; }
        QLabel#nodesEmpty    { color: #808080; padding: 32px; }

        QLabel#nodeResultBody .node-desc {
            color: #d8d8d8;
            margin-bottom: 8px;
        }
        QLabel#nodeResultBody .node-section {
            margin-top: 6px;
        }
        QLabel#nodeResultBody .node-section-title {
            color: #4A9EFF;
            font-weight: 600;
            font-size: %9;
            margin-bottom: 4px;
        }
        QLabel#nodeResultBody ul.pin-list {
            margin: 0;
            padding-left: 18px;
        }
        QLabel#nodeResultBody ul.pin-list li {
            margin-bottom: 2px;
        }
        QLabel#nodeResultBody .kw-inline {
            color: #6a6a6a;
            font-size: %10;
            margin-top: 10px;
            margin-bottom: 4px;
            line-height: 1.4;
        }
        QLabel#nodeResultBody .kw-sep {
            color: #4a4a4a;
            margin: 0 4px;
        }
        QLabel#nodeResultBody .pin-name {
            color: #ffffff;
            font-weight: 600;
        }
        QLabel#nodeResultBody .pin-type {
            color: #888888;
            font-size: %10;
        }
        QLabel#nodeResultBody .pin-desc {
            color: #c8c8c8;
        }
        QLabel#nodeResultBody .pins-empty {
            color: #777777;
            font-style: italic;
            padding-left: 18px;
        }

        /* ---------- Settings gear button (top-left overlay) ---------- */
        QPushButton#settingsBtn {
            background: #232323;
            border: 1px solid #2c2c2c;
            border-radius: %2;
            color: #cccccc;
            font-size: %15;
            font-weight: 600;
            padding: 0px;
        }
        QPushButton#settingsBtn:hover {
            background: #2a2a2a;
            color: #ffffff;
            border: 1px solid #3a3a3a;
        }
        QPushButton#settingsBtn:pressed {
            background: #1d1d1d;
        }

        /* ---------- Settings dialog ---------- */
        QDialog#settingsDialog {
            background: #1e1e1e;
        }
        QLabel#settingsTitleIcon {
            color: #4A9EFF;
            font-size: %8;
            font-weight: 700;
            background: transparent;
        }
        QLabel#settingsTitle {
            color: #ffffff;
            font-size: %8;
            font-weight: 700;
            background: transparent;
        }
        QPushButton#settingsCloseBtn {
            background: transparent;
            border: 1px solid transparent;
            border-radius: %12;
            color: #aaaaaa;
            font-size: %9;
        }
        QPushButton#settingsCloseBtn:hover {
            background: #2a2a2a;
            color: #ffffff;
            border: 1px solid #3a3a3a;
        }
        QLabel#settingsSubtitle {
            color: #9a9a9a;
            font-size: %9;
            background: transparent;
        }
        QFrame#settingsCard {
            background: #232323;
            border: 1px solid #2c2c2c;
            border-radius: %2;
        }
        QLabel#settingsSectionTitle {
            color: #b0b0b0;
            font-size: %9;
            font-weight: 700;
            letter-spacing: 1px;
            background: transparent;
        }
        QLabel#settingsHint {
            color: #777777;
            font-size: %10;
            background: transparent;
        }
        QLabel#settingsValueLbl {
            color: #4A9EFF;
            font-size: %9;
            font-weight: 600;
            background: transparent;
        }

        /* Language switcher */
        QComboBox#settingsLangCombo {
            background: #1c1c1c;
            border: 1px solid #333333;
            border-radius: %12;
            padding: %5 %6;
            padding-right: 36px;
            color: #ffffff;
            font-size: %1;
            min-height: %7;
        }
        QComboBox#settingsLangCombo:hover {
            border: 1px solid #4A9EFF;
        }
        QComboBox#settingsLangCombo::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 30px;
            border: none;
        }
        QComboBox#settingsLangCombo::down-arrow {
            image: none;
            width: 0;
            height: 0;
            border: none;
            padding: 0;
            margin: 0;
        }
        QComboBox QAbstractItemView {
            background: #1c1c1c;
            color: #ffffff;
            border: 1px solid #333333;
            selection-background-color: #2e4663;
            selection-color: #ffffff;
            outline: 0;
        }

        /* Volume-style slider: thin line + circular handle */
        QSlider#settingsSlider {
            background: transparent;
            min-height: %16;
        }
        QSlider#settingsSlider::groove:horizontal {
            background: #2f2f2f;
            height: 2px;
            border-radius: 1px;
            margin: 0 %17;
        }
        QSlider#settingsSlider::sub-page:horizontal {
            background: #4A9EFF;
            height: 2px;
            border-radius: 1px;
        }
        QSlider#settingsSlider::add-page:horizontal {
            background: #2f2f2f;
            height: 2px;
            border-radius: 1px;
        }
        QSlider#settingsSlider::handle:horizontal {
            background: #ffffff;
            border: 1px solid #cccccc;
            width: %18;
            height: %18;
            margin: -7px 0;
            border-radius: %19;
        }
        QSlider#settingsSlider::handle:horizontal:hover {
            background: #f5f5f5;
            border: 1px solid #4A9EFF;
        }
        QSlider#settingsSlider::handle:horizontal:pressed {
            background: #4A9EFF;
            border: 1px solid #4A9EFF;
        }

        /* Settings dialog action buttons */
        QPushButton#settingsResetBtn {
            background: transparent;
            border: 1px solid #444;
            border-radius: %12;
            color: #cccccc;
            padding: %5 %6;
            font-size: %1;
        }
        QPushButton#settingsResetBtn:hover {
            background: #2a2a2a;
            color: #ffffff;
        }
        QPushButton#settingsCancelBtn {
            background: #2d2d2d;
            border: 1px solid #444;
            border-radius: %12;
            color: #cccccc;
            padding: %5 16px;
            font-size: %1;
        }
        QPushButton#settingsCancelBtn:hover {
            background: #3a3a3a;
            color: #ffffff;
        }
        QPushButton#settingsApplyBtn {
            background: #4A9EFF;
            border: 1px solid #4A9EFF;
            border-radius: %12;
            color: #ffffff;
            font-weight: 600;
            padding: %5 %6;
            font-size: %1;
        }
        QPushButton#settingsApplyBtn:hover {
            background: #5DABFF;
            border: 1px solid #5DABFF;
        }
        QPushButton#settingsApplyBtn:pressed {
            background: #3A8AEB;
            border: 1px solid #3A8AEB;
        }

        /* ---------- Generic ---------- */
        QPushButton    { background: #2d2d2d; border: 1px solid #444;
                         border-radius: %12; padding: %5 18px; color: #ffffff; }
        QPushButton:hover   { background: #3a3a3a; }
        QPushButton:pressed { background: #252525; }
        QLineEdit      { background: #2d2d2d; border: 1px solid #444;
                         border-radius: %20; padding: 4px 8px; color: #ffffff; }
        QMenu          { background: #2d2d2d; color: #ffffff; border: 1px solid #444; }
        QMenu::item:selected { background: #3a3a3a; }
        QScrollArea    { border: none; background: transparent; }
        QScrollBar:vertical {
            background: #1e1e1e; width: 10px; margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #3a3a3a; border-radius: 4px; min-height: 24px;
        }
        QScrollBar::handle:vertical:hover { background: #4a4a4a; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QListWidget    { background: #2d2d2d; color: #ffffff; border: 1px solid #444;
                         border-radius: %20; padding: 4px; }
        QListWidget::item:selected { background: #3a3a3a; }
    )")
    .arg(px(fsBase))     // 1
    .arg(px(rad8))       // 2
    .arg(px(navH))       // 3
    .arg(px(fsMed))      // 4
    .arg(px(padBtn))     // 5
    .arg(px(padN))       // 6
    .arg(px(addH))       // 7
    .arg(px(fsTitle))    // 8
    .arg(px(fsSmall))    // 9
    .arg(px(fsTag))      // 10
    .arg(px(fsCard))     // 11
    .arg(px(rad6))       // 12
    .arg(px(rad22))      // 13
    .arg(px(fsResult))   // 14
    .arg(px(scaleFont(15, fontScale))) // 15 - gear icon font-size
    .arg(px(scaleUi(22, uiScale)))     // 16 - slider min-height
    .arg(px(scaleUi(8, uiScale)))      // 17 - groove horizontal margin
    .arg(px(scaleUi(14, uiScale)))     // 18 - handle width/height
    .arg(px(scaleUi(7, uiScale)))      // 19 - handle border-radius
    .arg(px(rad4))       // 20
    ;

    qApp->setStyleSheet(qss);
}