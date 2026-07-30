#pragma once
#include "ViewEditorsPage.h"
#include "ViewProjectsPage.h"
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
  void showViewEditorsPage();
  void showViewProjectsPage();
  void showMainPage();

private:
  QStackedWidget *stackedWidget;
  QWidget *mainPage;
  QPushButton *addEditorButton;
  QPushButton *viewEditorsButton;
  QPushButton *viewProjectsButton;
  ViewEditorsPage *viewEditorsPage;
  ViewProjectsPage *viewProjectsPage;
};
