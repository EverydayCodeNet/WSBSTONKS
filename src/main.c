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

kb_key_t key;
kb_lkey_t lkey;

typedef struct {
    int buyingpower;

    //stocks owned
} player_t;

typedef struct {
    int initialprice;
    uint8_t volatility, tempshares;
    int amount, average;
    
    //percent change - no need to store 
} stock_t;
stock_t stocks[10];


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

void YelText(void) {
    gfx_SetTextBGColor(0);
    gfx_SetTextFGColor(229);
    gfx_SetTextTransparentColor(0);
}

void startMenu() {
    uint8_t idx, y,height;
    int prevy;
    gfx_SetDrawScreen();
    WhiText();
    gfx_SetTextScale(2,2);
    gfx_PrintStringXY("STOCK SIMULATOR",160 - gfx_GetStringWidth("STOCK SIMULATOR") / 2,20);
    gfx_SetTextScale(1,1);
    //if user has played before, print high score
    gfx_PrintStringXY("You have $1,000 to spend.",160 - gfx_GetStringWidth("You have $1,000 to spend.")/ 2,50);
    gfx_PrintStringXY("Press ENTER to Continue...",160 - gfx_GetStringWidth("Press ENTER to Continue...") / 2,210);

    //previous high score - maybe 1 month time frame
    for (idx = 0; idx < 31; idx++) {
        y = prevy + randInt(-5,5);
        height = randInt(2,25);
        gfx_SetColor(255);
        gfx_VertLine((5 + 10*idx) + 2, 320 - y - (height/2),height*2);
        if (y > prevy) {
            gfx_SetColor(14);
        } else {
            gfx_SetColor(128);
        }
        gfx_FillRectangle(5 + 10*idx,320 - y,5,height);
        //gfx_FillRectangle(x,240 - y,5,height);

        prevy = y;
        delay(50);
    }
    do {
        kb_Scan();
    } while(!(kb_Data[6] & kb_Enter));
    delay(200);
}

void loadPrices() {
    uint8_t idx;
    for (idx = 0; idx < 5; idx++) {
        stock_t* stock = &(stocks[idx]);
        //65,4,135,55,10
        if (idx == 0) {
            stock->initialprice = 65;
        } else if (idx == 1) {
            stock->initialprice = 4;
        } else if (idx == 2) {
            stock->initialprice = 135;
        } else if (idx == 3) {
            stock->initialprice = 55;
        } else if (idx == 4) {
            stock->initialprice = 10;
        }
    }
}

void viewPortfolio() {
    gfx_ZeroScreen();
    WhiText();
    gfx_PrintStringXY("Investing",20,20);
}

void buyMenu() {
    uint8_t idx;
    int cost = 0;
    uint8_t menuSelected = 0;
    gfx_ZeroScreen();
    WhiText();
    gfx_SetTextScale(2,2);
    gfx_PrintStringXY("WSB STOCK EXCHANGE",160 - gfx_GetStringWidth("WSB STOCK EXCHANGE") / 2,20);
    
    gfx_SetTextScale(1,1);
    gfx_PrintStringXY("GME",20,55);
    gfx_PrintStringXY("NOK",20,85);
    gfx_PrintStringXY("AAPL",20,115);
    gfx_PrintStringXY("NIO",20,145);
    gfx_PrintStringXY("AMC",20,175);
    //gfx_PrintStringXY("MSFT",20,205);
    //gfx_PrintStringXY("AMZN",20,235);
    /*gfx_PrintStringXY("TWTR",20,190);
    gfx_PrintStringXY("AAL",20,210);
    gfx_PrintStringXY("TSLA",20,230);*/

    //three stocks per page
    //gray text
    //possibly replace aapl with dogecoin
    //gme, nok, aapl, nio, amc
    gfx_PrintStringXY("Gamestop",20,70);
    gfx_PrintStringXY("Nokia",20,100);
    gfx_PrintStringXY("Apple",20,130);
    gfx_PrintStringXY("NIO",20,160);
    gfx_PrintStringXY("AMC Entertainment",20,190);
    //gfx_PrintStringXY("Microsoft",20,220);
    //gfx_PrintStringXY("Nokia",20,250);
    /*gfx_PrintStringXY("Twitter",20,200);
    gfx_PrintStringXY("American Airlines",20,220);
    gfx_PrintStringXY("Tesla",20,240);*/

    
    gfx_HorizLine(0,200,320);
    gfx_PrintStringXY("Cost: $",320 - gfx_GetStringWidth("Cost: $xxxx"),210);
    gfx_PrintStringXY("Total: $",320 - gfx_GetStringWidth("Total: $xxxx"),220);

    //prices
    gfx_PrintStringXY("$65", 280 - gfx_GetStringWidth("$00"),55 );
    //gfx_PrintStringXY("",320 - gfx_GetStringWidth("000"),55 + 30*menuSelected);
    //gfx_PrintInt(stocks[menuSelected].tempshares,1);
    gfx_PrintStringXY("$4", 280 - gfx_GetStringWidth("$00"),85);
    gfx_PrintStringXY("$135",280 - gfx_GetStringWidth("$00"),115);
    gfx_PrintStringXY("$55",280 - gfx_GetStringWidth("$00"),145);
    gfx_PrintStringXY("$10",280 - gfx_GetStringWidth("$00"),175);

    YelText();
    gfx_PrintStringXY("+/-",280 - gfx_GetStringWidth("000"),70 + 30*menuSelected);
    //shares number
    WhiText();
    gfx_PrintStringXY("",320 - gfx_GetStringWidth("000"),55);
    gfx_PrintInt(stocks[0].tempshares,1);
    gfx_PrintStringXY("",320 - gfx_GetStringWidth("000"),85);
    gfx_PrintInt(stocks[1].tempshares,1);
    gfx_PrintStringXY("",320 - gfx_GetStringWidth("000"),115);
    gfx_PrintInt(stocks[2].tempshares,1);
    gfx_PrintStringXY("",320 - gfx_GetStringWidth("000"),145);
    gfx_PrintInt(stocks[3].tempshares,1);
    gfx_PrintStringXY("",320 - gfx_GetStringWidth("000"),175);
    gfx_PrintInt(stocks[4].tempshares,1);


    gfx_SetColor(231);
    gfx_Rectangle(0,50,320,30);
    //Most Popular - $gme, $msft, $aapl, $nio, $amc, $amzn, $nok, $twtr, $aal, $tsla
    do {
        kb_Scan();
        gfx_SetTextScale(1,1);
        //needs to be cut down
        if (kb_Data[7] & kb_Down && menuSelected != 4) {
            BlaText();
            gfx_PrintStringXY("+/-",280 - gfx_GetStringWidth("000"),70 + 30*menuSelected);
            gfx_SetColor(0);
            gfx_Rectangle(0,50+ 30* menuSelected,320,30);
            menuSelected++;
            delay(150);
            YelText();
            gfx_PrintStringXY("+/-",280 - gfx_GetStringWidth("000"),70 + 30*menuSelected);
            gfx_SetColor(231);
            gfx_Rectangle(0,50+ 30* menuSelected,320,30);
        } else if (kb_Data[7] & kb_Up && menuSelected != 0) {
            BlaText();
            gfx_PrintStringXY("+/-",280 - gfx_GetStringWidth("000"),70 + 30*menuSelected);
            gfx_SetColor(0);
            gfx_Rectangle(0,50+ 30* menuSelected,320,30);
            menuSelected--;
            delay(150);
            YelText();
            gfx_PrintStringXY("+/-",280 - gfx_GetStringWidth("000"),70 + 30*menuSelected);
            gfx_SetColor(231);
            gfx_Rectangle(0,50+ 30* menuSelected,320,30);
        }
        
        if (kb_Data[6] & kb_Add || kb_Data[6] & kb_Sub) {
            BlaText();
            gfx_PrintStringXY("",320 - gfx_GetStringWidth("000"),55 + 30*menuSelected);
            gfx_PrintInt(stocks[menuSelected].tempshares,1);

            //block out cost 
            gfx_SetColor(0);
            gfx_FillRectangle(280,210,50,20);

            /*gfx_PrintStringXY("Cost: $",320 - gfx_GetStringWidth("Cost: $xxxxx"),210);
            gfx_PrintInt(cost,1);
            gfx_PrintStringXY("Total: $",320 - gfx_GetStringWidth("Total: $xxxxx"),220);
            gfx_PrintInt(1000 - cost,1);*/

            if (kb_Data[6] & kb_Add) {
                stocks[menuSelected].tempshares++;
                cost += stocks[menuSelected].initialprice;
                delay(150);
                
            } else if (kb_Data[6] & kb_Sub && stocks[menuSelected].tempshares > 0) {
                stocks[menuSelected].tempshares--;
                cost -= stocks[menuSelected].initialprice;
                delay(150);
            }
            WhiText();
            gfx_PrintStringXY("",320 - gfx_GetStringWidth("000"),55 + 30*menuSelected);
            gfx_PrintInt(stocks[menuSelected].tempshares,1);

            /*for (idx = 0; idx < 6; idx++) {
                cost += stocks[idx].initialprice * stocks[idx].tempshares;
            }*/
            gfx_PrintStringXY("Cost: $",320 - gfx_GetStringWidth("Cost: $xxxx"),210);
            gfx_PrintInt(cost,1);
            gfx_PrintStringXY("Total: $",320 - gfx_GetStringWidth("Total: $xxxx"),220);
            gfx_PrintInt(1000 - cost,1);
            //cost = 0;
        }

        
        //change to canquit, must buy 
    } while(kb_Data[6] != kb_Enter);
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

void drawCandles() {
    uint8_t idx;
    for (idx = 0; idx < 31; idx++) {
        prices_t price = prices[idx];
        prices_t next = prices[idx + 1];
        gfx_SetColor(255);
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
    }    
}

void main(void) {
    gfx_Begin();
    
    srand(rtc_Time());

    //or load data
    loadPrices();

    gfx_ZeroScreen();
    startMenu();
    //sell menu should be the same thing
    buyMenu();
    viewPortfolio();

    //generates prices for one stock
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