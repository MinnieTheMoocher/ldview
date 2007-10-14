#include "LDSnapshotTaker.h"
#include "LDUserDefaultsKeys.h"
#include "LDrawModelViewer.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCImage.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCUserDefaults.h>

LDSnapshotTaker::LDSnapshotTaker(LDrawModelViewer *m_modelViewer):
m_modelViewer(m_modelViewer),
m_imageType(ITPng),
m_trySaveAlpha(false)
{
}

LDSnapshotTaker::~LDSnapshotTaker(void)
{
}

void LDSnapshotTaker::dealloc(void)
{
	TCObject::dealloc();
}

bool LDSnapshotTaker::saveImage(
	const char *filename,
	int imageWidth,
	int imageHeight,
	bool zoomToFit)
{
	char *cameraGlobe = TCUserDefaults::stringForKey(CAMERA_GLOBE_KEY, NULL,
		false);
	bool saveAlpha = false;
	TCByte *buffer = grabImage(imageWidth, imageHeight, zoomToFit || cameraGlobe
		!= NULL, NULL, &saveAlpha);
	bool retValue = false;

	delete cameraGlobe;
	if (buffer)
	{
		if (m_imageType == ITPng)
		{
			retValue = writePng(filename, imageWidth, imageHeight, buffer,
				saveAlpha);
		}
		else if (m_imageType == ITBmp)
		{
			retValue = writeBmp(filename, imageWidth, imageHeight, buffer);
		}
		delete buffer;
	}
	return retValue;
}

bool LDSnapshotTaker::staticImageProgressCallback(
	CUCSTR message,
	float progress,
	void* userData)
{
	return ((LDSnapshotTaker*)userData)->imageProgressCallback(message,
		progress);
}

bool LDSnapshotTaker::imageProgressCallback(CUCSTR message, float progress)
{
	bool aborted;
	if (message == NULL)
	{
		message = _UC("");
	}

	TCProgressAlert::send("LDSnapshotTaker", message, progress, &aborted);
	return !aborted;
}

bool LDSnapshotTaker::writeImage(
	const char *filename,
	int width,
	int height,
	TCByte *buffer,
	char *formatName,
	bool saveAlpha)
{
	TCImage *image = new TCImage;
	bool retValue;
	char comment[1024];

	if (saveAlpha)
	{
		image->setDataFormat(TCRgba8);
	}
	image->setSize(width, height);
	image->setLineAlignment(4);
	image->setImageData(buffer);
	image->setFormatName(formatName);
	image->setFlipped(true);
	if (strcasecmp(formatName, "PNG") == 0)
	{
		strcpy(comment, "Software:!:!:LDView");
	}
	else
	{
		strcpy(comment, "Created by LDView");
	}
	if (m_productVersion.size() > 0)
	{
		strcat(comment, " ");
		strcat(comment, m_productVersion.c_str());
	}
	image->setComment(comment);
	if (TCUserDefaults::longForKey(AUTO_CROP_KEY, 0, false))
	{
		image->autoCrop((TCByte)m_modelViewer->getBackgroundR(),
			(TCByte)m_modelViewer->getBackgroundG(),
			(TCByte)m_modelViewer->getBackgroundB());
	}
	retValue = image->saveFile(filename, staticImageProgressCallback, this);
	image->release();
	return retValue;
}

bool LDSnapshotTaker::writeBmp(
	const char *filename,
	int width,
	int height,
	TCByte *buffer)
{
	return writeImage(filename, width, height, buffer, "BMP", false);
}

bool LDSnapshotTaker::writePng(
	const char *filename,
	int width,
	int height,
	TCByte *buffer,
	bool saveAlpha)
{
	return writeImage(filename, width, height, buffer, "PNG", saveAlpha);
}

void LDSnapshotTaker::calcTiling(
	int desiredWidth,
	int desiredHeight,
	int &bitmapWidth,
	int &bitmapHeight,
	int &numXTiles,
	int &numYTiles)
{
	if (desiredWidth > bitmapWidth)
	{
		numXTiles = (desiredWidth + bitmapWidth - 1) / bitmapWidth;
	}
	else
	{
		numXTiles = 1;
	}
	bitmapWidth = desiredWidth / numXTiles;
	if (desiredHeight > bitmapHeight)
	{
		numYTiles = (desiredHeight + bitmapHeight - 1) / bitmapHeight;
	}
	else
	{
		numYTiles = 1;
	}
	bitmapHeight = desiredHeight / numYTiles;
}

bool LDSnapshotTaker::canSaveAlpha(void)
{
	if (m_trySaveAlpha && m_imageType == ITPng)
	{
		GLint alphaBits;

		glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
		return alphaBits > 0;
	}
	return false;
}

void LDSnapshotTaker::renderOffscreenImage(void)
{
	TCAlertManager::sendAlert(makeCurrentAlertClass(), this);
	m_modelViewer->update();
}

TCByte *LDSnapshotTaker::grabImage(
	int &imageWidth,
	int &imageHeight,
	bool zoomToFit,
	TCByte *buffer,
	bool *saveAlpha)
{
	GLenum bufferFormat = GL_RGB;
	bool origForceZoomToFit = m_modelViewer->getForceZoomToFit();
	TCVector origCameraPosition = m_modelViewer->getCamera().getPosition();
	TCFloat origXPan = m_modelViewer->getXPan();
	TCFloat origYPan = m_modelViewer->getYPan();
	bool origAutoCenter = m_modelViewer->getAutoCenter();
	GLint viewport[4];
	int newWidth, newHeight;
	int numXTiles, numYTiles;
	int xTile;
	int yTile;
	TCByte *smallBuffer;
	int bytesPerPixel = 3;
	int bytesPerLine;
	int smallBytesPerLine;
	bool canceled = false;
	bool bufferAllocated = false;

	if (zoomToFit)
	{
		m_modelViewer->setForceZoomToFit(true);
	}
	glGetIntegerv(GL_VIEWPORT, viewport);
	newWidth = viewport[2];
	newHeight = viewport[3];
	calcTiling(imageWidth, imageHeight, newWidth, newHeight, numXTiles,
		numYTiles);
	if (canSaveAlpha())
	{
		bytesPerPixel = 4;
		bufferFormat = GL_RGBA;
		m_modelViewer->setSaveAlpha(true);
		if (saveAlpha)
		{
			*saveAlpha = true;
		}
	}
	else
	{
		if (saveAlpha)
		{
			*saveAlpha = false;
		}
	}
	imageWidth = newWidth * numXTiles;
	imageHeight = newHeight * numYTiles;
	smallBytesPerLine = TCImage::roundUp(newWidth * bytesPerPixel, 4);
	bytesPerLine = TCImage::roundUp(imageWidth * bytesPerPixel, 4);
	if (!buffer)
	{
		buffer = new TCByte[bytesPerLine * imageHeight];
		bufferAllocated = true;
	}
	if (numXTiles == 1 && numYTiles == 1)
	{
		smallBuffer = buffer;
	}
	else
	{
		smallBuffer = new TCByte[smallBytesPerLine * newHeight];
	}
	m_modelViewer->setNumXTiles(numXTiles);
	m_modelViewer->setNumYTiles(numYTiles);
	for (yTile = 0; yTile < numYTiles; yTile++)
	{
		m_modelViewer->setYTile(yTile);
		for (xTile = 0; xTile < numXTiles && !canceled; xTile++)
		{
			m_modelViewer->setXTile(xTile);
			renderOffscreenImage();
			TCProgressAlert::send("LDSnapshotTaker",
				TCLocalStrings::get(_UC("RenderingSnapshot")),
				(float)(yTile * numXTiles + xTile) / (numYTiles * numXTiles),
				&canceled);
			if (!canceled)
			{
				glReadPixels(0, 0, newWidth, newHeight, bufferFormat,
					GL_UNSIGNED_BYTE, smallBuffer);
				if (smallBuffer != buffer)
				{
					int y;

					for (y = 0; y < newHeight; y++)
					{
						int smallOffset = y * smallBytesPerLine;
						int offset = (y + (numYTiles - yTile - 1) * newHeight) *
							bytesPerLine + xTile * newWidth * bytesPerPixel;

						memcpy(&buffer[offset], &smallBuffer[smallOffset], smallBytesPerLine);
					}
					// We only need to zoom to fit on the first tile; the
					// rest will already be correct.
					m_modelViewer->setForceZoomToFit(false);
				}
			}
			else
			{
				canceled = true;
			}
		}
	}
	m_modelViewer->setXTile(0);
	m_modelViewer->setYTile(0);
	m_modelViewer->setNumXTiles(1);
	m_modelViewer->setNumYTiles(1);
	m_modelViewer->setSaveAlpha(false);
	if (canceled && bufferAllocated)
	{
		delete buffer;
		buffer = NULL;
	}
	if (smallBuffer != buffer)
	{
		delete smallBuffer;
	}
	if (zoomToFit)
	{
		m_modelViewer->setForceZoomToFit(origForceZoomToFit);
		m_modelViewer->getCamera().setPosition(origCameraPosition);
		m_modelViewer->setXYPan(origXPan, origYPan);
		m_modelViewer->setAutoCenter(origAutoCenter);
	}
	return buffer;
}