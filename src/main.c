/*
 *--------------------------------------
 * Program Name: WSBSTONKS
 * Author: Everyday Code
 * License:
 * Description: Stock simulator for the TI-84 Plus CE.
 *--------------------------------------
*/

#include <math.h>
#include <tice.h>
#include <keypadc.h>
#include <graphx.h>
#include <fileioc.h>

//loading screen - you have $10k to start - can you press any key to continue
//Everyday Stock Exchange 
//Most Popular - $gme, $amc, $msft, $aapl, $nio, $tsla, $amzn, $bac, $twtr, $aal 
//optional stats
//+/- next to each with current price, yellow box outline, 5 stocks per page - scroll
//sub total at the bottom and confirm

//takes you to portfolio page 
//buying power - money left over
//button at bottom to buy or sell - sell is same as buying

//kb_graph to draw line graph

typedef struct {
    int buyingpower;
} player_t;

typedef struct {
    int initialprice;
} stock_t;

typedef struct {
    //shouldn't be price high or low
    //should be variance. high and low is (y +/- variance / 2)
    int close, open;
    int price, high, low,variance;
    //open, close is top of rectangle
    int x;
} prices_t;

//which stock
prices_t prices[32];

void BlaText(void) {
    gfx_SetTextBGColor(255);
    gfx_SetTextFGColor(0);
    gfx_SetTextTransparentColor(255);
}

void WhiText(void) {
    gfx_SetTextBGColor(0);
    gfx_SetTextFGColor(255);
    gfx_SetTextTransparentColor(0);
}

void generatePrices() {
    uint8_t idx;
    //only for one stock - needs to go in struct
    uint8_t initialprice = 40;
    uint8_t volatility = 20;
    for (idx = 0; idx < 31; idx++) {
        //gfx_SetDrawScreen();
        
        prices_t* price = &(prices[idx]);
        prices_t prev = prices[idx - 1];
        price->variance = randInt(1,10);
        if (idx > 0) {
            //randint +/- volatility
            //make sure close is not equal to open - creates 0 candle stick
            if (prev.close + randInt(-volatility,volatility) == 0) {
                price->close = prev.close + randInt(-volatility,volatility) + 5;
            } else {
                price->close = prev.close + randInt(-volatility,volatility);
            }

            price->open = prev.close;
            
        } else {
            price->close = initialprice;
            price->open = initialprice + randInt(-volatility,volatility);
        }

        //if prev price is higher, close is lower on candlestick
        if (price->close > prev.close || idx == 0) {
            price->high = price->close + (price->variance / 2);
            price->low = price->open - (price->variance / 2);
        } else {
            price->high = price->open + (price->variance / 2);
            price->low = price->close - (price->variance / 2);
        }
        
        if (idx == 0) {
            price->x = 5;
        } else {
            price->x = prev.x + 10;
        }

        //test live generation
        /*WhiText();
        gfx_SetTextScale(2,2);
        gfx_PrintStringXY("Gamestop",20,20);
        gfx_SetTextScale(1,1);
        gfx_SetColor(0);
        gfx_FillRectangle(20,40,30,30);
        gfx_PrintStringXY("$",20,40);
        gfx_PrintInt(prices[idx].price,1);
        gfx_SetColor(255);
        gfx_VertLine(price->x + 2, price->low - (price->variance/2) + 120, price->variance*2);
        if (idx > 0) {
            //if the low finishes below previous low, draw red
            prices_t prev = prices[idx - 1];
            if (price->low > prev.low) {
                gfx_SetColor(128);
            } else {
                gfx_SetColor(204);
            }
        } else {
            gfx_SetColor(204);
        }
        gfx_FillRectangle(price->x,120 - (price->price - (price->variance/2)),5,price->variance);
        //x,y,width,height
        //gfx_FillRectangle(price->x,(price->low - (price->variance/2)) + 120,5,price->variance);
        delay(100);*/
    }
}

void drawStockStats() {
    WhiText();
    gfx_SetTextScale(2,2);
    gfx_PrintStringXY("Gamestop",20,20);
    gfx_SetTextScale(1,1);
    gfx_PrintStringXY("$",20,40);
    gfx_PrintInt(prices[30].close,1);
    gfx_PrintStringXY("High: $",20,50);
    gfx_PrintInt(prices[30].high,1);
    gfx_PrintStringXY("Low: $",20,60);
    gfx_PrintInt(prices[30].low,1);
}

//to do line graph do gfx_Line(price.x, price.price, prev.x, prev.y)
void drawCandles() {
    int idx;
    //used for candle wick
    //uint8_t temprand;
    for (idx = 0; idx < 31; idx++) {
        prices_t price = prices[idx];
        prices_t next = prices[idx + 1];
        gfx_SetColor(255);
        //causes candle stick to dance
        //temprand = randInt(1,5);
        //gfx_VertLine(price.x + 2, price.low - (price.variance/2) + 120, price.variance*2);
        gfx_VertLine(price.x + 2, 240 - price.high, price.high - price.low);

        if (idx > 0) {
            //if the low finishes below previous low, draw red
            prices_t prev = prices[idx - 1];
            if (price.close >= prev.close) {
                gfx_SetColor(14);
                gfx_FillRectangle(price.x,240 - price.close,5,price.close - price.open);
            } else {
                gfx_SetColor(128);
                gfx_FillRectangle(price.x,240 - price.open,5,price.open - price.close);
            }
        } else {
            gfx_SetColor(14);
            gfx_FillRectangle(price.x,240 - price.close,5,price.close - price.open);
        }
        gfx_SetColor(255);
        if (idx != 30) {
            gfx_Line(price.x,240 - price.close,next.x, 240 - next.close);
        }
        
        WhiText();
        //gfx_PrintStringXY("",price.x, price.low + 100);
        //gfx_PrintInt(price.price,1);
        
        //delay(150);
        //gfx_FillRectangle(price.x,price.price + 120,5,price.high - price.low);
    }    
}

void main(void) {
    gfx_Begin();
    
    srand(rtc_Time());
    gfx_ZeroScreen();
    generatePrices();

    do {
        kb_Scan();
        gfx_SetDrawBuffer();
        gfx_ZeroScreen();
        
        drawStockStats();
        drawCandles();
        gfx_BlitBuffer();
    } while(kb_Data[6] != kb_Clear);

    gfx_End();
}
