#include "GST_Streamer.h"
#include <assert.h>
#include "gst\interfaces\xoverlay.h"
#include "gst\gstclock.h"
#include <stdio.h>

GST_Streamer::GST_Streamer() : GSTMainLoop()
{
	SetInfo(new VideoInfo());
	win = new GtkWin();
}

GST_Streamer::~GST_Streamer()
{
}

GstState GST_Streamer::GetState() {

	if (pipeline) {
		GstState current = GST_STATE_NULL;
		GstState pending = GST_STATE_NULL;
		//g_print("pred zistovanim stavu \n");
		GstStateChangeReturn ret = gst_element_get_state(pipeline, &current, &pending, GST_SECOND);
		//g_print("po zistovanim stavu \n");

		if (ret != GST_STATE_CHANGE_FAILURE) {
			return current;
		}
	}

	return GST_STATE_NULL;
}

GstElement * GST_Streamer::GetPipeline() const
{
	return pipeline;
}

inline bool GST_Streamer::exists_test(const std::string& name) {

	std::string path = name.c_str();
	FILE *file;

	path.erase(0, 8);

	if (fopen_s(&file ,path.c_str(), "r") == 0) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

/*
	set <path> <seek_in_seconds>
	e.g. set mp4/car.mp4 12.5
	nacita subor, presunie sa na <seek> sekundu
	ale NEZACNE prehravat (az ked sa posle prikaz play)
*/
int GST_Streamer::Set(std::string path, double seek)
{
	GstState state = GstState::GST_STATE_VOID_PENDING;

	this->Stop();

	this->GetInfo()->ActualPath(path);
	
	if (!GetInfo()->IsLive() && seek > 0) {
		
		while (this->GetState() != GstState::GST_STATE_NULL) {
			printf("State: %i \n", this->GetState());
		}
		this->Play();
		
		while (this->GetState() != GstState::GST_STATE_PLAYING) {
			printf("State: %i \n", this->GetState());
		}
		this->Pause();

		this->Seek(seek);
	}

	return 0;
}

int GST_Streamer::Pause()
{
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	if (pipeline)
	{
		GstState current = GST_STATE(pipeline);
		GstState pending = GST_STATE_PENDING(pipeline);

		if (pending == GST_STATE_VOID_PENDING && current == GST_STATE_PLAYING)
		{
			gst_element_set_state(pipeline, GST_STATE_PAUSED);
		}
	}

	return 0;
}

int GST_Streamer::Stop()
{
	g_print("Stop_pipeline. \n");

	if (pipeline)
	{
		GstState current_state = GST_STATE_NULL;
		GstState pending_state = GST_STATE_NULL;
		GstStateChangeReturn ret = gst_element_get_state(pipeline, &current_state, &pending_state, GST_SECOND);

		g_print("Pipeline status(current, pending): %i, %i \n", current_state, pending_state);

		if (current_state == GST_STATE_PLAYING || current_state == GST_STATE_PAUSED || current_state == GST_STATE_READY
			|| pending_state == GST_STATE_PLAYING)
		{
			unrefPipeline();
		}
	}

	return 0;
}

int GST_Streamer::Start()
{
	if (pipeline)
	{
		GstState current = GST_STATE_TARGET(pipeline);
		GstState pending = GST_STATE_PENDING(pipeline);

		if (pending == GST_STATE_VOID_PENDING &&
			(current == GST_STATE_PAUSED || current == GST_STATE_READY))
		{
			gst_element_set_state(pipeline, GST_STATE_PLAYING);
		}

		if (current == GST_STATE_NULL
			|| current == GST_STATE_PAUSED
			|| current == GST_STATE_READY)
		{
			gst_element_set_state(pipeline, GST_STATE_PLAYING);
		}
	}

	return 0;
}

void GST_Streamer::Seek(double s) {

	if (!info->IsLive()) {
		if (pipeline)
		{
			GstState current_state = GST_STATE_NULL;
			GstState pending_state = GST_STATE_NULL;
			GstStateChangeReturn ret = gst_element_get_state(pipeline, &current_state, &pending_state, GST_SECOND);

			if (current_state == GST_STATE_PLAYING || current_state == GST_STATE_PAUSED || pending_state == GST_STATE_PLAYING)
			{
				gint64 ns;
				ns = s * 1000000000;
				seek_to_time(GetPipeline(), ns);
			}
		}
	}
}

int GST_Streamer::Play()
{
	GstState current;
	GstState target;

	g_print("Start_Playing. \n");

	if (!pipeline) {
		int ret = Connect();
		if (ret == -1)
		{
			assert(ret != -1);
			unrefPipeline();
		}
		return -1;
	}
	else {
		current = GetState();
		target = GST_STATE_TARGET(pipeline);
	}

	if (current == GST_STATE_NULL || target == GST_STATE_NULL) {
		int ret = Connect();
		if (ret == -1)
		{
			assert(ret != -1);
			unrefPipeline();
		}
	}
	else if ((current == GST_STATE_PAUSED || current == GST_STATE_READY)
		&& target != GST_STATE_PLAYING)
		Start();

	return 0;
}

int GST_Streamer::AutoPipelineCreatorInit() {
	source = gst_element_factory_make("uridecodebin", "uridecodebin");
	sink = gst_element_factory_make("d3dvideosink", "videosink");

	if (!sink || !pipeline || !source)
		return -1;
	else {
		gst_bin_add_many(GST_BIN(pipeline),
			source,
			sink,
			NULL);
		//return 1;

		g_object_set(G_OBJECT(source), "uri", GetInfo()->ActualPath().c_str(), NULL);

		g_signal_connect(source, "pad-added", G_CALLBACK(on_pad_added), sink);
	}
	return 1;
}

int GST_Streamer::SetupHttpElements()
{
	// inicializacia potrebnych komponentov
	source = gst_element_factory_make("souphttpsrc", "DSCtrlSrc");
	elements.push_back(new GstElement());
	elements[0] = gst_element_factory_make("multipartdemux", "DSCtrlFilter0");
	elements.push_back(new GstElement());
	elements[1] = gst_element_factory_make("jpegdec", "DSCtrlFilter1");
	elements.push_back(new GstElement());
	elements[2] = gst_element_factory_make("queue2", "DSCtrlFilter2");
	

	// kontrola ci sa elementy vytvorili
	if (!source || !elements[0] || !elements[1] || !elements[2])
		return -1;
	else
		gst_bin_add_many(GST_BIN(pipeline),
			source,
			elements[0],
			elements[1],
			elements[2],
			NULL);

	// nastavenie parametrov pre source
	g_object_set(source,
		"location", GetInfo()->ActualPath().c_str(),
		NULL);
	g_object_set(elements[2],
		"use-buffering", TRUE,
		"max-size-bytes", 0,
		"max-size-time", 50000000000,
		"max-size-buffers", 2000,
		NULL);
	
	//ak je potrebne meno a heslo tak sa nastavy
	if ((strcmp(GetInfo()->Username().c_str(), "") || strcmp(GetInfo()->Password().c_str(), "")))
	{
		g_object_set(source,
			"user-id", GetInfo()->Username(),
			"user-pw", GetInfo()->Password(),
			NULL);
	}

	return 0;
}

int GST_Streamer::SetupRtspElements()
{
	source = gst_element_factory_make("rtspsrc", "DSCtrlSrc");
	elements.push_back(new GstElement());
	elements[0] = gst_element_factory_make("rtph264depay", "DSCtrlFilter0");
	elements.push_back(new GstElement());
	elements[1] = gst_element_factory_make("ffdec_h264", "DSCtrlFilter1");
	elements.push_back(new GstElement());
	elements[2] = gst_element_factory_make("queue2", "DSCtrlFilter2");

	if (!source || !elements[0] || !elements[1] || !elements[2])
		return -1;
	else
		gst_bin_add_many(GST_BIN(pipeline),
			source,
			elements[0],
			elements[1],
			elements[2],
			NULL);
	int param;
	gboolean drop;
	
	g_object_get(source, "buffer-mode", &param, NULL);

	g_object_set(source,
		"location", GetInfo()->ActualPath().c_str(),
		"latency", 0,
		"debug", FALSE,
		"protocols", GST_RTSP_LOWER_TRANS_TCP,
		NULL);

	g_object_set(elements[2],
		"use-buffering", TRUE,
		"max-size-bytes", 0,
		"max-size-time", 50000000000,
		"max-size-buffers", 2000,
		"high-percent", 99,
		"low-percent",10,
		NULL);

	if ((strcmp(GetInfo()->Username().c_str(), "") || strcmp(GetInfo()->Password().c_str(), "")))
	{
		g_object_set(source,
			"user-id", GetInfo()->Username().c_str(),
			"user-pw", GetInfo()->Password().c_str(),
			NULL);
	}

	return 0;
}

int GST_Streamer::LinkHttpElements()
{
	if (!gst_element_link(source, elements[0]))
	{
		gst_object_unref(source);
		gst_object_unref(elements[0]);
		gst_object_unref(elements[1]);
		gst_object_unref(elements[2]);
		gst_object_unref(sink);
		return -1;
	}

	//linkovanie elementov
	if (!gst_element_link_many(
		elements[1],
		elements[2],
		sink,
		NULL))
	{
		gst_object_unref(source);
		gst_object_unref(elements[0]);
		gst_object_unref(elements[1]);
		gst_object_unref(elements[2]);
		gst_object_unref(sink);
		return -1;
	}

	g_signal_connect(elements[0], "pad-added", G_CALLBACK(GST_Streamer::on_pad_added), elements[1]);
	gst_element_sync_state_with_parent(elements[0]);

	return 0;
}

int GST_Streamer::LinkRtspElements()
{
	if (!gst_element_link_many(elements[0],
		elements[1],
		elements[2],
		sink,
		NULL))
	{
		gst_object_unref(source);
		gst_object_unref(elements[0]);
		gst_object_unref(elements[1]);
		gst_object_unref(elements[2]);
		gst_object_unref(sink);
		return -1;
	}

	g_signal_connect(source, "pad-added", G_CALLBACK(GST_Streamer::on_pad_added), elements[0]);
	gst_element_sync_state_with_parent(elements[0]);

	return 0;
}

int GST_Streamer::SetupFileElements()
{
	source = gst_element_factory_make("uridecodebin", "FileSrc");

	if (!source)
		return -1;
	else
		gst_bin_add_many(GST_BIN(pipeline),
			source,
			NULL);

	g_object_set(source,
		"uri", GetInfo()->ActualPath().c_str(),
		NULL);

	return 0;
}

int GST_Streamer::LinkFileElements()
{
	g_signal_connect(source, "pad-added", G_CALLBACK(on_pad_added), sink);

	return 0;
}

void GST_Streamer::SetNext()
{
	if (GetInfo()->GetPath_list().size() >= 1) {
		Set(GetInfo()->GetPath_list()[0], 0);
		GetInfo()->RemovePathFromList();
	}
}

void GST_Streamer::Add(std::string str) {
	GetInfo()->AddPathToList(str);
}

int GST_Streamer::Connect()
{
	if (pipeline)
	{
		GstState current = GST_STATE(pipeline);
		GstState pending = GST_STATE_PENDING(pipeline);

		if (pipeline
			&& (current == GST_STATE_PLAYING
				|| current == GST_STATE_READY
				|| current == GST_STATE_PAUSED))
		{

			return -1;
		}
	}


	DataInit();

	if (pipeline == NULL) {
		pipeline = gst_pipeline_new("VideoPipeline");
	}

	GstBus* bus = gst_element_get_bus(this->pipeline);
	gst_bus_add_watch(bus, GST_Streamer::bus_call, this);
	gst_object_unref(bus);

	//this->AutoPipelineCreatorInit();

	//sink = new GstElement();
	sink = gst_element_factory_make("d3dvideosink", "d3dvideosink_0");

	if (!pipeline || !sink) {
		return -1;
	}
	else {
		gst_bin_add_many(GST_BIN(pipeline),
			sink,
			NULL);
	}

	//MJPEG over HTTP multipart
	if (GetInfo()->ActualPath().find("http://") == 0)
	{
		if (SetupHttpElements() == -1)
			return -1;

		if (LinkHttpElements() == -1)
			return -1;
		info->IsLive(TRUE);
	}

	//RTP/RTPS/H264/MJPEG
	else if (GetInfo()->ActualPath().find("rtsp://") == 0)
	{
		if (SetupRtspElements() == -1)
			return -1;

		if (LinkRtspElements() == -1)
			return -1;
		info->IsLive(TRUE);
	}

	//file
	else if (GetInfo()->ActualPath().find("file://") == 0) {

		if (exists_test(GetInfo()->ActualPath())) {
			g_print("File exists. \n");
			if (SetupFileElements() == -1)
				return -1;

			if (LinkFileElements() == -1)
				return -1;
			info->IsLive(FALSE);
		}
		else {
			g_print("File does not exists. \n");
		}
	}

	else {
		if (SetupFileElements() == -1)
			return -1;

		if (LinkFileElements() == -1)
			return -1;
	}

	this->CreateUI();
	g_print("UI is created\n");

	if (!GetInfo()->IsLive())
		g_timeout_add(200, (GSourceFunc)cb_get_position, this);

	g_print("Status: %i \n", GetState());

	GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);

	if (ret == GST_STATE_CHANGE_FAILURE)
	{
		gst_object_unref(pipeline);
		return -1;
	}

	if (GetInstanceCounter() == 0) {
		RunLoop();
	}

	return 0;
}

int GST_Streamer::CreateUI() {
	win->create_ui(GST_X_OVERLAY(sink), pipeline);

	return 0;
}

void GST_Streamer::on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
	char* name;
	GstPad *sinkpad;
	GstElement *decoder = (GstElement *)data;

	name = gst_pad_get_name(pad);
	//g_print("A new pad %s was created", name);
	g_free(name);
	/* We can now link this pad with the vorbis-decoder sink pad */
	sinkpad = gst_element_get_static_pad(decoder, "sink");
	gst_pad_link(pad, sinkpad);
	gst_object_unref(sinkpad);
}

gboolean GST_Streamer::bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
	GST_Streamer *streamer = (GST_Streamer *)data;

	//g_print("Got %s message\n", GST_MESSAGE_TYPE_NAME(msg));

	switch (GST_MESSAGE_TYPE(msg))
	{
	case GST_MESSAGE_STATE_CHANGED: {
		GstState current = GST_STATE_NULL;
		GstState pending = GST_STATE_NULL;
		GstStateChangeReturn ret = gst_element_get_state(streamer->pipeline, &current, &pending, GST_SECOND);

		if (current < GST_STATE_PAUSED) {
			streamer->GetWin()->SetBlackScreen();
		}

		g_print("Current state %i, pending state %i, change return %i \n", current, pending, ret);
		break;
	}
	case GST_MESSAGE_EOS:
		g_print("End of stream\n");

		streamer->SetNext();
		streamer->Play();

		break;
	case GST_MESSAGE_ERROR: {
		gchar *debug;
		GError *error;
		gst_message_parse_error(msg, &error, &debug);
		g_free(debug);
		g_printerr("Error: %s\n", error->message);
		g_error_free(error);
		break;
	}
	case GST_MESSAGE_BUFFERING: {

		guint cbuffer , mbuffer ;
		guint cbytes , mbytes ; 
		guint64 ctime , mtime ;
		guint hpercent, lpercent; 

		if (streamer->elements[2]) {
			g_object_get(streamer->elements[2],
				"current-level-buffers", &cbuffer,
				"current-level-bytes", &cbytes,
				"current-level-time", &ctime,
				"max-size-buffers", &mbuffer,
				"max-size-bytes", &mbytes,
				"max-size-time", &mtime,
				"high-percent", &hpercent,
				"low-percent", &lpercent,
				NULL);

			//printf("%i\n", streamer->GetState());

			double val = (double)hpercent / 100;
			if ((cbuffer >= val*mbuffer && mbuffer != 0) || (cbytes >= val*mbytes && mbytes != 0) || (ctime >= val*mtime && mtime != 0)) {
				printf("Buffer info:\n");
				printf("high/low: %i/%i", hpercent, lpercent);
				printf("\tbuffer: %i/%i\n", cbuffer, mbuffer);
				printf("\tbytes: %i/%i\n", cbytes, mbytes);
				printf("\ttime: %" GST_TIME_FORMAT "/%" GST_TIME_FORMAT "\n", GST_TIME_ARGS(ctime), GST_TIME_ARGS(mtime));
			}
		}
		break;
	}
	default:
		break;
	}
	return TRUE;
}

boolean GST_Streamer::cb_get_position(GST_Streamer *streamer) {
	gint64 pos, len;
	GstFormat  gstFor = GST_FORMAT_TIME;
	GstState state = streamer->GetState();

	//printf("%s %i\n", streamer->GetInfo()->IsLive() ? "true" : "false", state);
	if (!streamer->GetInfo()->IsLive() && (state == GST_STATE_PAUSED || state == GST_STATE_PLAYING)) {

		if (gst_element_query_duration(streamer->GetPipeline(), &gstFor, &len)) {
			streamer->GetInfo()->SetDuration(len);
		}
		else {
			g_printerr("Could not query current duration.\n");
		}

		if (gst_element_query_position(streamer->GetPipeline(), &gstFor, &pos)) {
			streamer->GetInfo()->SetPosition(pos);
		}
		else {
			g_printerr("Could not query current position.\n");
		}
	}

	return TRUE;
}

void GST_Streamer::seek_to_time(GstElement *pipeline, gint64 time_nanoseconds)
{
	if (!gst_element_seek(pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
		GST_SEEK_TYPE_SET, time_nanoseconds,
		GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
		g_print("Seek failed!\n");
	}
}

GtkWin * GST_Streamer::GetWin() const
{
	return win;
}

void GST_Streamer::SetWin(GtkWin * val)
{
	win = val;
}

VideoInfo * GST_Streamer::GetInfo() const
{
	return info;
}

void GST_Streamer::SetInfo(VideoInfo * val)
{
	info = val;
}

int GST_Streamer::SetVideoInfo(std::string path)
{
	info->ActualPath(path);

	return 0;
}

void GST_Streamer::DataInit()
{
	g_print("Data initialization. \n");
	gtk_init(NULL, NULL);

	if (!gst_is_initialized()) {
		gst_init_check(NULL, NULL, NULL);
	}
}

void GST_Streamer::clear()
{
	this->pipeline = NULL;
}

void GST_Streamer::unrefPipeline()
{
	g_print("Unrefing pipeline. \n");
	if (this->pipeline)
	{
		GstStateChangeReturn ret;
		gst_element_set_state(this->pipeline, GST_STATE_NULL);
		ret = gst_element_get_state(this->pipeline, NULL, NULL, GST_SECOND);

		assert(ret == GST_STATE_CHANGE_SUCCESS);
		gst_object_unref(this->pipeline);
		elements.clear();
	}

	this->pipeline = NULL;
}