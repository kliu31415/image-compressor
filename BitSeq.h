struct BitSeq
{
    vector<bool> data;
    unsigned pos;
    BitSeq()
    {
        pos = 0;
    }
    void readFromFile(const char *name)
    {
        ifstream fin(name, ios::binary);
        if(fin.fail())
            cerr << "failed to open file " << name << endl;
        unsigned sz;
        fin.read(reinterpret_cast<char*>(&sz), 4);
        uint8_t *dat = new uint8_t[(sz+7)/8];
        fin.read(reinterpret_cast<char*>(dat), (sz+7)/8);
        data.resize(sz);
        for(unsigned i=0; i<sz; i++)
            data[i] = ((dat[i/8] >> (7-(i%8))) & 1);
        fin.close();
        delete[] dat;
    }
    BitSeq(const char *name)
    {
        readFromFile(name);
        pos = 0;
    }
    decltype(data[0]) operator[](unsigned idx)
    {
        return data[idx];
    }
    decltype(data[0]) back()
    {
        return data.back();
    }
    unsigned size() const
    {
        return data.size();
    }
    void push_back(bool v)
    {
        data.push_back(v);
    }
    void write_back(unsigned v)
    {
        for(int i=31; i>=0; i--)
        {
            data.push_back((v >> i) & 1);
        }
    }
    void write_back(int v)
    {
        for(int i=31; i>=0; i--)
        {
            data.push_back(((*reinterpret_cast<unsigned*>(&v) >> i) & 1));
        }
    }
    void write_back(uint8_t v)
    {
        for(int i=7; i>=0; i--)
        {
            data.push_back((v>>i) & 1);
        }
    }
    void write_nibble(uint8_t v)
    {
        for(int i=3; i>=0; i--)
        {
            data.push_back((v>>i) & 1);
        }
    }
    void write_bits(unsigned v, int bits)
    {
        for(int i=bits-1; i>=0; i--)
        {
            data.push_back((v>>i) & 1);
        }
    }
    void write_back(const char *x)
    {
        int i = 0;
        while(true)
        {
            if(x[i] == '0')
                data.push_back(0);
            else if(x[i] == '1')
                data.push_back(1);
            else break;
            i++;
        }
    }
    void write_back(string x)
    {
        push_back(x.c_str());
    }
    void pop_back()
    {
        data.pop_back();
    }
    void clear()
    {
        data.clear();
    }
    unsigned readUnsigned()
    {
        unsigned v = 0;
        for(int i=31; i>=0; i--)
            v += data[pos++] * (1u<<i);
        return v;
    }
    int readInt()
    {
        unsigned v = readUnsigned();
        return *reinterpret_cast<int*>(&v);
    }
    uint8_t readByte()
    {
        uint8_t v = 0;
        for(int i=7; i>=0; i--)
            v += data[pos++] * (1<<i);
        return v;
    }
    unsigned readBits(int bits)
    {
        unsigned v = 0;
        for(int i=bits-1; i>=0; i--)
            v += data[pos++] * (1<<i);
        return v;
    }
    uint8_t readNibble()
    {
        uint8_t v = 0;
        for(int i=3; i>=0; i--)
            v += data[pos++] * (1<<i);
        return v;
    }
    bool readBit()
    {
        return data[pos++];
    }
    string toString(unsigned a = 0, unsigned len = 0)
    {
        if(len == 0)
            len = data.size() - a;
        string res;
        res.reserve(len);
        for(unsigned i=a; i<a+len; i++)
        {
            if(data[i] == 0)
                res += '0';
            else res += '1';
        }
        return res;
    }
    void writeToFile(const char *name)
    {
        remove(name);
        ofstream fout(name, ios::binary);
        unsigned sz = data.size();
        fout.write(reinterpret_cast<char*>(&sz), 4);
        while(data.size() % 8 != 0)
            push_back(0);
        uint8_t *dat = new uint8_t[(sz+7)/8];
        memset(dat, 0, (sz+7)/8);
        for(unsigned i=0; i<sz; i++)
            dat[i/8] += ((int)data[i]) << (7 - (i%8));
        fout.write(reinterpret_cast<char*>(dat), (sz+7)/8);
        fout.close();
        delete[] dat;
    }
};
