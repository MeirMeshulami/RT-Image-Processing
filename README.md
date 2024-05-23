# CCTV

CCTV is a project that performs motion detection, object detection, and recognition. It can be used for various purposes such as counting specific objects or detecting, recognizing, and capturing thieves.

## Features
- **Real-time Motion Detection**: The system supports real-time motion detection, allowing it to monitor and respond to movement instantly.
- **Object Recognition**: It can recognize multiple types of objects, making it versatile for various applications.
- **Image/Video Capture**: The project includes functionality to capture and store images or videos of detected objects.
- **Detection Mode Toggle**: Users can easily switch detection modes using a button or checkbox in the interface.
- **Cross-Platform Compatibility**: The project is designed to work across multiple platforms, ensuring broad usability.
- **Beautiful GUI with UX**: The system includes a user-friendly graphical interface designed with user experience in mind.

## Installation
1. **Clone the Project**:
   ```sh
   git clone https://github.com/MeirMeshulami/RT-Image-Processing.git
   cd RT-Image-Processing

2. **Install OpenCV, gRPC, and CMake.**
   - Follow the instructions on their respective websites for installation.

3. **Build the Server**:
    ```sh
    cd server
    mkdir build
    cd build
    cmake ..
    make

4. **Build the Client**:
    ```sh
    cd ../../client
    mkdir build
    cd build
    cmake ..
    make

5. **Run the Application**:
  - Run the camera application:
    ```sh
    ./camera.exe

  - Run the GUI application:
    ```sh
    ./GUI.exe

## Contact Information
For support or questions, please contact meir.meshoulami@gmail.com.




