#include "GtkWin.h"
#include <gtk\gtkwidget.h>
#include "gst\interfaces\xoverlay.h"
#include <gtk\gtkmain.h>
#include <gdk\gdkwin32.h>
#include <cairo.h>

GtkWin::GtkWin()
{
	  
	
}


GtkWin::~GtkWin()
{
}

int GtkWin::GetHeight() const
{
	return height;
}

void GtkWin::SetHeight(int val)
{
	height = val;
}

int GtkWin::GetWidth() const
{
	return width;
}

void GtkWin::SetWidth(int val)
{
	width = val;
}

int GtkWin::GetX() const
{
	return x;
}

void GtkWin::SetX(int val)
{
	x = val;
}

int GtkWin::GetY() const
{
	return y;
}

void GtkWin::SetY(int val)
{
	y = val;
}

void GtkWin::SetDimension(int h, int w)
{
	width = w;
	height = h;
}

void GtkWin::SetPosition(int x, int y) {
	this->x = x;
	this->y = y;
}

void GtkWin::UpdateWindow(bool isNew)
{
	if (isNew) {
		gtk_window_set_default_size(GTK_WINDOW(main_window), width, height);
	}
	else {
		gtk_window_resize(GTK_WINDOW(main_window), width, height);
		g_print("New Window size(width/height): %i/%i\n", width, height);
	}

	gtk_window_move(GTK_WINDOW(main_window), x, y);
	g_print("Window is set to new position (x,y): [%i,%i] \n", x,y);
}

void GtkWin::realize_cb(GtkWidget *widget, GstXOverlay *overlay) {
	GdkWindow *window = gtk_widget_get_window(widget);
	guintptr window_handle;

	if (!gdk_window_ensure_native(window))
		g_error("Couldn't create native window needed for GstXOverlay!");

	/* Retrieve window handler from GDK */
	#if defined (GDK_WINDOWING_WIN32)
		window_handle = (guintptr)GDK_WINDOW_HWND(window);
	#elif defined (GDK_WINDOWING_QUARTZ)
		window_handle = gdk_quartz_window_get_nsview(window);
	#elif defined (GDK_WINDOWING_X11)
		window_handle = GDK_WINDOW_XID(window);
	#endif
	/* Pass it to playbin2, which implements XOverlay and will forward it to the video sink */
	gst_x_overlay_set_window_handle(overlay, window_handle);
}

void GtkWin::SetBlackScreen() {
	GtkAllocation allocation;
	GdkWindow *window = gtk_widget_get_window(video_window);
	cairo_t *cr;

	/* Cairo is a 2D graphics library which we use here to clean the video window.
	* It is used by GStreamer for other reasons, so it will always be available to us. */
	gtk_widget_get_allocation(video_window, &allocation);
	cr = gdk_cairo_create(window);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, 0, 0, allocation.width, allocation.height);
	cairo_fill(cr);
	cairo_destroy(cr);
}

/* This function is called when the main window is closed */
void GtkWin::delete_event_cb(GtkWidget *widget, GdkEvent *event, gpointer *data) {
	gtk_main_quit();
}

/* This creates all the GTK+ widgets that compose our application, and registers the callbacks */
void GtkWin::create_ui(GstXOverlay *overlay, GstElement *pipeline) {
	
	if (!main_window) {
		main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		g_signal_connect(G_OBJECT(main_window), "delete-event", G_CALLBACK(delete_event_cb), NULL);
		g_print("Is main_window NULL : %s\n", !main_window ? "yes" : "no");
	}

	if (!video_window) {
		video_window = gtk_drawing_area_new();
		gtk_widget_set_double_buffered(video_window, FALSE);
		g_print("Is video_window NULL : %s\n", !video_window ? "yes" : "no");
	}
	
	g_signal_connect(video_window, "realize", G_CALLBACK(realize_cb), overlay);
	//g_signal_connect(video_window, "expose_event", G_CALLBACK(expose_cb), pipeline);

	if (!find_widget_in_main_win()) {
		gtk_container_add(GTK_CONTAINER(main_window), video_window);
		UpdateWindow(TRUE);
		gtk_window_set_decorated(GTK_WINDOW(main_window),FALSE);
		gtk_widget_show_all(main_window);
		SetBlackScreen();
	}
	else {
		SetOverlayToWindow(overlay);
		g_print("Set new overlay to old win. \n");
	}
}

bool GtkWin::find_widget_in_main_win() {
	
	GList *children, *iter;
	GtkWidget *win;

	children = gtk_container_get_children(GTK_CONTAINER(main_window));

	for (iter = children; iter != NULL; iter = g_list_next(iter)) {
		win = GTK_WIDGET(iter->data);
		if (win == video_window)
			return TRUE;
	}
		
	return FALSE;
}

void GtkWin::SetOverlayToWindow(GstXOverlay *overlay) {
	GdkWindow *window = gtk_widget_get_window(video_window);
	guintptr window_handle;

	if (!gdk_window_ensure_native(window))
		g_error("Couldn't create native window needed for GstXOverlay!");

	/* Retrieve window handler from GDK */
	#if defined (GDK_WINDOWING_WIN32)
		window_handle = (guintptr)GDK_WINDOW_HWND(window);
	#elif defined (GDK_WINDOWING_QUARTZ)
		window_handle = gdk_quartz_window_get_nsview(window);
	#elif defined (GDK_WINDOWING_X11)
		window_handle = GDK_WINDOW_XID(window);
#endif
	/* Pass it to playbin2, which implements XOverlay and will forward it to the video sink */
	gst_x_overlay_set_window_handle(overlay, window_handle);
}

void GtkWin::Foreground()
{
	gtk_window_present(GTK_WINDOW(main_window));
}

void GtkWin::Background()
{
	gtk_window_set_keep_below(GTK_WINDOW(main_window), TRUE);
}