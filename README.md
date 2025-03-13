# Smart-File-Categorization-and-Organization-System

![File Organizer Pro]([![temp-Image0-B7-Uu4.avif](https://i.postimg.cc/Lsf54F8T/temp-Image0-B7-Uu4.avif)](https://postimg.cc/xkfng7HJ))
![Qt](https://img.shields.io/badge/Qt-6.6.1-green)
![C++](https://img.shields.io/badge/C%2B%2B-17-orange)

A professional desktop application built with Qt and C++ that automatically organizes files into categories and subcategories based on their file extensions.

![File Organizer Pro Screenshot]([![temp-Image0-B7-Uu4.avif](https://i.postimg.cc/Lsf54F8T/temp-Image0-B7-Uu4.avif)](https://postimg.cc/xkfng7HJ))

## Features

- **Smart File Categorization**: Automatically sorts files into appropriate categories based on file extensions
- **Subcategory Support**: Creates nested folder structures for programming files (e.g., C++, Python, Java)
- **Professional UI**: Clean, modern interface with intuitive controls
- **Progress Tracking**: Real-time progress bar showing organization status
- **Detailed Statistics**: Provides comprehensive statistics after organization is complete
- **Empty Folder Detection**: Notifies when selected folders don't contain any files
- **File Collision Handling**: Automatically renames files to avoid overwriting existing ones

## Supported File Categories

File Organizer Pro supports the following file categories and subcategories:

### Images
- `.jpg`, `.jpeg`, `.png`, `.gif`, `.bmp`, `.tiff`, `.webp`, `.svg`

### Documents
- `.pdf`, `.doc`, `.docx`, `.txt`, `.rtf`, `.odt`, `.xls`, `.xlsx`, `.ppt`, `.pptx`

### Videos
- `.mp4`, `.avi`, `.mkv`, `.mov`, `.wmv`, `.flv`, `.webm`

### Audio
- `.mp3`, `.wav`, `.ogg`, `.m4a`, `.flac`, `.aac`

### Archives
- `.zip`, `.rar`, `.7z`, `.tar`, `.gz`, `.bz2`

### Code (with subcategories)
- **C++**: `.cpp`, `.cc`, `.cxx`, `.c++`
- **C**: `.c`
- **Headers**: `.h`, `.hpp`, `.hxx`
- **Python**: `.py`
- **Java**: `.java`
- **JavaScript**: `.js`
- **Web**: `.html`, `.css`
- **PHP**: `.php`
- **Swift**: `.swift`
- **Data**: `.json`, `.xml`

## Installation

### Prerequisites
- Qt 6.6.1 or later
- C++17 compatible compiler
- CMake 3.16 or later

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/noogler-aditya/smart-File-categorization-and-Organization-System.git
   cd smart-File-categorization-and-Organization-System
   ```

2. Create a build directory and run CMake:
   ```bash
   mkdir build && cd build
   cmake ..
   ```

3. Build the project:
   ```bash
   make
   ```

4. Run the application:
   ```bash
   ./file_organizer
   ```


## Usage

1. Launch the application
2. Click the "Select Folder" button to choose a folder to organize
3. The application will automatically:
   - Detect file types in the selected folder
   - Create appropriate category folders
   - Move files to their respective categories
   - Display progress in real-time
4. When complete, a summary dialog will show statistics about the organized files

## Development

### Project Structure

- `main.cpp` - Application entry point
- `mainwindow.h/cpp` - Main application window and logic
- `mainwindow.ui` - UI definition file
- `resources.qrc` - Application resources (icons, etc.)

### Building for Development

For development builds with debugging information:

```bash
mkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Acknowledgments

- [Qt Framework](https://www.qt.io/) for the GUI components
- [Feather Icons](https://feathericons.com/) for the application icons

## Contact

Your Name - [@yourusername](https://twitter.com/yourusername) - email@example.com

Project Link: [https://github.com/yourusername/file-organizer-pro](https://github.com/yourusername/file-organizer-pro) 
