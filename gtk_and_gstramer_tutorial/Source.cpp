#include <string.h>


#include <thread>
#include <gtk/gtkmain.h>
#include <iostream>
#include "GST_Streamer.h"


/* This function is called when the GUI toolkit creates the physical window that will hold the video.
* At this point we can retrieve its handler (which has a different meaning depending on the windowing system)
* and pass it to GStreamer through the XOverlay interface. */

//void ovladac(GST_Streamer *streamer) {
//	char c;
//
//	do {
//		std::cout << "Zadaj prikaz:";
//
//		c = getchar();
//
//		switch (c)
//		{
//		case 'p':
//			streamer->Play();
//			break;
//		case 'o':
//			streamer->Pause();
//			break;
//		case 's':
//			streamer->Stop();
//			break;
//		case 'i':
//			streamer->Set("file:///D:\\shared\\1.mp4", 0);
//			break;
//		case 'j':
//			streamer->Set("http://192.168.1.11/mjpg/video.mjpg", 0);
//			break;
//		case 'k':
//			streamer->Set("rtsp://192.168.1.11/mpeg4/media.amp", 0);
//			break;
//		case 'f':
//			streamer->Win()->Foreground();
//			break;
//		case 'b':
//			streamer->Win()->Background();
//			break;
//		case 'd':
//			streamer->Win()->SetPosition(-10,-40);
//			streamer->Win()->UpdateWindow(FALSE);
//			break;
//		default:
//			break;
//		}
//
//	} while (c != 'e');
//}


int CountWords(std::string str)
{
	int numspaces = 0;
	
	for (int i = 0; i<int(str.length()); i++)
	{
		if (isspace(str[i]))
			numspaces++;
	}
	return numspaces+1;

}

std::string NormalizeString(std::string str) {
	while (isspace(str[str.length() - 1])) {
		str.pop_back();
	}

	while (isspace(str[0])) {
		str.erase(0, 1);
	}
	return str;
}

void PrintInfo(GST_Streamer * streamer)
{
	printf("Info:\n");
	printf("\tPath: %s \n", streamer->GetInfo()->ActualPath().c_str());
	printf("\tDuration: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(streamer->GetInfo()->GetDuration()));
	printf("\tPosition: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(streamer->GetInfo()->GetPosition()));

	GstState state = streamer->GetState();
	if (state == GST_STATE_NULL)
		printf("\tState: null \n");
	else if (state == GST_STATE_VOID_PENDING)
		printf("\tState: void pending \n");
	else if (state == GST_STATE_PAUSED)
		printf("\tState: paused \n");
	else if (state == GST_STATE_PLAYING)
		printf("\tState: playing \n");
}

bool ManageWindow(GST_Streamer * streamer, gchar ** argv, int i)
{
	if (argv[i + 1] == NULL || argv[i + 2] == NULL || argv[i + 3] == NULL || argv[i + 4] == NULL) {
		return FALSE;
	}

	gboolean wasPlayed = FALSE;
	gboolean wasPaused = FALSE;

	gint64 position = 0;
	gdouble doublepos = 0;
	GstState state = streamer->GetState();

	if (state == GstState::GST_STATE_PLAYING || state == GstState::GST_STATE_PAUSED) {

		position = streamer->GetInfo()->GetPosition();
		doublepos = static_cast<gdouble>(position);
		doublepos = doublepos / 1000000000;

		if (state == GstState::GST_STATE_PLAYING)
			wasPlayed = TRUE;
		if (state == GstState::GST_STATE_PAUSED)
			wasPaused = TRUE;

	}

	//if (streamer->GetInfo()->IsLive()) {
		streamer->Stop();
		while (streamer->GetState() != GstState::GST_STATE_NULL) {
			g_print("state: %i, isLive: %s\n", streamer->GetState(), streamer->GetInfo()->IsLive() ? "True" : "False");
		}
	/*}
	else {
		streamer->Pause();
		while (streamer->GetState() != GstState::GST_STATE_PAUSED) {
			g_print("state: %i, isLive: %s\n", streamer->GetState(), streamer->GetInfo()->IsLive() ? "True" : "False");
		}
	}*/	
	
	streamer->GetWin()->SetX(strtol(argv[i + 1], NULL, 10));
	streamer->GetWin()->SetY(strtol(argv[i + 2], NULL, 10));
	streamer->GetWin()->SetWidth(strtol(argv[i + 3], NULL, 10));
	streamer->GetWin()->SetHeight(strtol(argv[i + 4], NULL, 10));
	streamer->GetWin()->UpdateWindow(FALSE);

	if (wasPlayed) {
		streamer->Set(streamer->GetInfo()->ActualPath(), doublepos);
		streamer->Play();
		wasPlayed = FALSE;
	}
	else if (wasPaused) {
		streamer->Set(streamer->GetInfo()->ActualPath(), doublepos);
		wasPlayed = FALSE;	
	}
	g_print("win is updated.\n");

	return TRUE;
}

bool DoJob(int argc, gchar ** argv, GST_Streamer * streamer)
{
	int i = 0;

	for (i = 0; i < argc; i++) {
	
		if (strcmp(argv[i], "play") == 0) {
			streamer->Play();
		}
		else if (strcmp(argv[i], "stop") == 0) {
			streamer->Stop();
		}
		else if (strcmp(argv[i], "pause") == 0) {
			streamer->Pause();
		}
		else if (strcmp(argv[i], "set") == 0) {
			if (argv[i + 1] == NULL) {
				g_print("Path Parameter is missing. \n");
			}
			else if (argv[i + 2] == NULL) {
				g_print("Seek parameter is set to 0. \n");
				streamer->Set(argv[i + 1], 0);
				i++;
			}
			else {
				streamer->Set(argv[i + 1], strtol(argv[i + 2], NULL, 10));
				i += 2;
			}
		}
		else if (strcmp(argv[i], "add") == 0) {
			streamer->Add(argv[i + 1]);
			i += 2;
		}
		else if (strcmp(argv[i], "window") == 0) {
			if (ManageWindow(streamer, argv, i));
				i += 4;
		}
		else if (strcmp(argv[i], "foreground") == 0) {
			streamer->GetWin()->Foreground();
		}
		else if (strcmp(argv[i], "background") == 0) {
			streamer->GetWin()->Background();
		}
		else if (strcmp(argv[i], "info") == 0) {
			PrintInfo(streamer);
		}
		else if (strcmp(argv[i], "seek") == 0) {
			i++;
			streamer->Seek(atoi(argv[i]));
		}
		else if (strcmp(argv[i], "exit") == 0) {
			return FALSE;
		}
		else {
			printf("Neznamy parameter '%s'. \n", argv[i]);
		}
	}

	return TRUE;
}

void ovladac3(GST_Streamer *streamer) {
	
	boolean stop = TRUE;

	do {
		std::string str;
		gchar **argv;
		int argc;
		char c = 0;
		std::cout << ">";

		getline(std::cin, str);

		if (str == "")
			continue;

		str = NormalizeString(str);
		argv = g_strsplit(str.c_str(), " ", 0);
		argc = CountWords(str);

		stop = DoJob(argc, argv, streamer);

	} while (stop);
}

int Run(GST_Streamer *data) {
	
	//data->SetVideoInfo("http://192.168.1.11/mjpg/video.mjpg");//file:///D:\\shared\\1.mp4
	//data->SetVideoInfo("rtsp://192.168.1.11/axis-media/media.amp");
	//data->SetVideoInfo("file:///C:\\1.mp4");
	data->SetVideoInfo("file:///D:\\shared\\1.mp4");
	/*data->Add("file:///D:\\shared\\2.mp4");
	data->Add("file:///D:\\shared\\3.mp4");
	data->Add("file:///D:\\shared\\4.mp4");*/
	data->Play();
	
	return 0;
}

int main(int argc, char *argv[]) {
	
	GST_Streamer *streamer = new GST_Streamer();
	
	streamer->SetSingleThread(new std::thread(&Run, streamer));
	ovladac3(streamer);

	return 0;
}
