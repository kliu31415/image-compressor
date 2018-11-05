namespace algo1{
BitSeq encode1(unsigned w, unsigned h, uint8_t *r, uint8_t *g, uint8_t *b)
{
    BitSeq data;
    data.write_back(MAGIC_FILEID_NUMBER | 0x0101);
    data.write_back(h);
    data.write_back(w);
    unsigned pos = 0;
    for(unsigned i=0; i<h; i++)
    {
        for(unsigned j=0; j<w; j++, pos++)
        {
            data.write_back(r[pos]);
            data.write_back(g[pos]);
            data.write_back(b[pos]);
        }
    }
    return data;
}
void decode1(BitSeq &data, unsigned &w, unsigned &h, uint8_t *&r, uint8_t *&g, uint8_t *&b)
{
    h = data.readUnsigned();
    w = data.readUnsigned();
    r = new uint8_t[w*h];
    g = new uint8_t[w*h];
    b = new uint8_t[w*h];
    unsigned pos = 0;
    for(unsigned i=0; i<h; i++)
    {
        for(unsigned j=0; j<w; j++, pos++)
        {
            r[pos] = data.readByte();
            g[pos] = data.readByte();
            b[pos] = data.readByte();
        }
    }
}
}
