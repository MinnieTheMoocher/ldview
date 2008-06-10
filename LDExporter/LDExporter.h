#ifndef __LDEXPORTER_H__
#define __LDEXPORTER_H__

#include <LDLoader/LDLPrimitiveCheck.h>
#include <LDLoader/LDLCamera.h>
#include <TCFoundation/TCVector.h>
#include <TCFoundation/TCStlIncludes.h>
#include <LDExporter/LDExporterSetting.h>
#include <TCFoundation/mystring.h>

class LDLAutoCamera;
class TREModel;

typedef std::list<LDExporterSetting> LDExporterSettingList;

class LDExporter: public LDLPrimitiveCheck
{
public:
	static int run(void);
	virtual void setBoundingBox(const TCVector &min, const TCVector &max)
	{
		m_boundingMin = min;
		m_boundingMax = max;
	}
	virtual void setCenter(const TCVector &center) { m_center = center; }
	virtual void setRadius(TCFloat radius) { m_radius = radius; }
	virtual void setCamera(const LDLCamera &camera) { m_camera = camera; }
	virtual int doExport(LDLModel *pTopModel);
	virtual int doExport(TREModel *pTopModel);
	virtual int doExport(LDLModel *pTopLDLModel, TREModel *pTopTREModel);
	virtual void setFilename(const char *filename) { m_filename = filename; }
	virtual void setRotationMatrix(const float *matrix)
	{
		memcpy(m_rotationMatrix, matrix, sizeof(m_rotationMatrix));
	}
	virtual void setFov(TCFloat value) { m_fov = value; }
	virtual void setXPan(TCFloat value) { m_xPan = value; }
	virtual void setYPan(TCFloat value) { m_yPan = value; }
	virtual void setAppName(const char *value) { m_appName = value; }
	virtual void setAppVersion(const char *value) { m_appVersion = value; }
	virtual void setAppUrl(const char *value) { m_appUrl = value; }
	virtual void setAppCopyright(const char *value) { m_appCopyright = value; }
	virtual const LDExporterSettingList &getSettings(void) const;
	virtual LDExporterSettingList &getSettings(void);
	virtual std::string getExtension(void) const;
	virtual ucstring getTypeDescription(void) const;
	virtual bool usesLDLModel(void) const { return true; }
	virtual bool usesTREModel(void) const { return false; }
protected:
	LDLCamera m_camera;
	TCFloat m_rotationMatrix[16];
	TCFloat m_width;
	TCFloat m_height;
	TCFloat m_fov;
	TCFloat m_xPan;
	TCFloat m_yPan;
	TCVector m_boundingMin;
	TCVector m_boundingMax;
	TCVector m_center;
	TCFloat m_radius;

	bool m_primSub;
	float m_seamWidth;
	bool m_edges;
	bool m_conditionalEdges;
	std::string m_udPrefix;
	std::string m_filename;
	std::string m_appName;
	std::string m_appVersion;
	std::string m_appUrl;
	std::string m_appCopyright;

	mutable LDExporterSettingList m_settings;
	mutable bool m_settingsInitialized;

	LDExporter(const char *udPrefix = "");
	virtual ~LDExporter(void);
	void dealloc(void);
	virtual int runInternal(void);
	virtual int runInternal(LDExporter *pExporter);
	static std::string getFilename(void);
	TCFloat getHFov(void);
	virtual bool addSetting(const LDExporterSetting &setting) const;
	virtual bool addSetting(CUCSTR name, TCFloat value, const char *key,
		TCFloat min, TCFloat max) const;
	virtual bool addSetting(CUCSTR name, long value, const char *key,
		long min, long max) const;
	virtual void addPrimSubSetting(void) const;
	virtual void addSeamWidthSetting(void) const;
	virtual int getNumEdgesSettings(void) const { return 1; }
	virtual void addEdgesSettings(void) const;
	virtual void addEdgesSetting(void) const;
	virtual void addGeometrySetting(void) const;
	virtual void addGeometrySettings(void) const;
	virtual void addConditionalEdgesSetting(void) const;
	virtual int getNumGeometrySettings(void) const { return 3; }
	virtual void initSettings(void) const;

	// NOTE: loadDefaults is NOT virtual: it's called from the constructor.
	void loadDefaults(void);

	std::string udKey(const char *key) const;
	void setStringForKey(const char* value, const char* key,
		bool sessionSpecific = true);
	char* stringForKey(const char* key, const char* defaultValue = NULL,
		bool sessionSpecific = true);
	void setLongForKey(long value, const char* key,
		bool sessionSpecific = true);
	long longForKey(const char* key, long defaultValue = 0,
		bool sessionSpecific = true);
	void setBoolForKey(bool value, const char *key,
		bool sessionSpecific = true);
	bool boolForKey(const char *key, bool defaultValue = false,
		bool sessionSpecific = true);
	std::string pathForKey(const char *key, const char *defaultValue = NULL,
		bool sessionSpecific = true);
	void setFloatForKey(float value, const char* key,
		bool sessionSpecific = true);
	float floatForKey(const char* key, float defaultValue = 0.0f,
		bool sessionSpecific = true);
};

#endif // __LDEXPORTER_H__
