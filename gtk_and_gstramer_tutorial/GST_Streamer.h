#include <gst/gst.h>
#include <glib.h>
#include <iostream>
#include <vector>
#include <string>
#include <gst/rtsp/gstrtsptransport.h>

#pragma once
#include "VideoInfo.h"
#include "GSTMainLoop.h"
#include "GtkWin.h"


class GST_Streamer : public GSTMainLoop
{
private:
	
	GstElement *pipeline;
	GstElement *source;
	GstElement *sink;
	GtkWin *win;
	
	std::vector <GstElement*> elements;
	VideoInfo *info;
public:

	GST_Streamer();
	~GST_Streamer();

	GtkWin * GetWin() const;
	void SetWin(GtkWin * val);
	VideoInfo * GetInfo() const;
	void SetInfo(VideoInfo * val);
	GstState GetState();
	GstElement * GetPipeline() const;

	inline bool exists_test(const std::string& name);
	int Set(std::string path, double seek);
	int Pause();
	int Stop();
	int Start();
	void Seek(double ns);
	int Play();
	int AutoPipelineCreatorInit();
	int SetupHttpElements();
	int SetupRtspElements();
	int LinkHttpElements();
	int LinkRtspElements();
	//int CreateHttpsPipe();
	//int CreateRtspPipe();
	int Connect();
	int CreateUI();
	static void on_pad_added(GstElement *element, GstPad *pad, gpointer data);
	static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);
	static boolean cb_get_position(GST_Streamer *streamer);
	static void seek_to_time(GstElement *pipeline, gint64 time_nanoseconds);
	int SetVideoInfo(std::string path);
	void DataInit();

	void clear();
	void unrefPipeline();
	int SetupFileElements();
	int LinkFileElements();
	void SetNext();
	void Add(std::string str);
};
