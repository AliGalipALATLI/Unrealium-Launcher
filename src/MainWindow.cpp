#include "MainWindow.h"
#include "AddEditorDialog.h"
#include "DownloadEditorDialog.h"
#include "NodeDetailDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Unreal Launcher");
  setMinimumSize(800, 600);
  resize(800, 600);

  stackedWidget = new QStackedWidget(this);
  setCentralWidget(stackedWidget);

  // Page 0: Main Page with search sidebar
  mainPage = new QWidget();
  QHBoxLayout *pageLayout = new QHBoxLayout(mainPage);
  pageLayout->setContentsMargins(16,16,16,16);

  // Left: search sidebar (fixed)
  QWidget *left = new QWidget(mainPage);
  left->setFixedWidth(340);
  QVBoxLayout *leftLayout = new QVBoxLayout(left);
  leftLayout->setContentsMargins(8,8,8,8);

  QLineEdit *searchEdit = new QLineEdit(left);
  searchEdit->setPlaceholderText("Search nodes...");
  leftLayout->addWidget(searchEdit);

  QListView *resultsView = new QListView(left);
  QStandardItemModel *resultsModel = new QStandardItemModel(this);
  resultsView->setModel(resultsModel);
  leftLayout->addWidget(resultsView);

  pageLayout->addWidget(left);

  // Right: main content (buttons centered)
  QWidget *right = new QWidget(mainPage);
  QVBoxLayout *rightLayout = new QVBoxLayout(right);
  rightLayout->addStretch();

  viewEditorsButton = new QPushButton("View Editors", this);
  connect(viewEditorsButton, &QPushButton::clicked, this,
          &MainWindow::showViewEditorsPage);

  addEditorButton = new QPushButton("Add Editor", this);
  connect(addEditorButton, &QPushButton::clicked, this,
          &MainWindow::openAddEditorDialog);

  downloadEditorButton = new QPushButton("Download Editor", this);
  connect(downloadEditorButton, &QPushButton::clicked, this,
          &MainWindow::openDownloadEditorDialog);

  // Place buttons in the layout and make their sizes identical
  QSize btnSize(240, 56);
  viewEditorsButton->setFixedSize(btnSize);
  addEditorButton->setFixedSize(btnSize);
  downloadEditorButton->setFixedSize(btnSize);

  rightLayout->addWidget(viewEditorsButton, 0, Qt::AlignCenter);
  rightLayout->addSpacing(12);
  rightLayout->addWidget(addEditorButton, 0, Qt::AlignCenter);
  rightLayout->addSpacing(12);
  rightLayout->addWidget(downloadEditorButton, 0, Qt::AlignCenter);

  rightLayout->addStretch();
  pageLayout->addWidget(right);

  stackedWidget->addWidget(mainPage);

  // Load nodes from repo assistant/nodes (checking multiple locations)
  QString nodesDir = NodeManager::getNodesPath();
  QVector<NodeEntry> nodes = NodeManager::loadNodes(nodesDir);

  // Wire search
  connect(searchEdit, &QLineEdit::textChanged, this, [=](const QString &text){
      QVector<NodeEntry> res = NodeManager::filterNodes(nodes, text, 50);
      resultsModel->clear();
      for (const auto &n : res) {
          QStandardItem *it = new QStandardItem(n.name);
          it->setData(n.id, Qt::UserRole);
          it->setToolTip(n.description);
          resultsModel->appendRow(it);
      }
  });

  // Click opens detail dialog
  connect(resultsView, &QListView::clicked, this, [=](const QModelIndex &idx){
      if (!idx.isValid()) return;
      QString id = idx.data(Qt::UserRole).toString();
      for (const auto &n : nodes) {
          if (n.id == id) {
              NodeDetailDialog *dlg = new NodeDetailDialog(n, this);
              dlg->exec();
              delete dlg;
              break;
          }
      }
  });

  // Page 1: View Editors Page
  viewEditorsPage = new ViewEditorsPage(this);
  connect(viewEditorsPage, &ViewEditorsPage::backRequested, this,
          &MainWindow::showMainPage);
  stackedWidget->addWidget(viewEditorsPage);
}

void MainWindow::openAddEditorDialog() {
  AddEditorDialog dialog(this);
  dialog.exec();
}

void MainWindow::openDownloadEditorDialog() {
  DownloadEditorDialog dialog(this);
  dialog.exec();
}

void MainWindow::showViewEditorsPage() {
  viewEditorsPage->loadEditors();
  stackedWidget->setCurrentIndex(1);
}

void MainWindow::showMainPage() { stackedWidget->setCurrentIndex(0); }
