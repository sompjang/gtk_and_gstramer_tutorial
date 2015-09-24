#pragma once
#include <glib.h>
#include <thread>
class GSTMainLoop
{

private:
	
	GMainLoop* loop = NULL;
	
	int InstanceCounter;
	std::thread* singleThread = NULL;

public:
	GSTMainLoop();
	~GSTMainLoop();
	
	std::thread* GetSingleThread() const;
	void SetSingleThread(std::thread* val);
	int GetInstanceCounter() const;
	void SetInstanceCounter(int val);
	void Run();
	void Delete();
	void RunLoop();
};
