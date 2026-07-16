#pragma once
#include "ViewEditorsPage.h"
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QLineEdit>
#include <QListView>
#include <QStandardItemModel>

#include "NodeManager.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);

private slots:
  void openAddEditorDialog();
  void openDownloadEditorDialog();
  void showViewEditorsPage();
  void showMainPage();

private:
  QStackedWidget *stackedWidget;
  QWidget *mainPage;
  QPushButton *addEditorButton;
  QPushButton *downloadEditorButton;
  QPushButton *viewEditorsButton;
  ViewEditorsPage *viewEditorsPage;
};
