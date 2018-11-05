namespace algo4{
void encodeRuns(BitSeq &data, unsigned w, unsigned h, uint8_t *x)
{
    int RUN_LENGTH = sqrt(quality) + 1;
    for(unsigned i=0; i<h; i++)
    {
        for(unsigned j=0; j<w; j+=RUN_LENGTH)
        {
            if((i ^ j) & 0xf < 8)
                RUN_LENGTH--;
            int avg = 0;
            for(unsigned k=j; k<min(w, j+RUN_LENGTH); k++)
                avg += x[i*w + k];
            avg /= RUN_LENGTH;
            data.write_back((uint8_t)avg);
            int sumX = RUN_LENGTH * (RUN_LENGTH + 1) / 2;
            int sumX2 = (RUN_LENGTH) * (RUN_LENGTH+1) * (2*RUN_LENGTH + 1) / 6;
            int numerator1 = 0, numerator2 = 0;
            int denominator = RUN_LENGTH * sumX2 - sumX * sumX;
            for(unsigned k=j; k<min(w, j+RUN_LENGTH); k++)
            {
                numerator1 += x[i*w + k] - avg;
                numerator2 += (k - j) * (x[i*w + k] - avg);
            }
            numerator1 *= sumX2;
            numerator2 *= sumX;
            if(denominator == 0)
            {
                data.write_back("000000");
                continue;
            }
            double slope = (numerator1 - numerator2) / (double)denominator;
            if(slope == 0)
            {
                data.write_back("000000");
                continue;
            }
            if(slope < 0)
            {
                data.write_back("0");
                slope = -slope;
            }
            else data.write_back("1");
            int l2 = log2(slope) * 10 + 2;
            uint8_t val = min(31, max(1, l2));
            data.write_bits(val, 5);
            if((i ^ j) & 0xf < 8)
                RUN_LENGTH++;
        }
        if(i & 1)
            RUN_LENGTH++;
        else RUN_LENGTH--;
    }
}
BitSeq encode4(unsigned w, unsigned h, uint8_t *r, uint8_t *g, uint8_t *b)
{
    BitSeq data;
    data.write_back(MAGIC_FILEID_NUMBER | 0x0401);
    data.write_back(h);
    data.write_back(w);
    data.write_back(quality);
    encodeRuns(data, w, h, r);
    encodeRuns(data, w, h, g);
    encodeRuns(data, w, h, b);
    return data;
}
void decodeRuns(BitSeq &data, unsigned w, unsigned h, uint8_t q, uint8_t *x)
{
    int RUN_LENGTH = sqrt(q) + 1;
    uint8_t *t = new uint8_t[w];
    for(unsigned i=0; i<h; i++)
    {
        for(unsigned j=0; j<w; j+=RUN_LENGTH)
        {
            if((i ^ j) & 0xf < 8)
                RUN_LENGTH--;
            int avg = data.readByte();
            bool sign = data.readBit();
            double slope = data.readBits(5);
            if(slope != 0)
            {
                slope = pow(2, (slope - 2) / 10);
                if(sign == 0)
                    slope = -slope;
            }
            for(unsigned k=j; k<min(w, j+RUN_LENGTH); k++)
                x[i*w + k] = min(255, max(0, (int)(avg + slope * (k - (RUN_LENGTH-1)/2.0 - j))));
            if((i ^ j) & 0xf < 8)
                RUN_LENGTH++;
        }
        if(w >= 2)
        {
            for(unsigned j=1; j<w-1; j++)
                t[j] = (x[i*w + j - 1] + 2*x[i*w + j] + x[i*w + j + 1]) / 4;
            memcpy(x + i*w + 1, t, w - 2);
        }
        if(i & 1)
            RUN_LENGTH++;
        else RUN_LENGTH--;
    }
    delete[] t;
}
void decode4(BitSeq &data, unsigned &w, unsigned &h, uint8_t *&r, uint8_t *&g, uint8_t *&b)
{
    h = data.readUnsigned();
    w = data.readUnsigned();
    uint8_t q = data.readByte();
    cout << "quality = " << (int)q << endl;
    r = new uint8_t[w*h];
    g = new uint8_t[w*h];
    b = new uint8_t[w*h];
    decodeRuns(data, w, h, q, r);
    decodeRuns(data, w, h, q, g);
    decodeRuns(data, w, h, q, b);
}
}
