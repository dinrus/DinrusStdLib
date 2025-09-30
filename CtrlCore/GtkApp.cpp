#include <drx/CtrlCore/CtrlCore.h>

#ifdef GUI_GTK

#include <X11/Xlib.h>

#define CATCH_ERRORS 1

namespace drx {

#define LLOG(x) // DLOG(x)

#if CATCH_ERRORS
void CatchError(const gchar *log_domain,
             GLogLevelFlags log_level,
             const gchar *message,
             gpointer user_data)
{
	RLOG((tukk )message);
	// __BREAK__;
}
#endif

void Ctrl::PanicMsgBox(tukk title, tukk text)
{
	LLOG("PanicMsgBox " << title << ": " << text);
	UngrabMouse();
	char m[2000];
	*m = 0;
	if(system("which gxmessage") == 0)
		strcpy(m, "gxmessage -center \"");
	else
	if(system("which kdialog") == 0)
		strcpy(m, "kdialog --error \"");
	else
	if(system("which xmessage") == 0)
		strcpy(m, "xmessage -center \"");

	if(*m) {
		strcat(m, title);
		strcat(m, "\n");
		strcat(m, text);
		strcat(m, "\"");
		IGNORE_RESULT(system(m));
	}
	else {
		GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
		                                           GTK_BUTTONS_CLOSE, "%s: %s", title, text);
		gtk_dialog_run(GTK_DIALOG (dialog));
		gtk_widget_destroy(dialog);
	}
	__BREAK__;
}

i32 Ctrl::scale;

void InitGtkApp(i32 argc, tuk *argv, tukk *envptr)
{
	LLOG(rmsecs() << " InitGtkApp");
	
	XInitThreads(); // otherwise there are errors despide GuiLock

#if GTK_CHECK_VERSION(3, 10, 0)
	gdk_set_allowed_backends("x11"); // this fixes wayland issues
#endif

	EnterGuiMutex();
	gtk_init(&argc, &argv);

	Ctrl::SetUHDEnabled(true);
	
	Ctrl::scale = 1;
#if GTK_CHECK_VERSION(3, 10, 0)
	Ctrl::scale = gdk_window_get_scale_factor(gdk_screen_get_root_window(gdk_screen_get_default()));
#endif

	Ctrl::GlobalBackBuffer();
	Ctrl::ReSkin();
	g_timeout_add(20, (GSourceFunc) Ctrl::TimeHandler, NULL);
	InstallPanicMessageBox(Ctrl::PanicMsgBox);
	gdk_window_add_filter(NULL, Ctrl::RootKeyFilter, NULL);
#if CATCH_ERRORS
	g_log_set_default_handler (CatchError, 0);
#endif
}

void ExitGtkApp()
{
	LeaveGuiMutex();
}

}

#endif
