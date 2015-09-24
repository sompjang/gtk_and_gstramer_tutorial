///*
//Tool, ktory sluzi na dekodovanie rury vyskladanej pomocou Elementu decodebin.
//
//Pouzitie:
//
//PipeCreator.exe -u 'url' [-i 'username'] [-p 'password'] [-f 'filename']
//
//Parametre:
//
//-u, --url
//Url kamery, na ktoru sa chces napojit
//povinny parameter
//
//-i, --user_id
//Username
//nepovinny parameter
//
//-p, --user_pass
//Heslo ku kamere
//nepovinny parameter
//
//-f, --file
//Nazov suboru, kde chces ulozit nazov elemntov, z ktorych je pipa vyskladana.
//nepovinny parameter. default: pipe.txt
//*/
//
//#include <gst/gst.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <gst/app/gstappsink.h>
//#include <string.h>
//
//
//static GMainLoop *loop;
//typedef struct _MyData {
//	GstElement *pipeline;
//	GstElement *source;
//	GstElement *filter1;
//	GstElement *sink;
//} MyData;
//
//static gboolean	my_bus_callback(GstBus *bus, GstMessage *message, gpointer data)
//{
//	g_print("Got %s message\n", GST_MESSAGE_TYPE_NAME(message));
//	switch (GST_MESSAGE_TYPE(message)) {
//
//	case GST_MESSAGE_ERROR: {
//		GError *err;
//		gchar *debug;
//		gst_message_parse_error(message, &err, &debug);
//		if (err) {
//			g_printerr("ERROR: %s\n", err->message);
//			g_error_free(err);
//		}
//		if (debug) {
//			g_printerr("[Debug details: %s]\n", debug);
//			g_free(debug);
//		}
//		g_main_loop_quit(loop);
//		break;
//
//	case GST_MESSAGE_EOS:
//		/* end-of-stream */
//		g_main_loop_quit(loop);
//		g_print("EOS\n");
//		break;
//	default:
//		/* unhandled message */
//		//g_printerr ("Unexpected message of type %d\n", GST_MESSAGE_TYPE (message));
//		break;
//	}
//	}
//	/* we want to be notified again the next time there is a message
//	* on the bus, so returning TRUE (FALSE means we want to stop watching
//	* for messages on the bus and our callback should not be called again)
//	*/
//	return TRUE;
//}
//
//static void pad_added_handler(GstElement *src, GstPad *new_pad, GstElement *element) {
//	GstPad *sink_pad = gst_element_get_static_pad(element, "sink");
//	GstPadLinkReturn ret;
//	const GstCaps *new_pad_caps = NULL;
//	GstStructure *new_pad_struct = NULL;
//	const gchar *new_pad_type = NULL;
//
//	g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));
//
//	/* If our converter is already linked, we have nothing to do here */
//	if (gst_pad_is_linked(sink_pad)) {
//		g_print("  We are already linked. Ignoring.\n");
//		goto exit;
//	}
//
//	/* Check the new pad's type */
//	new_pad_caps = gst_pad_get_pad_template_caps(new_pad);
//	new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
//	new_pad_type = gst_structure_get_name(new_pad_struct);
//
//	/* Attempt the link */
//	ret = gst_pad_link(new_pad, sink_pad);
//	if (GST_PAD_LINK_FAILED(ret)) {
//		g_print("  Type is '%s' but link failed.\n", new_pad_type);
//	}
//	else {
//		g_print("  Link succeeded (type '%s').\n", new_pad_type);
//	}
//
//exit:
//	/* Unreference the new pad's caps, if we got them */
//	if (new_pad_caps != NULL)
//		gst_caps_unref((GstCaps *)new_pad_caps);
//
//	/* Unreference the sink pad */
//	gst_object_unref(sink_pad);
//}
//
//gboolean parse_options(int argc, char** argv, gchar **url, gchar **user_id, gchar **user_pass, gchar **filename) {
//
//	if (argc < 2) {
//		g_print("usage: %s <-u url> <-i user_id>  <-p password>\n", argv[0]);
//		return FALSE;
//	}
//
//	//gchar* url, user_id, user_pass;
//	GOptionContext *ctx;
//	GError *err = NULL;
//
//	GOptionEntry entries[] = {
//		{ "url", 'u', 0, G_OPTION_ARG_STRING, url, "url to connect", NULL },
//		{ "user_id", 'i', 0, G_OPTION_ARG_STRING, user_id, "username to login", NULL },
//		{ "user_pw", 'p', 0, G_OPTION_ARG_STRING, user_pass, "password to login", NULL },
//		{ "file", 'f', 0, G_OPTION_ARG_FILENAME, filename, "file to save decoded pipeline", NULL },
//		{ NULL }
//	};
//
//	ctx = g_option_context_new("- Your application");
//	g_option_context_add_main_entries(ctx, entries, NULL);
//	g_option_context_set_help_enabled(ctx, TRUE);
//	g_option_context_add_group(ctx, gst_init_get_option_group());
//	if (!g_option_context_parse(ctx, &argc, &argv, &err)) {
//		g_print("Failed to initialize: %s\n", err->message);
//		g_error_free(err);
//		return FALSE;
//	}
//	g_print("Run me with --help to see the Application options appended.\n");
//
//	return TRUE;
//}
//
//static void decodebin_path(GstChildProxy *child_proxy, GObject *object, gchar *name, gchar *filename) {
//
//	FILE *data;
//	char temp[256];
//
//	sprintf_s(temp, "\n%s", gst_plugin_feature_get_name(GST_PLUGIN_FEATURE(gst_element_get_factory(GST_ELEMENT_CAST(object)))));
//
//	int size = strlen(gst_plugin_feature_get_name(GST_PLUGIN_FEATURE(gst_element_get_factory(GST_ELEMENT_CAST(object)))));
//
//	/*if (filename ? fopen_s(&data, filename, "a") : fopen_s(&data, "pipe.txt", "a"))
//	{
//		printf("Error opening file\n");
//		exit(-1);
//	}*/
//
//	//fwrite(temp, size + 1, 1, data);
//	//fclose(data);
//
//	//Sleep(1000);
//}
//
//int	main(int argc, char *argv[])
//{
//	//counter = 0;
//	MyData data;
//	GstBus *bus;
//	GstStateChangeReturn ret;
//	gboolean terminate = FALSE;
//	gchar *url = NULL, *user_id = NULL, *user_pass = NULL, *filename = NULL;
//
//	gst_init(&argc, &argv);
//
//	if (!parse_options(argc, argv, &url, &user_id, &user_pass, &filename)) {
//		g_print("Nepodarilo sa rozparsovat. \n");
//		return -1;
//	}
//
//	g_print("url: %s \nusername: %s \npassword: %s\nfilename: %s\n", url, user_id, user_pass, filename);
//
//	data.source = gst_element_factory_make("filesrc", "source");
//	data.filter1 = gst_element_factory_make("decodebin2", "filter1");
//	data.sink = gst_element_factory_make("d3dvideosink", "sink");
//
//	data.pipeline = gst_pipeline_new("test-pipeline");
//
//	if (!data.pipeline || !data.source || !data.filter1 || !data.sink) {
//		g_printerr("Not all elements could be created.\n");
//		return -1;
//	}
//
//	g_object_set(data.source,
//		"location", "D:\\shared\\1.mp4",
//		NULL);
//
//	/*if (user_id != NULL && user_pass != NULL) {
//		g_object_set(data.source,
//			"user-id", user_id,
//			"user-pw", user_pass,
//			NULL);
//	}*/
//
//	gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.filter1, data.sink, NULL);
//
//	//g_signal_connect (data.source, "pad-added", G_CALLBACK (pad_added_handler), data.filter1);
//	//g_signal_connect (data.filter1, "pad_added", G_CALLBACK (pad_added_handler), data.sink);
//	//g_signal_connect (data.filter1, "child-added", G_CALLBACK (decodebin_path), filename);
//	gboolean result;
//	result = gst_element_link(data.source, data.filter1);
//	g_signal_connect(data.filter1, "pad_added", G_CALLBACK(pad_added_handler), data.sink);
//	g_signal_connect(data.filter1, "child-added", G_CALLBACK(decodebin_path), filename);
//
//	ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
//
//	if (ret == GST_STATE_CHANGE_FAILURE) {
//		g_printerr("Unable to set the pipeline to the playing state.\n");
//		gst_object_unref(data.pipeline);
//		return -1;
//	}
//
//	bus = gst_element_get_bus(data.pipeline);
//
//	/* wait until we either get an EOS or an ERROR message. Note that in a real
//	* program you would probably not use gst_bus_poll(), but rather set up an
//	* async signal watch on the bus and run a main loop and connect to the
//	* bus’s signals to catch certain messages or all messages */
//
//	guint bus_watch_id;
//	bus_watch_id = gst_bus_add_watch(bus, my_bus_callback, NULL);
//
//	loop = g_main_loop_new(NULL, FALSE);
//	g_main_loop_run(loop);
//
//	gst_element_set_state(data.pipeline, GST_STATE_NULL);
//	gst_object_unref(data.pipeline);
//	gst_object_unref(bus);
//	g_source_remove(bus_watch_id);
//	g_main_loop_unref(loop);
//	return 0;
//}