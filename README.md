# QR Code–Based Metro Ticketing System

A modern, cross-platform C++ and Python application for seamless metro ticketing, secure QR code validation, and digital wallet payments.  
Built as an advanced Object-Oriented Programming (OOP) project, this system brings real-world automation to public transport ticketing using **C++**, **OpenCV**, and **QR Code** technologies.

## Project Overview

This ticketing solution combines robust OOP architecture and efficient C++ modules to let commuters:

- **Register & Login:** Self-service user creation, profile, and payment management with digital wallet support.
- **Book & Pay for Tickets:**  Choose routes, pay with Bank Transfer, JazzCash, or EasyPaisa, and receive an instant, personalized ticket.
- **QR-Based Secure Entry:** Each booked ticket is encoded into a QR code containing user and travel info for fast, secure scan-in at stations.
- **Admin Controls:** Manage stations, view statistics, and monitor ticketing history.
- **Staff/Feedback System:** Collect station feedback and view overall ratings for service improvement.
- **JSON-Powered Data Management:** All data is managed efficiently with JSON for portability and transparency.

## Tech Stack

- **C++** (primary backend)  
- **OpenCV** (QR code generation/decoding & image handling)  
- **Python (with pyzbar, opencv-python, numpy)** (cross-platform QR code scanning interface)
- **JSON** (as universal data storage)
- **ZBar** (native library for barcode/QR code detection in Python)

## Features

- **Multi-Channel Payments**: Real-time wallet updates after ticket purchase.
- **Unique, Secure Tickets**: Each ticket is personalized and validated at entry via QR.
- **Station Management**: Add, remove, and manage station data with admin controls.
- **Comprehensive OOP Design**: Employs inheritance, polymorphism, encapsulation, composition, and aggregation.
- **Cross-Platform Compatible**: Run smoothly on Windows, Linux (Ubuntu/WSL), and macOS.

## Installation & Setup

### **Step-by-step instructions by OS**

### Linux (Ubuntu/WSL)

1. **Install OpenCV**
   ```bash
   sudo apt-get update
   sudo apt-get install libopencv-dev
   ```

2. **Install Python 3 and QR Libraries**
   ```bash
   sudo apt-get install python3-opencv libzbar0 python3-venv python3-dev build-essential pkg-config
   ```

3. **Set up Python environment**
   ```bash
   python3 -m venv venv
   source venv/bin/activate
   pip install pyzbar opencv-python numpy
   ```

4. **Build the C++ application**
   ```bash
   g++ -std=c++17 main.cpp payments.cpp tickets-QRgen.cpp qrcodegen.cpp -o metro $(pkg-config --cflags --libs opencv4)
   ```

5. **Ensure files are present:**
    - `passenger-staff.h`  `stations-metro.h`  `payments.h`  `payments.cpp`
   - `tickets-QRgen.cpp`   `tickets-QRgen.h` `QRdecode.h` `QRscanner.py`
   - `qrcodegen.hpp` `qrcodegen.cpp` `json.hpp`

6. **Run the application**
   ```bash
   ./metro
   ```

### Windows

1. **Install Python 3**  
   Download from [python.org](https://python.org) and ensure “Add to PATH” is checked.  
   Verify it works:
   ```cmd
   python --version
   ```

2. **Install required Python packages**
   ```cmd
   pip install opencv-python pyzbar numpy
   ```

3. **(If needed) Install Visual C++ Redistributable**  
   Download and install from Microsoft’s [official site](https://aka.ms/vs/17/release/vc_redist.x64.exe).

4. **Build the C++ application**

   Using `pkg-config` and OpenCV for MSYS2/WSL:
   ```cmd
   g++ -std=c++17 main.cpp payments.cpp tickets-QRgen.cpp qrcodegen.cpp -o metro $(pkg-config --cflags --libs opencv4)
   ```

   Using MinGW & OpenCV (replace versions if needed):
   ```cmd
   g++ -std=c++17 main.cpp qrdecode.cpp stations-metro.cpp ^
     -I"C:/opencv/build/include" ^
     -L"C:/opencv/build/x64/mingw/lib" ^
     -lopencv_core450 -lopencv_imgproc450 -lopencv_highgui450 ^
     -o metro.exe
   ```

5. **Ensure these files are in the same folder:**
    - `passenger-staff.h`  `stations-metro.h`  `payments.h`  `payments.cpp`
   - `tickets-QRgen.cpp`   `tickets-QRgen.h` `QRdecode.h` `QRscanner.py`
   - `qrcodegen.hpp`  `qrcodegen.cpp` `json.hpp`

6. **Run your application**
   ```cmd
   metro.exe
   ```

### macOS

1. **Install Homebrew (if not already installed)**
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. **Install necessary libraries**
   ```bash
   brew install python zbar
   pip3 install opencv-python pyzbar numpy
   ```

3. **(If needed) Symlink ZBar**
   ```bash
   mkdir -p ~/lib
   ln -s "$(brew --prefix zbar)/lib/libzbar.dylib" ~/lib/libzbar.dylib
   ```

4. **Build the C++ app**
   ```bash
   g++ -std=c++17 main.cpp payments.cpp tickets-QRgen.cpp qrcodegen.cpp -o metro $(pkg-config --cflags --libs opencv4)
   ```

5. **Ensure QR scanner and output file exist:**
    - `passenger-staff.h`  `stations-metro.h`  `payments.h`  `payments.cpp`
   - `tickets-QRgen.cpp`   `tickets-QRgen.h` `QRdecode.h` `QRscanner.py`
   - `qrcodegen.hpp`  `qrcodegen.cpp` `json.hpp`

6. **Run the app**
   ```bash
   ./metro
   ```

## Program Workflow

1. **Run the app**  
   Launch the C++ executable.
2. **Choose menu option (terminal menu-driven UI)**  
   Register, login, book a ticket, validate via QR, provide feedback, manage admin/staff features.
3. **(When prompted) Scan QR in camera window**  
   Python script opens camera, captures QR, exports decoded data to JSON.
4. **C++ app reads/validates ticket info**  
   Makes sure the ticket is valid for entry.
5. **Enjoy hassle-free, paperless metro travel!**




## Why This Project?

This system bridges the gap between robust academic OOP concepts and real-world transport automation. It exemplifies how modular C++ and cross-platform tools can build smart, future-ready solutions for urban mobility.

**Let’s make public transit smarter, together!**