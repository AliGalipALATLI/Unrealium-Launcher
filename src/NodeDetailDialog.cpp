#include "NodeDetailDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QFrame>
#include <QIcon>
#include <QCoreApplication>

namespace {
QLabel* makeSectionTitle(const QString& text, QWidget* parent) {
    QLabel* l = new QLabel(text, parent);
    QFont f = l->font();
    f.setBold(true);
    f.setPointSize(f.pointSize() + 1);
    l->setFont(f);
    return l;
}

QLabel* makePinLabel(const NodePin& p, QWidget* parent) {
    QString txt = QString("<b>%1</b> <span style='color:#888;'>(%2)</span>").arg(p.name, p.type);
    if (!p.description.isEmpty()) {
        txt += QString("<br><span style='color:#bbb;'>%1</span>").arg(p.description);
    }
    QLabel* l = new QLabel(txt, parent);
    l->setTextFormat(Qt::RichText);
    l->setWordWrap(true);
    l->setContentsMargins(4, 2, 4, 2);
    return l;
}
} // namespace

NodeDetailDialog::NodeDetailDialog(const NodeEntry& entry, QWidget* parent)
    : QDialog(parent) {
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setModal(true);
    setMinimumSize(540, 420);

    QVBoxLayout* main = new QVBoxLayout(this);
    main->setContentsMargins(8, 8, 8, 8);

    // Top toolbar with back arrow (close)
    QHBoxLayout* top = new QHBoxLayout();
    QToolButton* back = new QToolButton(this);
    back->setText("\u2190");
    back->setAutoRaise(true);
    back->setToolButtonStyle(Qt::ToolButtonTextOnly);
    connect(back, &QToolButton::clicked, this, &NodeDetailDialog::accept);
    top->addWidget(back);
    top->addStretch();
    main->addLayout(top);

    QLabel* title = new QLabel(entry.name, this);
    QFont f = title->font();
    f.setPointSize(14);
    f.setBold(true);
    title->setFont(f);
    main->addWidget(title);

    QLabel* desc = new QLabel(entry.description, this);
    desc->setWordWrap(true);
    desc->setTextFormat(Qt::RichText);
    main->addWidget(desc);

    // Inputs / Outputs
    if (!entry.inputs.isEmpty() || !entry.outputs.isEmpty()) {
        QFrame* sep = new QFrame(this);
        sep->setFrameShape(QFrame::HLine);
        sep->setFrameShadow(QFrame::Sunken);
        main->addWidget(sep);

        QHBoxLayout* io = new QHBoxLayout();

        QVBoxLayout* inCol = new QVBoxLayout();
        inCol->addWidget(makeSectionTitle("Input:", this));
        if (entry.inputs.isEmpty()) {
            inCol->addWidget(new QLabel("<i>None</i>", this));
        } else {
            for (const NodePin& p : entry.inputs) inCol->addWidget(makePinLabel(p, this));
        }
        inCol->addStretch();

        QVBoxLayout* outCol = new QVBoxLayout();
        outCol->addWidget(makeSectionTitle("Output:", this));
        if (entry.outputs.isEmpty()) {
            outCol->addWidget(new QLabel("<i>None</i>", this));
        } else {
            for (const NodePin& p : entry.outputs) outCol->addWidget(makePinLabel(p, this));
        }
        outCol->addStretch();

        io->addLayout(inCol);
        io->addLayout(outCol);
        main->addLayout(io);
    }

    main->addStretch();
}
