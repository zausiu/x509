////////////////////////////////////////////
// http://blog.ykyi.net/ presents ...
// created on Apr. 22 2018
//////////////////////////////////////////////

#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <stdexcept>
#include <vector>
#include <iostream>
#include "CuttleImpl.h"
#include "spdlog/fmt/fmt.h"
#include "json/json.h"
#include "ZooVisitor.h"
#include "SnazzyLogger.h"
#include "NeatUtilities.h"

CuttleImpl::CuttleImpl()
{
}

CuttleImpl::~CuttleImpl()
{
        delete crt_verifier_;
}

void CuttleImpl::init(unsigned short port, const char* ca_crt_path)
{
        struct sockaddr_in server_addr;

        server_fd_ = socket(AF_INET , SOCK_STREAM , 0);
        if (server_fd_ == -1)
        {
                throw std::runtime_error("Failed to create socket fd.");
        }

        // Prepare the sockaddr_in structure
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if(bind(server_fd_, (struct sockaddr *)&server_addr, 
                                sizeof(server_addr)) < 0)                       
        {
                std::string err_msg = fmt::format("Failed to bind server fd to 0.0.0.0:{}", port);
                throw std::runtime_error(err_msg); 
        } 

        int ret = listen(server_fd_ , 16);                                       
        if (ret != 0)
        {
                std::string err_msg = fmt::format("Failed to call listen.");
                throw std::runtime_error(err_msg);
        } 

        LOG->info("Listening on 0.0.0.0:{}", port);

        crt_verifier_ = new X509Verifier(ca_crt_path); 
}

void CuttleImpl::run()
{
        int fd;
        struct sockaddr_in addr;
        socklen_t addr_len;
        while (true)
        {
            fd = accept(server_fd_, (struct sockaddr*)(&addr), &addr_len);
            if (fd < 0)
            {
                    LOG->error("accept returns a error");
                    continue;
            }

            uint64_t client_ip = addr.sin_addr.s_addr;
            char ip_str[32];
            inet_ntop(addr.sin_family, &client_ip, ip_str, sizeof(ip_str));
            unsigned short client_port = addr.sin_port;
            LOG->info("accepted a connection from {}:{}.", ip_str, client_port);

            int json_len;
            int ret = recv(fd, &json_len, sizeof(json_len), MSG_WAITALL);
            if (ret < sizeof(json_len))
            {
                    LOG->error("Failed to read the first 4 bytes from client's request.");
                    close(fd);
                    continue;
            }

            std::vector<char> json_buff(json_len);
            char* json_data = json_buff.data();
            ret = recv(fd, json_data, json_len, MSG_WAITALL);
            if (ret < json_len)
            {
                    LOG->error("{} bytes has been received from client, less than expected {} bytes.", ret, json_len);
                    close(fd);
                    continue;
            }

            Json::Value jv_root;
            Json::Reader reader;
            if (reader.parse(json_data, jv_root, false))
            {
                    LOG->error("Failed to parse received json text.\n");
                    close(fd);
                    continue;
            }

            const std::string& crt_as_str = jv_root["crt"].asString();
            X509Crt client_crt(crt_as_str.data(), crt_as_str.size());
            bool legal = crt_verifier_->is_legal(client_crt.get_x509_crt());
            if (!legal)
            {
                    LOG->error("Occurred illegal certificate.");
                    continue;
            }

            long serial = client_crt.get_serial();
            // TODO retrieve info attached to serial and check permission

            int year = jv_root["year"].asInt();
            int month = jv_root["month"].asInt();
            int day = jv_root["day"].asInt();
            int hour = jv_root["hour"].asInt();

            const std::string& date = fmt::format("{0:4}-{1:02}-{2:02}", year, month, day);
            const Json::Value& jv_keyNsalt = zoo_visitor_.GetZnodeData(date, hour);
            
            const std::string& key_b64 = jv_keyNsalt["key"].asString();
            const std::string& salt_b64 = jv_keyNsalt["salt"].asString();
            Json::Value jv_resp; 
            jv_resp["year"] = year;
            jv_resp["month"] = month;
            jv_resp["day"] = day;
            jv_resp["hour"] = hour;
            jv_resp["key"] = client_crt.ceal_text_base64(key_b64.data(), key_b64.size());
            jv_resp["salt"] = client_crt.ceal_text_base64(salt_b64.data(), salt_b64.size());

            Json::FastWriter fastWriter;
            std::string resp_text = fastWriter.write(jv_resp);
            int resp_text_len = resp_text.size();
            std::vector<char> whole_resp(sizeof(resp_text_len) + resp_text_len);
            char* resp_data = whole_resp.data();
            int resp_data_len = whole_resp.size();
            memcpy(resp_data, &resp_data_len, sizeof(resp_data_len));
            memcpy(resp_data + sizeof(resp_data_len), resp_text.data(), resp_text_len);
            ret = send_all(fd, resp_data, resp_data_len);
            if (ret < 0)
            {
                    LOG->error("Failed to send response to client.\n");
            }
            close(fd);
        }
}
