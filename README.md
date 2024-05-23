# CCTV

<div align="center">
   &nbsp;&nbsp;<img align="right" height="200" style="border-radius:20px;" src="/resources/app.png" />&nbsp;&nbsp;
</div>


CCTV is a project that performs motion detection, object detection, and recognition. It can be used for various purposes such as counting specific objects or detecting, recognizing, and capturing thieves.


&nbsp;&nbsp;

&nbsp;&nbsp;

## Features

- **Real-time Motion Detection**: The system supports real-time motion detection, allowing it to monitor and respond to movement instantly.
- **Object Recognition**: It can recognize multiple types of objects, making it versatile for various applications.
- **Image/Video Capture**: The project includes functionality to capture and store images or videos of detected objects.
- **Detection Mode Toggle**: Users can easily switch detection modes using a button or checkbox in the interface.
- **Cross-Platform Compatibility**: The project is designed to work across multiple platforms, ensuring broad usability.
- **Beautiful GUI with UX**: The system includes a user-friendly graphical interface designed with user experience in mind.
- **High FPS Detection with GPU Acceleration**: Optional support for high FPS detection using CUDA for GPU acceleration.
  
<details>
  <summary><H2>Installation</H2></summary>

   1. **Clone the Project**:
      ```sh
      git clone https://github.com/MeirMeshulami/RT-Image-Processing.git
      cd RT-Image-Processing
      ```
   
   <div align="center">
      <img align="right" height="170" style=border-radius:20px src="/resources/detection_image.png"  />
   </div>
   
   2. **Install [OpenCV](https://opencv.org/releases/), [gRPC](https://github.com/grpc/grpc), and [CMake](https://cmake.org/download/).**
      - Follow the instructions on their respective websites for installation.
     
&nbsp;

3. **Optional: Install CUDA**:

     - For high FPS detection with GPU acceleration, install CUDA from the [NVIDIA website](https://developer.nvidia.com/cuda-downloads) and [cuDNN](https://developer.nvidia.com/cudnn).
     - Build OpenCV with the extra modules ([opencv_contrib](https://github.com/opencv/opencv_contrib)) and CUDA support. Follow these steps:
     - You can follow this [tutorial](https://techzizou.in/setup-opencv-dnn-cuda-module-for-windows/) for a detailed guide on setting up OpenCV with the CUDA DNN module on Windows.
       
       ```sh
       git clone https://github.com/opencv/opencv.git
       git clone https://github.com/opencv/opencv_contrib.git
       cd opencv
       mkdir build
       cd build
       cmake -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules -DWITH_CUDA=ON ..
       cmake --build . --config Release
       cmake --install . --config Release
       ```
</details>
<details>
  <summary><H2>Build and Run</H2></summary>
   
   4. **Build the server**:
   
       ```sh
       cd server
       mkdir build
       cd build
       cmake ..
       cmake --build . --config Release
       ```
   
   5. **Build the Client**:
      
       ```sh
       cd ../../client
       mkdir build
       cd build
       cmake ..
       cmake --build . --config Release
       ```
   
   6. **Run the Application**:&nbsp;
      &nbsp;
      - Run the camera application:
       
       ```sh
       Release\camera.exe
       ```
       
      - Run the GUI application:
       &nbsp;
       ```sh
       Release\GUI.exe
       ```
</details>

## Contact Information
For support or questions, please contact meir.meshoulami@gmail.com.

###

<div align="center">
    <img align="center" height="150" style=border-radius:20px; src="/resources/yolov5.png"  />
</div>

###

<div align="center">
  <img src="https://miro.medium.com/v2/resize:fit:560/0*OhqRsVej30htIkDL.png" height="30" alt="grpc logo"  />
  <img width="12" />
  <img src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/cplusplus/cplusplus-original.svg" height="30" alt="cplusplus logo"  />
  <img width="12" />
  <img src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/opencv/opencv-original.svg" height="30" alt="opencv logo"  />
  <img width="12" />
  <img src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/cmake/cmake-original.svg" height="30" alt="cmake logo"  />
  <img width="12" />
  <img src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/linux/linux-original.svg" height="30" alt="linux logo"  />
  <img width="12" />
  <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/8/81/Qt_logo_neon_2022.svg/224px-Qt_logo_neon_2022.svg.png" height="30" alt="QT logo"  />
  <img width="12" />
  <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/3/38/SQLite370.svg/382px-SQLite370.svg.png" height="30" alt="SQlite logo"  />
  <img width="12" />
  <img src="https://upload.wikimedia.org/wikipedia/commons/7/79/Docker_%28container_engine%29_logo.png" height="30" alt="Docker logo"  />
</div>



