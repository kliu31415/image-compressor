void display(string s)
{
    unsigned w, h;
    uint8_t *r, *g, *b;
    BitSeq data(s.c_str());
    cout << "encoded information contains " << data.size() << " bits" << endl;
    unsigned encoder = data.readUnsigned();
    if((encoder & 0xffff0000u) != MAGIC_FILEID_NUMBER)
    {
        cerr << "warning: magic number indicating filetype does not match!\n";
        cerr << "expected 0x" << hex << (MAGIC_FILEID_NUMBER>>16) << ", but found 0x" << (encoder>>16) << dec << endl;
    }
    switch(encoder & 0xffff)
    {
    case 0x0101u:
        cout << "using bitmap decoder" << endl;
        algo1::decode1(data, w, h, r, g, b);
        break;
    case 0x0201u:
        cout << "using 1D run decoder" << endl;
        algo2::decode2(data, w, h, r, g, b);
        break;
    case 0x0301u:
        cout << "using smart 1D run decoder" << endl;
        algo3::decode3(data, w, h, r, g, b);
        break;
    case 0x0401u:
        cout << "using smart 1D DCT" << endl;
        algo4::decode4(data, w, h, r, g, b);
        break;
    default:
        cerr << "error: no decoder found for encoding id 0x" << hex << encoder << dec << endl;
    }
    cout << fixed << setprecision(5) << "compression ratio: " << 100LL * data.size() / (double)(24*w*h + 2*8*sizeof(unsigned)) << "%" << endl;
    cout << "width = " << w << "  height = " << h << endl;
    unsigned window_w = w*scale, window_h = h*scale;
    SDL_Window *window = SDL_CreateWindow("image", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_w, window_h, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //drawing points to a texture is slower
    /*SDL_Texture *t = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, t);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    unsigned pos = 0;
    for(int i=0; i<h; i++)
    {
        for(int j=0; j<w; j++, pos++)
        {
            SDL_SetRenderDrawColor(renderer, r[pos], g[pos], b[pos], 255);
            SDL_RenderDrawPoint(renderer, j, i);
        }
    }
    SDL_SetRenderTarget(renderer, NULL);*/
    SDL_Surface *surf = SDL_CreateRGBSurface(0, w, h, 24, 0, 0, 0, 0);
    unsigned pos = 0;
    for(unsigned i=0; i<h; i++) //3 loops is empirically faster than 1 loop (due to 2 way set associative L1 cache?)
    {
        for(unsigned j=0; j<w; j++, pos++)
        {
            unsigned &pixel = *(unsigned*)(surf->pixels + i * surf->pitch + j * surf->format->BytesPerPixel);
            pixel = ((r[pos] >> surf->format->Rloss) << surf->format->Rshift);
        }
    }
    pos = 0;
    for(unsigned i=0; i<h; i++)
    {
        for(unsigned j=0; j<w; j++, pos++)
        {
            unsigned &pixel = *(unsigned*)(surf->pixels + i * surf->pitch + j * surf->format->BytesPerPixel);
            pixel |= ((g[pos] >> surf->format->Gloss) << surf->format->Gshift);
        }
    }
    pos = 0;
    for(unsigned i=0; i<h; i++)
    {
        for(unsigned j=0; j<w; j++, pos++)
        {
            unsigned &pixel = *(unsigned*)(surf->pixels + i * surf->pitch + j * surf->format->BytesPerPixel);
            pixel |= ((b[pos] >> surf->format->Bloss) << surf->format->Bshift);
        }
    }
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    SDL_RenderCopy(renderer, t, NULL, NULL);
    SDL_DestroyTexture(t);
    SDL_Event input;
    bool ret = false;
    while(!ret)
    {
        while(SDL_PollEvent(&input))
        {
            if(input.type == SDL_QUIT)
            {
                ret = true;
                break;
            }
        }
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    data.clear();
    delete[] r;
    delete[] g;
    delete[] b;
}
void readImagePixels(string name, unsigned &w, unsigned &h, uint8_t *&r, uint8_t *&g, uint8_t *&b)
{
    SDL_Surface *s = IMG_Load(name.c_str());
    if(s == NULL)
        cerr << "IMG_GetError(): " << IMG_GetError() << endl;
    h = s->h;
    w = s->w;
    r = new uint8_t[w*h];
    g = new uint8_t[w*h];
    b = new uint8_t[w*h];
    unsigned pos = 0;
    for(unsigned i=0; i<h; i++)
    {
        for(unsigned j=0; j<w; j++, pos++)
        {
            unsigned pixel = *(unsigned*)(s->pixels + i*s->pitch + j*s->format->BytesPerPixel);
            r[pos] = (((pixel & s->format->Rmask) >> s->format->Rshift) << s->format->Rloss);
            g[pos] = (((pixel & s->format->Gmask) >> s->format->Gshift) << s->format->Gloss);
            b[pos] = (((pixel & s->format->Bmask) >> s->format->Bshift) << s->format->Bloss);
        }
    }
    SDL_FreeSurface(s);
}
void encode(string x, BitSeq (*encodeFunc)(unsigned, unsigned, uint8_t*, uint8_t*, uint8_t*))
{
    uint8_t *r, *g, *b;
    unsigned w, h;
    readImagePixels(x, w, h, r, g, b);
    BitSeq data = encodeFunc(w, h, r, g, b);
    cout << "width = " << w << "  height = " << h << "\n";
    cout << "encoded information contains " << data.size() << " bits" << "\n";
    cout << fixed << setprecision(5) << "compression ratio: " << 100LL * data.size() / (double)(24*w*h + 2*8*sizeof(unsigned)) << "%" << endl;
    for(unsigned i=x.size()-1; i>0; i--)
    {
        if(x[i] == '.') //change file extension
        {
            x.resize(i+1);
            x += "abc";
            break;
        }
        else if(i == x.size()-1)
        {
            x += ".abc";
        }
    }
    data.writeToFile(x.c_str());
    delete[] r;
    delete[] g;
    delete[] b;
    data.clear();
}
