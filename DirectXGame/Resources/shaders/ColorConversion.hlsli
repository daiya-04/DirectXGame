
float32_t3 RGBToHSV(float32_t3 rgb){

    float32_t r = rgb.x;
    float32_t g = rgb.y;
    float32_t b = rgb.z;

    float32_t max = r > g ? r : g;
    max = max > b ? max : b;

    float32_t min = r < g ? r : g;
    min = min < b ? min : b;

    float32_t h = max - min;

    if(h > 0.0f){
        if(max == r){
            h = (g - b) / h;
            if(h < 0.0f){
                h += 6.0f;
            }
        }else if(max == g){
            h = 2.0f + (b - r) / h;
        }else if(max == b){
            h = 4.0f + (r - g) / h;
        }
    }
    h /= 6.0f;

    float32_t s = max - min;
    if(max != 0.0f){
        s /= max;
    }
    float32_t v = max;

    return float32_t3(h,s,v);
}

float32_t3 HSVToRGB(float32_t3 hsv){
    float32_t h = hsv.x * 360.0f;
    float32_t s = hsv.y;
    float32_t v = hsv.z;

    float32_t c = v * s;
    float32_t x = c * (1.0f - abs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float32_t m = v - c;

    float32_t3 rgb;

    if(h >= 0.0f && h < 60.0f){
        rgb = float32_t3(c, x, 0.0f);
    }else if(h >= 60.0f && h < 120.0f){
        rgb = float32_t3(x, c, 0.0f);
    }else if(h >= 120.0f && h < 180.0f){
        rgb = float32_t3(0.0f, c, x);
    }else if(h >= 180.0f && h < 240.0f){
        rgb = float32_t3(0.0f, x, c);
    }else if(h >= 240.0f && h < 300.0f){
        rgb = float32_t3(x, 0.0f, c);
    }else if(h >= 300.0f && h <360.0f){
        rgb = float32_t3(c, 0.0f, x);
    }

    rgb += float32_t3(m, m, m);

    return rgb;
}

