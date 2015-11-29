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

        #define SIG_SIZE 4
        #define Q_SIZE 1
        class serializable{
            friend serializer;
            friend deserializer;
            public:
                 sd_uint8_t start_sig[SIG_SIZE];
                 sd_uint8_t end_sig[SIG_SIZE];
                
                 virtual sd_uint32_t serializing(serializer * serialObj)=0;
                 virtual sd_uint32_t deserializing(deserializer * deserializeobj)=0;
                 void setsig(sd_uint8_t start[SIG_SIZE], sd_uint8_t end[SIG_SIZE]);
                 bool checksig(sd_uint8_t start[SIG_SIZE], sd_uint8_t end[SIG_SIZE]);
                 serializer * getSerializer();
                 deserializer * getDeserializer();

            protected:
                 serializer * m_serializer;
                 deserializer * m_deserializer;
        };
        struct dataElement{
            sd_uint16_t size;
            sd_uint8_t * buff;
        };
        struct objElement{
            sd_uint32_t size;
            serializable* obj;
        };
        

        class serializer {
         
            public:
                serializer();
                void setsig(sd_uint8_t start[SIG_SIZE], sd_uint8_t end[SIG_SIZE]);
                sd_uint16_t push_data(sd_uint8_t * buff, sd_uint16_t size);
                sd_uint16_t push_data(auto origin_data){
                    return push_data((sd_uint8_t*)&origin_data, sizeof(origin_data));
                }
                sd_uint16_t push_data(string origin_data);
                sd_uint32_t push_data(serializable& obj);
                bool writebuff(sd_uint8_t* buff, sd_uint32_t size);
                sd_uint32_t getPersistentSize();

                void release();

            private:
                sd_uint8_t m_start_sig[SIG_SIZE];
                sd_uint8_t m_end_sig[SIG_SIZE];
                sd_uint32_t m_totalsize={0}; //totalsize is the total size of the serializable object which use serializer to serializating.
                vector<dataElement> m_data_q;
                vector<objElement> m_obj_q;

        };

        class deserializer{

            public:
                deserializer(sd_uint8_t * inputbuff);
                sd_uint16_t pull_data(sd_uint8_t* outvalue, sd_uint16_t size);
                sd_uint16_t pull_data(auto& outvalue){
                    return pull_data((sd_uint8_t*)&outvalue, sizeof(outvalue));
                }
                sd_uint16_t pull_data(string& outvalue);
                sd_uint32_t pull_data(serializable & obj);
                sd_uint32_t getTotalSize();
                void setsig(sd_uint8_t start[SIG_SIZE], sd_uint8_t end[SIG_SIZE]);
                bool validate();
                bool readbuff();

                void release();
            protected:
                sd_uint8_t m_start_sig[SIG_SIZE];
                sd_uint8_t m_end_sig[SIG_SIZE];
                sd_uint8_t * m_in_buff;
                sd_uint32_t m_totalsize;
                sd_uint8_t * m_in_buff_end; //ptr to end_sig, it is used to be the end of readbuff;
                vector<dataElement> m_data_q;
                vector<objElement> m_obj_q;


         };   
    }
}
#endif
