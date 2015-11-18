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

        sd_uint16_t deserializer::pull_data(auto& outvalue){
         
            dataElement data;
            data.size = sizeof(outvalue);
            data.buff.reset(outvalue);
            m_data_q.push_back(data);
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
        sd_uint16_t deserializer::pull_data(string & outstring){
            dataElement data;
            data.size = 0;
            data.buff.reset((sd_uint8_t*)&outstring);
            m_data_q.push_back(data);
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
        sd_uint32_t deserializer::pull_data(serializable & obj){
            objElement data;
            data.size = 0;
            data.obj.reset(&obj);
            m_obj_q.push_back(data);
            return data.size;
        }

        sd_uint32_t serializer::getPersistentSize(){
            return m_totalsize;
        }

        bool serializer::writebuff(sd_uint8_t* buff, sd_uint32_t size){
            if(size<getPersistentSize())
                return false;
            sd_uint8_t * buff_head = buff;
            memcpy(buff_head, m_start_sig,3);
            buff_head +=3;
            memcpy(buff_head, &m_totalsize, sizeof(m_totalsize));
            sd_uint8_t qsize = m_data_q.size();
            memcpy(buff_head, &qsize, sizeof(qsize));
            buff_head += sizeof(qsize);
            for(auto& i: m_data_q ){
                memcpy(buff_head, &i.size, sizeof(i.size));
                buff_head+=sizeof(i.size);
                memcpy(buff_head, i.buff.get(), i.size);
                buff_head +=i.size;
            }

            qsize = m_obj_q.size();
            memcpy(buff_head, &qsize, sizeof(qsize));
            buff_head += sizeof(qsize);

            for(auto&i:m_obj_q){
                memcpy(buff_head, &i.size, sizeof(i.size));
                buff_head += sizeof(i.size);
                if(!i.obj.get()->getSerializer()->writebuff(buff_head, i.size))
                   return false;
                buff_head += i.size;
            }
            memcpy(buff_head, m_end_sig, 3);
            return true;
        }

       deserializer::deserializer(sd_uint8_t* inputbuff){
           m_in_buff = inputbuff;
       }

       deserializer::setsig(sd_uint8_t start[3], sd_uint8_t end[3]){
         
           memcpy(m_start_sig, start, 3);
           memcpy(m_end_sig, end, 3);
      
       }

       bool deserializer::validate(){
           sd_uint8_t sig[3];
           sd_uint8_t * buff_head = m_in_buff;
           if(memcmp(buff_head, m_start_sig, 3)!=0)
             return false;
       
           buff_head += 3;
           memcpy(&m_totalsize, buff_head, sizeof(m_totalsize));
           m_in_buff_end = buff_head +=m_totalsize-6;

           if(memcmp(buff_head, m_end_sig, 3)!=0)
             return false;
        
           return true;

       }

       bool deserializer::readbuff(){
         
           if(!validate())
            return false;

           sd_uint8_t * buff_head = m_in_buff+3+sizeof(m_totalsize);
           sd_uint8_t * buff_end = m_in_buff_end;

           sd_uint8_t qsize;
           memcpy(&qsize, buff_head, sizeof(qsize));
           buff_head += sizeof(qsize);
           
           if(qsize!=m_data_q.size()) //number of data element should match the number of datas want to pull;
               return false;

           for(sd_uint8_t i; i<qsize; i++){
               dataElement data = m_data_q[i];
               sd_uint16_t data_size;
               memcpy(&data_size, buff_head, sizeof(data_size));
               if(data_size!=data.size&&data.size!=0) //data_size should match
                   return false;
               else if(data.size==0){ //this is a string
                   data.size = data_size;
                   sd_uint8_t * temp = new sd_uint8_t[data.size];
                   buff_head += sizeof(data.size);
                   memcpy(temp, buff_head, data.size);
                   string * str = (string*) (data.buff.get());
                   str->append((char*) temp, (size_t)data.size);
                   delete temp;
               }else {
                buff_head += sizeof(data.size);
                memcpy(data.buff.get(), buff_head, data.size);
               }
               buff_head += data.size;
               m_data_q.push_back(data);
               if(buff_head>buff_end&&i<qsize)
                return false;
           }
            
           memcpy(&qsize, buff_head, sizeof(qsize));
           if(qsize!=m_obj_q.size())
               return false;

           buff_head += sizeof(qsize);
           

           for(sd_uint8_t i=0; i<qsize; i++){
            deserializer deserial(buff_head);
            m_obj_q[i].size = m_obj_q[i].obj->deserializing(&deserial);
            if(m_obj_q[i].size==0)
                return false;
            buff_head += m_obj_q[i].size;

           }

           return true;
           

       }
    }
}
