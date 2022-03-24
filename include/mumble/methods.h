
#include <iostream>
#include <vector>

std::vector<std::string> readPos(std::string path) {
    FILE* fp = fopen(path.c_str(), "r");
    if (fp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    char* line = NULL;
    std::vector<std::string> text;
    int count = 0;
    size_t len = 0;

    while ((getline(&line, &len, fp)) != -1) {
        // using printf() in all tests for consistency
        text.push_back(line);
        //printf("%i%s", count, text[count].c_str());
        //std::cout << text[count];
        //count++;
    }
    fclose(fp);
    if (line) {
        free(line);
    }

    return text;
}

std::vector<std::string> readPos(const char* path) {
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    char* line = NULL;
    std::vector<std::string> text;
    int count = 0;
    size_t len = 0;

    while ((getline(&line, &len, fp)) != -1) {
        // using printf() in all tests for consistency
        text.push_back(line);
        //printf("%i%s", count, text[count].c_str());
        //std::cout << text[count];
        //count++;
    }
    fclose(fp);
    if (line) {
        free(line);
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
    int dataSize = (strList[index].length() - 1) - dataIndex;
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
    int dataSize = (strList[index].length() - 1) - dataIndex;
    temp = strList[index].substr(dataIndex, dataSize);
    
    return stof(temp);
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
    int dataSize = (strList[index].length() - 1) - dataIndex;
    temp = strList[index].substr(dataIndex, dataSize);
    
    return stoi(temp);
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
    int dataSize = (strList[index].length() - 1) - dataIndex;
    temp = strList[index].substr(dataIndex, dataSize);
    
    return temp;
}