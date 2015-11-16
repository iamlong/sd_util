/*************************************************************************
	> File Name: serialize.h
	> Author:Steve 
	> Mail: longhairknight@hotmail.com
	> Created Time: 2015年11月 8日 10:19:18
 ************************************************************************/

#ifndef _SERIALIZE_H
#define _SERIALIZE_H
#include <include/external/stdtype.h>
#include <memory>
#include <vector>
#include <string.h>
using namespace std;
namespace util {
    namespace serialization {

        class serializer;

        class deserializer;

        class serializable{
            public:
                 virtual sd_uint32_t serializing(serializer * serialObj)=0;
                 virtual bool deserializing(deserializer * deserializeobj)=0;
                 void setsig(sd_uint8_t start[3], sd_uint8_t end[3]);
                 bool checksig(sd_uint8_t start[3], sd_uint8_t end[3]);
                 serializer * getSerializer();

                sd_uint8_t start_sig[3];
                sd_uint8_t end_sig[3];
            private:
                serializer * m_serializer;
                deserializer * m_deserializer;
        };
        struct dataElement{
            sd_uint16_t size;
            shared_ptr<sd_uint8_t> buff;
        };
        struct objElement{
            sd_uint32_t size;
            shared_ptr<serializable> obj;
        };
        class serializer {
            
            public:
                serializer(sd_uint8_t start[3], sd_uint8_t end[3]);
                sd_uint16_t push_data(auto data);
                sd_uint16_t push_data(string data);
                sd_uint32_t push_data(serializable& obj);
                bool writebuff(sd_uint8_t* buff, sd_uint32_t size);
                sd_uint32_t getPersistentSize();

            private:
                sd_uint8_t m_start_sig[3];
                sd_uint8_t m_end_sig[3];
                sd_uint32_t m_totalsize={0}; //totalsize is the total size of the serializable object which use serializer to serializating.
                vector<dataElement> m_data_q;
                vector<objElement> m_obj_q;

        };

        class deserializer{

            public:
                deserializer(sd_uint8_t * inputbuff);
                sd_uint16_t pull_data(auto& outvalue);
                sd_uint16_t pull_data(string& outvalue);
                sd_uint32_t pull_data(serializable & obj);
                setsig(sd_uint8_t start[3], sd_uint8_t end[3]);
            
            private:
                sd_uint8_t m_start_sig[3];
                sd_uint8_t m_end_sig[3];
                sd_uint8_t * m_in_buff;
                sd_uint32_t m_totalsize;
                sd_uint8_t * m_in_buff_end; //ptr to end_sig, it is used to be the end of readbuff;
                vector<dataElement> m_data_q;
                vector<objElement> m_obj_q;

                bool validate();
                bool readbuff();

         };   
    }
}
#endif
