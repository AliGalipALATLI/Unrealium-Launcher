#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>

class NewsPage : public QWidget {
    Q_OBJECT
public:
    explicit NewsPage(QWidget* parent = nullptr);

private:
    QVBoxLayout* m_listLayout = nullptr;
};
