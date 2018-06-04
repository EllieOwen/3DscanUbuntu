#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <OpenNI.h>

#include "OniSampleUtilities.h"

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

using namespace openni;

int main(int argc, const char *argv[])
{

//printf("argv0 %s, argv1 %s\n", argv[0], argv[1]);
	Status rc = OpenNI::initialize();
//printf("OpenNI::initialize() rc = %d\n",rc);
	if (rc != STATUS_OK)
	{
		printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
		return 1;
	}
//		printf("Initialize not failed\n%s\n", OpenNI::getExtendedError());

	Device device;
	if (argc > 1)
	{
		const char *myDir="RecordedScans/";
		char myPath[100];
		strcpy(myPath,myDir);
		strcat(myPath,argv[1]);

		rc = device.open(myPath);
	}
	rc = device.open(ANY_DEVICE);
      
//printf("device.open(ANY_DEVICE) rc = %d\n",rc);

	if (rc != STATUS_OK)
	{
		printf("Couldn't Open device\n%s\n", OpenNI::getExtendedError());
		return 2;
	}

	VideoStream depth;

	if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
	{
		rc = depth.create(device, SENSOR_DEPTH);        
//printf("depth.create(device, SENSOR_DEPTH) rc = %d\n",rc);
		if (rc != STATUS_OK)
		{
			printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
			return 3;
		}
	}

	rc = depth.start();        
//printf("depth.start() rc = %d\n",rc);
	if (rc != STATUS_OK)
	{
		printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
		return 4;
	}

//	Recorder recorder;
//	recorder.create("RecordedScans/myFrameStream");
//	recorder.attach(depth);
//	recorder.start();

	VideoFrameRef frame;
//	int frameNumber = 0;

	while (!wasKeyboardHit())
	{
//		printf("Frame Number %d\n",++frameNumber);
		int changedStreamDummy;
		VideoStream* pStream = &depth;
		rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, SAMPLE_READ_WAIT_TIMEOUT);        
//printf("OpenNI::waitForAnyStream(...) rc = %d\n",rc);
		if (rc != STATUS_OK)
		{
			printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
			continue;
		}

		rc = depth.readFrame(&frame);        
//printf("depth.readFrame(&frame) rc = %d\n",rc);
		if (rc != STATUS_OK)
		{
			printf("Read failed!\n%s\n", OpenNI::getExtendedError());
			continue;
		}

		if (frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
		{
			printf("Unexpected frame format\n");
			continue;
		}

		DepthPixel* pDepth = (DepthPixel*)frame.getData();
printf("memory address: %p\n", (void*)&pDepth);

		int middleIndex = (frame.getHeight()+1)*frame.getWidth()/2;
//printf("frame height = %d, frame width = %d\n", frame.getHeight(), frame.getWidth());

//		printf("Timestamp [%08llu] Depth %8d\n", (long long)frame.getTimestamp(), pDepth[middleIndex]);

		printf("Timestamp [%08llu] FrameIndex %d\n",(long long)frame.getTimestamp(), frame.getFrameIndex());

		int width = frame.getWidth();
		int height = frame.getHeight();
		int frameIndex = frame.getFrameIndex();
//printf("FrameIndex=%d\n",frameIndex);
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x, ++pDepth)
			{
//				printf("Frame Index=%d x=%d y=%d depth=%f inches\n",frameIndex, x, y, *pDepth / 25.4);
			}
//			printf("\n");
//sleep(1);
		}

	}
//	recorder.stop();
//	recorder.destroy();

	depth.stop();
	depth.destroy();
	device.close();
	OpenNI::shutdown();

	return 0;

}
