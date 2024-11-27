#ifndef DECISIONSTRUCTURE_H__
#define DECISIONSTRUCTURE_H__

#include <iostream>
#include <string>
#include <vector>

// 西瓜个体定义
class Watermelon
{
public:
	std::string id;							// 编号
	std::string Weather;						// 天气
	std::string Temperature;					// 温度
	std::string Humidity;						// 湿度
	std::string Wind;							// 风
	std::string Date;						// 能否约会

	friend
	std::ostream& operator<<(std::ostream& os, const Watermelon& wm)
	{
		os << wm.id << " " << wm.Weather << " " << wm.Temperature << " "
		   << wm.Humidity << " " << wm.Wind << " " << wm.Date ;
		os << std::endl;
		return os;
	}
};

struct Node
{
	std::string attribute;
	std::string edgeValue;
	std::vector<Node*> childs;
	Node() : attribute(""), edgeValue("") { }
};

template <typename T>
using Ptr = T*;
using TreeRoot = Ptr<Node>;

#endif // DECISIONSTRUCTURE_H__
