/*************************************************************************
	> File Name: ../src/serialize.cpp
	> Author: 
	> Mail: 
	> Created Time: 2015年11月 8日 10:51:21
 ************************************************************************/

#include<iostream>
#include <include/external/serialize.hpp>

using namespace std;


namespace util{
    namespace serialization {
        void  serializable::setsig(sd_uint8_t start[SIG_SIZE], sd_uint8_t end[SIG_SIZE]){
            memcpy(start_sig, start, SIG_SIZE);
            memcpy(end_sig, end, SIG_SIZE);
        }

        bool serializable::checksig(sd_uint8_t start[SIG_SIZE], sd_uint8_t end[SIG_SIZE]){
            return (memcmp(start, start_sig, SIG_SIZE)==0)&&(memcmp(end, end_sig, SIG_SIZE)==0);
        }

        serializer * serializable::getSerializer(){
            return m_serializer;
        }
        serializer::serializer(){
            m_totalsize = 2*SIG_SIZE + sizeof(m_totalsize)+2*Q_SIZE; //include start_sig and end_sig
        }

        void serializer::setsig(sd_uint8_t start[SIG_SIZE], sd_uint8_t end[SIG_SIZE]){
            memcpy(m_start_sig, start, SIG_SIZE);
            memcpy(m_end_sig, end, SIG_SIZE);
        
        }
        sd_uint16_t serializer::push_data(sd_uint8_t* buff, sd_uint16_t size){
            dataElement data;
            data.size = size;
            data.buff=new sd_uint8_t[data.size];
            memcpy(data.buff, buff, data.size);
            m_data_q.push_back(data);
            m_totalsize += data.size+sizeof(data.size);
            return data.size;
        }

        sd_uint16_t deserializer::pull_data(sd_uint8_t * outvalue, sd_uint16_t size){
         
            dataElement data;
            data.size = size;
            data.buff= outvalue;
            m_data_q.push_back(data);
            return data.size;
        }

        sd_uint16_t serializer::push_data(string origin_data){
            dataElement data;
            data.size = origin_data.size();
            data.buff=new sd_uint8_t[data.size];
            memcpy(data.buff, origin_data.data(), data.size);
            m_data_q.push_back(data);
            m_totalsize += data.size+sizeof(data.size);
            return data.size;

        }
        sd_uint16_t deserializer::pull_data(string & outstring){
            dataElement data;
            data.size = 0;
            data.buff=(sd_uint8_t*)&outstring;
            m_data_q.push_back(data);
            return data.size;
        }

        sd_uint32_t serializer::push_data(serializable& obj){
            objElement data;
            serializer * newserializer = new serializer();
            newserializer->setsig(obj.start_sig, obj.end_sig);
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
            memcpy(buff_head, m_start_sig,SIG_SIZE);
            buff_head +=SIG_SIZE;
            memcpy(buff_head, &m_totalsize, sizeof(m_totalsize));
            buff_head += sizeof(m_totalsize);
            
            sd_uint8_t qsize = m_data_q.size();
            memcpy(buff_head, &qsize, sizeof(qsize));
            buff_head += sizeof(qsize);
            for(auto& i: m_data_q ){
                memcpy(buff_head, &i.size, sizeof(i.size));
                buff_head+=sizeof(i.size);
                memcpy(buff_head, i.buff, i.size);
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
            memcpy(buff_head, m_end_sig, SIG_SIZE);
            return true;
        }

        void serializer:: release(){
            for(auto& i: m_data_q)
                delete i.buff;
            for(auto& i: m_obj_q)
                i.obj.reset();
        }


       deserializer::deserializer(sd_uint8_t* inputbuff){
           m_in_buff = inputbuff;
       }

       void deserializer::setsig(sd_uint8_t start[SIG_SIZE], sd_uint8_t end[SIG_SIZE]){
         
           memcpy(m_start_sig, start, SIG_SIZE);
           memcpy(m_end_sig, end, SIG_SIZE);
      
       }

       sd_uint32_t deserializer::getTotalSize(){
           
           return m_totalsize;

       }

       bool deserializer::validate(){
           sd_uint8_t sig[SIG_SIZE];
           sd_uint8_t * buff_head = m_in_buff;
           if(memcmp(buff_head, m_start_sig, SIG_SIZE)!=0)
             return false;
       
           buff_head += SIG_SIZE;
           memcpy(&m_totalsize, buff_head, sizeof(m_totalsize));
           m_in_buff_end = buff_head +=m_totalsize-2*SIG_SIZE;

           if(memcmp(buff_head, m_end_sig, SIG_SIZE)!=0)
             return false;
        
           return true;

       }

       bool deserializer::readbuff(){
         
           if(!validate())
            return false;

           sd_uint8_t * buff_head = m_in_buff+SIG_SIZE+sizeof(m_totalsize);
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
                   string * str = (string*) (data.buff);
                   str->append((char*) temp, (size_t)data.size);
                   delete temp;
               }else {
                buff_head += sizeof(data.size);
                memcpy(data.buff, buff_head, data.size);
               }
               buff_head += data.size;
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
