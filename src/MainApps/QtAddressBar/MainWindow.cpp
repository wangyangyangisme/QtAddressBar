#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QProcess>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    InitUI();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotShowChild(const QModelIndex &index)
{
    QString path = m_pFileSystemModel->fileInfo(index).absoluteFilePath();
    qDebug() << "path:" << path;

    if(m_pFileSystemModel->fileInfo(index).isDir()) {
        m_currentPath = path;
        ui->addressbar->UpdateCurrentPath(path);

        ui->tableView->setRootIndex(m_pFileSystemModel->index(m_currentPath));
        ui->tableView->clearSelection();
        ui->tableView->scrollToTop();
    }
    else
    {
#ifdef _WIN32
        path.replace("/","\\");
#endif
        QProcess::startDetached("explorer "+path);
    }
}

void MainWindow::slotSelectCurrent(const QModelIndex &index)
{

}

void MainWindow::slotCurrentPathChanged(const QString &path)
{
    m_currentPath = path;
    m_pFileSystemModel->setRootPath(m_currentPath);

    ui->tableView->setRootIndex(m_pFileSystemModel->index(m_currentPath));
    ui->tableView->scrollToTop();

    ui->addressbar->UpdateCurrentPath(path);
}

void MainWindow::onCompleterChoosed(const QString &path)
{
    QString pathStr = QString(path);
    pathStr.replace("\\", "/");
    ui->addressbar->UpdateCurrentPath(pathStr);
}

void MainWindow::onTextChanged(const QString &text)
{
    int index = text.lastIndexOf("/");
    QString dirStr = text.left(index + 1);
//    QString completeStr = text.right(text.length() - index -1);

    if(m_completerDir == dirStr) {
        return;
    }
    else{
        m_completerDir = dirStr;
    }

    QFileInfo info(dirStr);
    if(info.isDir()) {
        m_pCompleterModel->refresh(m_pCompleterModel->index(dirStr));
    }
}

void MainWindow::InitUI()
{
    m_currentPath = QString("C:/Windows/System32/sk-SK");
    m_pFileSystemModel = new FileSystemModel(this);
    m_pFileSystemModel->setRootPath(m_currentPath);
    ui->addressbar->setText(m_currentPath);

    ui->tableView->setModel(m_pFileSystemModel);
    ui->tableView->setRootIndex(m_pFileSystemModel->index(m_currentPath));
    ui->tableView->scrollToTop();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    ui->tableView->setShowGrid(false);  // disable the table grid.
    ui->tableView->verticalHeader()->setDefaultSectionSize(25);  // set row height.
    ui->tableView->horizontalHeader()->setHighlightSections(false);
    ui->tableView->setFrameShape(QFrame::NoFrame);
    ui->tableView->installEventFilter(this);

    m_pCompleterModel = new QDirModel(this);
    m_pCompleterModel->setReadOnly(false);
    m_pCompleterModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    m_pCompleter = new QCompleter(m_pCompleterModel, this);
    ui->addressbar->setCompleter(m_pCompleter);

    connect(m_pCompleter, SIGNAL(activated(const QString&)), this, SLOT(onCompleterChoosed(const QString&)));
    connect(ui->addressbar, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));

    connect(ui->tableView, &QTableView::doubleClicked, this, &MainWindow::slotShowChild);
    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::slotSelectCurrent);

    connect(ui->addressbar, &QtAddressBar::SCurrentPathChanged, this, &MainWindow::slotCurrentPathChanged);
}

void MainWindow::on_pushButton_clicked()
{
    m_currentPath = QString("C:/Windows/System32/sk-SK");
    ui->addressbar->UpdateCurrentPath(m_currentPath);
}

bool MainWindow::event(QEvent *event)
{
//    qDebug() << event->type();

    if(event->type() == QEvent::NonClientAreaMouseButtonPress || event->type() == QEvent::MouseButtonPress ||
       event->type() == QEvent::WindowDeactivate) {
        ui->addressbar->closeMenu();
    }

    return QMainWindow::event(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->tableView)
    {
//        qDebug() << event->type();
        if(event->type() == QEvent::FocusIn)
        {
            ui->addressbar->closeMenu();
        }
    }

    return QMainWindow::eventFilter(watched, event);
}
