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

kb_key_t key;

typedef struct {
    int buyingpower;
    int value, prevvalue;
    int high, low, volatility;

    //know if player is buying or selling when entering trademenu
    bool sell;

    uint8_t scl;

    //print stock with big percent change
    //uint8_t news;
    int day;
    //bool wsb;

    //after thirty days unlock crypto currencies?
    //dogecoin, litecoin, bitcoin, ethereum
} player_t;
player_t player;

typedef struct {
    int initialprice, prevprice, currentprice;

    //candlesticks
    int prevclose;
    int close, open, high, low; 
    uint8_t volatility, tempshares;
    int average;
    unsigned int shares;

    int percentchange;
} stock_t;
stock_t stocks[5];

void tradeMenu(void);
void endGame(void);

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

void RedText(void) {
    gfx_SetTextBGColor(0);
    gfx_SetTextFGColor(224);
    gfx_SetTextTransparentColor(0);
}

void GreText(void) {
    gfx_SetTextBGColor(0);
    gfx_SetTextFGColor(4);
    gfx_SetTextTransparentColor(0);
}

void startMenu() {
    uint8_t idx, y,height;
    int prevy;
    gfx_SetDrawScreen();
    WhiText();
    gfx_SetTextScale(2,2);
    gfx_PrintStringXY("WSB STOCK SIMULATOR",160 - gfx_GetStringWidth("WSB STOCK SIMULATOR") / 2,20);
    gfx_SetTextScale(1,1);
    //if user has played before, print high score?
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

        prevy = y;
        delay(50);
    }
    do {
        kb_Scan();
    } while(!(kb_Data[6] & kb_Enter));

    //this doesn't work - need a way to disable keyboard interrupts
    delay(200);
}

void loadPrices() {
    uint8_t idx;
    for (idx = 0; idx < 5; idx++) {
        stock_t* stock = &(stocks[idx]);
        //65,4,12,55,10
        if (idx == 0) {
            stock->initialprice = 65;
            stock->volatility = 15;
        } else if (idx == 1) {
            stock->initialprice = 4;
            //change volatility to a float for this stock?
            stock->volatility = 1;
        } else if (idx == 2) {
            stock->initialprice = 12;
            stock->volatility = 3;
        } else if (idx == 3) {
            stock->initialprice = 55;
            stock->volatility = 7;
        } else if (idx == 4) {
            stock->initialprice = 10;
            stock->volatility = 2;
        }

        stock->close = stock->initialprice;
        stock->prevprice = stock->initialprice;
        stock->currentprice = stock->initialprice;
        
    }
    player.scl = 25;
    player.buyingpower = 1000;
}

void updatePrices() {
    uint8_t idx;
    player.prevvalue = player.value;
    player.value = player.buyingpower;
    player.high = player.buyingpower;
    player.low = player.buyingpower;
    for (idx = 0; idx < 5; idx++) {
        stock_t* stock = &(stocks[idx]);
 
        stock->close = stock->prevprice + randInt(-stock->volatility,stock->volatility);
        stock->open = stock->prevprice;

        if (stock->close > stock->open) {
            stock->high = stock->close + (stock->close / 3);
            stock->low = stock->open - (stock->open / 3);
        } else {
            stock->high = stock->open + (stock->open / 3);
            stock->low = stock->close - (stock->close / 3);
        }

        stock->currentprice = stock->close;
        stock->prevprice = stock->open;

        player.high += stock->high;
        player.low -= stock->low;

        player.value += stock->currentprice * stock->shares;
    }
    
}

void viewPortfolio() {
    int numLoops = 0;
    uint8_t day = 0;
    bool canQuit,promptEnd, trade = 0;

    gfx_ZeroScreen();
    WhiText();
    gfx_SetTextScale(2,2);
    gfx_PrintStringXY("Investing",20,20);
    gfx_PrintStringXY("$",20,50);
    
    gfx_PrintInt(player.value,1);

    WhiText();
    gfx_SetTextScale(1,1);
    gfx_PrintStringXY("Buying Power: $",20,222);
    gfx_PrintInt(player.buyingpower,1);

    GreText();
    gfx_PrintStringXY("Buy: +",228, 222);
    RedText();
    gfx_PrintStringXY("Sell: -",278, 222);

    gfx_SetColor(255);
    gfx_HorizLine(0,210,320);
    gfx_VertLine(270,210,30);
    gfx_VertLine(220,210,30);
    do {
        kb_Scan();
        numLoops++;

        if (numLoops % 1000 == 0 && day < 30) {
            player.day++;
            day++;
            updatePrices();
            
            if (player.value >= player.prevvalue) {
                gfx_SetColor(14);
            } else {
                gfx_SetColor(128);
            }
            
            gfx_Line(10+10*(day - 1),160 - (player.prevvalue / player.scl),10+10*day,160 - (player.value / player.scl));

            BlaText();
            gfx_SetTextScale(1,1);
            gfx_PrintStringXY("Day: ",320 - gfx_GetStringWidth("Day: 30  "),20);
            gfx_PrintInt(player.day - 1,1);
        
            gfx_SetTextScale(2,2);
            
            gfx_PrintStringXY("$",20,50);
            gfx_PrintInt(player.prevvalue,1);
            
            WhiText();
            gfx_PrintStringXY("$",20,50);

            if (player.value > 1000) {
                GreText();
            } else if (player.value == 1000) {
                WhiText();
            } else {
                RedText();
            }
            gfx_PrintInt(player.value,1);
            WhiText();
            gfx_SetTextScale(1,1);
            gfx_PrintStringXY("Day: ",320 - gfx_GetStringWidth("Day: 30  "),20);
            gfx_PrintInt(player.day,1);

            if (player.day + 1 == 30) {
                promptEnd = true;
                canQuit = true;
            }
        }
       
        //can quit variable here
        if (kb_Data[6] & kb_Add || kb_Data[6] & kb_Sub) {
            trade = true;
            canQuit = true;
            if (kb_Data[6] & kb_Add) {
                player.sell = false;
            } else if (kb_Data[6] & kb_Sub) {
                player.sell = true;
            }
            tradeMenu();
        }

        if (kb_Data[6] & kb_Clear) {
            canQuit = true;
            trade = false;
        }


    } while (canQuit != true);

    if (trade == true) {
        //gfx_SwapDraw();
        delay(200);
        tradeMenu();
    } else if (promptEnd == true) {
        delay(200);
        endGame();
    }
}

void tradeMenu() {
    uint8_t idx;
    int cost = 0;
    uint8_t menuSelected = 0;
    bool canQuit = false;
    gfx_SetDrawScreen();
    gfx_ZeroScreen();
    WhiText();
    gfx_SetTextScale(2,2);
    gfx_PrintStringXY("WSB STOCK EXCHANGE",160 - gfx_GetStringWidth("WSB STOCK EXCHANGE") / 2,20);
    
    YelText();
    gfx_SetTextScale(1,1);
    if (player.sell == true) {
        gfx_PrintStringXY("Press ENTER to SELL.",20,220);
    } else {
        gfx_PrintStringXY("Press ENTER to BUY.",20,220);
    }
    
    gfx_PrintStringXY("GME",20,55);
    gfx_PrintStringXY("NOK",20,85);
    gfx_PrintStringXY("BB",20,115);
    gfx_PrintStringXY("NIO",20,145);
    gfx_PrintStringXY("AMC",20,175);

    //gray text
    //possibly replace aapl with dogecoin
    WhiText();
    gfx_PrintStringXY("Gamestop",20,70);
    gfx_PrintStringXY("Nokia",20,100);
    gfx_PrintStringXY("Blackberry",20,130);
    gfx_PrintStringXY("NIO",20,160);
    gfx_PrintStringXY("AMC Entertainment",20,190);

    gfx_PrintStringXY("Buying Power: ",20,210);
    gfx_PrintInt(player.buyingpower,1);

    gfx_SetColor(255);
    gfx_HorizLine(0,200,320);
    gfx_PrintStringXY("Cost: $",320 - gfx_GetStringWidth("Cost: $xxxxxx"),210);
    gfx_PrintStringXY("Total: $",320 - gfx_GetStringWidth("Total: $xxxxxx"),220);

    //prices
    gfx_PrintStringXY("$", 280 - gfx_GetStringWidth("$00"),55);
    gfx_PrintInt(stocks[0].close,1);
    gfx_PrintStringXY("$", 280 - gfx_GetStringWidth("$00"),85);
    gfx_PrintInt(stocks[1].close,1);
    gfx_PrintStringXY("$",280 - gfx_GetStringWidth("$00"),115);
    gfx_PrintInt(stocks[2].close,1);
    gfx_PrintStringXY("$",280 - gfx_GetStringWidth("$00"),145);
    gfx_PrintInt(stocks[3].close,1);
    gfx_PrintStringXY("$",280 - gfx_GetStringWidth("$00"),175);
    gfx_PrintInt(stocks[4].close,1);

    YelText();
    gfx_PrintStringXY("+/-",280 - gfx_GetStringWidth("000"),67 + 30*menuSelected);

    if (player.sell == true) {
        for (idx = 0; idx < 5; idx++) {
            stocks[idx].tempshares = stocks[idx].shares;
            cost -= stocks[idx].tempshares * stocks[idx].close;
        }
    }
    //shares number
    WhiText();
    //tempshares if buying?
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

    //outline menu options
    gfx_SetColor(231);
    gfx_Rectangle(0,50,320,30);

    do {
        kb_Scan();
        gfx_SetTextScale(1,1);

        if (kb_Data[7] & kb_Down && menuSelected != 4) {
            BlaText();
            gfx_PrintStringXY("+/-",280 - gfx_GetStringWidth("000"),67 + 30*menuSelected);
            gfx_SetColor(0);
            gfx_Rectangle(0,50+ 30* menuSelected,320,30);
            menuSelected++;
            delay(150);
            YelText();
            gfx_PrintStringXY("+/-",280 - gfx_GetStringWidth("000"),67 + 30*menuSelected);
            gfx_SetColor(231);
            gfx_Rectangle(0,50+ 30* menuSelected,320,30);
        } else if (kb_Data[7] & kb_Up && menuSelected != 0) {
            BlaText();
            gfx_PrintStringXY("+/-",280 - gfx_GetStringWidth("000"),67 + 30*menuSelected);
            gfx_SetColor(0);
            gfx_Rectangle(0,50+ 30* menuSelected,320,30);
            menuSelected--;
            delay(150);
            YelText();
            gfx_PrintStringXY("+/-",280 - gfx_GetStringWidth("000"),67 + 30*menuSelected);
            gfx_SetColor(231);
            gfx_Rectangle(0,50+ 30* menuSelected,320,30);
        }
        
        if (kb_Data[6] & kb_Add || kb_Data[6] & kb_Sub) {
            BlaText();
            gfx_PrintStringXY("",320 - gfx_GetStringWidth("000"),55 + 30*menuSelected);
            gfx_PrintInt(stocks[menuSelected].tempshares,1);

            //block out cost 
            gfx_SetColor(0);
            gfx_FillRectangle(260,210,50,20);

            if (kb_Data[6] & kb_Add) {
                stocks[menuSelected].tempshares++;
                if (player.sell == false) {
                    cost += stocks[menuSelected].close;
                } else {
                    cost -= stocks[menuSelected].close;
                }    
            } else if (kb_Data[6] & kb_Sub && stocks[menuSelected].tempshares > 0) {
                stocks[menuSelected].tempshares--;
    
                if (player.sell == false) {
                    cost -= stocks[menuSelected].close;
                } else {
                    cost += stocks[menuSelected].close;
                }
            }
            delay(150);
            WhiText();
            gfx_PrintStringXY("",320 - gfx_GetStringWidth("000"),55 + 30*menuSelected);
            gfx_PrintInt(stocks[menuSelected].tempshares,1);

            gfx_PrintStringXY("Cost: $",320 - gfx_GetStringWidth("Cost: $xxxxxx"),210);
            gfx_PrintInt(cost,1);
            gfx_PrintStringXY("Total: $",320 - gfx_GetStringWidth("Total: $xxxxxx"),220);
            gfx_PrintInt(player.buyingpower - cost,1);
        }

        if (kb_Data[6] & kb_Enter) {
            if (player.sell == false) {
                if (player.buyingpower - cost >= 0) {
                    canQuit = true;
                }
            } else {
                for (idx = 0; idx < 5; idx++) {
                    if (stocks[idx].tempshares <= stocks[idx].shares) {
                        canQuit = true;
                    }
                }
            }
        }
    } while(canQuit != true);
    
    player.buyingpower -= cost;
    player.value = 0;

    for (idx = 0; idx < 5; idx++) {
        if (player.sell == false) {
            player.value += stocks[idx].tempshares*stocks[idx].close;
            stocks[idx].average = (stocks[idx].close * stocks[idx].tempshares + stocks[idx].shares * stocks[idx].average) / (stocks[idx].tempshares + stocks[idx].shares);
            stocks[idx].shares += stocks[idx].tempshares;
        } else {
            //doesnt work well
            stocks[idx].average = (stocks[idx].average * stocks[idx].shares - (stocks[idx].tempshares * stocks[idx].close)) / (stocks[idx].shares - stocks[idx].tempshares);
            stocks[idx].shares -= stocks[idx].tempshares;
        }
        stocks[idx].tempshares = 0;
    }

    player.value += player.buyingpower;
    player.sell = false;
    
    viewPortfolio();
}

//display after 30 days, give option to return to portfolio
void endGame() {
    bool canQuit = false;
    bool cont = false;
    uint8_t idx,best,worst = 0;
    gfx_ZeroScreen();
    for (idx = 0; idx < 5; idx++) {
        stock_t* stock = &(stocks[idx]);
        stock->percentchange = stock->initialprice / stock->close;
        if (stock->percentchange > stocks[best].percentchange) best = idx;

        if (idx == 0) {
            worst = idx;
        } else if (stock->percentchange < stocks[worst].percentchange) {
            worst = idx;
        }
    }
    YelText();
    gfx_SetTextScale(2,2);
    gfx_PrintStringXY("CONGRATULATIONS!",160 - gfx_GetStringWidth("CONGRATULATIONS!") / 2,20);
    gfx_PrintStringXY("You Made: $",20,50);
    gfx_PrintInt(player.value - 1000,1);
    gfx_SetTextScale(1,1);
    WhiText();

    //can be optimized to print string
    gfx_PrintStringXY("Best Stock: ",20,80);
    if (best == 0) {
        gfx_PrintStringXY("GME",20 + gfx_GetStringWidth("Best Stock: "),80);
    } else if (best == 1) {
        gfx_PrintStringXY("NOK",20 + gfx_GetStringWidth("Best Stock: "),80);
    } else if (best == 2) {
        gfx_PrintStringXY("BB",20 + gfx_GetStringWidth("Best Stock: "),80);
    } else if (best == 3) {
        gfx_PrintStringXY("NIO",20 + gfx_GetStringWidth("Best Stock: "),80);
    } else if (best == 4) {
        gfx_PrintStringXY("AMC",20 + gfx_GetStringWidth("Best Stock: "),80);
    }

    gfx_PrintStringXY("Worst Stock: ",20,100);
    if (worst == 0) {
        gfx_PrintStringXY("GME",20 + gfx_GetStringWidth("Worst Stock: "),100);
    } else if (worst == 1) {
        gfx_PrintStringXY("NOK",20 + gfx_GetStringWidth("Worst Stock: "),100);
    } else if (worst == 2) {
        gfx_PrintStringXY("BB",20 + gfx_GetStringWidth("Worst Stock: "),100);
    } else if (worst == 3) {
        gfx_PrintStringXY("NIO",20 + gfx_GetStringWidth("Worst Stock: "),100);
    } else if (worst == 4) {
        gfx_PrintStringXY("AMC",20 + gfx_GetStringWidth("Worst Stock: "),100);
    }
    
    gfx_PrintStringXY("Press ENTER to Continue",160 - gfx_GetStringWidth("Press ENTER to Continue") / 2,190);
    gfx_PrintStringXY("Press CLEAR to Quit",160 - gfx_GetStringWidth("Press CLEAR to Quit") / 2,210);

    do {
        kb_Scan();
        if (kb_Data[6] & kb_Clear) {
            canQuit = true;
            cont = false;
        } else if (kb_Data[6] & kb_Enter) {
            canQuit = true;
            cont = true;
        }
    } while (canQuit != true);
    
    //viewportfolio doesn't work here because clear triggers exit
    if (cont == true) {
        delay(200);
        tradeMenu();
    } else {
        delay(200);
        //maybe save stats here
    }
}

void main(void) {
    gfx_Begin();
    srand(rtc_Time());
    //or load data
    loadPrices();
    gfx_ZeroScreen();
    startMenu();
    tradeMenu();
    gfx_End();
}