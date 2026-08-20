#include "ArrowComboBox.h"
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>

ArrowComboBox::ArrowComboBox(QWidget* parent) : QComboBox(parent) {
    m_anim = new QPropertyAnimation(this, "arrowRotation");
    m_anim->setDuration(160);
    m_anim->setEasingCurve(QEasingCurve::InOutQuad);
}

void ArrowComboBox::setArrowRotation(qreal r) {
    if (qFuzzyCompare(m_rotation, r)) return;
    m_rotation = r;
    emit arrowRotationChanged();
    update();
}

void ArrowComboBox::paintEvent(QPaintEvent* e) {
    QComboBox::paintEvent(e);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);

    const int arrowW  = 10;
    const int arrowH  = 6;
    const int rightPad = 16;
    const int cx = width() - rightPad - arrowW / 2;
    const int cy = height() / 2;

    p.save();
    p.translate(cx, cy);
    p.rotate(m_rotation);
    p.translate(-cx, -cy);

    QPainterPath path;
    path.moveTo(cx - arrowW / 2.0, cy - arrowH / 2.0);
    path.lineTo(cx,              cy + arrowH / 2.0);
    path.lineTo(cx + arrowW / 2.0, cy - arrowH / 2.0);
    path.lineTo(cx - arrowW / 2.0 + 1.0, cy - arrowH / 2.0);

    p.setBrush(QColor("#dcdcdc"));
    p.drawPath(path);
    p.restore();
}

void ArrowComboBox::showPopup() {
    QComboBox::showPopup();
    m_anim->stop();
    m_anim->setStartValue(m_rotation);
    m_anim->setEndValue(180.0);
    m_anim->start();
}

void ArrowComboBox::hidePopup() {
    QComboBox::hidePopup();
    m_anim->stop();
    m_anim->setStartValue(m_rotation);
    m_anim->setEndValue(0.0);
    m_anim->start();
}