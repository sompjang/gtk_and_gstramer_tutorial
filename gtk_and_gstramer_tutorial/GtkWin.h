#pragma once
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

#include <gdk/gdk.h>
#if defined (GDK_WINDOWING_X11)
#include <gdk/gdkx.h>
#elif defined (GDK_WINDOWING_WIN32)
#include <gdk/gdkwin32.h>
#elif defined (GDK_WINDOWING_QUARTZ)
#include <gdk/gdkquartz.h>
#endif

class GtkWin
{
private:
	GtkWidget *main_window;		/* The uppermost window, containing all other windows */
	GtkWidget *video_window;	/* The drawing area where the video will be shown */
	int height = 480;			//video height
	int width = 640;			//video width
	int x = -5;					//main_window position x
	int y = -5;					//main_window position y

public:

	GtkWin();
	~GtkWin();

	int GetY() const;
	void SetY(int val);
	int GetX() const;
	void SetX(int val);
	int GetWidth() const;
	void SetWidth(int val);
	int GetHeight() const;
	void SetHeight(int val);
	void SetDimension(int h, int w);
	void SetPosition(int x, int y);
	void UpdateWindow(bool isNew);
	static void realize_cb(GtkWidget *widget, GstXOverlay *overlay);
	static gboolean expose_cb(GtkWidget *widget, GdkEventExpose *event, GstElement *pipeline);
	void SetBlackScreen();
	static void delete_event_cb(GtkWidget *widget, GdkEvent *event, gpointer *data);
	void create_ui(GstXOverlay *overlay, GstElement *pipeline);
	bool find_widget_in_main_win();
	void SetOverlayToWindow(GstXOverlay *overlay);
	void Foreground();
	void Background();
};

