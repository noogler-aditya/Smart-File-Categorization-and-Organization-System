#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QString>
#include <QMap>
#include <QThread>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <filesystem>
#include <string>
#include <QSet>
#include <QDebug>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class FileOrganizer : public QObject
{
    Q_OBJECT

public:
    explicit FileOrganizer(QObject *parent = nullptr) : QObject(parent) {}

    void organize(const QString& path, const QMap<QString, QString>& categories);

signals:
    void progressUpdated(int value, const QString& currentFile);
    void organizationCompleted(int totalProcessed);
    void errorOccurred(const QString& error);
    void statusUpdated(const QString& status);

private:
    void createDirectories(const QString& basePath);
    QString getFileCategory(const std::filesystem::path& file, const QMap<QString, QString>& categories);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getSelectedFolder() const { return selectedFolderPath; }

private slots:
    void selectFolder();
    void handleOrganizationComplete(int totalProcessed);
    void handleError(const QString& error);
    void showAboutDialog();

private:
    Ui::MainWindow *ui;
    QString selectedFolderPath;
    int totalFiles;
    int processedFiles;
    bool isOrganizing;
    
    void resetProgress();
    void updateFolderDisplay();
    void updateProgress(int value, const QString& status);
    void createDirectories();
    QString getFileCategory(const std::filesystem::path& file);
    void organizeFiles();
    void startFileOrganization();
    int countFiles(const std::filesystem::path& path);
    QSet<QString> detectRequiredCategories();
    
    // Helper method to create and show a styled message box
    int showStyledMessageBox(const QString& title, QMessageBox::Icon icon, 
                           const QString& text, const QString& informativeText = QString(),
                           bool isError = false);

    // File categories map
    const QMap<QString, QString> fileCategories = {
        // Images
        {".jpg", "Images"}, {".jpeg", "Images"}, {".png", "Images"},
        {".gif", "Images"}, {".bmp", "Images"}, {".tiff", "Images"},
        {".webp", "Images"}, {".svg", "Images"},
        
        // Documents
        {".pdf", "Documents"}, {".doc", "Documents"}, {".docx", "Documents"},
        {".txt", "Documents"}, {".rtf", "Documents"}, {".odt", "Documents"},
        {".xls", "Documents"}, {".xlsx", "Documents"}, {".ppt", "Documents"},
        {".pptx", "Documents"},
        
        // Videos
        {".mp4", "Videos"}, {".avi", "Videos"}, {".mkv", "Videos"},
        {".mov", "Videos"}, {".wmv", "Videos"}, {".flv", "Videos"},
        {".webm", "Videos"},
        
        // Audio
        {".mp3", "Audio"}, {".wav", "Audio"}, {".ogg", "Audio"},
        {".m4a", "Audio"}, {".flac", "Audio"}, {".aac", "Audio"},
        
        // Archives
        {".zip", "Archives"}, {".rar", "Archives"}, {".7z", "Archives"},
        {".tar", "Archives"}, {".gz", "Archives"}, {".bz2", "Archives"},
        
        // Code - with subcategories
        {".cpp", "Code/C++"}, {".cc", "Code/C++"}, {".cxx", "Code/C++"}, {".c++", "Code/C++"},
        {".c", "Code/C"}, 
        {".h", "Code/Headers"}, {".hpp", "Code/Headers"}, {".hxx", "Code/Headers"},
        {".py", "Code/Python"}, 
        {".java", "Code/Java"}, 
        {".js", "Code/JavaScript"}, 
        {".html", "Code/Web"}, {".css", "Code/Web"}, 
        {".php", "Code/PHP"}, 
        {".swift", "Code/Swift"},
        {".json", "Code/Data"}, {".xml", "Code/Data"}
    };
};

#endif // MAINWINDOW_H 