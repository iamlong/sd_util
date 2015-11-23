
#include <gtest/gtest.h>
#include <serialize.hpp>

using namespace util::serialization;

class testSerial : public serializable{
    public:
        int a;
        char b;
        long c;
        double d;
        float e;

        using serializable::setsig;
        using serializable::checksig;
        
        testserial(){
            sd_uint8_t start[]="abc";
            sd_uint8_t end[]="cba";
            setsig(start, end);

        }
        sd_uint32_t serializing(serializer* serialObj){
            
            serialObj->push_data(a);
            serialObj->push_data(b);
            serialObj->push_data(c);
            serialObj->push_data(d);
            serialObj->push_data(e);
            return serialObj->getPersistentSize();   
        }
        sd_uint32_t deserializing(deserializer* deserialobj){
            //deserialobj->setsig(start_sig, end_sig);
            deserialobj->setsig((sd_uint8_t*)"abc",(sd_uint8_t*)"cba");
            deserialobj->validate();
            deserialobj->pull_data(a);
            deserialobj->pull_data(b);
            deserialobj->pull_data(c);
            deserialobj->pull_data(d);
            deserialobj->pull_data(e);
            return deserialobj->getTotalSize();
  
        }
        
};

TEST(sERIALIZETEST, PRIMARYDATA){
   testSerial * testobj1=new testSerial();

   testobj1->a = 10;
   testobj1->b = 'b';
   testobj1->c = 20;
   testobj1->d = 30;

   testobj1->e = 10.10;

   serializer ser_a;
   ser_a.setsig((sd_uint8_t*)"abc", (sd_uint8_t*)"cba");

   sd_uint32_t size = testobj1->serializing(&ser_a);
   sd_uint8_t * buff = new sd_uint8_t[size];
   ser_a.writebuff(buff, size);
    ser_a.release();
   
   testSerial * testobj2 = new testSerial();
   deserializer deser_a(buff);
  
    deser_a.setsig((sd_uint8_t*)"abc", (sd_uint8_t*)"cba");
   testobj2->deserializing(&deser_a);
   deser_a.readbuff();
    deser_a.release();
   ASSERT_EQ(testobj1->a, testobj2->a);
   ASSERT_EQ(testobj1->b, testobj2->b);
   ASSERT_EQ(testobj1->c, testobj2->c);
   ASSERT_EQ(testobj1->d, testobj2->d);
   ASSERT_EQ(testobj1->e, testobj2->e);
    
    delete testobj1;
    delete testobj2;
}

int main(int argc, char **argv) {  
  testing::InitGoogleTest(&argc, argv);  
  return RUN_ALL_TESTS();  
}
