#include "ns3/aodv-module.h"
#include "ns3/internet-module.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/energy-module.h" //may not be needed here...
#include "ns3/aqua-sim-ng-module.h"
#include "ns3/applications-module.h"
#include "ns3/log.h"
#include "ns3/callback.h"

// 不懂为啥有两个头文件不让引用
// #include "ns3/aodv-helper.h"

// #include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/wifi-module.h"
#include "ns3/v4ping-helper.h"
#include <iostream>
#include <cmath>
#include "mix_lawyer_core_11_14.h"
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

// #include "ns3/netanim-module.h"

#include <unordered_set> 

// 如果允许二维绘图
#include "json.hpp"


#ifdef HAS_NETSIMULYZER
#include "ns3/netsimulyzer-module.h"
#endif

#define AQUA_SEND_ENABLE 1
#define AQUA_RECV_ENABLE 2
#define AODV_SEND_ENABLE 4
#define AODV_RECV_ENABLE 8

// Bound 用于计算优先级
#define Bound 20000000
#define xxxxx 1834248161
#define xxxxy 354706161
#define xxxxz 137333332
#define xxxxa 80039497

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MXR_SCF");
std::ofstream final_sim_file;

std::ofstream port_message("./port_message.txt");   

// 某人是真的喜欢用全局变量啊？
// 这代码的丑陋程度简直已经丧心病狂了！
std::map<uint32_t, Ptr<netsimulyzer::ThroughputSink>> macTxTraceSeries;
std::map<uint32_t, Ptr<netsimulyzer::ThroughputSink>> flowRxTraceSeries;
std::map<uint32_t, Ptr<netsimulyzer::SeriesCollection>> macCollections;

// std::map<int, int> time_packet_pair;
std::vector<std::map<int, int>> http_client;
std::vector<std::map<int, int>> http_server;
std::vector<std::map<int, int>> voip_client;
std::vector<std::map<int, int>> voip_server;
std::vector<std::map<int, int>> csmxp_client;
std::vector<std::map<int, int>> csmxp_server;

int packet_processed[100000];
int statics_total_bit = 0;

nlohmann::json aqua_message;
nlohmann::json aodv_message;

std::vector<nlohmann::json> aqua_message_vector;

// std::vector<nlohmann::json> aodv_message_vector;
nlohmann::json aodv_message_vector; // 虽然叫vector 但是却是个json

nlohmann::json node_2d_move_vector; // 虽然叫vector 但是却是个json

nlohmann::json packet_type_json;


int USV_AUV_FATHER_TO_CHILD[1000][2];
int AUV_AUV_DIRECT[1000][2];
int UAV_USV_CHILD_TO_FATHER[1000][2];

std::vector<Ptr<MixApp>> usv_app_lawyer;
std::vector<Ptr<MixApp>> uav_app_lawyer;
std::vector<Ptr<MixApp>> auv_app_lawyer;
std::vector<Ptr<MixApp>> all_app_lawyer;

Ipv4Address CreateIpv4Address(int j) {
    std::string address = "10.0.0." + std::to_string(j);
    return Ipv4Address(address.c_str());
}

// 注意， 在此处，将所有的x,y实际位置进行了缩放， 减小了十倍
uint32_t
ContextToNodeId(std::string context)
{
    std::string sub = context.substr(10);
    uint32_t pos = sub.find("/Device");
    return atoi(sub.substr(0, pos).c_str());
}


void
// Define callback function to track node mobility
CourseChanged(std::string context, Ptr<const MobilityModel> model)
{
    const auto position = model->GetPosition();
    // Write coordinates to log
    // std::cout << Simulator::Now().GetSeconds() << " Course Change Position: [" << position.x << ", "
    //           << position.y << ", " << position.z << "]\n";
    
    // Add data point to XYSeries
    int node_id = (int)ContextToNodeId(context);
    // 跟上边井水不犯河水
    node_2d_move_vector[std::to_string(node_id)][std::to_string(Simulator::Now().GetNanoSeconds())]["x"] = position.x;
    node_2d_move_vector[std::to_string(node_id)][std::to_string(Simulator::Now().GetNanoSeconds())]["y"] = position.y;
    node_2d_move_vector[std::to_string(node_id)][std::to_string(Simulator::Now().GetNanoSeconds())]["z"] = position.z;

}


void record_node_pos( NodeContainer nodeAll, int node_id) {
    const auto position = nodeAll.Get(node_id)->GetObject<MobilityModel>()->GetPosition();
    node_2d_move_vector[std::to_string(node_id)][std::to_string(Simulator::Now().GetNanoSeconds())]["x"] = position.x;
    node_2d_move_vector[std::to_string(node_id)][std::to_string(Simulator::Now().GetNanoSeconds())]["y"] = position.y;
    node_2d_move_vector[std::to_string(node_id)][std::to_string(Simulator::Now().GetNanoSeconds())]["z"] = position.z;
    // std::cout << Simulator::Now().GetSeconds() << " Course Change Position: [" << position.x << ", "
    //           << position.y << ", " << position.z << "]\n";
}

void plan_node_vel( NodeContainer nodeC, int node_id, Vector3D vel) {
    // 这个函数决定了只有这种的模式才能改
    // ->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(vel);
}



void
Aqua2DplotCallback(int from_id, int to_id, int size)
{
    nlohmann::json cur_message_data_json;
  
    std::cout<< "time = "    << Simulator::Now().GetSeconds()
             << "from = "    << from_id
             << "to = "      << to_id
             << "size = "    << size
             << std::endl;
    cur_message_data_json["time"]            = Simulator::Now().GetSeconds();
    cur_message_data_json["message_from"]    = from_id;
    cur_message_data_json["message_receive"] = to_id;
    cur_message_data_json["message_size"]    = size;
    aqua_message_vector.push_back(cur_message_data_json);
}

void
Aodv2DplotCallback(std::string context, Ptr<const Packet> p)
{
    nlohmann::json cur_message_data_json;
  
    // std::cout<< "@@@@@@@@@@@@@@@@@@@@@@@@@@@@Aodv2DplotCallback:"
            //  << "size = "    << p->GetSize()
            //  << std::endl;
}

void
Aodv2DplotCallback2(std::string context, Ptr<const Packet> p)
{
    nlohmann::json cur_message_data_json;
  
    // std::cout<< "############################Aodv2DplotCallback:"
    //          << "size = "    << p->GetSize()
    //          << std::endl;
}

// 不知道能不能用上
void Unicast2(Ptr<Ipv4Route> route, Ptr<const Packet> packet, const Ipv4Header& header)
{

}


// 接收 保持原先
void Ipv4TxTrace(std::string context,
                                Ptr<const Packet> p,
                                Ptr<Ipv4> ipv4,
                                uint32_t interfaceIndex)
{
    Ipv4Header recvHeader;
    p->PeekHeader(recvHeader);
    int packet_id = p->GetUid();
    // 在内部处理报文并获取到报文种类， 只处理一次。
    // if (packet_processed[packet_id] != 1) {
    if (0) {
        packet_processed[packet_id] = 1;
        // 输出一个报文种类
        // std::cout << "begin to diff stream! " << std::endl;
        int protocol_id = (int)recvHeader.GetProtocol();
        // std::cout << "protocol-id = " <<protocol_id << std::endl;
        if (protocol_id == 17 ) {
            UdpHeader udp_header_recv;
            Ptr<Packet> tempPacket = p->Copy();
            tempPacket->RemoveHeader(recvHeader);
            tempPacket->PeekHeader(udp_header_recv);
            
// std::ofstream port_message("./port_message.txt");   
            // 
            if (udp_header_recv.GetSourcePort() != 654 )
            std::cout << "time = " << Simulator::Now()
                        << ", port = " << udp_header_recv.GetSourcePort()
                      << ", packet_size = " << p->GetSize() 
                      << "dst port = " << udp_header_recv.GetDestinationPort()
                      << std::endl;

        }
    }

    if (p->GetSize() > 10000)
    std::cout << "TTTTTTx trace:  " 
              << "cur_time = " << Simulator::Now()
              << "s, cur_node_ID = " << ContextToNodeId(context)
              << ", packet size = " << p->GetSize() 
              << ", packet uid = " << p->GetUid()
              << ", send ip = " << recvHeader.GetSource()
              << "recv ip = " << recvHeader.GetDestination()
              << ", protol-id = " << recvHeader.GetProtocol()
              <<  " ." << std::endl;
    // else return;
    
    // 单位都是ns
    if (p->GetSize() > 100)
    aodv_message_vector[std::to_string(p->GetUid())][std::to_string(Simulator::Now().GetNanoSeconds())+"TX"] = ContextToNodeId(context);

}

// 为了防止同一时间， 发送方 + 1
void Ipv4RxTrace(std::string context,
                                Ptr<const Packet> p,
                                Ptr<Ipv4> ipv4,
                                uint32_t interfaceIndex)
{
    Ipv4Header recvHeader;
    p->PeekHeader(recvHeader);

    int xx = (int)recvHeader.GetProtocol();
    // 使用copydata可以简单的处理
    // uint32_t
    // Packet::CopyData(uint8_t* buffer, uint32_t size) const
    // {
    //     return m_buffer.CopyData(buffer, size);
    // }

// 复制数据可以很容易的进行。
    if (p->GetSize() > 10000)
    std::cout << "RRRRRRx trace:  " 
              << "cur_time = " << Simulator::Now()
              << "s, cur_node_ID = " << ContextToNodeId(context)
              << ", packet size = " << p->GetSize() 
              << ", packet uid = " << p->GetUid()
              << ", send ip = " << recvHeader.GetSource()
              << "recv ip = " << recvHeader.GetDestination()
              << ", protol-id = " << xx
              <<  " ." << std::endl;
    // else return;

    if (p->GetSize() > 100)
    aodv_message_vector[std::to_string(p->GetUid())][std::to_string(1+Simulator::Now().GetNanoSeconds())+"RX"] = ContextToNodeId(context);
}


int test()
{
    // std::cout << "2022/10/31: begin!" << std::endl;
    // std::cout << "2023/9/7: begin!" << std::endl;
    std::cout << "2023/10/10: begin!" << std::endl;
    return 0;
}

// 真逆天哇
NetDeviceContainer devices; // aqua device
NetDeviceContainer wifi_devices;
Ipv4InterfaceContainer interfaces;

MixLawyerTable::MixLawyerTable()
{
}

MixLawyerTable::~MixLawyerTable()
{
}

// 
int MixLawyerTable::queryAqNext(int dst)
{
    int res = -1;
    int res_prioty = 996;
    for (auto iter1 = aq_table.begin(); iter1 != aq_table.end(); ++iter1)
    {
        if (iter1->first != dst)
            continue;
        for (auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); ++iter2)
        {
            if (iter2->second < res_prioty)
            {
                res = iter2->first;
                res_prioty = iter2->second;
            }
        }
    }
    return res;
}

int MixLawyerTable::queryUsvNext(int dst)
{
    int res = -1;
    for (auto iter1 = usv_children.begin(); iter1 != usv_children.end(); ++iter1)
    {
        if (std::find(iter1->second.begin(), iter1->second.end(), dst) != iter1->second.end())
        {
            res = iter1->first;
            break;
        }
    }

    return res;
}

void MixLawyerTable::printAqtable()
{
    NS_LOG_INFO("DST  | NXT  |  PRIOTY");
    for (auto iter1 = aq_table.begin(); iter1 != aq_table.end(); ++iter1)
    {
        for (auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); ++iter2)
        {
            std::cout << iter1->first << "   \t" << iter2->first << "   \t" << iter2->second << std::endl;
        }
    }
}

void MixLawyerTable::printChildtable()
{
    NS_LOG_INFO("father  | children");
    for (auto iter1 = usv_children.begin(); iter1 != usv_children.end(); ++iter1)
    {
        std::cout << iter1->first << " \t";
        for (auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); ++iter2)
        {
            std::cout << (*iter2) << ", ";
        }
        std::cout << "\n";
    }
}

void MixLawyerTable::addEntry(int father, int child)
{
    // std::cout<<"addEntry: "<<father<<","<<child<<std::endl;
    auto iter = usv_children.find(father);
    if (iter == usv_children.end())
    {
        std::vector<int> child_vc;
        child_vc.push_back(child);
        usv_children.insert({father, child_vc});
    }
    else
    {
        auto iter1 = iter->second;
        // 下面语句判断是否存在，不存在的话就添加
        // std::binary_search(v.begin(), v.end(), key)
        if (std::find(iter1.begin(), iter1.end(), child) == iter1.end())
        {
            iter->second.push_back(child);
        }
    }
}

void MixLawyerTable::addEntry(int dst, int nxt, int prioty)
{
    auto iter = aq_table.find(dst);
    if (iter == aq_table.end())
    {
        std::map<int, int> list1;
        // std::cout << "add once" << std::endl;
        list1.insert({nxt, prioty});
        aq_table.insert({dst, list1});
    }
    else
    {
        // NS_LOG_INFO("Route exists");
        auto iter1 = iter->second.find(nxt);
        if (iter1 == iter->second.end())
        {
            iter->second.insert({nxt, prioty});
        }
        else
        {
            iter1->second = prioty;
        }
    }
}




MixRoute::MixRoute(/* args */)
{
}

MixRoute::~MixRoute(/* args */)
{
}


/**
 * mytype = 1: 水声节点下一跳优先级表
 * mytype = 2: 无人船父子节点关系表
*/
int MixRoute::getRoute(int mytype, int target)
{
    // 水声节点的查询
    if (mytype == 1)
    {
        // std::cout<< "getRoute result: Next hop = " << mix_table.queryAqNext(target) <<std::endl;
        return mix_table.queryAqNext(target);
    }
    // 无人船节点的查询
    if (mytype == 2)
    {
        return mix_table.queryUsvNext(target);
    }
    return -1;
}

// 离谱， 这里写的是什么东西
int MixRoute::getBestRoute()
{
    return -1;
}

MixApp::MixApp()
    : m_socket(0),
      m_peer(),
      m_packetSize(0),
      m_nPackets(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_packetsSent(0),
      m_type(0)
{
    if (m_type == 0)
    {
    }
}

MixApp::~MixApp()
{
    m_socket = 0;
}

// void MixApp::makeRouteCallback(Ptr<AquaSimStaticRouting> routingS)
// {
//     routingS->SetRecvCallback(MakeCallback(&MixApp::HandleRead2, this));
// }

// 这里是将水声部分的报文 强行读取出来的方法， 不知道为什么水声设备的 sink-socket 设置的回调无法正常工作
void MixApp::SetMyStaticRouting(Ptr<AquaSimStaticRouting> routingS)
{
    m_routing = routingS;
    routingS->SetRecvCallback(MakeCallback(&MixApp::HandleRead2, this));
}


// 本来， 创建若干个socket是这里的任务， 但是莫名其妙的会有bug， 
// 大概率感觉是因为工厂模式造成的实例化不同步。
void MixApp::Setup(int featur_flag, int app_type, int node_type, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
    // std::cout << "carry setup: "
    //           << "app type:" << app_type << std::endl;

    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;

    aqua_s = (AQUA_SEND_ENABLE & featur_flag) > 0;
    aqua_r = (AQUA_RECV_ENABLE & featur_flag) > 0;
    aodv_s = (AODV_SEND_ENABLE & featur_flag) > 0;
    aodv_r = (AODV_RECV_ENABLE & featur_flag) > 0;
    my_node_type = node_type;


    if (node_type == 0) {
        is_usv = true;
    }
    else if (node_type == 1)
    {
        is_uav = true;
    } 
    else if (node_type == 2) {
        is_auv = true; 
    }

    // NS_LOG_INFO("APP Setup: "
    //             << "My featur falg is "
    //             << featur_flag
    //             << "aqua_s is"
    //             << aqua_s);

    if (app_type == aqua_sender)
    {
        m_type = aqua_sender;
        m_tid = TypeId::LookupByName("ns3::PacketSocketFactory");
    }
    if (app_type == aqua_receiver)
    {
        m_type = aqua_receiver;
        m_tid = TypeId::LookupByName("ns3::PacketSocketFactory");
    }
    if (app_type == aodv_sender)
    {
        m_type = aodv_sender;
        m_tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    }
    if (app_type == aodv_receiver)
    {
        m_type = aodv_receiver;
        m_tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    }
}

/**
 * @brief 对 MIX 层进行初始化，设定三种节点种类额相关参数|为后续节点信息处理提供基础
 * 需要对重要的数据结构进行初始话
 * 例如 路由表。
 * 报文头以及 socket种类。0
 */
void MixApp::StartApplication(void)
{
    callback_test = 996;
    m_running = 1;

    auto mobility = GetNode()->GetObject<MobilityModel>();
    const auto position = mobility->GetPosition();
    std::cout << "----" << std::endl;
    // 这一步中的 GetNode 尚需 后续处理
    std::cout << "node-" << GetNode()->GetId() 
    << ", type = " << m_type 
    << ", my_node_type = " << my_node_type
    <<", m_device_aq_id = " << m_device_aq_id
    <<", m_device_wifi_id = " << m_device_wifi_id
    << ", pos.x = " << position.x
    << ", pos.y = " << position.y
    << ", pos.z = " << position.z
    << std::endl;
    // 要在这里写， 能给1，2，3轮流发送。

    // TODO: 添加 新的socket
    if (aqua_s)
    {
        // std::cout<<"\nset AQUA ONCE\n";
        m_tid = TypeId::LookupByName("ns3::PacketSocketFactory");
        for (uint32_t i = 0; i < devices.GetN(); i++)
        {
            // std::cout << AquaSimAddress::ConvertFrom(devices.Get(i)->GetAddress()).GetAsInt() << std::endl;
            if (is_usv && m_device_aq_id != -1)
            {
                socketAddr.SetSingleDevice(2); // 作用？  0和1 是已经有了的东西。  wo
            }
            else
            {
                socketAddr.SetSingleDevice(0);
            }
            // socketAddr.SetAllDevices();
            socketAddr.SetPhysicalAddress(devices.Get(i)->GetAddress());
            socketAddr.SetProtocol(1);
            Ptr<Socket> new_socket = Socket::CreateSocket(GetNode(), m_tid);
            new_socket->Bind(socketAddr);
            new_socket->Connect(socketAddr);
            new_socket->SetAllowBroadcast(true);
            socket_vc.push_back(new_socket);
        }
        // std::cout<<GetNode()->GetDevice(2)-><<std::endl;
    }

    if (aqua_r)
    {
        // m_localAddress.SetSingleDevice(0); // 作用？
        // m_localAddress.SetPhysicalAddress(devices.Get(2)->GetAddress());
        // m_localAddress.SetProtocol(1); // different

        // m_socket = Socket::CreateSocket(GetNode(), m_tid);
        // m_socket->Bind(m_localAddress);
        // m_socket->Listen();

        // 这个回调函数很重要
        // std::cout << "set the callback for recv" << std::endl;
        // m_socket->SetRecvCallback(MakeCallback(&MixApp::HandleRead1, this));
    }

    if (aodv_s)
    {
        // std::cout<<"\nset AODV ONCE\n";
        m_tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        for (uint32_t i = 0; i < interfaces.GetN(); i++)
        {
            Address serverAddress;
            serverAddress = Address(interfaces.GetAddress(i));
            udp_send_socket = Socket::CreateSocket(GetNode(), m_tid);

            udp_send_socket->Connect(InetSocketAddress(Ipv4Address::ConvertFrom(serverAddress), port));
            udp_send_socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
            udp_send_socket->SetAllowBroadcast(true);
            udp_socket_vc.push_back(udp_send_socket);

            // HTTP sockets 但是http流是3
            udp_send_socket_2 = Socket::CreateSocket(GetNode(), m_tid);
            udp_send_socket_2->Connect(InetSocketAddress(Ipv4Address::ConvertFrom(serverAddress), port_2));
            udp_send_socket_2->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
            udp_send_socket_2->SetAllowBroadcast(true);
            udp_socket_vc_2.push_back(udp_send_socket_2);

            // CSMXP sockets 但是csxmp流是2
            udp_send_socket_3 = Socket::CreateSocket(GetNode(), m_tid);
            udp_send_socket_3->Connect(InetSocketAddress(Ipv4Address::ConvertFrom(serverAddress), port_3));
            udp_send_socket_3->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
            udp_send_socket_3->SetAllowBroadcast(true);
            udp_socket_vc_3.push_back(udp_send_socket_3);
        }
    }

    if (aodv_r)
    {
        m_tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        udp_listen_socket = Socket::CreateSocket(GetNode(), m_tid);
        InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(),
                                                    port);
                                                    // port);
        udp_listen_socket->Bind(local);
        udp_listen_socket->SetRecvCallback(MakeCallback(&MixApp::HandleRead1, this));

        m_tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        udp_listen_socket_2 = Socket::CreateSocket(GetNode(), m_tid);
        local = InetSocketAddress(Ipv4Address::GetAny(),
                                                    port_2);
        udp_listen_socket_2->Bind(local);
        udp_listen_socket_2->SetRecvCallback(MakeCallback(&MixApp::HandleRead1, this));

        m_tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        udp_listen_socket_3 = Socket::CreateSocket(GetNode(), m_tid);
        local = InetSocketAddress(Ipv4Address::GetAny(),
                                                    port_3);
                                                    // port);
        udp_listen_socket_3->Bind(local);
        udp_listen_socket_3->SetRecvCallback(MakeCallback(&MixApp::HandleRead1, this));
    }
}

/**
 * @brief 关闭socket
 *
 */
void MixApp::StopApplication(void)
{
    m_running = false;

    // if (m_sendEvent.IsRunning ())
    //   {
    //     Simulator::Cancel (m_sendEvent);
    //   }

    // if (m_socket)
    //   {
    //     m_socket->Close ();
    //   }
}

void MixApp::ScheduleTx()
{
}

// id form 1 - n
// 加一个发送次数
// 
void MixApp::PlanTx(double send_time, int dst_type, int target, int packet_sizeM, int stream_type)
{
    // Simulator::Schedule(Seconds(send_time), &MixApp::SendtoDevice, this, dst_type, target);
    Simulator::Schedule(Seconds(send_time), &MixApp::SendtoDevice, this, dst_type, target, packet_sizeM, stream_type);
}

// 这里是设备的一种发送
// void  MixApp::SendtoDevice(int target_type, int deviceId){
void  MixApp::SendtoDevice(int target_type, int deviceId, int packet_sizeM, int stream_type){
    final_sim_file << "在"<< 1.0*Simulator::Now().GetNanoSeconds() / 1000000000 << "s"
                    << "节点-" << GetNode()->GetId() << "向节点-"<<deviceId
                    << "发送数据包，数据包大小为" << packet_sizeM <<"bit"
                    << std::endl;
    NS_LOG_INFO ("\nStart message send---------------------");
    if (target_type == IS_AODV_DIST) {
        NS_LOG_INFO( " MixApp::SendtoDevice:  target_type IS_AODV_DIST "
                    << "target-wifi-device id = "<<deviceId );
        this->SendtoAodv(deviceId, packet_sizeM, stream_type);
    } else if ( target_type == IS_AQUA_DIST ) {
        NS_LOG_INFO( " MixApp::SendtoDevice:  target_type IS_AQUA_DIST" 
                    << "target-AQUA-device id = "<<deviceId );
        // this->SendtoAqua(deviceId);
        this->SendtoAqua(deviceId, packet_sizeM, stream_type);
    }
}

void  MixApp::SendtoAodv(int deviceId, int packet_sizeM, int stream_type) {
    Ptr<Packet> packet = Create<Packet>(packet_sizeM - 28);
    this->SendPacketToAodv(deviceId, false, packet, stream_type);
}

// 只处理无报文头的作为发送者的消息
void  MixApp::SendPacketToAodv(int deviceId, bool withHead, Ptr<Packet> packet, int stream_type) {
// void  MixApp::SendPacketToAodv(int deviceId, bool withHead, Ptr<Packet> packet) {
    if (withHead) {
        MixPacketHeader recvHeader;
        packet->RemoveHeader(recvHeader);
        recvHeader.Set_recv_id(deviceId);
        recvHeader.Set_send_id(m_device_wifi_id);
        recvHeader.Set_ts(); // 讲道理不删

        packet->AddHeader(recvHeader);

        this->SendAodvPacket(deviceId, packet);
        return;
    }
    int type_bit_mess = 0;
    if (stream_type == 1) type_bit_mess = IS_VOIP;
    else if (stream_type == 2) type_bit_mess = IS_HTTP;
    else if (stream_type == 3) type_bit_mess = IS_CXMS;
    else  type_bit_mess = IS_CBR;

    if (is_auv) {
        NS_LOG_INFO( " MixApp::SendPacketToAodv " );
        // send to father. 由 RECV1 进行后续处理
        // recv1 调用 SendPacketToAodv

        MixPacketHeader mix_header;
        mix_header.Set_recv_id(default_father_id );
        mix_header.Set_send_id(m_device_aq_id);
        mix_header.Set_dist_id(deviceId);
        mix_header.Set_souc_id(m_device_aq_id);
        mix_header.Set_type( IS_MESSAGE | IS_AODV_DIST | IS_AUV_SEND | type_bit_mess); // TODO: 修改 标志位使得能够转发
        mix_header.Set_ts();
        packet->AddHeader(mix_header);

        this->SendAquaPacket(default_father_id, packet); // 这里确实有问题。   

        // SendPacket
    } else {
        NS_LOG_INFO( " MixApp::SendPacketToAodv " );
        MixPacketHeader mix_header;
        mix_header.Set_recv_id(deviceId);
        mix_header.Set_send_id(m_device_wifi_id);
        mix_header.Set_dist_id(deviceId);
        mix_header.Set_souc_id(m_device_wifi_id);
        // TODO: 修改 标志位 协助转发
        if (is_usv) {
            mix_header.Set_type( IS_MESSAGE | IS_AODV_DIST | IS_USV_SEND | type_bit_mess);
        } else if (is_uav) {
            mix_header.Set_type( IS_MESSAGE | IS_AODV_DIST | IS_UAV_SEND | type_bit_mess);
        }
        mix_header.Set_ts();
        // mix_header.Print(std::cout);
        packet->AddHeader(mix_header); 
        this->SendAodvPacket(deviceId, packet);               
    }


}

void  MixApp::SendtoAqua(int deviceId, int packet_sizeM, int stream_type) {
    if (is_usv) {
        // 查询路由表， send to usv-father
        Ptr<Packet> packet = Create<Packet>(packet_sizeM - 28);
        this->SendPacketToAqua (deviceId, false, packet, stream_type);
        
    } else if (is_uav){
        // 首先， 发送到默认的父节点
        // recv1 的处理。 检测是否是自己的子节点。
        // 发给
        Ptr<Packet> packet = Create<Packet>(packet_sizeM - 28);
        this->SendPacketToAqua (deviceId, false, packet, stream_type);
    } else if (is_auv) {
        NS_LOG_INFO ("Devive -" << m_device_aq_id
                    <<" send aqua packet to device-"
                    << deviceId);
        Ptr<Packet> packet = Create<Packet>(packet_sizeM - 28);
        this->SendPacketToAqua (deviceId, false, packet, stream_type);
    }
}

void MixApp::SendPacketToAqua(int deviceId, bool withHead, Ptr<Packet> packet, int stream_type){
    if (withHead) {
        MixPacketHeader recvHeader;
        packet->RemoveHeader(recvHeader);
        recvHeader.Set_recv_id(deviceId);
        recvHeader.Set_send_id(m_device_aq_id);
        // 有报文头就不处理
        recvHeader.Set_ts(); // 讲道理不改

        packet->AddHeader(recvHeader);
        this->SendAquaPacket(deviceId, packet);
        return ;
    }
    int type_bit_mess = 0;
    if (stream_type == 1) type_bit_mess = IS_VOIP;
    else if (stream_type == 2) type_bit_mess = IS_CXMS;
    else if (stream_type == 3) type_bit_mess = IS_HTTP;
    else  type_bit_mess = IS_CBR;

    if (is_usv) {
        // 查询路由表， send to usvfather
        //
        MixPacketHeader mix_header;
        mix_header.Set_recv_id(m_device_wifi_id);
        mix_header.Set_send_id(m_device_wifi_id);
        mix_header.Set_dist_id(deviceId);
        mix_header.Set_souc_id(m_device_wifi_id);
        // TODO: 添加标志， 协助转发
        mix_header.Set_type( IS_MESSAGE | IS_AQUA_DIST | IS_USV_SEND | IS_DIRECT | type_bit_mess); 
        mix_header.Set_ts();
        
        NS_LOG_INFO ("USV-" <<m_device_wifi_id
                    << "Send packet to aqua Device-"
                    << deviceId);
        packet->AddHeader(mix_header);
        this->recv1(packet);

    } else if (is_uav){
        MixPacketHeader mix_header;
        mix_header.Set_recv_id(default_father_id);
        mix_header.Set_send_id(m_device_wifi_id);
        mix_header.Set_dist_id(deviceId);
        mix_header.Set_souc_id(m_device_wifi_id);
        // 检查好标志位
        // TODO: 添加标志， 协助转发
        mix_header.Set_type( IS_MESSAGE | IS_AQUA_DIST | IS_UAV_SEND | IS_DIRECT | type_bit_mess);
    
        mix_header.Set_ts();
        // mix_header.Print(std::cout);
        packet->AddHeader(mix_header); 
        this->SendAodvPacket(uav_default_usv_father_id, packet);  
    } else if (is_auv) {
        int dst = mix_route.getRoute(1, deviceId);
        NS_LOG_INFO ("AUV-" << m_device_aq_id
                    << " send packet to aq_device-"
                    << deviceId  
                    << "query route dst = " 
                    << dst);
        if (dst != -1) {
            NS_LOG_INFO ("In routing table, direct-send");
            MixPacketHeader mix_header;
            mix_header.Set_recv_id(dst);
            mix_header.Set_send_id(m_device_aq_id);
            mix_header.Set_dist_id(dst);
            mix_header.Set_souc_id(m_device_aq_id);
            // 检查好标志位
            // TODO: 添加标志， 协助转发
            mix_header.Set_type( IS_MESSAGE | IS_AQUA_DIST | IS_AUV_SEND | IS_DIRECT | type_bit_mess);
        
            mix_header.Set_ts();
            packet->AddHeader (mix_header);

            this->SendAquaPacket (dst, packet);
        }
        else {
            MixPacketHeader mix_header;
            mix_header.Set_recv_id(default_father_id);
            mix_header.Set_send_id(m_device_aq_id);
            mix_header.Set_dist_id(deviceId);
            mix_header.Set_souc_id(m_device_aq_id);
            // 检查好标志位
            // TODO: 添加标志， 协助转发
            mix_header.Set_type( IS_MESSAGE | IS_AQUA_DIST | IS_AUV_SEND | IS_RELAY | type_bit_mess);
            mix_header.Set_ts();
            packet->AddHeader (mix_header);
            this->SendAquaPacket (default_father_id, packet);
        }
    }
}


void MixApp::SendAquaPacket(int device_num, Ptr<Packet> packet)
{
    aodv_message_vector[std::to_string(packet->GetUid())][std::to_string(Simulator::Now().GetNanoSeconds())+"TX"]
        = GetNode()->GetId();
    MixPacketHeader recvHeader;
    packet->PeekHeader(recvHeader);
    int type = recvHeader.Get_type(); // value from packet 解析
    int stream_type = 0;
    if (type & IS_VOIP ) stream_type = 1;
    else if (type & IS_CXMS) stream_type = 3;
    else if (type & IS_HTTP) stream_type = 2;
    else if (type & IS_CBR) stream_type = 0;
    NS_LOG_INFO("MixApp::SendAquaPacket -- Send to device id - " << device_num << "." );
    recvHeader.Print(std::cout);

    packet_type_json[std::to_string(packet->GetUid())]["stream_type"]
        = stream_type;
    packet_type_json[std::to_string(packet->GetUid())]["packet_size"]
        = packet->GetSize();
    socket_vc[device_num - 1]->Send(packet);
    // Aqua2DplotCallback (m_device_aq_id, device_num, packet->GetSize());
    NS_LOG_INFO("MixApp::SendAquaPacket -- socket_vc.size() = " << socket_vc.size() << "." );
}

void
MixApp::SendAodvPacket(int device_num, Ptr<Packet> packet)
{
    
    MixPacketHeader recvHeader;
    packet->PeekHeader(recvHeader);
    int type = recvHeader.Get_type(); // value from packet 解析
    int stream_type = 1;
    if (type & IS_VOIP ) stream_type = 1;
    else if (type & IS_HTTP ) stream_type = 2;
    else if (type & IS_CXMS ) stream_type = 3;
    else if (type & IS_CBR) stream_type = 0;
    packet_type_json[std::to_string(packet->GetUid())]["stream_type"]
        = stream_type;
    packet_type_json[std::to_string(packet->GetUid())]["packet_size"]
        = packet->GetSize();
    recvHeader.Print(std::cout);
    NS_LOG_INFO("MixApp::SendAodvPacket -- Send to device id - "
                << device_num << ", stream_type = " << stream_type << ".");
    switch (stream_type)
    {
    case 1:
        udp_socket_vc[device_num - 1]->Send(packet);
        break;
    case 2:
        udp_socket_vc_2[device_num - 1]->Send(packet);
        break;
    case 3:
        udp_socket_vc_3[device_num - 1]->Send(packet);
        break;
    default:
        udp_socket_vc[device_num - 1]->Send(packet);
        break;
    }
}

// aodv packet process
void MixApp::HandleRead1(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address from;
    while ((packet = socket->RecvFrom(from)))
    {
        if (packet->GetSize() > 0)
        {
            recv1(packet);
            std::map<uint32_t, Ptr<netsimulyzer::ThroughputSink>>::iterator it =
                flowRxTraceSeries.find(GetNode()->GetId());
            it->second->AddPacketSize(packet->GetSize());
            std::cout<< "flowRxTraceSeries-" << GetNode()->GetId() << ", add size " << packet->GetSize() << std::endl;
            final_sim_file << "在"<< 1.0*Simulator::Now().GetNanoSeconds() / 1000000000 << "s"
            << "节点-" << GetNode()->GetId() << "收到射频网络数据包并处理" << std::endl;
        }
    }
}

// aqua node handle message
// only aqua message handle here
void MixApp::HandleRead2(Ptr<Packet> packet)
{
    // NS_LOG_INFO("MixApp::HandleRead2 -- get message");
    // std::cout << "\nPacket message: packet.size=" << p->GetSize() << std::endl;
    std::map<uint32_t, Ptr<netsimulyzer::ThroughputSink>>::iterator it =
        flowRxTraceSeries.find(GetNode()->GetId());
    it->second->AddPacketSize(packet->GetSize());
    std::cout<< "flowRxTraceSeries-" << GetNode()->GetId() << ", add size " << packet->GetSize() << std::endl;
    if (is_usv)
    {
        packet->RemoveAtStart(30);
        recv1(packet);
    }
    else
    {
        packet->RemoveAtStart(30);
        aodv_message_vector[std::to_string(packet->GetUid())][std::to_string(Simulator::Now().GetNanoSeconds())+"RX"] 
        = GetNode()->GetId();
        recv2(packet);
    }
    final_sim_file << "在"<< 1.0*Simulator::Now().GetNanoSeconds() / 1000000000 << "s"
            << "节点-" << GetNode()->GetId() << "收到水声网络数据包并处理" << std::endl;
}

/**
 * @brief USV / UAV 节点信息处理流程
 *
 * @param p
 * 
 * 1. 如果 消息 到达 (from uav/usv/auv)， 则接收并打印
 * 2. 如果 中转 给 auv.
 * 3. 如果 中转 给 usv.
 * 
 */
void MixApp::recv1(Ptr<Packet> packet)
{
    // 来源1： 无人船接收到的aqua水声信息
    // 来源2： aodv网络中的直达信息 / 中继信息
    
    // MixPacketHeader temp;
    // packet->RemoveHeader(temp);

    // if (aodvPacketSet.count(temp.Get_ts()) > 0) {
    //     return ;
    // } else {
    //     aodvPacketSet.insert(temp.Get_ts());
    // }

    // packet->AddHeader(temp);

    // 需要处理
    //
    NS_LOG_INFO("\nrecv1: node with aqua device-"
                << m_device_aq_id
                << " and wifi device-" << m_device_wifi_id
                << ": Get Message! \n"
                << "packet Size with MixPacketHeader: " << packet->GetSize()
                << ", recv time:" << 1.0*Simulator::Now().GetNanoSeconds() / 1000000000 << "s");
    // NS_LOG_INFO(Simulator::Now());

    // NS_LOG_INFO("\njust with MixPacketHeader, packet-length="
    //         << packet->GetSize()<<"\n");

    MixPacketHeader recvHeader;
    UdpHeader udpHeader;
    packet->PeekHeader(udpHeader);
    if (udpHeader.GetDestinationPort() == port ||
        udpHeader.GetDestinationPort() == port_2 ||
        udpHeader.GetDestinationPort() == port_3 ) {
        packet->RemoveHeader(udpHeader); // CP: 额外的udp头
    }

    packet->RemoveHeader(recvHeader);
    recvHeader.Print(std::cout);

    int type = recvHeader.Get_type(); // value from packet 解析
    
    // 到此处， 必定是无人机或者无人船
    int target_id = recvHeader.Get_dist_id();
    if ( (type&IS_AODV_DIST) ) {
        if (target_id == m_device_wifi_id) {
            NS_LOG_INFO("Get Message !!! finish!\n");
            statics_total_bit += packet->GetSize();
            return ;
        }
        else {
            // 作为aodv网络设备接收到 。 中继信号肯定是  1
            // 这里应该是 帮助auv 到 aodv网络专用， 仅一种情况会执行到这里
            // NS_LOG_INFO ("HELP AUV");
            packet->AddHeader(recvHeader);
            this->SendPacketToAodv (target_id, true, packet, 0);
        }
        return ;
    }


    if ( (type&IS_AQUA_DIST) ) {
        int next_usv = mix_route.getRoute(2, target_id);
        NS_LOG_INFO("Need send to wifi device=" << next_usv);
        if (m_device_wifi_id == next_usv) {
            NS_LOG_INFO("Send to ME or MY children");
            packet->AddHeader(recvHeader);
            recv2(packet);
        }
        else {
            NS_LOG_INFO("Send to OTHER usv");
            packet->AddHeader(recvHeader);
            this->SendPacketToAodv(next_usv, true, packet, 0);  
            NS_LOG_INFO("Send to OTHER usv finish");

        }
    }
}

/**
 * @brief AUV 节点信息处理流程
 *
 * 1. 最简单的 直发报文
 * 2. 发给船
 * 
 * @param p
 */
void MixApp::recv2(Ptr<Packet> packet)
{
    MixPacketHeader temp;
    packet->RemoveHeader(temp);

    if (aquaPacketSet.count(temp.Get_ts()) > 0) {
        return ;
    } else {
        aquaPacketSet.insert(temp.Get_ts());
    }

    packet->AddHeader(temp);



    NS_LOG_INFO("\nrecv2: node with aq device-" << m_device_aq_id
                << ", with wifi device id=" << m_device_wifi_id
                << " : Get Message! "
                << "packet Size with MixPacketHeader: " << packet->GetSize()
                << ", recv time:" << 1.0*Simulator::Now().GetNanoSeconds()/1000000000 << "s");
    
    MixPacketHeader recvHeader;
    packet->RemoveHeader(recvHeader);
    recvHeader.Print(std::cout);

    // recvHeader.Print(std::cout);
    int type = recvHeader.Get_type(); // value from packet 解析
    int target_id = recvHeader.Get_dist_id();

    // int my_id = 
    // if ( ) {
    //     Aqua2DplotCallback (recvHeader.Get_send_id(), recvHeader.Get_recv_id(), packet->GetSize());
    // }

    if ( (type&IS_AQUA_DIST) ) {
        if ( target_id == m_device_aq_id) {
            if (type&IS_MESSAGE) {
                NS_LOG_INFO ("Get MESSAGE");
                statics_total_bit += packet->GetSize();
            } 
            else {
                NS_LOG_INFO("message is route request, mode = RELAY"
                    << ", reply to USV as relay");
            }
        } else {
            if (type&IS_DIRECT) {
                packet->AddHeader(recvHeader);
                this->SendPacketToAqua(target_id, true, packet, 0);
            } else if (type&IS_RELAY) {
                packet->AddHeader(recvHeader);
                NS_LOG_INFO ("HELP AQUA RELAY comm, target aqua-id =  "
                        << target_id);
                        // << "To avoid crash, delay 10s");
                // 为了防止冲突， 在处理中继任务时需要延迟发送。
                this->SendPacketToAqua( target_id, true, packet, 0);
                // Simulator::Schedule(Seconds(10 + Simulator::Now().GetSeconds()), 
                //         &MixApp::
            }
        }
    } else if ( (type&IS_AODV_DIST) ) {
        packet->AddHeader(recvHeader);
        recv1(packet);
    }
}

void MixApp::packetMessagePrint(Ptr<Packet> p)
{

}


// aqua node handle message
void MixApp::HandleRead3(Ptr<Packet> p)
{
    std::cout << "Get Message!" << std::endl;
    //   << "test callback value " << callback_test << std::endl;
}

void MixApp::SetMyAqDevice(Ptr<AquaSimNetDevice> device)
{
    m_device_aq = device;
    m_device_aq_id = AquaSimAddress::ConvertFrom(m_device_aq->GetAddress()).GetAsInt();
}

void MixApp::SetMyWifiDevice(Ptr<NetDevice> device, int i)
{
    m_device_wifi = device;
    m_device_wifi_id = i + 1;
}


// 这行代码代表什么呢？ 修改。
// 对于潜艇， 父节点id为父节点水声设备id
// 对于无人船， father - id 为无人船的 wifi节点id
void MixApp::InitialMyRoute()
{

    // 增加节点名字 id 属性。
    int m = m_device_aq_id;
    // std::cout<< "test void MixApp::InitialMyRoute(), m = " << m << std::endl;
    int nn = 0;
    // 如果是普通潜航器
    if (is_auv) {
        // 普通的节点， 添加直接可达的节点， 根据 AUV_AUV_DIRECT
        for (int i = 0; i < 1000; i ++) {
            if (AUV_AUV_DIRECT[i][0] == m) {
                this->mix_route.mix_table.addEntry(AUV_AUV_DIRECT[i][1], AUV_AUV_DIRECT[i][1], 10);
                nn++;
            }
            if (AUV_AUV_DIRECT[i][0] == 0) {
                break;
            }
        }
        // 添加父节点 根据 USV_AUV_FATHER_TO_CHILD， 右边相等， 左边 是 wifi号+ auv节点数
        for (int i = 0; i < 1000; i ++) {
            if (USV_AUV_FATHER_TO_CHILD[i][1] == m) {
                default_father_id = USV_AUV_FATHER_TO_CHILD[i][0] + auv_num; // 此处需要格外注意， 水声设备了解的号只有 aq 号
                break;
            }
            if (USV_AUV_FATHER_TO_CHILD[i][0] == 0) {
                break;
            }
        }
        std::cout<< "add entry : " << nn << ", default_father_id = " << default_father_id << std::endl;
    } 

    // if (m > auv_num && m <= auv_num+usv_withAq_num) {
    if (is_usv) {
        // 这里对所有的无人船节点处理
        // 前方为 wifi 编号， 后方为 aqua 编号
        // 这里必须是全知全能的。 根据 USV_AUV_FATHER_TO_CHILD
        for (int i = 0; i < 1000; i ++) {
            if (USV_AUV_FATHER_TO_CHILD[i][0] == 0) {
                std::cout<< "add father-child count = " << i << std::endl;
                break;
            }
            this->mix_route.mix_table.addEntry(USV_AUV_FATHER_TO_CHILD[i][0], USV_AUV_FATHER_TO_CHILD[i][1]);
        }
    }

    if (is_uav) {
        int n = m_device_wifi_id;
        uav_default_usv_father_id = 996;
        for (int i = 0; i < 1000; i++) {
            if (n == UAV_USV_CHILD_TO_FATHER[i][0]) {
                uav_default_usv_father_id = UAV_USV_CHILD_TO_FATHER[i][1];
            }
            if (UAV_USV_CHILD_TO_FATHER[i][0] == 0) {
                break;
            }
        }
        std::cout<< "add uav_default_usv_father_id = " << uav_default_usv_father_id << std::endl;
        if (uav_default_usv_father_id == 996) {
            std::cout << "configure file without message about it";
            uav_default_usv_father_id = 1;
        }
    }

}

int MixApp::GetPriotyByDelay(uint64_t delay) {
    int res = 5;
    while (delay > Bound) {
        res += 5;
        delay -= Bound;
    }
    return res;
}

int MixApp::Get_aq_device_id() {
    return m_device_aq_id;
}
int MixApp::Get_wifi_device_id(){
    return m_device_wifi_id;
}

// 想写成类的静态方法不知道为啥失败了
void set_net_num(int aauv_num, int uusv_num, int uuav_num, int uusv_withAq_num) {
    auv_num = aauv_num;
    usv_num = uusv_num;
    uav_num = uuav_num;
    usv_withAq_num = uusv_withAq_num;
}



MixPacketHeader::MixPacketHeader(/* args */)
{
}

MixPacketHeader::~MixPacketHeader()
{
}

void MixPacketHeader::Set_recv_id(uint32_t id)
{
    recv_id = id;
}
uint32_t MixPacketHeader::Get_recv_id(void) const
{
    return recv_id;
}
// Time SeqTsHeader::GetTs (void) const{}

void MixPacketHeader::Set_send_id(uint32_t id)
{
    send_id = id;
}
uint32_t MixPacketHeader::Get_send_id(void) const
{
    return send_id;
}

void MixPacketHeader::Set_dist_id(uint32_t id)
{
    dist_id = id;
}
uint32_t MixPacketHeader::Get_dist_id(void) const
{
    return dist_id;
}

void MixPacketHeader::Set_souc_id(uint32_t id)
{
    souc_id = id;
}
uint32_t MixPacketHeader::Get_souc_id(void) const
{
    return souc_id;
}

void MixPacketHeader::Set_type(uint32_t value)
{
    packet_type = value;
}
uint32_t MixPacketHeader::Get_type(void) const
{
    return packet_type;
}

void MixPacketHeader::Set_ts()
{
    // if (time) {
    //     m_timestamp = time;
    // }
    m_timestamp = Simulator::Now().GetNanoSeconds();
}
uint64_t MixPacketHeader::Get_ts(void) const
{
    return m_timestamp;
}

TypeId
MixPacketHeader::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::MixPacketHeader")
                            .SetParent<Header>()
                            .AddConstructor<MixPacketHeader>();
    return tid;
}

TypeId
MixPacketHeader::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

// 输出流到 os 之上
void MixPacketHeader::Print(std::ostream &os) const
{

    os <<"PacketHeader::Print : "
       << "recv_id=" << recv_id
       << ", send_id=" << send_id
       << ", dist_id=" << dist_id
       << ", souc_id=" << souc_id
       << ", packet_type=" << packet_type;
    if (packet_type&IS_AQUA_DIST) {
        os << ", target device is aqua";
    }
    else if (packet_type&IS_AODV_DIST) {
        os << ", target device is aodv";
    }
    if (packet_type&IS_AUV_SEND) {
        os << ", send device is AUV";
    }
    else if (packet_type&IS_USV_SEND) {
        os << ", send device is USV";
    }
    else if (packet_type&IS_UAV_SEND) {
        os << ", send device is UAV";
    }
       
    os << ", timestamp = " << 1.0*m_timestamp/1000000000 << "s"
       << "." << std::endl;
}

uint32_t
MixPacketHeader::GetSerializedSize(void) const
{
    return 4 + 4 + 4 + 4 + 4 + 8;
}

void MixPacketHeader::Serialize(Buffer::Iterator start) const
{
    // NS_LOG_FUNCTION(this << &start);
    Buffer::Iterator i = start;
    i.WriteHtonU32(recv_id);
    i.WriteHtonU32(send_id);
    i.WriteHtonU32(dist_id);
    i.WriteHtonU32(souc_id);
    i.WriteHtonU32(packet_type);
    i.WriteHtonU64(m_timestamp);
}

uint32_t
MixPacketHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;
    recv_id = i.ReadNtohU32();
    send_id = i.ReadNtohU32();
    dist_id = i.ReadNtohU32();
    souc_id = i.ReadNtohU32();
    packet_type = i.ReadNtohU32();
    m_timestamp = i.ReadNtohU64();

    return GetSerializedSize();
}





void test(Ptr<MixApp> usv1_app_lawyer)
{
    std::cout << "test send, my id=" << usv1_app_lawyer->m_device_aq_id << std::endl;
    Ptr<Packet> packet = Create<Packet>(1024);
    usv1_app_lawyer->SendAquaPacket(3, packet);
}



// 
void setColorById (PointerValue txXySeries, int id) {
    int mod = 4;
    if ((id+mod*996)%mod == 0 ) {
        txXySeries.Get<netsimulyzer::XYSeries>()->SetAttribute("Color",
                                                netsimulyzer::DARK_RED_VALUE);
    }
    else if ((id+mod*996)%mod == 1 ) {
        txXySeries.Get<netsimulyzer::XYSeries>()->SetAttribute("Color",
                                                netsimulyzer::DARK_GREEN_VALUE);
    }
    else if ((id+mod*996)%mod == 2 ) {
        txXySeries.Get<netsimulyzer::XYSeries>()->SetAttribute("Color",
                                                netsimulyzer::DARK_BLUE_VALUE);
    }
    else if ((id+mod*996)%mod == 3 ) {
        txXySeries.Get<netsimulyzer::XYSeries>()->SetAttribute("Color",
                                                netsimulyzer::DARK_ORANGE_VALUE);
    }
    else if ((id+mod*996)%mod == 4 ) {
        txXySeries.Get<netsimulyzer::XYSeries>()->SetAttribute("Color",
                                                netsimulyzer::DARK_YELLOW_VALUE);
    }
    else if ((id+mod*996)%mod == 5 ) {
        txXySeries.Get<netsimulyzer::XYSeries>()->SetAttribute("Color",
                                                netsimulyzer::DARK_PURPLE_VALUE);
    } else {
        txXySeries.Get<netsimulyzer::XYSeries>()->SetAttribute("Color",
                                                netsimulyzer::DARK_PURPLE_VALUE);
    }
}

int configureFlow (int from_id, int end_id, int node_type, int begin_time, int end_time ) {

    if (node_type == 1) {
        std::cout << "configureFlow VOIP-FLOW, FLOW-END-ID = " << end_id << std::endl;
        int flow_num = voip_client.size();
        int flow_index = rand() % flow_num;
        for ( auto cur_p = voip_client[flow_index].begin(); cur_p != voip_client[flow_index].end(); cur_p++ ) {
            if (end_id > auv_num) {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AODV_DIST, end_id-auv_num, cur_p->second, node_type);
                std::cout << "configureFlow IS_AODV_DIST targetId = " << end_id-auv_num << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;
            } else {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AQUA_DIST, end_id, cur_p->second, node_type);
                std::cout << "configureFlow IS_AQUA_DIST targetId = " << end_id << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;;
            }
            std::cout << "vpip_num = " << flow_num << ", voip_index = " << flow_index
                <<", at time = " << cur_p->first 
                << "ms, packetsize = " <<cur_p->second<<std::endl;
        }
        for ( auto cur_p = voip_server[flow_index].begin(); cur_p != voip_server[flow_index].end(); cur_p++ ) {
            if (end_id > auv_num) {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AODV_DIST, end_id-auv_num, cur_p->second, node_type);
                std::cout << "configureFlow IS_AODV_DIST targetId = " << end_id-auv_num << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;
            } else {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AQUA_DIST, end_id, cur_p->second, node_type);
                std::cout << "configureFlow IS_AQUA_DIST targetId = " << end_id << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;
            }
            std::cout << "vpip_num = " << flow_num << ", voip_index = " << flow_index
                <<", at time = " << cur_p->first 
                << "ms, packetsize = " <<cur_p->second<<std::endl;
        }
    }
    else if (node_type == 3) {
        std::cout << "configureFlow CXMSA-FLOW, FLOW-END-ID = " << end_id << std::endl;
        int flow_num = csmxp_client.size();
        int flow_index = rand() % flow_num;
        for ( auto cur_p = csmxp_client[flow_index].begin(); cur_p != csmxp_client[flow_index].end(); cur_p++ ) {
            if (end_id > auv_num) {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AODV_DIST, end_id-auv_num, cur_p->second, node_type);
                std::cout << "configureFlow IS_AODV_DIST targetId = " << end_id-auv_num << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;
            } else {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AQUA_DIST, end_id, cur_p->second, node_type);
                std::cout << "configureFlow IS_AQUA_DIST targetId = " << end_id << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;
            }
            std::cout << "cxmsa_num = " << flow_num << ", cxmsa_index = " << flow_index
                <<", at time = " << cur_p->first 
                << "ms, packetsize = " <<cur_p->second<<std::endl;
        }
        for ( auto cur_p = csmxp_server[flow_index].begin(); cur_p != csmxp_server[flow_index].end(); cur_p++ ) {
            if (end_id > auv_num) {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AODV_DIST, end_id-auv_num, cur_p->second, node_type);
                std::cout << "configureFlow IS_AODV_DIST targetId = " << end_id-auv_num << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;
            } else {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AQUA_DIST, end_id, cur_p->second, node_type);
                std::cout << "configureFlow IS_AQUA_DIST targetId = " << end_id << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;
            }
            std::cout << "cxmsa_num = " << flow_num << ", cxmsa_index = " << flow_index
                <<", at time = " << cur_p->first 
                << "ms, packetsize = " <<cur_p->second<<std::endl;
        }
    }
    else if (node_type == 2) {
        std::cout << "configureFlow HTTP-FLOW, FLOW-END-ID = " << end_id << std::endl;
        int flow_num = http_client.size();
        int flow_index = rand() % flow_num;
        for ( auto cur_p = http_client[flow_index].begin(); cur_p != http_client[flow_index].end(); cur_p++ ) {
            if (end_id > auv_num) {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AODV_DIST, end_id-auv_num, cur_p->second, node_type);
                std::cout << "configureFlow IS_AODV_DIST targetId = " << end_id-auv_num << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;
            } else {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AQUA_DIST, end_id, cur_p->second, node_type);
                std::cout << "configureFlow IS_AQUA_DIST targetId = " << end_id << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;
            }
            std::cout << "http_num = " << flow_num << ", http_index = " << flow_index
                <<", at time = " << cur_p->first 
                << "ms, packetsize = " <<cur_p->second<<std::endl;
        }
        for ( auto cur_p = http_server[flow_index].begin(); cur_p != http_server[flow_index].end(); cur_p++ ) {
            if (end_id > auv_num) {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AODV_DIST, end_id-auv_num, cur_p->second, node_type);
                std::cout << "configureFlow IS_AODV_DIST targetId = " << end_id-auv_num << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;
            } else {
                all_app_lawyer[from_id-1]->PlanTx(begin_time + cur_p->first * 0.01, IS_AQUA_DIST, end_id, cur_p->second, node_type);
                std::cout << "configureFlow IS_AQUA_DIST targetId = " << end_id << "at second:" << begin_time + cur_p->first * 0.01 << std::endl;
            }
            std::cout << "http_num = " << flow_num << ", http_index = " << flow_index
                <<", at time = " << cur_p->first 
                << "ms, packetsize = " <<cur_p->second<<std::endl;
        }
    }
    else {
        // type==0 CBR默认流，此时begin_time和end_time都生效，且每隔半秒发一次包
        int m_packetSize = 1024;
        for (double i = begin_time; i - end_time < 0; i += 0.5 ) {
            if (end_id > auv_num) {
                all_app_lawyer[from_id-1]->PlanTx(i, IS_AODV_DIST, end_id-auv_num, m_packetSize, node_type);
                std::cout << "configureFlow IS_AODV_DIST targetId = " << end_id-auv_num << std::endl;
            } else {
                all_app_lawyer[from_id-1]->PlanTx(i, IS_AQUA_DIST , end_id, m_packetSize, node_type);
                std::cout << "configureFlow IS_AQUA_DIST targetId = " << end_id << std::endl;
            }
        }
    }

    return 1;
}


using json = nlohmann::json;

/** 
 * 0节点id： 
 * 1节点类型：
 * 2x
 * 3y
 * 4z
 * 5移动模型
 * 6有radio？
 * 7有light？
 * 8有sound？
*/
int node_mess[100][10];



int main(int argc, char *argv[])
{
    // Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
    // Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
    final_sim_file <<"仿真参数配置开始" << std::endl;

    bool useStaticRouting = false;
    std::string nodeConfigPath;
    std::string streamConfigPath;
    std::string routingTablePath = "./static_routing_dense.json";
    CommandLine cmd;
    cmd.AddValue("useStaticRouting", "Whether use static routing for wifi or not.", useStaticRouting);
    cmd.AddValue("nodeConfigPath", "node config file path.", nodeConfigPath);
    cmd.AddValue("streamConfigPath", "stream config file path.", streamConfigPath);
    cmd.AddValue("routingTablePath", "routing table path(valid if static routing is enabled.)", routingTablePath);
    cmd.Parse(argc, argv);

     const char* fifoPath = "/tmp/myfifo";
     // 打开命名管道以供写入。管道由QT程序负责创建。
     int fifoFd = open(fifoPath, O_WRONLY);
     if (fifoFd < 0) {
         perror("writer can't open fifo.");
     } else {
         std::cout << "writer opened fifo." << std::endl;
     }
     close(STDOUT_FILENO);
     // 将标准输出复制到FIFO文件描述符
     if (dup(fifoFd) == -1) {
         perror("Error duplicating FIFO FD to STDOUT");
         return 1;
     }

    srand (89);
    // 初始化可能的流发送序列
    json flow_series_json;
    std::ifstream flow_series_json_file("./typical_flow.json");
    flow_series_json_file >> flow_series_json;

    json http_series_list = flow_series_json.at("HTTP");
    for (int i = 0; i < (int)http_series_list.size(); i ++) {
        std::map<int, int> time_packet_pair1;
        std::map<int, int> time_packet_pair2;
        json stream_cur = http_series_list.at(i);
        json stream_cur_client = stream_cur.at("client");
        json stream_cur_server = stream_cur.at("server");
        for (int j = 0; j < (int)stream_cur_client.size(); j++) {
            std::cout << "time = " << stream_cur_client.at(j).at("time") 
                << ", size = " << stream_cur_client.at(j).at("size");
            time_packet_pair1.insert(time_packet_pair1.end(), {stream_cur_client.at(j).at("time"),
             stream_cur_client.at(j).at("size")});
        }
        for (int j = 0; j < (int)stream_cur_server.size(); j++) {
            time_packet_pair2.insert(time_packet_pair2.end(), {stream_cur_server.at(j).at("time"),
             stream_cur_server.at(j).at("size")});
        }
        http_client.push_back(time_packet_pair1);
        http_server.push_back(time_packet_pair2);
        time_packet_pair1.clear();
        time_packet_pair2.clear();
    }

    json voip_series_list = flow_series_json.at("VOIP");
    for (int i = 0; i < (int)voip_series_list.size(); i ++) {
        std::map<int, int> time_packet_pair1;
        std::map<int, int> time_packet_pair2;
        json stream_cur = voip_series_list.at(i);
        json stream_cur_client = stream_cur.at("client");
        json stream_cur_server = stream_cur.at("server");
        for (int j = 0; j < (int)stream_cur_client.size(); j++) {
            std::cout << "time = " << stream_cur_client.at(j).at("time") 
                << ", size = " << stream_cur_client.at(j).at("size");
            time_packet_pair1.insert(time_packet_pair1.end(), {stream_cur_client.at(j).at("time"),
             stream_cur_client.at(j).at("size")});
        }
        for (int j = 0; j < (int)stream_cur_server.size(); j++) {
            time_packet_pair2.insert(time_packet_pair2.end(), {stream_cur_server.at(j).at("time"),
             stream_cur_server.at(j).at("size")});
        }
        voip_client.push_back(time_packet_pair1);
        voip_server.push_back(time_packet_pair2);
        time_packet_pair1.clear();
        time_packet_pair2.clear();
    }

    json cxmsp_series_list = flow_series_json.at("CXMSP");
    for (int i = 0; i < (int)cxmsp_series_list.size(); i ++) {
        std::map<int, int> time_packet_pair1;
        std::map<int, int> time_packet_pair2;
        json stream_cur = cxmsp_series_list.at(i);
        json stream_cur_client = stream_cur.at("client");
        json stream_cur_server = stream_cur.at("server");
        for (int j = 0; j < (int)stream_cur_client.size(); j++) {
            std::cout << "time = " << stream_cur_client.at(j).at("time") 
                << ", size = " << stream_cur_client.at(j).at("size");
            time_packet_pair1.insert(time_packet_pair1.end(), {stream_cur_client.at(j).at("time"),
             stream_cur_client.at(j).at("size")});
        }
        for (int j = 0; j < (int)stream_cur_server.size(); j++) {
            time_packet_pair2.insert(time_packet_pair2.end(), {stream_cur_server.at(j).at("time"),
             stream_cur_server.at(j).at("size")});
        }
        csmxp_client.push_back(time_packet_pair1);
        csmxp_server.push_back(time_packet_pair2);
        time_packet_pair1.clear();
        time_packet_pair2.clear();
    }

    for (int i = 0; i < 1000; i++) for (int j = 0; j < 2; j++) {USV_AUV_FATHER_TO_CHILD[i][j]=0;}
    for (int i = 0; i < 1000; i++) for (int j = 0; j < 2; j++) {AUV_AUV_DIRECT[i][j]=0;}
    for (int i = 0; i < 1000; i++) for (int j = 0; j < 2; j++) {UAV_USV_CHILD_TO_FATHER[i][j]=0;}

    // cp: 把流配置文件读取提前到这了，以便对是否使用静态路由进行设置
    // 流配置文件
    json flow;
    std::ifstream jjfile(streamConfigPath);
    jjfile >> flow;
    if (flow.contains("use_static_routing")) {
        useStaticRouting = flow.at("use_static_routing");
    }

    json j;
    std::ifstream jfile(nodeConfigPath);

    jfile >> j;
    int radio_length = j.at("CHANNEL_MESAGE").at("RADIO_LEN");
    int light_length = j.at("CHANNEL_MESAGE").at("LIGHT_LEN");
    int sound_length = j.at("CHANNEL_MESAGE").at("SOUND_LEN");

    // LOG_MARK:
    std::cout << "radio_length = " << radio_length 
              << ", light_length = " <<  light_length
              << ", sound_length = " <<  sound_length << std::endl;
    int auv_num = j.at("node_num").at("auv_num");
    int uav_num = j.at("node_num").at("uav_num");
    int usv_num = j.at("node_num").at("usv_num");
    int total_num = 1+auv_num+uav_num+usv_num;
    final_sim_file << "射频距离 = " << radio_length 
              << "m, 光距离 = " <<  light_length
              << "m, 声距离 = " <<  sound_length <<"m"<< std::endl;

    std::cout << "auv_num = " << auv_num 
              << ", uav_num = " <<  uav_num
              << ", auv_num = " <<  usv_num 
              << ", total_num = " <<  total_num 
              << std::endl;

    std::vector<int> auv_id;
    std::vector<int> usv_id; 
    std::vector<int> uav_id; 
    std::vector<int> usv_with_aq_id; 

    json device_m = j.at("DEVICE_MESSAGE");
    std::cout << "size() = " << (int)device_m["AUV_ID"].size(); 

    for (int i = 0; i < (int)device_m["AUV_ID"].size(); i++) {
        auv_id.push_back(device_m["AUV_ID"][i]);
    }
    for (int i = 0; i < (int)device_m["USV_ID"].size(); i++) {
        usv_id.push_back(device_m.at("USV_ID").at(i));
    }
    for (int i = 0; i < (int)device_m["UAV_ID"].size(); i++) {
        uav_id.push_back(device_m.at("UAV_ID").at(i));
    }
    for (int i = 0; i < (int)device_m["USV_WITH_AQUA"].size(); i++) {
        usv_with_aq_id.push_back(device_m.at("USV_WITH_AQUA").at(i));
    }

    set_net_num (auv_num, usv_num, uav_num, usv_with_aq_id.size());

    for (int i = 0; i < (int)device_m["USV_AUV_FATHER_TO_CHILD"].size(); i++) {
        USV_AUV_FATHER_TO_CHILD[i][0] = device_m["USV_AUV_FATHER_TO_CHILD"][i][0];
        USV_AUV_FATHER_TO_CHILD[i][1] = device_m["USV_AUV_FATHER_TO_CHILD"][i][1];
    }
    for (int i = 0; i < (int)device_m["AUV_AUV_DIRECT"].size(); i++) {
        AUV_AUV_DIRECT[i][0] = device_m["AUV_AUV_DIRECT"][i][0];
        AUV_AUV_DIRECT[i][1] = device_m["AUV_AUV_DIRECT"][i][1];
    }
    for (int i = 0; i < (int)device_m["UAV_USV_CHILD_TO_FATHER"].size(); i++) {
        UAV_USV_CHILD_TO_FATHER[i][0] = device_m["UAV_USV_CHILD_TO_FATHER"][i][0];
        UAV_USV_CHILD_TO_FATHER[i][1] = device_m["UAV_USV_CHILD_TO_FATHER"][i][1];
    }


    // int node_mess[100][10];
    json pos;
    pos[0] = j.at("AUV_POS");
    pos[1] = j.at("UAV_POS");
    pos[2] = j.at("USV_POS");
    for (int kk = 0; kk < 3; kk++) {
        json auv_pos = pos[kk];
        for (int i = 0; i < (int)auv_pos.size(); i++) {
            // std::cout << "node id = " << auv_pos[i]["node_id"]
            //         << "position_x = " << auv_pos[i]["position_x"] << std::endl;
            int node_id = (int)auv_pos[i]["node_id"];
            node_mess[node_id][0] = node_id;
            node_mess[node_id][2] = auv_pos[i]["position_x"];
            node_mess[node_id][3] = auv_pos[i]["position_y"];
            node_mess[node_id][4] = auv_pos[i]["position_z"];
            node_mess[node_id][5] = auv_pos[i]["type"];
        }
    }


    NodeContainer base_station;
    NodeContainer auv;
    NodeContainer usv;
    NodeContainer uav;

    base_station.Create(1); // 0
    auv.Create(auv_num); // 1 to auv_num
    usv.Create(usv_num); // auv_num+1 to auv_num+usv_num 
    uav.Create(uav_num); // auv_num+usv_num+1 to  auv_num+usv_num+uav_num+1
    NodeContainer nodeAll = NodeContainer(base_station, auv, usv, uav);
    
//    MobilityHelper mobility;
//    for (int kk = 1; kk < total_num; kk ++) {
//        // std::cout << "kk is well, id = " << kk << std::endl;
//        if (node_mess[kk][0] == 0) {
//            std::cout << "error !! node not configure, node id = " << kk << std::endl;
//        }
//        if ((int)node_mess[kk][5] == 1) {
//            mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
//        }
//        else if ((int)node_mess[kk][5] == 2) {
//            mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
//                    "Mode",
//                    StringValue("Time"),
//                    "Time",
//                    StringValue("2s"),
//                    "Speed",
//                    StringValue("ns3::UniformRandomVariable[Min=20.0|Max=40.0]"),
//                    // StringValue("ns3::ConstantRandomVariable[Constant=20.0]"),
//                    "Bounds",
//                    StringValue("0|20000|0|20000"));
//        } else if ((int)node_mess[kk][5] == 3) {
//            // 配置 额外的模式
//        Simulator::Schedule(Seconds(0), &plan_node_vel, auv, 1, Vector(40, 0.0, 0.0));
//        Simulator::Schedule(Seconds(10), &plan_node_vel, auv, 1, Vector(-40, 0.0, 0.0));
//        Simulator::Schedule(Seconds(20), &plan_node_vel, auv, 1, Vector(0, 1.0, 0.0));
//        }
//        mobility.Install( nodeAll.Get(kk));
//
//        Ptr<Object> object = nodeAll.Get(kk);
//        Ptr<MobilityModel> model = object->GetObject<MobilityModel>();
//        model->SetPosition(Vector(node_mess[kk][2], node_mess[kk][3], node_mess[kk][4]));
//    }
    MobilityHelper mobility;
    int vel_t_n = 0;
    json VEL_T = j.at("VEL_T");

    for (int kk = 1; kk < total_num; kk ++) {
        // std::cout << "kk is well, id = " << kk << std::endl;
        if (node_mess[kk][0] == 0) {
            std::cout << "error !! node not configure, node id = " << kk << std::endl;
        }
        if ((int)node_mess[kk][5] == 1) {
            mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        }
        else if ((int)node_mess[kk][5] == 2) {


            //  mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
            //         "Mode",
            //         StringValue("Distance"),
            //         "Distance",
            //         DoubleValue(200.0),
            //         "Speed",
            //         StringValue("ns3::UniformRandomVariable[Min=20.0|Max=80.0]"),
            //         // StringValue("ns3::ConstantRandomVariable[Constant=20.0]"),
            //         "Bounds",
            //         StringValue("0|20000|0|20000"));

            mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                      "Mode",
                                      StringValue("Time"),
                                      "Time",
                                      StringValue("2s"),
                                      "Speed",
                                      StringValue("ns3::UniformRandomVariable[Min=20.0|Max=40.0]"),
                                      // StringValue("ns3::ConstantRandomVariable[Constant=20.0]"),
                                      "Bounds",
                                      StringValue("0|20000|0|20000"));
        } else if ((int)node_mess[kk][5] == 3) {

            mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
            mobility.Install( nodeAll.Get(kk));

            // 配置 额外的模式
            Ptr<Object> object = nodeAll.Get(kk);
            Ptr<MobilityModel> model = object->GetObject<MobilityModel>();

            model->SetPosition(Vector(node_mess[kk][2], node_mess[kk][3], node_mess[kk][4]));

            if (vel_t_n == (int)VEL_T.size()) continue; // 提供的序列不够
            json vel_tr_now = VEL_T[vel_t_n++];
            for (int vel_line_count = 0; vel_line_count < (int)vel_tr_now.size(); vel_line_count++) {
                json vel_tr_item = vel_tr_now[vel_line_count];
                Simulator::Schedule(Seconds(vel_tr_item["time"]),  &plan_node_vel, nodeAll,
                                    kk, Vector(vel_tr_item["VEL_X"], vel_tr_item["VEL_Y"],
                                           vel_tr_item["VEL_Z"]));
            }
            continue;
        }
        mobility.Install( nodeAll.Get(kk));

        Ptr<Object> object = nodeAll.Get(kk);
        Ptr<MobilityModel> model = object->GetObject<MobilityModel>();
        model->SetPosition(Vector(node_mess[kk][2], node_mess[kk][3], node_mess[kk][4]));
    }

    // DEBUG: 输出观测位置配置正确与否
    for (auto i = 1; i < total_num; i++ ) {
        Ptr<MobilityModel> mobility = nodeAll.Get(i)->GetObject<MobilityModel>();
        const auto position = mobility->GetPosition();
        // std::cout << "node-id = " << i
        //           << ", position.x = " << position.x 
        //           << ", position.y = " << position.y 
        //           << ", position.z = " << position.z << std::endl;
        final_sim_file << "节点信息： id = " << i
                  << ", position.x = " << position.x 
                  << ", position.y = " << position.y 
                  << ", position.z = " << position.z << std::endl;
    }

    PacketSocketHelper socketHelper; 
    socketHelper.Install(auv);

    double simStop = (double)j["SIM_TIME"]; 
    int range = sound_length;

    test();
    LogComponentEnable("MXR_SCF", LOG_LEVEL_INFO);
    LogComponentEnable("Ipv4StaticRouting", ns3::LOG_LEVEL_LOGIC);
    
    // LogComponentEnable("Ipv4EndPointDemux", LOG_LEVEL_INFO);
 


    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211b);

    WifiMacHelper wifiMac;
    wifiMac.SetType ("ns3::AdhocWifiMac");

    
    YansWifiChannelHelper wifiChannel;
    double txpDistance = radio_length;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel",
                            "Speed", 
                            DoubleValue(299792458));  
                            
    wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel",
                                  "MaxRange",
                                  DoubleValue(txpDistance));

    YansWifiPhyHelper wifiPhy;
    wifiPhy.Set("TxPowerStart", DoubleValue(20)); // dBm (1.26 mW)
    wifiPhy.Set("TxPowerEnd", DoubleValue(20));

    wifiPhy.SetChannel (wifiChannel.Create ());


    for (int i = 0; i < usv_num; i++) {
        wifi_devices.Add(wifi.Install(wifiPhy, wifiMac, usv.Get(i)));
    }
    for (int i = 0; i < uav_num; i++) {
        wifi_devices.Add(wifi.Install(wifiPhy, wifiMac, uav.Get(i)));
    }

    // TODO: STATIC ROUTING CONFIG
    AodvHelper aodv;
    InternetStackHelper stack;
    if (!useStaticRouting) {
        stack.SetRoutingHelper(aodv);
    }
    stack.Install(usv);
    stack.Install(uav);

    Ipv4AddressHelper address;
    address.SetBase("10.0.0.0", "255.0.0.0");
    interfaces = address.Assign(wifi_devices);

    // 设置静态路由
     if (useStaticRouting) {
         Ipv4StaticRoutingHelper staticRouting;
         json routes;
         std::ifstream routeFile(routingTablePath);
         routeFile >> routes;
         json nodeRouteTables = routes.at("node_route_tables");
         for (int i = 0; i < (int)nodeRouteTables.size(); i ++) {
             json nodeRouteTable = nodeRouteTables.at(i);
             int nodeId = nodeRouteTable["node_id"];
             json nodeRoutes = nodeRouteTable["routes"];

             auto node = nodeAll.Get(nodeId);
             Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
             Ptr<Ipv4StaticRouting> routing = staticRouting.GetStaticRouting(ipv4);

             for (int k = 0; k < (int)nodeRoutes.size(); k ++) {
                 json route = nodeRoutes.at(k);
                 int type = route["type"];
                 int dest = route["dest"];
                 int nextHop = route["next_hop"];
                 if (type == 1) {
                     std::cout << "set VoIP routing for " << ipv4->GetAddress(1, 0) << std::endl;
                     routing->AddHostRouteToVoIP(CreateIpv4Address(dest - auv_num), CreateIpv4Address(nextHop - auv_num), 1);
                 } else if (type == 2) {
                     std::cout << "set HTTP routing for " << ipv4->GetAddress(1, 0) << std::endl;
                     routing->AddHostRouteToHTTP(CreateIpv4Address(dest - auv_num), CreateIpv4Address(nextHop - auv_num), 1);
                 } else {
                     std::cout << "set CSMXP routing for " << ipv4->GetAddress(1, 0) << std::endl;
                     routing->AddHostRouteToCSMXP(CreateIpv4Address(dest - auv_num), CreateIpv4Address(nextHop - auv_num), 1);
                 }
             }
         }
     }

    for (int i = 0; i < usv_num; i++) {
        Ptr<MixApp> usv1_app_lawyer = CreateObject<MixApp>();
        if (i < (int)usv_with_aq_id.size()){ // 限制了 配备水声的无人船必须是 前几个。
            usv1_app_lawyer->Setup(AQUA_SEND_ENABLE | AQUA_RECV_ENABLE | AODV_SEND_ENABLE | AODV_RECV_ENABLE,
                    aodv_sender, 0, 1024, 1000, DataRate("1Mbps"));
        }
        else {
            usv1_app_lawyer->Setup(AODV_SEND_ENABLE | AODV_RECV_ENABLE,
                    aodv_sender, 0, 1024, 1000, DataRate("1Mbps"));
        }
        usv.Get(i)->AddApplication(usv1_app_lawyer);
        usv_app_lawyer.push_back(usv1_app_lawyer);
        usv_app_lawyer[i]->SetMyWifiDevice(wifi_devices.Get(i), i);
    }

    for (int i = 0; i < uav_num; i++) {
        Ptr<MixApp> uav1_app_lawyer = CreateObject<MixApp>();

        uav1_app_lawyer->Setup(AODV_SEND_ENABLE | AODV_RECV_ENABLE,
                aodv_sender, 1, 1024, 1000, DataRate("1Mbps"));

        uav.Get(i)->AddApplication(uav1_app_lawyer);
        uav_app_lawyer.push_back(uav1_app_lawyer);
        uav_app_lawyer[i]->SetMyWifiDevice(wifi_devices.Get(i+usv_num), i+usv_num);
    }



    for (int i = 0; i < auv_num; i++) {
        Ptr<MixApp> auv1_app_lawyer = CreateObject<MixApp>();
        auv1_app_lawyer->Setup(AQUA_SEND_ENABLE | AQUA_RECV_ENABLE,
                             aqua_sender, 2, 1024, 1000, DataRate("1Mbps"));
        auv.Get(i)->AddApplication(auv1_app_lawyer);
        auv_app_lawyer.push_back(auv1_app_lawyer);
    }
    NS_LOG_INFO ("MIX--APP SET win!");

    AquaSimChannelHelper channel = AquaSimChannelHelper::Default();
    channel.SetPropagation("ns3::AquaSimRangePropagation");
    AquaSimHelper asHelper = AquaSimHelper::Default();
    asHelper.SetChannel(channel.Create());
    // asHelper.SetMac("ns3::AquaSimAloha");
    // asHelper.SetMac("ns3::AquaSimAloha");

///  
    ObjectFactory m_routing;
    m_routing.SetTypeId("ns3::AquaSimStaticRouting");

// 水声设备的都可以在此处奢侈陈工
    for (int i = 0; i < auv_num; i ++ ) {
        Ptr<AquaSimNetDevice> newDevice = CreateObject<AquaSimNetDevice>();
        
        Ptr<AquaSimStaticRouting> auv1_routing = m_routing.Create<AquaSimStaticRouting>();
        auv_app_lawyer[i] ->SetMyStaticRouting(auv1_routing);
        newDevice->SetRouting(auv1_routing);

        devices.Add(asHelper.CreateWithoutRouting(auv.Get(i), newDevice));
        
        auv_app_lawyer[i]->SetMyAqDevice(newDevice);
        newDevice->GetPhy()->SetTransRange(range);

    }

    for (int i = 0; i < usv_num; i++) {
        if (i < (int)usv_with_aq_id.size()){ // 限制了 配备水声的无人船必须是 前几个。
            Ptr<AquaSimNetDevice> newDevice4 = CreateObject<AquaSimNetDevice>();
            Ptr<AquaSimStaticRouting> usv1_routing = m_routing.Create<AquaSimStaticRouting>();
            usv_app_lawyer[i]->SetMyStaticRouting(usv1_routing);
            newDevice4->SetRouting(usv1_routing);
            devices.Add(asHelper.CreateWithoutRouting(usv.Get(i), newDevice4));
            
            usv_app_lawyer[i]->SetMyAqDevice(newDevice4);
            newDevice4->GetPhy()->SetTransRange(range);
        }
        else {
            // no need to do 
        }
    }

    for (int i = 0; i < auv_num; i ++ ) {
        all_app_lawyer.push_back(auv_app_lawyer[i]);
        auv_app_lawyer[i]->SetStartTime(Seconds(2));
        auv_app_lawyer[i]->SetStopTime(Seconds(simStop));
        auv_app_lawyer[i]->InitialMyRoute();
    }
    for (int i = 0; i < usv_num; i++ ) {
        all_app_lawyer.push_back(usv_app_lawyer[i]);
        usv_app_lawyer[i]->SetStartTime(Seconds(2));
        usv_app_lawyer[i]->SetStopTime(Seconds(simStop));
        usv_app_lawyer[i]->InitialMyRoute();
    }
    for (int i = 0; i < uav_num; i++ ) {
        all_app_lawyer.push_back(uav_app_lawyer[i]);
        uav_app_lawyer[i]->SetStartTime(Seconds(2));
        uav_app_lawyer[i]->SetStopTime(Seconds(simStop));
        uav_app_lawyer[i]->InitialMyRoute();
    }
    NS_LOG_INFO("Setting success: aqua_devices.num=" 
            << devices.GetN()
            << ", wifi_devices.num=" << wifi_devices.GetN()
            << ", interfaces.num=" << interfaces.GetN());

    json stream_list = flow.at("stream");
  
    std::vector<std::string> flowType; // 这里可以看出来不同流的 stream-type编号
    flowType.push_back("default"); flowType.push_back("VOIP");
    flowType.push_back("HTTP");    flowType.push_back("CSXMP");

    std::cout << "all_app_lawyer.size() = " << all_app_lawyer.size();
    for (int i = 0; i < (int)stream_list.size(); i++) {
        json stream_cur = stream_list.at(i);
        std::cout << "configure flow-" << stream_cur["stream_id"]
                  << ", send id = " << stream_cur["from_id"]
                  << ", end_id = " << stream_cur["end_id"]
                  << ", flow type = " << flowType[(int)stream_cur["steam_type"]] 
                  << std::endl;
        final_sim_file << "配置流 " << stream_cur["stream_id"]
                  << ", 发送节点 ID = " << stream_cur["from_id"]
                  << ", 接受节点 ID = " << stream_cur["end_id"]
                  << ", 流种类为" << flowType[(int)stream_cur["steam_type"]] 
                  << std::endl;
        if (configureFlow(stream_cur["from_id"],
                          stream_cur["end_id"],
                          stream_cur["steam_type"],
                          stream_cur["begin_time"],
                          stream_cur["end_time"]) == 1)
        {
            std::cout << "configure success" << std::endl;
        }
        else
        {
            std::cout << "error flow configure" << std::endl;
        }
    }

    // TRACE 部分
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::ostringstream oss;
    oss << "../trace_output/trace_" << timestamp << ".json";

    std::ostringstream logFile;
    logFile << "../log_output/log_" << timestamp << ".txt";
    final_sim_file.open(logFile.str());

    auto orchestrator = CreateObject<netsimulyzer::Orchestrator>(oss.str());
    // flowRxTraceSeries
    for (int i = 1; i <= total_num-1; i++) {
        Ptr<netsimulyzer::ThroughputSink> flowTxTrace =
            CreateObject<netsimulyzer::ThroughputSink>(orchestrator,
            "node-" + std::to_string(i) + "-FlowRx"  );
        flowTxTrace->SetAttribute("Unit", StringValue("Mb/s"));
        
        PointerValue txXySeries;

        flowTxTrace->GetAttribute("XYSeries", txXySeries);
        txXySeries.Get<netsimulyzer::XYSeries>()->SetAttribute("LabelMode",
                                                        StringValue("Hidden"));
        setColorById (txXySeries, i);
        flowRxTraceSeries.insert(
            std::pair<uint32_t, Ptr<netsimulyzer::ThroughputSink> >(i, flowTxTrace));
    }
    PointerValue xAxis;
    PointerValue yAxis;
    for (int i = 1; i <= total_num - 1; i++)
    {
        Ptr<netsimulyzer::SeriesCollection> macCollection =
            CreateObject<netsimulyzer::SeriesCollection>(orchestrator);

        macCollection->SetAttribute("Name",
                                    StringValue("Node-" + std::to_string(i) + " Traffic Rx"));
        macCollection->GetAttribute("XAxis", xAxis);
        xAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name", StringValue("Time (s)"));
        macCollection->GetAttribute("YAxis", yAxis);
        yAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name",
                                                           StringValue("Throughput (Mb/s)"));
        macCollection->Add(flowRxTraceSeries.find(i)->second->GetSeries());
    }

    std::vector<nlohmann::json> aqua_node_positon_vector;

    for (auto i = 0; i < (int)auv_id.size(); i++ ) {
        nlohmann::json cur_positon_message;
        auto mobility = nodeAll.Get( auv_id[i] )->GetObject<MobilityModel>();
        const auto position = mobility->GetPosition();
        std::cout << "node-id = " << auv_id[i] 
                  << ", position.x = " << position.x 
                  << ", position.y = " << position.y 
                  << ", position.z = " << position.z
                  << std::endl;
        cur_positon_message["node_id"] = auv_id[i];
        cur_positon_message["position_x"] = position.x;
        cur_positon_message["position_y"] = position.y;
        cur_positon_message["position_z"] = position.z;

        aqua_node_positon_vector.push_back(cur_positon_message);
    }
    for (auto i = 0; i < (int)usv_with_aq_id.size(); i++ ) {
        nlohmann::json cur_positon_message;
        auto mobility = nodeAll.Get( usv_with_aq_id[i] )->GetObject<MobilityModel>();

        const auto position = mobility->GetPosition();
        std::cout << "node-id = " << usv_with_aq_id[i] 
                  << ", position.x = " << position.x 
                  << ", position.y = " << position.y 
                  << ", position.z = " << position.z
                  << std::endl;
        cur_positon_message["node_id"] = usv_with_aq_id[i];
        cur_positon_message["position_x"] = position.x;
        cur_positon_message["position_y"] = position.y;
        cur_positon_message["position_z"] = position.z;

        aqua_node_positon_vector.push_back(cur_positon_message);
    }

    std::cout <<"aodv net message!!!-----" << std::endl;
    std::vector<nlohmann::json> aodv_node_positon_vector;
// 把 aodv网络中的
    for (auto i = 0; i < (int)usv_id.size(); i++ ) {
        nlohmann::json cur_positon_message;
        auto mobility = nodeAll.Get( usv_id[i] )->GetObject<MobilityModel>();
        const auto position = mobility->GetPosition();
        std::cout << "node-id = " << usv_id[i] 
                  << ", position.x = " << position.x 
                  << ", position.y = " << position.y 
                  << ", position.z = " << position.z
                  << std::endl;

        cur_positon_message["node_id"] = usv_id[i];
        cur_positon_message["position_x"] = position.x;
        cur_positon_message["position_y"] = position.y;
        cur_positon_message["position_z"] = position.z;

        aodv_node_positon_vector.push_back(cur_positon_message);
    }
    // 把 aodv网络中的
    for (auto i = 0; i < (int)uav_id.size(); i++ ) {
        auto mobility = nodeAll.Get( uav_id[i] )->GetObject<MobilityModel>();
        if (!mobility)
        {
            NS_LOG_DEBUG("Mobility poll activated on Node with no Mobility Model, ignoring");
            return {};
        }

        const auto position = mobility->GetPosition();

        std::cout << "node-id = " << uav_id[i] 
                  << ", position.x = " << position.x 
                  << ", position.y = " << position.y 
                  << ", position.z = " << position.z
                  << std::endl;

        nlohmann::json cur_positon_message;

        cur_positon_message["node_id"] = uav_id[i];
        cur_positon_message["position_x"] = position.x;
        cur_positon_message["position_y"] = position.y;
        cur_positon_message["position_z"] = position.z;

        aodv_node_positon_vector.push_back(cur_positon_message);
    }

    Config::Connect("/NodeList/*/$ns3::MobilityModel/CourseChange",
                    MakeBoundCallback(&CourseChanged));
    for (int i = 1; i <= auv_num + usv_num + uav_num; i++ )
        Simulator::Schedule(Seconds(simStop-0.1), &record_node_pos, nodeAll, i);

    Config::Connect("/NodeList/*/$ns3::Ipv4L3Protocol/Tx",
                MakeCallback(&Ipv4TxTrace));
    Config::Connect("/NodeList/*/$ns3::Ipv4L3Protocol/Rx",
                MakeCallback(&Ipv4RxTrace));

    // Config::Connect("/NodeList/*/$ns3::Ipv4L3Protocol/Tx",
    //             MakeCallback(&Ipv4TxTrace));
    // Config::Connect("/NodeList/*/$ns3::Ipv4L3Protocol/Rx",
    //             MakeCallback(&Ipv4RxTrace));

    final_sim_file << "网络仿真开始" << std::endl;

    // 仿真开始
    std::cout << "-----------Running Simulation-----------\n";
    Simulator::Stop(Seconds(simStop));
    Simulator::Run();
    Simulator::Destroy();


    // TRACE 部分
    aqua_message["Node_positon"] = aqua_node_positon_vector;
    aqua_message["Message"]      = aqua_message_vector;
    aqua_message["sim_time"]     = simStop;
    aqua_message["node_num"]     = auv_num + usv_num + uav_num;
    aodv_message["Node_positon"] = aodv_node_positon_vector;
    aodv_message["Message"] = aodv_message_vector;

    nlohmann::json plot_2d;
    plot_2d["aqua"] = aqua_message;
    plot_2d["aodv"] = aodv_message;
    plot_2d["node_pos_series"] = node_2d_move_vector;
    plot_2d["stream"] = stream_list;
    plot_2d["packet_type"] = packet_type_json;
    plot_2d["log_path"] = logFile.str();

    // 把输入文件的一些信息复制过来
    plot_2d["DEVICE_MESSAGE"] = j["DEVICE_MESSAGE"];
    plot_2d["CHANNEL_MESAGE"] = j["CHANNEL_MESAGE"];

    orchestrator->append_2d_message(plot_2d);
    
    // std::ofstream aqua_message_out("./A-FINISH-2d-plot.json");   
    // aqua_message_out << plot_2d;          //输出json对象hututu信息到文件./new.json中，std::setw(4)用于设置增加打印空格
    // aqua_message_out.close();
    
    std::cout << "save to " << oss.str() << std::endl;
    final_sim_file << "仿真运行结束： 网络运行时间：" << simStop
                   << "s, 网络接收数据包" << statics_total_bit /8<<"Byte\n" << std::endl;
    final_sim_file << " " << std::endl;
    close(fifoFd);
    final_sim_file.close();

    return 0;
}