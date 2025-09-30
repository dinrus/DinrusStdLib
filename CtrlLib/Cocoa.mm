#ifndef flagSKELETON

#include <drx/CtrlLib/CtrlLib.h>
#include <drx/CtrlCore/CocoMM.h>

namespace drx {

#ifdef GUI_COCOA

FileSelNative::FileSelNative()
{
	confirm = true;
	multi = hidden = false;
	activetype = 0;
}

FileSelNative& FileSelNative::AllFilesType()
{
	return Type(t_("All files"), "*.*");
}

bool FileSelNative::Execute0(i32 open, tukk title)
{
	path.Clear();
	Ctrl::ReleaseCtrlCapture();
	if(!title)
		title = open ? t_("Open") : t_("Save as");
	CFRef<CFTxtRef> mmtitle = CFTxtCreateWithCTxt(NULL, title, kCFTxtEncodingUTF8);

	if(open) {
		NSOpenPanel *panel = [NSOpenPanel openPanel];
		[panel setAllowsMultipleSelection: multi ? YES : NO];
		[panel setMessage:(NSTxt *)~mmtitle];
		if(open == 2) {
			[panel setCanChooseDirectories: YES];
			[panel setCanChooseFiles: NO];
		}
		else {
			[panel setCanChooseDirectories: NO];
			[panel setCanChooseFiles: YES];
		}
		if([panel runModal] == NSModalResponseOK) {
			NSArray* urls = [panel URLs];
			for(i32 i = 0; i < urls.count; i++)
				path.Add([[urls objectAtIndex:i] fileSystemRepresentation]);
		}
	}
	else {
		NSSavePanel *panel = [NSSavePanel savePanel];
		[panel setMessage:(NSTxt *)~mmtitle];
		if([panel runModal] == NSModalResponseOK)
			path.Add([[panel URL] fileSystemRepresentation]);
	}
	return path.GetCount();
}

#endif

}
#endif