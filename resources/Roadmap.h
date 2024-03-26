// MILESTONE #1:  
// Everything is local, on Windows, single thread
// TASK 1: Read video (frame after frame)
// TASK 2: If the current frame is very similar to the previous one - continue to the next one.
// 			Camera Part should not "send" an image if it's (almost) exactly the same as the previous one.
//			Examine the "absolute difference" between the current image and the last sent image every time.
//			--> Write a google test of your decision if to take a frame or not.
//
// TASK 3: Run detection with Yolo
//
// TASK 4: Calc average value per channel (averate R, average G, average B) of an image. 
//			Then, Calc average value per channel of a sub image (defined by detection rectangle)
//			--> Write a google test, in which you will create images in run time and verify you get the right sum.
while user not press Esc :
{
	// Camera Part
	Mat cap = read image from mp4 video on disc
		push image to queue // std::queue
		sleep 1 msec.

		//Backend Part
		read image from queue
		run detection with OpenCV & Yolo(v5 or later if possible) detect cars.
		For every detection :
	Calculate average color per channel in the detected rectangle(Average R, G, and B)
		save results to CSV //Columns: Time, Top, Left, Width, Height, AvgR, AvgG, AvgB

		// write a logs message with std::cout at every step from the above.
}

// MILESTONE #2:
// TASK 1: Use CMake instead of .sln in Visual Studio solution
// TASK 2: SQLite3 instead of CSV (google "C++ sqlite on windows")
// TASK 3: Use real logging library instead of std::cout (google "C++ logging library" and chooose one)
// TASK *(start now, continue over time): (large task!): Create "Admin" UI with the QT library, that shows the images and detections from the camera.

// MILESTONE #3:
// TASK 1: Use Multi threading: Separate the Camera part and the Backend part into two separate threads. (Use C++ multi threading, std::thread)
// TASK 2: The Queue must be THREAD SAFE to avoid race conditions. how will you do that?
// 			Write your own Queue class. Use C++ cross platform utilities (https://en.cppreference.com/w/cpp/thread/mutex)
//		--> Write a unit test to try and reach a race condition. make sure it doesn't get there.
// TASK 3: Make the queue a circular queue with 5 frames, reduce the frame rate of the camera to 3 fps.
//		--> Write some unit tests, in particular - that the 6th frame overrides the 1st one.
while user not press Esc :
{
	// Camera Thread
	while true:
	cap = read image from webcam
		push image to queue
		sleep 1 sec.
}

// Backend Thread
while user not press Esc :
{
	read image from queue
		run detection with OpenCV& Yolo(v5 or later if possible)
		save results to DB(sqlite)
}

// MILESTONE #4:
// TASK 1: Use gRPC for comunication between camera and backend threads
// TASK 2: Use multi PROCESSING instead of multi threading
// 			NOTE: You will a queue only on the backend side, no? (the camera reads an image and sends it)
//			NOTE: You can do more than one startup project in Visual Studio.
//
// TASK 3: That's about time to add configuration to your product. 
//			Use a json file (google around about how can you read json with C++) as configuration.
//			The threshold of the camera, size of the queue on backend side, 
//			the IP and port of the camera and backend - they all should be configurable!

// MILESTONE #5:
// TASK 1: Run camera process on WSL2. Adjust the config accordingly.
// TASK 2: Wrap the camera process with a dockerfile
// TASK 3: Run the camera process on WSL2 on a different computer.


// MILESTONE #6:
// TASK 1: Run camera process on RP [*!]
// TASK 2: Add distance sensor (Ultra Sonic) to RP, 
// 		add another gRPC message/function to handle "someone is close" alerts
