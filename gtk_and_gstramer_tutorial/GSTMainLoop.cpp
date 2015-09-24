#include "GSTMainLoop.h"
#include <gtk\gtkmain.h>


GSTMainLoop::GSTMainLoop()
{}


GSTMainLoop::~GSTMainLoop()
{
	Delete();
}

void GSTMainLoop::Run() {

	if (InstanceCounter == 0)
	{
		singleThread = new std::thread(&GSTMainLoop::RunLoop, this);
	}

	InstanceCounter++;
}

void GSTMainLoop::RunLoop()
{
	
	InstanceCounter++;
	g_print("Before gtk_main \n");
	gtk_main();
	g_print("After gtk_main \n");
}

std::thread* GSTMainLoop::GetSingleThread() const
{
	return singleThread;
}

void GSTMainLoop::SetSingleThread(std::thread* val)
{
	singleThread = val;
}

int GSTMainLoop::GetInstanceCounter() const
{
	return InstanceCounter;
}

void GSTMainLoop::SetInstanceCounter(int val)
{
	InstanceCounter = val;
}

void GSTMainLoop::Delete() {
	InstanceCounter--;

	if (InstanceCounter == 0)
	{
		if (singleThread)
		{
			gtk_main_quit();
			singleThread->join();
			delete singleThread;
		}
	}
}