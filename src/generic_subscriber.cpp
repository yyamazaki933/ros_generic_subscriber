#include "generic_subscriber/generic_subscriber.hpp"

GenericSubscriber::GenericSubscriber(const std::string &sub_topic, const std::function<void(const std::vector<uint8_t> &)> &callback)
{
    sub_ = nh_.subscribe<const topic_tools::ShapeShifter::ConstPtr &>(sub_topic, 10, &GenericSubscriber::topicCallback, this);
    callback_ = callback;
    return;
}

GenericSubscriber::~GenericSubscriber()
{
}

void GenericSubscriber::topicCallback(const topic_tools::ShapeShifter::ConstPtr &topic_msg)
{
    const std::string &md5sum = topic_msg->getMD5Sum();
    const std::string &datatype = topic_msg->getDataType();
    const std::string &definition = topic_msg->getMessageDefinition();
    const uint32_t topic_msg_size = topic_msg->size();

    std::vector<uint8_t> data;
    data.resize(topic_msg_size);
    ros::serialization::OStream stream(data.data(), topic_msg_size);
    topic_msg->write(stream);

    if (!is_initialized)
    {
        initialize(definition);
        is_initialized = true;
    }

    callback_(data);
    return;
}

void GenericSubscriber::initialize(const std::string &definition)
{
    ROS_INFO_STREAM("Initializing Subscriber");

    std::cout << "---" << std::endl;
    std::cout << definition << std::endl;

    std::cout << "---" << std::endl;

    std::vector<msgtype> rootdatas;

    auto blocks = split(definition, '=');
    for (size_t i = 0; i < blocks.size(); i++)
    {
        auto lines = split(blocks[i], '\n');

        std::string custom_typename;
        std::vector<msgtype> custom_typedatas;
        for (auto &&line : lines)
        {
            auto s = split(line, ' ');
            if (s.size() != 2)
                continue;

            if (s[0] == "MSG:")
            {
                custom_typename = split(s[1], '/')[1];
                continue;
            }

            msgtype type(s[1], s[0]);

            if (i == 0)
                rootdatas.push_back(type);
            else
                custom_typedatas.push_back(type);
        }

        if (custom_typename != "")
            typedefs_[custom_typename] = custom_typedatas;
    }

    for (auto &&data : rootdatas)
        makeMsgDataList(data, "");
    return;
}

void GenericSubscriber::makeMsgDataList(msgtype data, const std::string prefix)
{
    if (prefix != "")
        data.name = prefix + '.' + data.name;

    if (isBasicType(data.type))
    {
        datalist_.push_back(data);
        ROS_INFO_STREAM("datalist: " << data.name << ':' << data.type);
        return;
    }

    try
    {
        auto childdatas = typedefs_.at(data.type);
        for (auto &&childdata : childdatas)
            makeMsgDataList(childdata, data.name);
    }
    catch (std::out_of_range &)
    {
        ROS_INFO_STREAM("datalist: " << data.name << ':' << data.type << " (unsupported)");
    }
    return;
}

bool GenericSubscriber::isBasicType(const std::string &type)
{
    for (auto &&basic_type : ros_basic_types)
    {
        if (type == basic_type)
            return true;
    }
    return false;
}

std_msgs::Header GenericSubscriber::parseHeader(const std::vector<uint8_t> &data, const uint8_t start)
{
    std_msgs::Header header;
    header.seq = parseUInt32(data, start);
    header.stamp = parseTime(data, start + 4);
    header.frame_id = parseString(data, start + 12);
    return header;
}

ros::Time GenericSubscriber::parseTime(const std::vector<uint8_t> &data, const uint8_t start)
{
    ros::Time time;
    time.sec = parseUInt32(data, start);
    time.nsec = parseUInt32(data, start + 4);
    return time;
}

std::string GenericSubscriber::parseString(const std::vector<uint8_t> &data, const uint8_t start)
{
    uint32_t len = parseUInt32(data, start);
    char c_arr[len];
    for (size_t i = 0; i < len; i++)
    {
        char c = data[start + 4 + i];
        c_arr[i] = c;
    }
    return std::string(c_arr, sizeof(c_arr) / sizeof(c_arr[0]));
}

uint8_t GenericSubscriber::parseUInt8(const std::vector<uint8_t> &data, const uint8_t start)
{
    return data[start];
}

uint16_t GenericSubscriber::parseUInt16(const std::vector<uint8_t> &data, const uint8_t start)
{
    uint16_t val = data[start] +
                   data[start + 1] * std::pow(2, 8);
    return val;
}

uint32_t GenericSubscriber::parseUInt32(const std::vector<uint8_t> &data, const uint8_t start)
{
    uint32_t val = data[start] +
                   data[start + 1] * std::pow(2, 8) +
                   data[start + 2] * std::pow(2, 16) +
                   data[start + 3] * std::pow(2, 24);
    return val;
}

uint64_t GenericSubscriber::parseUInt64(const std::vector<uint8_t> &data, const uint8_t start)
{
    uint64_t val = data[start] +
                   data[start + 1] * std::pow(2, 8) +
                   data[start + 2] * std::pow(2, 16) +
                   data[start + 3] * std::pow(2, 24) +
                   data[start + 4] * std::pow(2, 32) +
                   data[start + 5] * std::pow(2, 40) +
                   data[start + 6] * std::pow(2, 48) +
                   data[start + 7] * std::pow(2, 56);
    return val;
}
