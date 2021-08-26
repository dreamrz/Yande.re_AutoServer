#pragma once 

/* Type define */
typedef unsigned char byte;
typedef unsigned int uint32;
typedef unsigned int ulong;

using std::string;
using std::ifstream;

/* MD5 declaration. */
class MD5 {
public:
    MD5();
    MD5(const void* input, size_t length); //以内存段构造 
    MD5(const string& str); //以字符串构造 
    MD5(ifstream& in); //以文件流构造 
    void update(const void* input, size_t length); //内存段添加模式 
    void update(const string& str); //字符串模式 
    void update(ifstream& in); //文件流模式 
    const byte* digest(); //返回md5值 
    string toString(); //返回md5值 
    void reset(); //重新使用时必须重置一次 

private:
    void update(const byte* input, size_t length);
    void final();
    void transform(const byte block[64]);
    void encode(const uint32* input, byte* output, size_t length);
    void decode(const byte* input, uint32* output, size_t length);
    string bytesToHexString(const byte* input, size_t length);

    /* class uncopyable */
    MD5(const MD5&);
    MD5& operator=(const MD5&);

private:
    uint32 _state[4]; /* state (ABCD) */
    uint32 _count[2]; /* number of bits, modulo 2^64 (low-order word first) */
    byte _buffer[64]; /* input buffer */
    byte _digest[16]; /* message digest */
    bool _finished;   /* calculate finished ? */

    static const byte PADDING[64]; /* padding for calculate */
    static const char HEX[16];
    enum { BUFFER_SIZE = 1024 };
};
