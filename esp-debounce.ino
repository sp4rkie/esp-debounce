#include "mylcf.h"
#include "myota.h"
#include <Ticker.h>

#ifdef ESP8266
#define GPIO_KEY 4
#else
#define GPIO_KEY 27
#endif

#define DEBOUNCE_MASK   0xff00  // implies 8 equal scans in a row for a key to trigger
#define DEBOUNCE_MASKr  0xfeff  // implies 8 equal scans in a row for a key to trigger
#define DEBOUNCE_RATE        3  // scan rate in ms to scan key state 
#define DEBOUNCE_CNT       100  // max. cnt of debounces ( == debounce timeout): DEBOUNCE_RATE x DEBOUNCE_CNT

#define KEY_NONE      0
#define KEY_RELEASED  1
#define KEY_PRESSED   2

class Button
{
    private:
    public:
        _u8 button;
        _u16 state = ~0;     // init once only in the hope it will keep track always from that point on
        _u8 debounces;
        void 
        begin(_u8 btn) {
            reset();
            button = btn;
            pinMode(button, INPUT_PULLUP);
        }
        void 
        reset() {
//            state = ~0;     // start with GPIO scans all open ( == 1)  // VOID - SEE COMMENT ABOVE
            debounces = 0;  
        }
        _u32 
        debounce() {
            ++debounces;    // debounces run so far
            state = state << 1 | digitalRead(button) | DEBOUNCE_MASK << 1;
            return state == DEBOUNCE_MASK ? KEY_PRESSED : state == DEBOUNCE_MASKr ? KEY_RELEASED : KEY_NONE;
        }
};

Button button;
Ticker timer;
_u8 key_state;

void
scankey()
{
    _u32 k_stat = button.debounce();

//Serial.printf(".%x", button.state);
    if (k_stat != KEY_NONE) {
        key_state = k_stat;
//Serial.printf(">%x", key_state);
    }
    if (k_stat != KEY_NONE || button.debounces > DEBOUNCE_CNT) {
        // reset if either key press/release or debounce timed out
        timer.detach();
        button.reset();
    }
}

void IRAM_ATTR 
keyintr() 
{
    if (!button.debounces) {
        timer.attach_ms(DEBOUNCE_RATE, scankey);
        button.debounce();  // initial read button
    }
}

#define DEBUG
#define TIMEOUT_BETWEEN_KEYS 500
#define TIMEOUT_KEY_PRESS 300
#define TIMEOUT_MIN_KEY_PRESS 1000
#define TIMEOUT_MAX_KEY_PRESS 5000
#define TIMEOUT_TO_BASE_STATE 10000
#define TIMEOUT_WDT           12000

#ifdef DEBUG
#define PR pr
#else
#define PR
#endif

/* inputs for lower/upper left/right bell buttons */
#define GPIO_BUTT_LL   4
#define GPIO_BUTT_LR  17
#define GPIO_BUTT_UL  22
#define GPIO_BUTT_UR  10

/* outputs */
#define GPIO_UDOOR_OPEN  27
#define GPIO_UDOOR_CLOSE 18
#define GPIO_TS4ab        9
#define GPIO_LDOOR       24

#define RESET(why) \
    PR("reset:  due to %s\n", (why), 0, 0, 0, 0); \
    state = EXPECT_1st_PRESS; \
    timeout = TIMEOUT_WDT; \
    active_edge_button = ~0;

#define ERR(a, b) \
    pr("err%02d:  butt==%s\n", (a), buttons[(b)].str, 0, 0, 0); \
    action_bell("error", (b)); \
    RESET("error");

int state;
int timeout;
unsigned active_edge_state;
unsigned active_edge_button = ~0; /* try to remember this for issuing a proper timeout fct */
unsigned diff_state;
struct timeval last_state;
struct timeval now_state;
unsigned diff_poll;
struct timeval last_poll;
struct timeval now_poll;

char *states[] = {

#define EXPECT_1st_PRESS   0
    "1st_PRESS",
#define EXPECT_1st_REL     1
    "1st_REL  ",
#define EXPECT_2nd_PRESS   2
    "2nd_PRESS",
#define EXPECT_2nd_REL     3
    "2nd_REL  ",
#define EXPECT_3rd_PRESS   4
    "3rd_PRESS",
#define EXPECT_3rd_TIMEOUT 5
    "3rd_TIMEOUT",
#define EXPECT_3rd_REL     6
    "3rd_REL  ",
#define EXPECT_4th_PRESS   7
    "4th_PRESS",
#define EXPECT_4th_REL     8
    "4th_REL  ",
#define EXPECT_any_REL     9
    "any_REL  ",
};

struct _buttons {
    int gpio;    /* GPIO port */
    char *str;   /* string for log file */
    char *edge;  /* edge to detect */
} buttons[] = {
{
#define BUTT_LL 0
    GPIO_BUTT_LL,
    "GPIO_BUTT_LL",
//              # lower button on ELCON no longer supports this
//    "both",
    "falling",          /* 74HC14 smith hex inverter */
} , {
#define BUTT_LR 1
    GPIO_BUTT_LR,
    "GPIO_BUTT_LR",
    "falling",          /* 74HC14 smith hex inverter */
} , {
#define BUTT_UL 2
    GPIO_BUTT_UL,
    "GPIO_BUTT_UL",
    "both", 
} , {
#define BUTT_UR 3
    GPIO_BUTT_UR,
    "GPIO_BUTT_UR",
    "falling",          /* 74HC14 smith hex inverter */
}
};

int
pr(char *a, char *b, char *c, char *d, char *e, char *f)
{
    struct timeval tme;
    char buff[256];

    gettimeofday(&tme, 0);
    strftime(buff, 256, "%a %b %d %T", localtime(&tme.tv_sec));
    Serial.printf("%s.%06d ", buff, (int)tme.tv_usec);
    Serial.printf(a, b, c, d, e, f);
    return 0;
}

int
action_bell(char *why, int button)
{
    pr("bell:   due to %s %s\n", why, buttons[button].str, 0, 0, 0);
    return 0;
}

int
action_ldoor(char *why, int mode)
{
    pr("ldoor:  due to %s mode %d\n", why, mode, 0, 0, 0);
    return 0;
}

int
action_udoor(char *why, int mode, int delayed)
{
    pr("udoor:  due to %s mode %d delayed %d\n", why, mode, delayed, 0, 0);
    return 0;
}

/*
 * give t2 - t1 in microseconds if value smaller than about 10s / 0 otherwise
 */
int
usecs_diff(struct timeval *t1, struct timeval *t2)
{
    if (t2->tv_sec >= t1->tv_sec + 10) {
        return 0;
    }
    return (t2->tv_sec - t1->tv_sec) * 1000000 + t2->tv_usec - t1->tv_usec;
}

int
eval_state(int is_timeout, int button)
{
if (!(is_timeout && state == EXPECT_1st_PRESS && timeout == TIMEOUT_WDT && !diff_state)) /* dont log standard WDT */
PR("got:    %s in state %s within %4d real %d\n", 
                is_timeout ? "TIMEOUT" : active_edge_state ? "PRESS  " : "RELEASE",
                states[state], 
                timeout, 
                diff_state / 1000,
                0);

    if (state == EXPECT_1st_PRESS) {
        if (is_timeout) {
#if 0 /* normal since WDT */
            if (active_edge_state) {
                ERR(1, button);
            } else {
                ERR(2, button);
            }
#endif /* normal since WDT */
        } else {
            if (active_edge_state) {
                state = EXPECT_1st_REL;
                timeout = TIMEOUT_KEY_PRESS;
            } else {
                ERR(3, button);
            }
        }
    } else if (state == EXPECT_1st_REL) {
        if (is_timeout) {
            if (active_edge_state) {
                action_bell("still pressed", button);
                state = EXPECT_any_REL;
                timeout = TIMEOUT_TO_BASE_STATE;
            } else {
                ERR(4, button);
            }
        } else {
            if (active_edge_state) {
                ERR(5, button);
            } else {
                state = EXPECT_2nd_PRESS;
                timeout = TIMEOUT_BETWEEN_KEYS;
            }
        }
    } else if (state == EXPECT_any_REL) {
        if (is_timeout) {
            if (active_edge_state) {
                ERR(21, button); /* force reset if key hung */
            } else {
                ERR(22, button); /* force reset if key hung */
            }
        } else {
            if (active_edge_state) {
                ERR(23, button);
            } else {
                RESET("key release");
            }
        }
    } else {
        if (is_timeout) {
            if (active_edge_state) {
                ERR(24, button);
            } else {
                ERR(25, button);
            }
        } else {
            if (active_edge_state) {
                ERR(26, button);
            } else {
                ERR(27, button);
            }
        }
    }
if (!(is_timeout && state == EXPECT_1st_PRESS && timeout == TIMEOUT_WDT && !diff_state)) /* dont log standard WDT */
PR("expct:  %s within %4d\n", states[state], timeout, 0, 0, 0);
    return 0;
}

void 
setup() 
{
    Serial.begin(115200);
    mysetup_intro(__FILE__, 1);
    button.begin(GPIO_KEY);
//    attachInterrupt(digitalPinToInterrupt(GPIO_KEY), keyintr, FALLING);  // GPIO goes low if closed/ active
    attachInterrupt(digitalPinToInterrupt(GPIO_KEY), keyintr, CHANGE);  // must handle both release and press

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    RESET("init");
}

void 
loop() 
{
    static _u32 old_keystate;
    static _u32 old_millis;
    static _u8 ledtoggle;
    _u32 do_something;

    myloop_intro();
    if (key_state != old_keystate) {
        do_something = 1;           // tag key change status
        old_keystate = key_state;
    } else if (millis() > old_millis + timeout) {
        do_something = 2;           // tag timeout
    } else {
        do_something = 0;           // nothing special to do
    }
    if (do_something) {
        gettimeofday(&now_poll, 0);
        diff_poll = usecs_diff(&last_poll, &now_poll);
        last_poll = now_poll;
        if (!diff_poll || diff_poll >= 20000)  {
            ledtoggle = !ledtoggle;
//Serial.printf(".");
        }
    }
    // key activity recorded
    if (do_something == 1) {
        if (BUTT_UL == BUTT_UL) {
            if (active_edge_button == ~0) {
                active_edge_button = BUTT_UL;
            }
            // let this run into a timeout if buttons are different (i.e. ignore different buttons)
            if (active_edge_button == BUTT_UL) {
                active_edge_state = key_state == KEY_PRESSED;
                now_state = now_poll;
                diff_state = usecs_diff(&last_state, &now_state);
                last_state = now_state;
                eval_state(0, BUTT_UL);
            }
        } else {
            action_bell("immediate", BUTT_UL);
        }
    }
    // timeout occurred
    if (do_something == 2) {
        now_state = now_poll;
        diff_state = usecs_diff(&last_state, &now_state);
        last_state = now_state;
        // default to LL if no valid button is avail for timeout (may not happen anyway)
        eval_state(1, active_edge_button == ~0 ? BUTT_LL : active_edge_button);
    }
    if (do_something) {
        old_millis = millis();
    }
}

