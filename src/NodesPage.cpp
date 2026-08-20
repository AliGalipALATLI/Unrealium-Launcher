#include "NodesPage.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QShortcut>
#include <QKeySequence>
#include <QMouseEvent>
#include <QStyle>

namespace {

QString escapeHtml(const QString& s) {
    QString out = s;
    out.replace('&', "&amp;");
    out.replace('<', "&lt;");
    out.replace('>', "&gt;");
    return out;
}

QString buildPinsHtml(const QVector<NodePin>& pins, const QString& emptyText) {
    if (pins.isEmpty()) {
        return QString("<div class='pins-empty'>%1</div>").arg(escapeHtml(emptyText));
    }
    QString html;
    html += "<ul class='pin-list'>";
    for (const NodePin& p : pins) {
        html += "<li>";
        html += QString("<span class='pin-name'>%1</span> ").arg(escapeHtml(p.name));
        if (!p.type.isEmpty()) {
            html += QString("<span class='pin-type'>(%1)</span> ").arg(escapeHtml(p.type));
        }
        if (!p.description.isEmpty()) {
            html += QString("<span class='pin-desc'>%1</span>").arg(escapeHtml(p.description));
        }
        html += "</li>";
    }
    html += "</ul>";
    return html;
}

QString buildKeywordsHtml(const QVector<QString>& keywords) {
    if (keywords.isEmpty()) return QString();
    QStringList parts;
    for (const QString& k : keywords) {
        parts << escapeHtml(k);
    }
    return QString("<div style='color:#6a6a6a;font-size:11px;margin-top:10px;margin-bottom:4px;line-height:1.4;'>%1</div>")
        .arg(parts.join(" <span style='color:#4a4a4a;margin:0 4px;'>&middot;</span> "));
}

QString buildBodyHtml(const NodeEntry& node) {
    QString html;
    html += "<div class='node-desc'>";
    html += escapeHtml(node.description);
    html += "</div>";

    html += "<div class='node-section'>";
    html += QString("<div class='node-section-title'>Input%1 (%2)</div>")
                .arg(node.inputs.size() == 1 ? QString() : QString("s"))
                .arg(node.inputs.size());
    html += buildPinsHtml(node.inputs, "No input pins.");
    html += "</div>";

    html += "<div class='node-section'>";
    html += QString("<div class='node-section-title'>Output%1 (%2)</div>")
                .arg(node.outputs.size() == 1 ? QString() : QString("s"))
                .arg(node.outputs.size());
    html += buildPinsHtml(node.outputs, "No output pins.");
    html += "</div>";

    html += buildKeywordsHtml(node.keywords);

    return html;
}

} // namespace

NodeResultCard::NodeResultCard(QWidget* parent) : QFrame(parent) {
    setObjectName("nodeResultCard");
    setCursor(Qt::PointingHandCursor);
    setFrameShape(QFrame::NoFrame);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(14, 12, 14, 12);
    lay->setSpacing(4);

    auto* title = new QLabel(this);
    title->setObjectName("nodeResultTitle");
    lay->addWidget(title);

    m_divider = new QFrame(this);
    m_divider->setObjectName("nodeResultDivider");
    m_divider->setFrameShape(QFrame::HLine);
    m_divider->setFrameShadow(QFrame::Plain);
    m_divider->hide();
    lay->addWidget(m_divider);

    m_bodyLabel = new QLabel(this);
    m_bodyLabel->setObjectName("nodeResultBody");
    m_bodyLabel->setWordWrap(true);
    m_bodyLabel->setTextFormat(Qt::RichText);
    m_bodyLabel->hide();
    lay->addWidget(m_bodyLabel);
}

void NodeResultCard::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) emit clicked();
    else QFrame::mousePressEvent(e);
}

void NodeResultCard::setSelected(bool sel) {
    setProperty("selected", sel);

    if (sel) {
        m_divider->show();
        m_bodyLabel->show();
        if (layout()) layout()->setSpacing(8);
    } else {
        m_divider->hide();
        m_bodyLabel->hide();
        if (layout()) layout()->setSpacing(4);
    }

    style()->unpolish(this);
    style()->polish(this);
}

NodesPage::NodesPage(QWidget* parent) : QWidget(parent) {
    setObjectName("nodesPage");

    m_nodes = NodeManager::loadNodes(NodeManager::getNodesPath());

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(28, 24, 28, 24);
    outer->setSpacing(0);

    auto* header = new QLabel("Node Assistant", this);
    header->setObjectName("pageTitle");
    outer->addWidget(header);

    auto* sub = new QLabel("Ask anything about Blueprint nodes.", this);
    sub->setObjectName("pageSubtitle");
    outer->addWidget(sub);
    outer->addSpacing(18);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setObjectName("nodeSearch");
    m_searchEdit->setPlaceholderText("Search nodes…  (e.g. branch, vector, tick)");
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setMinimumHeight(46);
    outer->addWidget(m_searchEdit);

    auto* focusSc = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this);
    focusSc->setContext(Qt::WidgetWithChildrenShortcut);
    connect(focusSc, &QShortcut::activated, this, [this]{
        m_searchEdit->setFocus(Qt::ShortcutFocusReason);
        m_searchEdit->selectAll();
    });

    outer->addSpacing(14);

    m_scroll = new QScrollArea(this);
    m_scroll->setObjectName("nodesScroll");
    m_scroll->setWidgetResizable(true);

    auto* listContainer = new QWidget(m_scroll);
    m_listLayout = new QVBoxLayout(listContainer);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(10);
    m_listLayout->addStretch(1);

    m_scroll->setWidget(listContainer);
    outer->addWidget(m_scroll, 1);

    connect(m_searchEdit, &QLineEdit::textChanged, this, &NodesPage::onSearchChanged);
    onSearchChanged(QString());
}

void NodesPage::clearResults() {
    while (m_listLayout->count() > 1) {
        QLayoutItem* it = m_listLayout->takeAt(0);
        if (auto* w = it->widget()) w->deleteLater();
        delete it;
    }
}

void NodesPage::appendResultCard(const NodeEntry& node) {
    auto* card = new NodeResultCard(m_scroll->widget());
    card->nodeId = node.id;
    card->setSelected(node.id == m_selectedId);

    // Populate labels by walking children (title is first, body is last)
    auto labels = card->findChildren<QLabel*>();
    if (labels.size() >= 2) {
        labels[0]->setText(node.name);                // title
        labels[1]->setText(buildBodyHtml(node));      // structured body
    }

    connect(card, &NodeResultCard::clicked, this, [this, id = node.id]{
        selectNode(id);
    });

    m_listLayout->insertWidget(m_listLayout->count() - 1, card);
}

void NodesPage::onSearchChanged(const QString& text) {
    m_selectedId.clear();

    clearResults();
    QVector<NodeEntry> results = NodeManager::filterNodes(m_nodes, text, 50);
    for (const auto& n : results) {
        appendResultCard(n);
    }

    if (results.isEmpty()) {
        auto* empty = new QLabel("No nodes match your search.", m_scroll->widget());
        empty->setObjectName("nodesEmpty");
        empty->setAlignment(Qt::AlignCenter);
        m_listLayout->insertWidget(m_listLayout->count() - 1, empty);
    }
}

void NodesPage::selectNode(const QString& id) {
    m_selectedId = id;

    clearResults();
    for (const auto& n : m_nodes) {
        if (n.id == m_selectedId) {
            appendResultCard(n);
            break;
        }
    }

    for (const auto& n : m_nodes) {
        if (n.id == m_selectedId) {
            m_searchEdit->blockSignals(true);
            m_searchEdit->setText(n.name);
            m_searchEdit->blockSignals(false);
            break;
        }
    }
}
