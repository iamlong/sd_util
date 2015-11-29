
#include <gtest/gtest.h>
#include <serialize.hpp>

using namespace util::serialization;

class testsubserial : public serializable {
    public:
        int sub_a;
        int sub_b;

    testsubserial(){
        setsig((sd_uint8_t*)"sub",(sd_uint8_t*)"bus");
    }

    sd_uint32_t serializing( serializer * serialobj ){
        m_serializer = serialobj;
        serialobj->setsig(start_sig, end_sig);
        serialobj->push_data(sub_a);
        serialobj->push_data(sub_b);
        return serialobj->getPersistentSize();
    }

    sd_uint32_t deserializing(deserializer * deserialobj){
        m_deserializer = deserialobj;
        deserialobj->setsig(start_sig, end_sig);
        deserialobj->pull_data(sub_a);
        deserialobj->pull_data(sub_b);
        return deserialobj->getTotalSize();
    }
};

class testSerial : public serializable{
    public:
        int a;
        char b;
        long c;
        double d;
        float e;
        string f;
        testsubserial s;

        using serializable::setsig;
        using serializable::checksig;
        
        testSerial(){
            setsig((sd_uint8_t*)"abc", (sd_uint8_t*)"cba");

        }
        sd_uint32_t serializing(serializer* serialObj){
            serialObj->setsig(start_sig, end_sig);
            serialObj->push_data(a);
            serialObj->push_data(b);
            serialObj->push_data(c);
            serialObj->push_data(d);
            serialObj->push_data(e);
            serialObj->push_data(f);
            serialObj->push_data((serializable&)s);
            return serialObj->getPersistentSize();   
        }
        sd_uint32_t deserializing(deserializer* deserialobj){
            deserialobj->setsig(start_sig, end_sig);
            deserialobj->validate();
            deserialobj->pull_data(a);
            deserialobj->pull_data(b);
            deserialobj->pull_data(c);
            deserialobj->pull_data(d);
            deserialobj->pull_data(e);
            deserialobj->pull_data(f);
            deserialobj->pull_data((serializable&)s);
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
    testobj1->f="my test string here!";
    testobj1->s.sub_a = 10;
    testobj1->s.sub_b = 11;

   serializer ser_a;

   sd_uint32_t size = testobj1->serializing(&ser_a);
   sd_uint8_t * buff = new sd_uint8_t[size];
   ser_a.writebuff(buff, size);
    ser_a.release();
   
   testSerial * testobj2 = new testSerial();
   deserializer deser_a(buff);
  
   testobj2->deserializing(&deser_a);
   deser_a.readbuff();
   ASSERT_EQ(testobj1->a, testobj2->a);
   ASSERT_EQ(testobj1->b, testobj2->b);
   ASSERT_EQ(testobj1->c, testobj2->c);
   ASSERT_EQ(testobj1->d, testobj2->d);
   ASSERT_EQ(testobj1->e, testobj2->e);
   ASSERT_EQ(testobj1->f, testobj2->f);
   ASSERT_EQ(testobj1->s.sub_a, testobj2->s.sub_a);
   ASSERT_EQ(testobj1->s.sub_b, testobj2->s.sub_b);
    
    delete testobj1;
    delete testobj2;
}

int main(int argc, char **argv) {  
  testing::InitGoogleTest(&argc, argv);  
  return RUN_ALL_TESTS();  
}
