#include <drx/Core/config.h>

#ifdef TARGET_COCOA

#include <Carbon/Carbon.h> 
#include <drx/CtrlLib/CtrlLib.h>
#include <drx/CtrlCore/CocoMM.h>
#include "ChCocoMM.h"

void Coco_PaintCh(uk cgcontext, i32 type, i32 value, i32 state)
{
	auto cg = (CGContextRef) cgcontext;
	if(drx::IsUHDMode())
		CGContextScaleCTM(cg, 2, 2);
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
			NSScroller *scroller = [[NSScroller alloc] initWithFrame:NSMakeRect(0, 0, 200, 20)];
		    scroller.floatVal = 0;
		    scroller.knobProportion = 1;
			scroller.knobStyle = NSScrollerKnobStyleDefault;
			scroller.scrollerStyle = NSScrollerStyleLegacy;
			scroller.frame = CGRectMake(0, 0, 140, 140);
			if(type == COCO_SCROLLTHUMB)
				[scroller drawKnob];
			else
				[scroller drawKnobSlotInRect:dirtyRect highlight:YES];
			[scroller release];
		}
		else {
			NSButton *bc = [[NSButton alloc] init];
			bc.allowsMixedState = type == COCO_CHECKBOX;
			bc.title = @"";
			bc.controlSize = type == COCO_RADIOBUTTON ? NSControlSizeSmall : NSControlSizeRegular;
			bc.frame = CGRectMake(0, 0, 140, 40);
			bc.buttonType = drx::decode(type, COCO_CHECKBOX, NSButtonTypeSwitch, COCO_RADIOBUTTON, NSButtonTypeRadio, NSButtonTypePushOnPushOff);
			bc.bezelStyle = type == COCO_BUTTON ? NSBezelStyleRounded : NSBezelStyleRegularSquare;
			bc.state = drx::decode(value, 0, NSControlStateValOff, 1, NSControlStateValOn, NSControlStateValMixed);
	        [bc highlight: state == drx::CTRL_PRESSED];
			bc.enabled = state != drx::CTRL_DISABLED;
			[bc drawRect:dirtyRect];
			[bc release];
		}
	}
}

i32   Coco_Metric(i32 k)
{
	SInt32 m;
	GetThemeMetric(k, &m);
	return m;
}

#endif
