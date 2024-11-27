#ifndef REQUIRE_H__
#define REQUIRE_H__

#include "decisionstructure.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stack>
#include <string>
#include <map>
#include <vector>

inline void assure(std::ifstream& in,
                   const std::string& filename = "")
{
	using namespace std;
	if (!in) {
		fprintf(stderr, "Could not open file: %s\n", filename.c_str());
		exit(1);
	}
}

inline void assure(std::ofstream& out,
                   const std::string& filename = "")
{
	using namespace std;
	if (!out) {
		fprintf(stderr, "Could not open file: %s\n", filename.c_str());
	}
}

void load_file(std::vector<Watermelon>& datas,
               std::vector<std::string>& attributes,
               const std::string& filename)
{
	std::ifstream istrm(filename);
	assure(istrm, filename);

	if (istrm.is_open()) {
		char buffer[128];
		bool firstline = true;
		while (!istrm.eof()) {
			istrm.getline(buffer, 128);
			const char* id = strtok(buffer, " ");
			const char* Weather = strtok(NULL, " ");
			const char* Temperature = strtok(NULL, " ");
			const char* Humidity = strtok(NULL, " ");
			const char* Wind = strtok(NULL, " ");
			const char* Date = strtok(NULL, " ");

			// Check if the first line.
			// The first line contains attributes of datas.
			//
			if (firstline) {
				attributes.push_back(std::string(id));
				attributes.push_back(std::string(Weather));
				attributes.push_back(std::string(Temperature));
				attributes.push_back(std::string(Humidity));
				attributes.push_back(std::string(Wind));
				attributes.push_back(std::string(Date));
				firstline = false;
			}
			else {
				Watermelon data;
				data.id = std::string(id);
				data.Weather = std::string(Weather);
				data.Temperature = std::string(Temperature);
				data.Humidity = std::string(Humidity);
				data.Wind = std::string(Wind);
				data.Date = std::string(Date);
				datas.push_back(data);
			}
		}
	}
}

void match_properties(std::vector<Watermelon> datas,
                      std::vector<std::string> attributes,
                      std::map<std::string, std::vector<std::string>>& map_attr)
{
	int index = 0;
	for (auto attribute : attributes) {
		std::vector<std::string> attrTmp;
		for (auto data : datas) {
			switch (index) {
			case 0:
				if (!attrTmp.empty() &&
				        std::find(attrTmp.begin(), attrTmp.end(), data.id) == attrTmp.end()) {
					attrTmp.push_back(data.id);
				}
				else if (attrTmp.empty()) {
					attrTmp.push_back(data.id);
				}
				break;
			case 1:
				if (!attrTmp.empty() &&
				        std::find(attrTmp.begin(), attrTmp.end(), data.Weather) == attrTmp.end()) {
					attrTmp.push_back(data.Weather);
				}
				else if (attrTmp.empty()) {
					attrTmp.push_back(data.Weather);
				}
				break;
			case 2:
				if (!attrTmp.empty() &&
				        std::find(attrTmp.begin(), attrTmp.end(), data.Temperature) == attrTmp.end()) {
					attrTmp.push_back(data.Temperature);
				}
				else if (attrTmp.empty()) {
					attrTmp.push_back(data.Temperature);
				}
				break;
			case 3:
				if (!attrTmp.empty() &&
				        std::find(attrTmp.begin(), attrTmp.end(), data.Humidity) == attrTmp.end()) {
					attrTmp.push_back(data.Humidity);
				}
				else if (attrTmp.empty()) {
					attrTmp.push_back(data.Humidity);
				}
				break;
			case 4:
				if (!attrTmp.empty() &&
				        std::find(attrTmp.begin(), attrTmp.end(), data.Wind) == attrTmp.end()) {
					attrTmp.push_back(data.Wind);
				}
				else if (attrTmp.empty()) {
					attrTmp.push_back(data.Wind);
				}
				break;
			
			case 5:
				if (!attrTmp.empty() &&
				        std::find(attrTmp.begin(), attrTmp.end(), data.Date) == attrTmp.end()) {
					attrTmp.push_back(data.Date);
				}
				else if (attrTmp.empty()) {
					attrTmp.push_back(data.Date);
				}
				break;
			default:
				break;
			}
		}
		index++;
		map_attr[attribute] = attrTmp;
	}
}

bool belongs_same_label(std::vector<Watermelon> datas,
                        std::string label = "yes")
{
	if (datas.empty()) {
		std::cout << "the datas is empty" << std::endl;
		exit(1);
	}

	for (auto iter = datas.begin(); iter != datas.end(); ++iter) {
		if (iter->Date != label) {
			return false;
		}
	}

	return true;
}

std::string majority_of_category(std::vector<Watermelon> datas)
{
	int positivecategory = 0;
	int negativecategory = 0;

	for (auto data : datas) {
		if (data.Date == "yes") {
			++positivecategory;
		}
		else if (data.Date == "no") {
			++negativecategory;
		}
	}
	return (positivecategory > negativecategory ? "yes" : "no");
}

double calculate_information_entropy(std::vector<Watermelon> datas,
                                     std::string mapAttr = "",
                                     std::string attribute = "Date")
{
	// Ent(D) = -∑(k=1, |Y|) p_k * log2(p_k)
	//
	int size = 0;
	double entropy = 0;
	int positive = 0;
	int negative = 0;

	// Beacuse of the datas only have two label.
	// So entropy = positiveSample + negativeSample
	//
	if (attribute == "Weather") {
		for (auto data : datas) {
			if (data.Weather == mapAttr) {
				if (data.Date == "yes") {
					++positive;
				}
				else {
					++negative;
				}
				++size;
			}
		}
	}
	else if (attribute == "Temperature") {
		for (auto data : datas) {
			if (data.Temperature == mapAttr) {
				if (data.Date == "yes") {
					++positive;
				}
				else {
					++negative;
				}
				++size;
			}
		}
	}
	else if (attribute == "Humidity") {
		for (auto data : datas) {
			if (data.Humidity == mapAttr) {
				if (data.Date == "yes") {
					++positive;
				}
				else {
					++negative;
				}
				++size;
			}
		}
	}
	else if (attribute == "Wind") {
		for (auto data : datas) {
			if (data.Wind == mapAttr) {
				if (data.Date == "yes") {
					++positive;
				}
				else {
					++negative;
				}
				++size;
			}
		}
	}
	
	else if (attribute == "Date") {
		size = datas.size();
		auto judget = [&](Watermelon wm) { if (wm.Date == "yes") { ++positive; } else { ++negative; }};
		for_each(datas.begin(), datas.end(), judget);
	}

	if (positive == 0 || negative == 0) {
		return 0;
	}
	else {
		entropy = -(((double)positive / size) * log2((double)positive / size) + ((double)negative / size) * log2((double)negative / size));
	}

	return entropy;
}

double proportion(std::vector<Watermelon>& datas,
                  std::string mapAttr = "",
                  std::string attribute = "")
{
	int size = datas.size();
	double proportion = 0;

	if (attribute == "Weather") {
		for (auto data : datas) {
			if (data.Weather == mapAttr) {
				++proportion;
			}
		}
	}
	else if (attribute == "Temperature") {
		for (auto data : datas) {
			if (data.Temperature == mapAttr) {
				++proportion;
			}
		}
	}
	else if (attribute == "Humidity") {
		for (auto data : datas) {
			if (data.Humidity == mapAttr) {
				++proportion;
			}
		}
	}
	else if (attribute == "Wind") {
		for (auto data : datas) {
			if (data.Wind == mapAttr) {
				++proportion;
			}
		}
	}
	

	proportion /= size;

	return proportion;
}

double calculate_information_gain(std::vector<Watermelon>& datas,
                                  std::string attribute,
                                  std::map<std::string, std::vector<std::string>> map_attr)
{
	// Gain(D) = Ent(D) - ∑(v=1, V) |D^|/|D| * Ent(D)
	//
	double gain = calculate_information_entropy(datas);
	std::vector<std::string> attrs = map_attr[attribute];

	for (auto attr : attrs) {
		gain -= proportion(datas, attr, attribute) * calculate_information_entropy(datas, attr, attribute);
	}

	return gain;
}

std::pair<std::string, std::vector<std::string>> optimal_attribute(std::vector<Watermelon>& datas,
        std::vector<std::string>& attributes,
        std::map<std::string, std::vector<std::string>> map_attr)
{
	std::map<std::string, double> map_gains;

	for (auto attribute : attributes) {
		map_gains[attribute] = calculate_information_gain(datas, attribute, map_attr);
	}

	// Sort the information gain and select the attribute of the maximum
	// information gain.The biggest value is in the first.
	//
	std::vector<std::pair<std::string, double>> vec_map_gains(map_gains.begin(), map_gains.end());
	auto compare_x_y = [](const std::pair<std::string, double> x, const std::pair<std::string, double> y) {
		return x.second > y.second;
	};

	std::sort(vec_map_gains.begin(), vec_map_gains.end(), compare_x_y);

	auto search = map_attr.find(vec_map_gains[0].first);
	if (search != map_attr.end()) {
		return std::make_pair(search->first, search->second);
	}
	else {
		return std::make_pair(std::string(""), std::vector<std::string>());
	}
}

std::vector<Watermelon> remain_watermelon_datas(std::vector<Watermelon> datas,
        std::string mapAttr,
        std::string attribute)
{
	std::vector<Watermelon> tmp;

	if (attribute == "Weather") {
		for (auto data : datas) {
			if (data.Weather == mapAttr) {
				tmp.push_back(data);
			}
		}
	}
	else if (attribute == "Temperature") {
		for (auto data : datas) {
			if (data.Temperature == mapAttr) {
				tmp.push_back(data);
			}
		}
	}
	else if (attribute == "Humidity") {
		for (auto data : datas) {
			if (data.Humidity == mapAttr) {
				tmp.push_back(data);
			}
		}
	}
	else if (attribute == "Wind") {
		for (auto data : datas) {
			if (data.Wind == mapAttr) {
				tmp.push_back(data);
			}
		}
	}
	
	return tmp;
}

std::string print_tree(TreeRoot pTree, int depth)
{
	for (int i = 0; i < depth; ++i) {
		std::cout << '\t';
	}

	if (!pTree->edgeValue.empty()) {
		std::cout << "--" << pTree->edgeValue << "--" << std::endl;
		for (int i = 0 ; i < depth; ++i) {
			std::cout << '\t';
		}
	}

	std::string indent(depth * 2, ' ');
    std::string result = indent +pTree->attribute  + ": " + "\n";
	for (auto child : pTree->childs) {
		result += print_tree(child, depth + 1);
	}
	return result;
}

#endif // REQUIRE_H__
