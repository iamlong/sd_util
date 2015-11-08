/*************************************************************************
	> File Name: ../src/serialize.cpp
	> Author: 
	> Mail: 
	> Created Time: 2015年11月 8日 10:51:21
 ************************************************************************/

#include<iostream>
#include <include/external/serialize.h>

using namespace std;


namespace util{
    namespace serialization {
        void  serializable::setsig(sd_uint8_t start[3], sd_uint8_t end[3]){
            memcpy(start_sig, start, 3);
            memcpy(end_sig, end, 3);
        }

        bool serializable::checksig(sd_uint8_t start[3], sd_uint8_t end[3]){
            return (memcmp(start, start_sig, 3)==0)&&(memcmp(end, end_sig, 3)==0);
        }
        serializer::serializer(sd_uint8_t start[3], sd_uint8_t end[3]){
            memcpy(m_start_sig, start, 3);
            memcpy(m_end_sig, end, 3);
            m_totalsize = 6 + sizeof(m_totalsize); //include start_sig and end_sig
        }
        sd_uint16_t serializer::push_data(auto origin_data){
            dataElement data;
            data.size = sizeof(origin_data);
            data.buff.reset(new sd_uint8_t[data.size]);
            memcpy(data.buff.get(), &origin_data, data.size);
            m_data_q.push_back(data);
            m_totalsize += data.size+sizeof(data.size);
            return data.size;
        }
        sd_uint16_t serializer::push_data(string origin_data){
            dataElement data;
            data.size = origin_data.size()+1;
            data.buff.reset(new sd_uint8_t[data.size]);
            memcpy(data.buff.get(), origin_data.data(), data.size);
            m_data_q.push_back(data);
            m_totalsize += data.size+sizeof(data.size);
            return data.size;

        }

        sd_uint32_t serializer::push_data(serializable& obj){
            objElement data;
            serializer * newserializer = new serializer(obj.start_sig, obj.end_sig);
            data.size = obj.serializing(newserializer);
            data.obj.reset(&obj);
            m_obj_q.push_back(data);
            m_totalsize += data.size+sizeof(data.size);
            return data.size;
            
        }

        sd_uint32_t serializer::getPersistentSize(){
            return m_totalsize;
        }

        bool serializer::writebuff(sd_uint8_t* buff, sd_uint32_t size){
            if(size<getPersistentSize())
                return false;
            sd_uint8_t * buff_head = buff; 
            for(auto& i: m_data_q ){
                memcpy(buff_head, &i.size, sizeof(i.size));
                buff_head+=sizeof(i.size);
                memcpy(buff_head, i.buff.get(), i.size);
                buff_head +=i.size;
            }
            for(auto&i:m_obj_q){
                memcpy(buff_head, &i.size, sizeof(i.size));
                buff_head += sizeof(i.size);
                if(!i.obj.get()->getSerializer()->writebuff(buff_head, i.size))
                   return false;
                buff_head += i.size;
            }
        }

        

    }
}
