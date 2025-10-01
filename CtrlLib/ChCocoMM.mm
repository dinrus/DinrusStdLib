#include <drx/Core/config.h>

#ifdef TARGET_COCOA

#include <Carbon/Carbon.h> 

#endif

#include <drx/CtrlLib/CtrlLib.h>

#ifdef GUI_COCOA

#include <drx/CtrlCore/CocoMM.h>
#include "ChCocoMM.h"
	
void Coco_PaintCh(uk cgcontext, i32 type, i32 value, i32 state)
{
	auto cg = (CGContextRef) cgcontext	;
	if(drx::IsUHDMode())
		CGContextScaleCTM(cg, 2, 2);
	CGRect cr = CGRectMake(0, 0, 140, 140);
	if(type == COCO_NSCOLOR) {
		CGContextSaveGState(cg);
		CGContextSetFillColorWithColor(cg, drx::decode(value,
			COCO_PAPER, [NSColor textBackgroundColor].CGColor,
			COCO_SELECTEDTEXT, [NSColor selectedTextColor].CGColor,
			COCO_SELECTEDPAPER, [NSColor selectedTextBackgroundColor].CGColor,
			COCO_DISABLED, [NSColor disabledControlTextColor].CGColor,
			COCO_WINDOW, [NSColor windowBackgroundColor].CGColor,
			COCO_SELECTEDMENUTEXT, [NSColor selectedMenuItemTextColor].CGColor,
			[NSColor textColor].CGColor
		));
		CGContextFillRect(cg, cr);
		CGContextRestoreGState(cg);
	}
	else
	if(type == COCO_NSIMAGE) {
		NSImage *img = [NSImage imageNamed:(value ? NSImageNameInfo : NSImageNameCaution)];
	    NSGraphicsContext *gc = [NSGraphicsContext graphicsContextWithCGContext:cg flipped:YES];
	    NSGraphicsContext* cgc = [NSGraphicsContext currentContext];
	    [NSGraphicsContext setCurrentContext:gc];
	    [img drawInRect:NSMakeRect(0, 0, 48, 48)];
		[NSGraphicsContext setCurrentContext:cgc];
	}
	else {
	    CGContextSaveGState(cg);
	    [NSGraphicsContext saveGraphicsState];
	    [NSGraphicsContext setCurrentContext:
			[NSGraphicsContext graphicsContextWithCGContext:cg flipped:YES]];
	
	    const CGRect dirtyRect = CGRectMake(20, 20, 100, 100);
	
		if(drx::findarg(type, COCO_SCROLLTHUMB, COCO_SCROLLTRACK) >= 0) {
			i32 cx = [NSScroller scrollerWidthForControlSize:NSControlSizeRegular scrollerStyle:NSScrollerStyleLegacy];
			NSScroller *scroller = [[NSScroller alloc] initWithFrame:NSMakeRect(0, 0, 100, cx)];
		    scroller.floatVal = 0;
		    scroller.knobProportion = 1;
			scroller.knobStyle = NSScrollerKnobStyleDefault;
			scroller.scrollerStyle = NSScrollerStyleLegacy;
			scroller.frame = cr;
			if(type == COCO_SCROLLTHUMB)
				[scroller drawKnob];
			else
				[scroller drawKnobSlotInRect:CGRectMake(20, 20, 100, cx) highlight:YES];
			[scroller release];
		}
		else
		if(type == COCO_TEXTFIELD) {
			NSTextField *tf = [[NSTextField alloc] init];
			tf.enabled = YES;
			tf.editable = YES;
			tf.bezeled = YES;
			tf.frame = CGRectMake(0, 0, 140, 40);
			[tf drawRect:dirtyRect];
			[tf release];
		}
		else {
			NSButton *bc = type == COCO_POPUPBUTTON ? [[NSPopUpButton alloc] init] : [[NSButton alloc] init];
			bc.allowsMixedState = type == COCO_CHECKBOX;
			bc.title = @"";
			bc.controlSize = type == COCO_RADIOBUTTON ? NSControlSizeSmall : NSControlSizeRegular;
			bc.frame = cr;
			bc.buttonType = drx::decode(type, COCO_CHECKBOX, NSButtonTypeSwitch, COCO_RADIOBUTTON, NSButtonTypeRadio, NSButtonTypePushOnPushOff);
			bc.bezelStyle = type == COCO_BUTTON ? NSBezelStyleRounded : NSBezelStyleRegularSquare;
			bc.state = drx::decode(value, 0, NSControlStateValOff, 1, NSControlStateValOn, NSControlStateValMixed);
	        [bc highlight: state == drx::CTRL_PRESSED];
			bc.enabled = state != drx::CTRL_DISABLED;
			[bc drawRect:dirtyRect];
			[bc release];
		}
	
	    [NSGraphicsContext restoreGraphicsState];
	    CGContextRestoreGState(cg);
	}
}

#endif
