
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

std::vector<std::string> readPos(std::string path) {

    std::ifstream file(path);

    std::vector<std::string> text;
    std::string buf;
    
    while (std::getline(file, buf)) {
        text.push_back(buf);
    }
    
    return text;
}

std::vector<std::string> readPos(const char* path) {

    std::ifstream file(path);

    std::vector<std::string> text;
    std::string buf;

    while (std::getline(file, buf)) {
        text.push_back(buf);
    }

    return text;
}

bool assignValue_b(std::vector<std::string> strList, std::string label) {

    std::string temp;
    int index = -1;

    for (int i = 0; i < strList.size(); i++) {
        if (strList[i].find(label) != -1) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        std::cout << "Label not found" << std::endl;
        return false;
    }

    int dataIndex = strList[index].find("=") + 2;
    int dataSize = (strList[index].length()) - dataIndex;
    temp = strList[index].substr(dataIndex, dataSize);
    
    return (temp == "True");
}

float assignValue_f(std::vector<std::string> strList, std::string label) {

    std::string temp;
    int index = -1;

    for (int i = 0; i < strList.size(); i++) {
        if (strList[i].find(label) != -1) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        std::cout << "Label not found" << std::endl;
        return -1;
    }

    int dataIndex = strList[index].find("=") + 2;
    int dataSize = (strList[index].length()) - dataIndex;
    temp = strList[index].substr(dataIndex, dataSize);
    
    return std::stof(temp);
}

int assignValue_i(std::vector<std::string> strList, std::string label) {

    std::string temp;
    int index = -1;

    for (int i = 0; i < strList.size(); i++) {
        if (strList[i].find(label) != -1) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        std::cout << "Label not found" << std::endl;
        return -1;
    }

    int dataIndex = strList[index].find("=") + 2;
    int dataSize = (strList[index].length()) - dataIndex;
    temp = strList[index].substr(dataIndex, dataSize);
    
    return std::stoi(temp);
}

std::string assignValue_s(std::vector<std::string> strList, std::string label) {

    std::string temp;
    int index = -1;

    for (int i = 0; i < strList.size(); i++) {
        if (strList[i].find(label) != -1) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        std::cout << "Label not found" << std::endl;
        return label;
    }

    int dataIndex = strList[index].find("=") + 2;
    int dataSize = (strList[index].length()) - dataIndex;
    temp = strList[index].substr(dataIndex, dataSize);
    
    return temp;
}