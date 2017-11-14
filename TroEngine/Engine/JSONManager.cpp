#include "JSONManager.h"
#include "Application.h"

//-----------------------------------------
//				JSONManager
//-----------------------------------------

JSONManager::JSONManager(bool start_enabled) :Module(start_enabled)
{
	SetName("JSONManager");
}

JSONManager::~JSONManager()
{
}

bool JSONManager::Awake(JSONDoc* config)
{
	bool ret = true;

	EDITOR_LOG("JSONManager Staring...");

	return ret;
}

bool JSONManager::CleanUp()
{
	bool ret = true;

	EDITOR_LOG("Cleaning JSONManager...");

	for (std::list<JSONDoc*>::iterator json_doc = loaded_files.begin(); json_doc != loaded_files.end();)
	{
		(*json_doc)->CleanUp();
		RELEASE(*json_doc);

		json_doc = loaded_files.erase(json_doc);
	}

	return ret;
}

JSONDoc * JSONManager::LoadJSONFile(const char * path)
{
	JSONDoc* doc = nullptr;

	//Check if the file is already loaded
	for (std::list<JSONDoc*>::iterator json_doc = loaded_files.begin(); json_doc != loaded_files.end(); ++json_doc)
	{
		if (path == (*json_doc)->GetPath())
		{
			doc = (*json_doc);
			break;
		}
	}

	//if the file is not loaded, load it
	if (doc == nullptr)
	{
		JSON_Value* doc_data = json_parse_file(path);

		if (doc_data == nullptr)
		{
			EDITOR_LOG("Error loading file: %s.", path);
		}
		else 
		{
			JSON_Object* obj_data = json_value_get_object(doc_data);

			EDITOR_LOG("Success loading file: %s.", path);

			doc = new JSONDoc(path, doc_data, obj_data);

			loaded_files.push_back(doc);

		}
	}

	return doc;
}

JSONDoc * JSONManager::CreateJSONFile(const char * path)
{
	JSONDoc* doc = nullptr;

	//Check if the file already exists
	bool exist = false;
	for (std::list<JSONDoc*>::iterator json_doc = loaded_files.begin(); json_doc != loaded_files.end(); ++json_doc)
	{
		if (path == (*json_doc)->GetPath())
		{
			exist = true;
			break;
		}
	}

	if (exist)
	{
		EDITOR_LOG("File %s already exists.", path);
	}
	else
	{
		JSON_Value* doc_data = json_value_init_object();
		JSON_Object* object_data = json_value_get_object(doc_data);

		doc = new JSONDoc(path, doc_data, object_data);
		loaded_files.push_back(doc);
	}

	return doc;
}

//------------------------------
//			JSONDoc
//------------------------------

JSONDoc::JSONDoc(const char * path, JSON_Value * value, JSON_Object * obj) : value(value), root(obj), object(obj), path(path)
{
}

JSONDoc::~JSONDoc()
{
}

const char * JSONDoc::GetStringFromArray(const std::string & array_keyword, int index)
{
	const char* ret = nullptr;

	JSON_Array* array = json_object_get_array(object, array_keyword.c_str());

	if (array != nullptr)
	{
		if (FindArrayValue(array_keyword.c_str(), index, json_value_type::JSONString))
		{
			ret = json_array_get_string(array, index);
		}
	}

	return ret;
}

bool JSONDoc::GetBoolFromArray(const std::string & array_keyword, int index)
{
	bool ret = false;

	JSON_Array* array = json_object_get_array(object, array_keyword.c_str());

	if (array != nullptr)
	{
		if (FindArrayValue(array_keyword.c_str(), index, json_value_type::JSONBoolean))
		{
			ret = json_array_get_boolean(array, index);
		}
	}

	return ret;
}

double JSONDoc::GetNumberFromArray(const std::string & array_keyword, int index)
{
	double ret = 0;

	JSON_Array* array = json_object_get_array(object, array_keyword.c_str());

	if (array != nullptr)
	{
		if (FindArrayValue(array_keyword.c_str(), index, json_value_type::JSONNumber))
		{
			ret = json_array_get_number(array, index);
		}
	}

	return ret;
}

void JSONDoc::SetString(const std::string& keyword, const char* str)
{
	json_object_dotset_string(object, keyword.c_str(), str);
}

void JSONDoc::SetBool(const std::string& keyword, bool value)
{
	json_object_dotset_boolean(object, keyword.c_str(), value);
}

void JSONDoc::SetNumber(const std::string& keyword, double num)
{
	json_object_dotset_number(object, keyword.c_str(), num);
}

void JSONDoc::SetVector3(const std::string & keyword, float3 vec)
{
	AddArray(keyword);
	AddNumberToArray(keyword, vec.x);
	AddNumberToArray(keyword, vec.y);
	AddNumberToArray(keyword, vec.z);
}

const char* JSONDoc::GetString(const std::string& keyword) const
{
	return json_object_dotget_string(object, keyword.c_str());
}

bool JSONDoc::GetBool(const std::string& keyword) const
{
	return json_object_dotget_boolean(object, keyword.c_str());
}

double JSONDoc::GetNumber(const std::string& keyword) const
{
	return json_object_dotget_number(object, keyword.c_str());;
}

std::string JSONDoc::GetPath() const
{
	return path;
}

float3 JSONDoc::GetVector3(const std::string & keyword)
{
	float3 ret = float3::zero;
	ret.x = GetNumberFromArray(keyword, 0);
	ret.y = GetNumberFromArray(keyword, 1);
	ret.z = GetNumberFromArray(keyword, 2);
	return ret;
}

float4 JSONDoc::GetVector4(const std::string & keyword)
{
	float4 ret = float4::zero;
	ret.x = GetNumberFromArray(keyword, 0);
	ret.y = GetNumberFromArray(keyword, 1);
	ret.z = GetNumberFromArray(keyword, 2);
	ret.w = GetNumberFromArray(keyword, 3);
	return ret;
}

int JSONDoc::GetArraySize(const std::string & array_keyword) const
{
	int ret = 0;

	JSON_Array* array = json_object_get_array(object, array_keyword.c_str());

	if (array != nullptr)
	{
		ret = json_array_get_count(array);
	}

	return ret;
}

void JSONDoc::SetVector4(const std::string & keyword, float4 vec)
{
	AddArray(keyword);
	AddNumberToArray(keyword, vec.x);
	AddNumberToArray(keyword, vec.y);
	AddNumberToArray(keyword, vec.z);
	AddNumberToArray(keyword, vec.w);
}

void JSONDoc::AddArray(const std::string & keyword)
{
	JSON_Value* va = json_value_init_array();
	JSON_Array* arr = json_value_get_array(va);

	json_object_dotset_value(object, keyword.c_str(), va);
}

void JSONDoc::AddStringToArray(const std::string & array_keyword, const char * str)
{
	JSON_Array* array = json_object_get_array(object, array_keyword.c_str());

	if (array != nullptr)
	{
		json_array_append_string(array, str);
	}
}

void JSONDoc::AddBoolToArray(const std::string & array_keyword, bool value)
{
	JSON_Array* array = json_object_get_array(object, array_keyword.c_str());

	if (array != nullptr)
	{
		json_array_append_boolean(array, value);
	}
}

void JSONDoc::AddNumberToArray(const std::string & array_keyword, double value)
{
	JSON_Array* array = json_object_get_array(object, array_keyword.c_str());

	if (array != nullptr)
	{
		json_array_append_number(array, value);
	}
}

void JSONDoc::AddSectionToArray(const std::string & array_keyword)
{
	JSON_Array* array = json_object_get_array(object, array_keyword.c_str());

	if (array != nullptr)
	{
		json_array_append_value(array, json_value_init_object());
	}
}

void JSONDoc::AddSection(const std::string & section)
{

}

void JSONDoc::MoveToRoot()
{
	object = root;
}

bool JSONDoc::MoveToSection(const std::string & section)
{
	bool ret = false;

	JSON_Object* obj = json_object_get_object(object, section.c_str());

	if (obj != nullptr)
	{
		object = obj;
		ret = true;
	}

	return ret;
}

bool JSONDoc::MoveToSectionFromArray(const std::string & array_keyword, int index)
{
	bool ret = false;

	JSON_Array* array = json_object_get_array(object, array_keyword.c_str());

	if (array != nullptr)
	{
		JSON_Object* obj = json_array_get_object(array, index);

		object = obj;
		ret = true;
	}

	return ret;
}

void JSONDoc::RemoveSection(const std::string & section)
{
	json_object_remove(object, section.c_str());
}

void JSONDoc::CleanArray(const std::string & array_keyword)
{
}

void JSONDoc::SaveFile()
{
	json_serialize_to_file_pretty(value, path.c_str());
}

void JSONDoc::CleanUp()
{
	json_value_free(value);
}

void JSONDoc::EraseContent()
{
	json_value_free(value);
	value = json_value_init_object();
	object = json_value_get_object(value);
	root = object;
}

bool JSONDoc::FindValue(const char * str, json_value_type type)
{
	bool ret = false;

	JSON_Value* val = json_object_dotget_value(object, str);

	if (val != nullptr && json_value_get_type(val) == type)
		ret = true;

	return ret;
}

bool JSONDoc::FindArrayValue(const char * array_keyword, int index, json_value_type type)
{
	bool ret = false;

	JSON_Array* array = json_object_get_array(object, array_keyword);

	if (array != nullptr)
	{
		JSON_Value* val = json_array_get_value(array, index);

		if (val != nullptr && json_value_get_type(val) == type)
			ret = true;
	}

	return ret;
}
