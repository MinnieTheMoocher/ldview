#import "LDViewController.h"
#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "OCLocalStrings.h"
#include <LDLib/LDrawModelViewer.h>
#include <TRE/TREMainModel.h>

@implementation LDViewController

- (id)init
{
	[OCLocalStrings loadStringTable:[[NSBundle mainBundle]
		pathForResource:@"LDViewMessages" ofType:@"ini"]];
	ldrawFileTypes = [[NSArray alloc] initWithObjects: @"ldr", @"dat", @"mpd",
		nil];
	TREMainModel::loadStudTexture([[[NSBundle mainBundle] pathForResource:
		@"StudLogo" ofType:@"png"] cStringUsingEncoding:NSASCIIStringEncoding]);
	return [super init];
}

- (ModelWindow *)currentModelWindow
{
	return (ModelWindow *)[[[NSApplication sharedApplication] mainWindow]
		delegate];
}

- (LDrawModelView *)currentModelView
{
	return [[self currentModelWindow] modelView];
}

- (BOOL)createWindow:(NSString *)filename
{
	ModelWindow *modelWindow = [[ModelWindow alloc] init];
	
	if (filename)
	{
		return [modelWindow openModel:filename];
	}
	else
	{
		return NO;
	}
}

- (IBAction)newWindow:(id)sender
{
	[self createWindow:nil];
}

- (BOOL)openFile:(NSString *)filename
{
	if (![[NSApplication sharedApplication] mainWindow])
	{
		return [self createWindow:filename];
	}
	else
	{
		return [[self currentModelWindow] openModel:filename];
	}
}

- (void)openModel
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];

	[openPanel setMessage:[OCLocalStrings get:@"SelectModelFile"]];
	if ([openPanel runModalForTypes:ldrawFileTypes] == NSOKButton)
	{
		[self openFile:[openPanel filename]];
	}
}

- (IBAction)openModel:(id)sender
{
	return [self openModel];
}

- (BOOL)application:(NSApplication *)theApplication
		   openFile:(NSString *)filename
{
	if ([self openFile:filename])
	{
		launchFileOpened = YES;
		return YES;
	}
	else
	{
		return NO;
	}
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	if (!launchFileOpened)
	{
		[self openModel];
	}
}

- (IBAction)resetView:(id)sender
{
	[[self currentModelView] resetView:sender];
}

@end