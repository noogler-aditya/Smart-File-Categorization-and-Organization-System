#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QStyle>
#include <QMessageBox>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <filesystem>
#include <stdexcept>
#include <QMap>
#include <QSet>
#include <QScreen>
#include <QDebug>
#include <QPixmap>
#include <QDate>

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , selectedFolderPath("")
    , totalFiles(0)
    , processedFiles(0)
    , isOrganizing(false)
{
    ui->setupUi(this);
    
    // Set window position to center of screen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);
    
    // Set window icon if available
    QIcon appIcon(":/icons/app_icon.png");
    if (!appIcon.isNull()) {
        this->setWindowIcon(appIcon);
    }
    
    // Connect the button click event
    connect(ui->selectFolderButton, &QPushButton::clicked, this, &MainWindow::selectFolder);
    
    // Connect menu actions
    connect(ui->actionSelect_Folder, &QAction::triggered, this, &MainWindow::selectFolder);
    connect(ui->actionExit, &QAction::triggered, this, &QApplication::quit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);
    
    // Update footer with current year
    int currentYear = QDate::currentDate().year();
    ui->footerLabel->setText(tr("© %1 File Organizer Pro - All Rights Reserved").arg(currentYear));
    
    // Initialize progress bar
    resetProgress();
    
    // Set default logo if not found
    if (ui->logoLabel->pixmap().isNull()) {
        // Create a colored square as a placeholder
        QPixmap placeholderLogo(64, 64);
        placeholderLogo.fill(QColor("#0d6efd"));
        ui->logoLabel->setPixmap(placeholderLogo);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetProgress()
{
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    ui->progressBar->setFormat("%p% (%v/%m files)");
    ui->progressBar->setAlignment(Qt::AlignCenter);
    ui->statusLabel->setText(tr("Ready to organize your files"));
    totalFiles = 0;
    processedFiles = 0;
}

int MainWindow::countFiles(const fs::path& path)
{
    int count = 0;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().parent_path() == path) {
            count++;
        }
    }
    return count;
}

void MainWindow::selectFolder()
{
    if (isOrganizing) {
        QMessageBox::warning(this, tr("Warning"),
                           tr("Please wait for the current organization to complete."));
        return;
    }

    QString folder = QFileDialog::getExistingDirectory(
        this,
        tr("Select Folder to Organize"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!folder.isEmpty()) {
        selectedFolderPath = folder;
        updateFolderDisplay();
        startFileOrganization();
    }
}

void MainWindow::updateFolderDisplay()
{
    if (selectedFolderPath.isEmpty()) {
        ui->statusLabel->setText(tr("No folder selected"));
        ui->selectFolderButton->setToolTip(tr("Click to select a folder"));
        resetProgress();
    } else {
        QString displayPath = selectedFolderPath;
        if (displayPath.length() > 50) {
            QString firstPart = displayPath.left(25);
            QString lastPart = displayPath.right(22);
            displayPath = firstPart + "..." + lastPart;
        }
        
        ui->statusLabel->setText(tr("Selected folder: %1").arg(displayPath));
        ui->selectFolderButton->setToolTip(selectedFolderPath);
    }
}

void MainWindow::updateProgress(int value, const QString& status)
{
    ui->progressBar->setValue(value);
    ui->progressBar->setFormat(QString("%p% (%1/%2 files)").arg(processedFiles).arg(totalFiles));
    if (!status.isEmpty()) {
        ui->statusLabel->setText(status);
    }
}

QSet<QString> MainWindow::detectRequiredCategories()
{
    QSet<QString> requiredCategories;
    fs::path sourcePath(selectedFolderPath.toStdString());
    
    // Debug: Print all file extensions found
    qDebug() << "Detecting categories from files:";
    
    for (const auto& entry : fs::directory_iterator(sourcePath)) {
        if (entry.is_regular_file() && entry.path().parent_path() == sourcePath) {
            QString extension = QString::fromStdString(entry.path().extension().string()).toLower();
            
            // Debug: Print each file and its extension
            qDebug() << "File:" << QString::fromStdString(entry.path().filename().string()) 
                     << "Extension:" << extension;
            
            if (fileCategories.contains(extension)) {
                QString category = fileCategories[extension];
                requiredCategories.insert(category);
                
                // For subcategories, also add the parent category
                if (category.contains('/')) {
                    QString parentCategory = category.split('/').first();
                    requiredCategories.insert(parentCategory);
                    qDebug() << "  -> Added parent category:" << parentCategory;
                }
                
                // Debug: Print category assignment
                qDebug() << "  -> Category:" << category;
            }
        }
    }
    
    // Debug: Print all detected categories
    qDebug() << "Detected categories:" << requiredCategories;
    
    return requiredCategories;
}

void MainWindow::createDirectories()
{
    QDir baseDir(selectedFolderPath);
    QSet<QString> categories = detectRequiredCategories();
    
    updateProgress(0, tr("Creating category folders..."));
    
    // Debug: Print categories being created
    qDebug() << "Creating directories for categories:" << categories;
    
    for (const QString& category : categories) {
        // Handle subcategories (paths with slashes)
        if (category.contains('/')) {
            QStringList parts = category.split('/');
            QString currentPath = selectedFolderPath;
            
            // Create each level of the path
            for (const QString& part : parts) {
                currentPath = QDir::cleanPath(currentPath + QDir::separator() + part);
                QDir dir(currentPath);
                
                if (!dir.exists() && !dir.mkpath(".")) {
                    throw std::runtime_error("Failed to create directory: " + currentPath.toStdString());
                }
                
                qDebug() << "Created directory:" << currentPath;
            }
        } 
        // Handle regular categories (no slashes)
        else if (!baseDir.mkdir(category)) {
            if (!baseDir.exists(category)) {
                throw std::runtime_error("Failed to create directory: " + category.toStdString());
            }
        }
    }
}

QString MainWindow::getFileCategory(const fs::path& file)
{
    QString extension = QString::fromStdString(file.extension().string()).toLower();
    
    // Debug: Print extension lookup with clear markers
    qDebug() << "\n=== File Category Lookup ===";
    qDebug() << "File:" << QString::fromStdString(file.filename().string());
    qDebug() << "Raw extension:" << QString::fromStdString(file.extension().string());
    qDebug() << "Lowercase extension:" << extension;
    qDebug() << "Extension exists in map:" << fileCategories.contains(extension);
    
    // Print all recognized extensions for debugging
    qDebug() << "Recognized extensions:";
    for (auto it = fileCategories.constBegin(); it != fileCategories.constEnd(); ++it) {
        if (it.value() == "Code") {
            qDebug() << "  " << it.key() << " -> " << it.value();
        }
    }
    
    // Only return a category if the extension is recognized
    if (fileCategories.contains(extension)) {
        QString category = fileCategories[extension];
        qDebug() << "Found category:" << category;
        qDebug() << "=========================\n";
        return category;
    }
    
    // Debug: No category found
    qDebug() << "No category found for extension:" << extension;
    qDebug() << "=========================\n";
    
    // Return empty string for unrecognized files
    return QString();
}

void MainWindow::organizeFiles()
{
    qDebug() << "\n=== Starting File Organization ===";
    fs::path sourcePath(selectedFolderPath.toStdString());
    
    // Count files in the root directory only
    totalFiles = countFiles(sourcePath);
    processedFiles = 0;
    
    qDebug() << "Total files to process:" << totalFiles;
    qDebug() << "Source path:" << QString::fromStdString(sourcePath.string());
    
    // Update progress bar range
    ui->progressBar->setRange(0, totalFiles > 0 ? totalFiles : 1);
    ui->progressBar->setFormat(QString("%p% (%1/%2 files)").arg(processedFiles).arg(totalFiles));
    
    if (totalFiles == 0) {
        updateProgress(100, tr("No files to organize!"));
        qDebug() << "No files found to organize";
        qDebug() << "=== Organization Complete ===\n";
        
        // Show a message if the folder doesn't contain any files
        showStyledMessageBox(
            tr("No Files Found"),
            QMessageBox::Information,
            tr("No files were found in the selected folder."),
            tr("Make sure the folder contains files in the root directory (not in subfolders).")
        );
        return;
    }
    
    // Process files
    for (const auto& entry : fs::directory_iterator(sourcePath)) {
        if (entry.is_regular_file() && entry.path().parent_path() == sourcePath) {
            try {
                fs::path currentFile = entry.path();
                QString filename = QString::fromStdString(currentFile.filename().string());
                QString extension = QString::fromStdString(currentFile.extension().string()).toLower();
                
                qDebug() << "\n--- Processing File ---";
                qDebug() << "Full path:" << QString::fromStdString(currentFile.string());
                qDebug() << "Filename:" << filename;
                qDebug() << "Raw extension:" << QString::fromStdString(currentFile.extension().string());
                qDebug() << "Lowercase extension:" << extension;
                
                QString category = getFileCategory(currentFile);
                
                // Skip files that don't have a recognized category
                if (category.isEmpty()) {
                    qDebug() << "Skipping file (no category found):" << filename;
                    processedFiles++;
                    updateProgress(processedFiles, tr("Skipped: %1").arg(filename));
                    continue;
                }
                
                fs::path targetDir = sourcePath / category.toStdString();
                fs::path targetPath = targetDir / currentFile.filename();
                
                qDebug() << "Moving file to category:" << category;
                qDebug() << "Target directory:" << QString::fromStdString(targetDir.string());
                qDebug() << "Target path:" << QString::fromStdString(targetPath.string());
                
                // If target file already exists, append a number
                int counter = 1;
                while (fs::exists(targetPath)) {
                    fs::path stem = targetPath.stem();
                    targetPath = targetDir / (stem.string() + "_" + std::to_string(counter++) + currentFile.extension().string());
                }
                
                fs::rename(currentFile, targetPath);
                
                processedFiles++;
                updateProgress(processedFiles, tr("Organized: %1").arg(filename));
            }
            catch (const std::exception& e) {
                QMessageBox::warning(this, tr("Error"), tr("Failed to move file: %1").arg(e.what()));
            }
        }
    }
}

void MainWindow::startFileOrganization()
{
    try {
        isOrganizing = true;
        ui->selectFolderButton->setEnabled(false);
        
        resetProgress();
        
        // Check if the folder is empty before proceeding
        fs::path sourcePath(selectedFolderPath.toStdString());
        bool hasFiles = false;
        
        for (const auto& entry : fs::directory_iterator(sourcePath)) {
            if (entry.is_regular_file() && entry.path().parent_path() == sourcePath) {
                hasFiles = true;
                break;
            }
        }
        
        if (!hasFiles) {
            // Show a message if the folder doesn't contain any files
            showStyledMessageBox(
                tr("Empty Folder"),
                QMessageBox::Information,
                tr("The selected folder does not contain any files to organize."),
                tr("Please select a folder that contains files.")
            );
            
            // Reset UI state
            isOrganizing = false;
            ui->selectFolderButton->setEnabled(true);
            ui->statusLabel->setText(tr("Selected folder is empty. Please choose another folder."));
            return;
        }
        
        createDirectories();
        organizeFiles();
        
        handleOrganizationComplete(processedFiles);
    }
    catch (const std::exception& e) {
        handleError(tr("An error occurred: %1").arg(e.what()));
        updateProgress(0, tr("Organization failed!"));
        isOrganizing = false;
        ui->selectFolderButton->setEnabled(true);
    }
}

void MainWindow::handleOrganizationComplete(int totalProcessed)
{
    isOrganizing = false;
    ui->selectFolderButton->setEnabled(true);

    // Update progress bar to 100%
    ui->progressBar->setValue(100);

    // Create statistics for each category
    QMap<QString, int> categoryStats;
    fs::path sourcePath(selectedFolderPath.toStdString());
    
    // Only count files in directories that actually exist
    QDir baseDir(selectedFolderPath);
    QStringList existingDirs = baseDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    
    for (const QString& category : existingDirs) {
        fs::path categoryPath = sourcePath / category.toStdString();
        int fileCount = 0;
        
        if (fs::exists(categoryPath)) {
            // Count files in the main category directory
            for (const auto& entry : fs::directory_iterator(categoryPath)) {
                if (entry.is_regular_file()) {
                    fileCount++;
                }
            }
            
            // Count files in subcategories
            QDir categoryDir(QString::fromStdString(categoryPath.string()));
            QStringList subDirs = categoryDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            
            for (const QString& subDir : subDirs) {
                QString subCategoryName = category + "/" + subDir;
                fs::path subCategoryPath = categoryPath / subDir.toStdString();
                int subFileCount = 0;
                
                for (const auto& entry : fs::directory_iterator(subCategoryPath)) {
                    if (entry.is_regular_file()) {
                        subFileCount++;
                    }
                }
                
                if (subFileCount > 0) {
                    categoryStats[subCategoryName] = subFileCount;
                }
            }
        }
        
        if (fileCount > 0) {
            categoryStats[category] = fileCount;
        }
    }

    // Create detailed completion message
    QString completionDetails;
    completionDetails = tr("File Organization Complete!\n\nStatistics:\n");
    completionDetails += tr("Total files processed: %1\n\n").arg(totalProcessed);
    completionDetails += tr("Files by category:\n");
    
    // Group statistics by main category
    QMap<QString, QStringList> groupedStats;
    int mainCategoryTotal = 0;
    QString currentMainCategory;
    
    for (auto it = categoryStats.constBegin(); it != categoryStats.constEnd(); ++it) {
        QString category = it.key();
        int count = it.value();
        
        if (category.contains('/')) {
            // This is a subcategory
            QString mainCategory = category.split('/').first();
            QString subCategory = category.split('/').last();
            
            if (!groupedStats.contains(mainCategory)) {
                groupedStats[mainCategory] = QStringList();
            }
            
            groupedStats[mainCategory].append(tr("  - %1: %2 files").arg(subCategory).arg(count));
        } else {
            // This is a main category
            mainCategoryTotal = count;
            currentMainCategory = category;
            
            if (!groupedStats.contains(category)) {
                groupedStats[category] = QStringList();
            }
            
            groupedStats[category].prepend(tr("- %1: %2 files").arg(category).arg(count));
        }
    }
    
    // Format the statistics with proper indentation
    for (auto it = groupedStats.constBegin(); it != groupedStats.constEnd(); ++it) {
        QString mainCategory = it.key();
        QStringList subStats = it.value();
        
        for (const QString& stat : subStats) {
            completionDetails += stat + "\n";
        }
    }

    // Update status label
    QString statusMessage = tr("Organization complete! Processed %1 files.").arg(totalProcessed);
    ui->statusLabel->setText(statusMessage);

    // Show completion dialog with statistics
    QMessageBox completionBox(this);
    completionBox.setWindowTitle(tr("Organization Complete"));
    completionBox.setIcon(QMessageBox::Information);
    completionBox.setText(statusMessage);
    completionBox.setDetailedText(completionDetails);
    
    // Style the message box
    completionBox.setStyleSheet(
        "QMessageBox {"
        "    background-color: #f0f0f0;"
        "}"
        "QMessageBox QLabel {"
        "    color: #424242;"
        "    font-size: 14px;"
        "}"
        "QPushButton {"
        "    background-color: #2196F3;"
        "    color: white;"
        "    border: none;"
        "    padding: 8px 16px;"
        "    border-radius: 4px;"
        "    font-size: 13px;"
        "    min-width: 100px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1976D2;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0D47A1;"
        "}"
        "QTextEdit {"
        "    background-color: white;"
        "    border: 1px solid #E0E0E0;"
        "    border-radius: 4px;"
        "    padding: 8px;"
        "    color: #424242;"
        "    font-size: 13px;"
        "}"
    );
    
    // Add custom button
    QPushButton *openFolderButton = completionBox.addButton(tr("Open Folder"), QMessageBox::ActionRole);
    completionBox.addButton(QMessageBox::Ok);

    // Center the dialog on the main window
    completionBox.setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            completionBox.size(),
            this->geometry()
        )
    );

    completionBox.exec();

    // Handle button click
    if (completionBox.clickedButton() == openFolderButton) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(selectedFolderPath));
    }
}

void MainWindow::handleError(const QString& error)
{
    showStyledMessageBox(
        tr("Error"),
        QMessageBox::Warning,
        error,
        QString(),
        true // isError = true
    );
}

int MainWindow::showStyledMessageBox(const QString& title, QMessageBox::Icon icon, 
                                   const QString& text, const QString& informativeText,
                                   bool isError)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setIcon(icon);
    msgBox.setText(text);
    
    if (!informativeText.isEmpty()) {
        msgBox.setInformativeText(informativeText);
    }
    
    msgBox.setStandardButtons(QMessageBox::Ok);
    
    // Apply styling based on whether it's an error or not
    if (isError) {
        // Error styling
        msgBox.setStyleSheet(
            "QMessageBox {"
            "    background-color: #f8f9fa;"
            "}"
            "QMessageBox QLabel {"
            "    color: #D32F2F;"
            "    font-size: 14px;"
            "    font-weight: bold;"
            "}"
            "QPushButton {"
            "    background-color: #D32F2F;"
            "    color: white;"
            "    border: none;"
            "    padding: 8px 16px;"
            "    border-radius: 4px;"
            "    font-size: 13px;"
            "    min-width: 100px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #C62828;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #B71C1C;"
            "}"
        );
    } else {
        // Standard styling
        msgBox.setStyleSheet(
            "QMessageBox {"
            "    background-color: #f8f9fa;"
            "}"
            "QMessageBox QLabel {"
            "    color: #0d6efd;"
            "    font-size: 14px;"
            "}"
            "QPushButton {"
            "    background-color: #0d6efd;"
            "    color: white;"
            "    border: none;"
            "    padding: 8px 16px;"
            "    border-radius: 4px;"
            "    font-size: 13px;"
            "    min-width: 100px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #0b5ed7;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #0a58ca;"
            "}"
        );
    }
    
    // Center the dialog on the main window
    msgBox.setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            msgBox.size(),
            this->geometry()
        )
    );
    
    return msgBox.exec();
}

void MainWindow::showAboutDialog()
{
    showStyledMessageBox(
        tr("About File Organizer Pro"),
        QMessageBox::Information,
        tr("<h3>File Organizer Pro</h3>"),
        tr("<p>A professional file organization tool that automatically sorts your files "
           "into categories and subcategories.</p>"
           "<p>Version 1.0.0</p>"
           "<p>Developed with Qt 6</p>")
    );
} 