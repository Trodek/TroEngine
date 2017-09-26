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

bool JSONManager::Awake()
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

JSONDoc::JSONDoc(const char * path, JSON_Value * value, JSON_Object * obj) : value(value), object(obj), path(path)
{
}

JSONDoc::~JSONDoc()
{
}

void JSONDoc::SetString(const char * keyword, const char * str)
{
	json_object_dotset_string(object, keyword, str);
}

void JSONDoc::SetBool(const char * keyword, bool value)
{
	json_object_dotset_boolean(object, keyword, value);
}

void JSONDoc::SetNumber(const char * keyword, double num)
{
	json_object_dotset_number(object, keyword, num);
}

const char * JSONDoc::GetString(const char * keyword) const
{
	return json_object_dotget_string(object, keyword);
}

bool JSONDoc::GetBool(const char * keyword) const
{
	return json_object_dotget_boolean(object, keyword);
}

double JSONDoc::GetNumber(const char * keyword) const
{
	return json_object_dotget_number(object, keyword);;
}

std::string JSONDoc::GetPath() const
{
	return path;
}

void JSONDoc::SaveFile()
{
	json_serialize_to_file_pretty(value, path.c_str());
}

void JSONDoc::CleanUp()
{
	json_value_free(value);
}
