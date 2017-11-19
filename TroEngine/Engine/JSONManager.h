#ifndef __JSONMANAGER__
#define __JSONMANAGER__

#include "Module.h"
#include "Parson\parson.h"
#include <list>
#include "MathGeoLib.h"

class JSONDoc
{
public:
	JSONDoc(const char* path, JSON_Value* value, JSON_Object* obj);
	~JSONDoc();

	std::string GetPath() const;
	const char* GetString(const std::string& keyword) const;
	bool GetBool(const std::string& keyword) const;
	double GetNumber(const std::string& keyword) const;
	float3 GetVector3(const std::string& keyword);
	float4 GetVector4(const std::string& keyword);
	int GetArraySize(const std::string& array_keyword) const;
	const char* GetStringFromArray(const std::string& array_keyword, int index);
	bool GetBoolFromArray(const std::string& array_keyword, int index);
	double GetNumberFromArray(const std::string& array_keyword, int index);

	void SetString(const std::string& keyword, const char* str);
	void SetBool(const std::string& keyword, bool value);
	void SetNumber(const std::string& keyword, double num);
	void SetVector3(const std::string& keyword, float3 vec);
	void SetVector4(const std::string& keyword, float4 vec);

	void AddArray(const std::string& keyword);
	void AddStringToArray(const std::string& array_keyword, const char* str);
	void AddBoolToArray(const std::string& array_keyword, bool value);
	void AddNumberToArray(const std::string& array_keyword, double value);
	void AddSectionToArray(const std::string& array_keyword);
	void AddSection(const std::string& section);

	void MoveToRoot();
	bool MoveToSection(const std::string& section);
	bool MoveToSectionFromArray(const std::string& array_keyword, int index);
	
	void RemoveSection(const std::string& section);
	void CleanArray(const std::string& array_keyword);

	void SaveFile();
	void CleanUp();
	void EraseContent();

private:
	bool FindValue(const char* str, json_value_type type);
	bool FindArrayValue(const char* array_keyword, int index, json_value_type type);

private:
	JSON_Value* value = nullptr;
	JSON_Object* object = nullptr;
	JSON_Object* root = nullptr;
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

