#include <ros/ros.h>
#include <std_msgs/Header.h>
#include <topic_tools/shape_shifter.h>

#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <iostream>
#include <cmath>

std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::string item;
    for (char ch : s)
    {
        if (ch == delim)
        {
            if (!item.empty())
                elems.push_back(item);
            item.clear();
        }
        else
        {
            item += ch;
        }
    }
    if (!item.empty())
        elems.push_back(item);
    return elems;
}

std::string ros_basic_types[] = {
    "time",
    // "duration",
    "string",
    // "bool",
    // "int8",
    // "int16",
    // "int32",
    // "int64",
    "uint8",
    "uint16",
    "uint32",
    "uint64",
    // "float32",
    // "float64",
};

class msgtype
{
private:
public:
    std::string name;
    std::string type;

    msgtype(std::string name_, std::string type_)
    {
        name = name_;
        type = type_;
    };
    ~msgtype() {};
};

class GenericSubscriber
{
private:
    ros::NodeHandle nh_;
    ros::Subscriber sub_;
    std::function<void(const std::vector<uint8_t> &)> callback_;
    std::vector<msgtype> datalist_;
    std::map<std::string, std::vector<msgtype>> typedefs_;
    bool is_initialized = false;

public:
    GenericSubscriber(const std::string &sub_topic, const std::function<void(const std::vector<uint8_t> &)> &callback);
    ~GenericSubscriber();

    void topicCallback(const topic_tools::ShapeShifter::ConstPtr &topic_msg);
    void initialize(const std::string &definition);
    void makeMsgDataList(msgtype data, const std::string prefix);
    bool isBasicType(const std::string &type);

    std_msgs::Header parseHeader(const std::vector<uint8_t> &data, const uint8_t start);

    ros::Time parseTime(const std::vector<uint8_t> &data, const uint8_t start);
    std::string parseString(const std::vector<uint8_t> &data, const uint8_t start);
    uint8_t parseUInt8(const std::vector<uint8_t> &data, const uint8_t start);
    uint16_t parseUInt16(const std::vector<uint8_t> &data, const uint8_t start);
    uint32_t parseUInt32(const std::vector<uint8_t> &data, const uint8_t start);
    uint64_t parseUInt64(const std::vector<uint8_t> &data, const uint8_t start);
};
