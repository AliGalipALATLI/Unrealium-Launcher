#pragma once
#include <QComboBox>
#include <QPropertyAnimation>

class ArrowComboBox : public QComboBox {
    Q_OBJECT
    Q_PROPERTY(qreal arrowRotation READ arrowRotation WRITE setArrowRotation NOTIFY arrowRotationChanged)
public:
    explicit ArrowComboBox(QWidget* parent = nullptr);

    qreal arrowRotation() const { return m_rotation; }
    void setArrowRotation(qreal r);

signals:
    void arrowRotationChanged();

protected:
    void paintEvent(QPaintEvent* e) override;
    void showPopup() override;
    void hidePopup() override;

private:
    QPropertyAnimation* m_anim = nullptr;
    qreal m_rotation = 0.0;
};