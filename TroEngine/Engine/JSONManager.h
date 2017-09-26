#ifndef __JSONMANAGER__
#define __JSONMANAGER__

#include "Module.h"
#include "Parson\parson.h"
#include <list>

class JSONDoc
{
public:
	JSONDoc(const char* path, JSON_Value* value, JSON_Object* obj);
	~JSONDoc();

	const char* GetString(const char* keyword) const;
	bool GetBool(const char* keyword) const;
	double GetNumber(const char* keyword) const;
	std::string GetPath() const;

	void SetString(const char* keyword, const char* str);
	void SetBool(const char* keyword, bool value);
	void SetNumber(const char* keyword, double num);

	void SaveFile();
	void CleanUp();

private:
	JSON_Value* value = nullptr;
	JSON_Object* object = nullptr;
	std::string path;
};

class JSONManager : public Module
{
public:
	JSONManager(bool start_enabled = true);
	~JSONManager();

	bool Awake(JSONDoc* config);
	bool CleanUp();

	JSONDoc* LoadJSONFile(const char* path);
	JSONDoc* CreateJSONFile(const char* path);

private:
	std::list<JSONDoc*> loaded_files;
};

#endif // !__JSONMANAGER__

