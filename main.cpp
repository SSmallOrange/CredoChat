#include <iostream>
#include <list>

#include "LocalConfig/LocalCfg.h"
#include "Log/LogManager.h"
#include "Concurrent/ThreadPool.h"

#include "Net/HttpClient.h"

static CommonModule::Logger::ptr g_logger;

template<typename T>
std::list<T> pool_thread_quick_sort(std::list<T> input) {
	if (input.empty())
	{
		return input;
	}
	std::list<T> result;
	result.splice(result.begin(), input, input.begin());
	T const& partition_val = *result.begin();
	typename std::list<T>::iterator divide_point =
		std::partition(input.begin(), input.end(),
			[&](T const& val) {return val < partition_val; });
	std::list<T> new_lower_chunk;
	new_lower_chunk.splice(new_lower_chunk.end(),
		input, input.begin(),
		divide_point);

	std::future<std::list<T> > new_lower = CommonModule::ThreadPool::instance().commit(pool_thread_quick_sort<T>, new_lower_chunk);

	std::list<T> new_higher(pool_thread_quick_sort(input));
	result.splice(result.end(), new_higher);
	result.splice(result.begin(), new_lower.get());
	return result;
}

void TestThreadPoolSort() {
	std::list<int> nlist = { 6,1,0,5,2,9,11 };

	auto sortlist = pool_thread_quick_sort<int>(nlist);

	for (auto& value : sortlist) {
		LOG_INFO(g_logger) << value << " ";
	}

	LOG_INFO(g_logger) << std::endl;
}


#include <iguana/json_reader.hpp>
#include <iguana/json_writer.hpp>

struct Sun
{
	std::string_view name;
	int age;
};

struct person
{
	std::string_view name;
	int age;
	std::vector<Sun> sun;
};

REFLECTION(person, name, age, sun);
REFLECTION(Sun, name, age);


std::string TestReflection() {
	person p = { "tom", 28 , std::vector<Sun>({ {"child", 11}, {"child222", 14}})};

	std::string ss;
	iguana::to_json(p, ss); // 序列化

	person p1;
	iguana::from_json(p1, ss); // 反序列化
	assert(p1.name == "tom");

	return ss;
}


int main() {
	std::string configFilePath = CommonModule::CurrentExecPath();
	CommonModule::LocalCfg* config = CommonModule::LocalCfg::GetInstance().get();
	// D:\Data\Project\CredoChat\CredoChat\bin\CredoChat.exe
	// std::string test = "hello world!!"; // "D:\\Data\\Project\\CredoChat\\CredoChat\\bin\\Localcfg.xml"
	if (config->OpenXMLDoc(configFilePath + "\\Localcfg.xml")) {
	 	std::string str = CommonModule::LocalCfg::GetInstance()->ReadConfigValue<std::string>("IP", "ClientInfo", "Localcfg.xml");
	}

	g_logger = LOG_NAME("root");

	TestThreadPoolSort();

	LOG_INFO(g_logger) << TestReflection();

	return 0;
}
