#include "consolecontrol.h"
#include "fuwautils.h"
#include "errorwrapper.h"

static HANDLE hconout = 0; // da coconut
static COORD virtualcursor = {0, 0}; //cursorpos gets transformed into this variable and onlx needs to be incremented unless the end of a box is reached or a new line or it stays the same if a scroll box moves this is where text gets printed to the screen in the input loop if all checks pass.
//maybe we need a variable to keep track of input focus!
CHAR_INFO * outputbuf = 0;
COORD consolesize = {0, 0};
typedef struct {
    SMALL_RECT coords;
    inputfieldstyle_t type;
    TCHAR * buf;
    int cursorpos;
} rects_t;
rects_t *rects = 0, **pinrects = 0, **poutrects = 0;
static short bottomrectheight = 0; //the lowest line that a text box is on
static short cinrects = 0, coutrects = 0;
static int ctotalconsolecharacters = 0; //consolesize squared
static int cmaxconsolecharacters = 0; //largest console can be resized to

static BOOL bitmapoutofdate = 1;
static int8_t * bitmap;
static int8_t * tempmap;
struct {
    int32_t valid;
    COORD pos;
} * pinklist; //pink was the color of the pixels when i was drawing to invent the algorithm
typedef enum {EMPTY = 0, FILLED, GREY, POSSIBLE_CORNER, USED_CORNER} pixeltype;

void (*initializeConsoleBuffersError)(icbe_t value) = NULL;
icbe_t initializeConsoleBuffers(HANDLE hhconout, COORD sizee){
    hconout = hhconout;
    icbe_t errortype = CONSOLE_ALREADY_INITIALIZED;
    static void * jumponce = &&initialize;
    goto *jumponce;
    initialize:
    COORD size = GetLargestConsoleWindowSize(hhconout);
    cmaxconsolecharacters = size.X*size.Y;
    outputbuf = (CHAR_INFO *)calloc(cmaxconsolecharacters, sizeof(CHAR_INFO));
    rects = (rects_t *)calloc(cmaxconsolecharacters * 2, sizeof(rects_t));
    pinrects = (rects_t **)calloc(cmaxconsolecharacters, sizeof(rects_t **));
    poutrects = (rects_t **)calloc(cmaxconsolecharacters, sizeof(rects_t **));
    consolesize.X = sizee.X; consolesize.Y = sizee.Y;
    ctotalconsolecharacters = sizee.X * sizee.Y;
    bitmapoutofdate = 1;
    if(!((int64_t)outputbuf & (int64_t)rects)){
        errortype = CALLOC_FAILED;
        goto alreadyinitialized;
    }
    jumponce = &&alreadyinitialized;
    return CONSOLE_INITIALIZED_SUCCESS;

    alreadyinitialized: //it's literally right here
    if(!initializeConsoleBuffersError){
            initializeConsoleBuffersError(errortype);
    }
    return errortype;
}
/*
    the printing method is this:
    We already have all text boxes in the char_info array
    so everything gets printed automatically but
    boxes are in their own malloc'd buffers as well.
    scrolling boxes reprint by first skipping the length of the
    scroll position and then getting characters, if there is a
    newline and we have lines left, print whitspace for the rest
    of the line, move down a line and skip the length of the
    scroll position in the buffer. If we reach the end of the box
    and have no more room, skip to the next newline and interpret
    it as normal. Scroll boxes also malloc space for scroll
    position which is 1 int.
    When we are taking input is when we have to reprint boxes.
    boxes decide cursor postion when you click into them,
    if you move into them using ctrl+arrow keys it restores the saved
    cursor position.
    When you type on the end of line in a scroll box it scrolls to
    the right, on a solid box it moves down to the next line.
    A newline instantly moves down to the next line.
    Cursor position is relative to the box, not the console.
    When you type into a box it reprints the line you type
    on only.
    we first have to define the new rect in
*/
int resizeConsoleBuffers(COORD size){ //idk bro
    if(consolesize.X > size.X && consolesize.Y > size.Y){
        goto norealloc;
    }
    //realloc(outputbuf, size.Y*size.X*sizeof(CHAR_INFO));
    bitmapoutofdate = 1;
    norealloc:
}

int writeToField(); //write to any field.
int takeInput();

/*
    all structures are passed as pointers

    SCROLLER, CENTER_JUSTIFY, and SELECTABLE do not affect the 
    dimensions of a field.
    a LINEIN type has a height of 1.
    a BOXIN type has any height.

    if MORE_FORMAT is not defined LINEIN and BOXIN start
    at the bottom of the lowest box and are full width.

    providing only LINEIN takes no arguments.                          DONE

    providing only BOXIN requires a short for height.                 DONE

    if MORE_FORMAT alone is defined then the function expects a       DONE
    pointer to a SMALL_RECT and these dimensions are used.

    if MORE_FORMAT and LINEIN are defined the function
    expects a COORD and two unsigned shorts and will try to place a 
    box of height 1, COORD is contained in the box, the box 
    attepts to be as wide as possible without exceeding the first
    short to the left and the second short to the right from the
    position of the COORD. If a short is negative that direction
    can be infinity.
    if MORE_FORMAT and CENTER_FIELD (BOXIN optional) are defined it expects a            DONE
    SMALL_RECT and tries to center the field horizontally 
    keeping the width and height and top position.

    if CENTER_FIELD & BOXIN is defined it takes a COORD as
    width and height
    if CENTER_FIELD and LINEIN is defined it takes only a short width

    if FILL or (FILL and BOXIN) the function expects only
    a COORD and makes the box as big as possible from that COORD

    if FILL and LINEIN is defined it expects a coord and makes
    the box horizontally as big as possible.

    if FILL and MORE_FORMAT the function finds the largest box
    within a SMALL_RECT
*/

#define ifa(b) if(a & (b)) //if is any
#define ifn(b) if(!(a & (b))) //if is none
#define ife(b) if(!(a ^ (b))) //if is exactly
#define ifaom(b) if(!((a & (b)) ^ (b))) //if is all or more
#define ifoa(b) if((!(a & ~(b)) && a)) //if is only any
#define ifnoo(b) if(!((a & (b)) & ((a & (b)) - 1))) //if is not only one
static fbe_t formatBox(_In_ inputfieldstyle_t *A, SMALL_RECT * b, va_list c){ //TODO check for invaid coordinates such as negatives or too large
    union {
        SMALL_RECT *rect;
        struct {
            COORD *pos;
            short left;
            short right;
        } formatline;
        COORD *fill;
        COORD *WxH;
    } d;
    int width = 0;
    int a = *A & LINEIN | BOXIN | MORE_FORMAT | CENTER_FIELD | FILL;
    SMALL_RECT e = {0,0,0,0};
    ifa(LINEIN | BOXIN){ //has LINEIN or BOXIN at least
        ifaom(BOXIN | LINEIN){
                return BAD_INPUT_FIELD_FORMAT;
            } //has either BOXIN or LINEIN
        ifoa(BOXIN | LINEIN){ //has only BOXIN or LINEIN
            e.Top = --bottomrectheight;
            e.Left = 0;
            e.Right = consolesize.X;
            e.Bottom = e.Top;
            ifa(BOXIN){
                e.Bottom -= va_arg(c, short) - 1;
                bottomrectheight = e.Bottom;
            } //has BOXIN or LINEIN and something else too
        }else ifa(LINEIN){
            ifnoo(CENTER_FIELD | MORE_FORMAT | FILL){
                return BAD_INPUT_FIELD_FORMAT; //has only LINEIN and one of the above
            }else ifa(CENTER_FIELD){
                width = va_arg(c, typeof(width));
                goto centerfield;
            }else ifa(MORE_FORMAT){
                //get coord and two shorts

            }else ifa(FILL){//all cases are finished for LINEIN
                //fill that hoe

            }
        }else{ //has BOXIN and some extra stuff
            ifn(MORE_FORMAT | FILL){
                d.WxH = va_arg(c, typeof(d.WxH));
                e.Top = e.Bottom = ++bottomrectheight;
                e.Left = 0;
                e.Right = width = d.WxH->X;
                e.Bottom -= d.WxH->Y;
            } //past this BOXIN has no effect on anything
        }
        goto loadrect;
    }
    // we have no LINEIN
    ifoa(MORE_FORMAT | BOXIN | CENTER_FIELD){
        ifn(MORE_FORMAT){
            return BAD_INPUT_FIELD_FORMAT;
        }
            d.rect = va_arg(c, typeof(d.rect));
            e.Bottom = d.rect->Bottom;
            e.Left = d.rect->Left;
            e.Right = d.rect->Right;
            e.Top = d.rect->Top;
        ifa(CENTER_FIELD){
            width = e.Right - e.Left;
            centerfield:
            e.Left = consolesize.X / 2 - width / 2; //rounds left when parity does not match
            e.Right = e.Left + width;
        }
        goto loadrect;
    }
    ifoa(FILL | BOXIN | MORE_FORMAT){
        ifa(MORE_FORMAT){
            d.rect = va_arg(c, typeof(d.rect));

        }
        //do normal fill
        loadrect:
        b->Top = e.Top; b->Bottom = e.Bottom; b->Left = e.Left; b->Right = e.Right;
        return 0;
    }
    return BAD_INPUT_FIELD_FORMAT;
/* 
    switch(a){ 
        case LINEIN:
            e.Top = ++bottomrectheight;
            e.Left = 0;
            e.Right = consolesize.X;
            e.Bottom = e.Top;
        case BOXIN: //expects 1 short for width
            e.Top = ++bottomrectheight;
            e.Left = 0;
            e.Right = consolesize.X;
            e.Bottom = e.Top - va_arg(c, short) - 1;
            bottomrectheight = e.Bottom;
        case MORE_FORMAT: //expects SMALL_RECT
            moreformat:
            d.rect = va_arg(c, typeof(d.rect));
            e.Bottom = d.rect->Bottom;
            e.Left = d.rect->Left;
            e.Right = d.rect->Right;
            e.Top = d.rect->Top;
        case LINEIN | MORE_FORMAT: //complicated

        case BOXIN | MORE_FORMAT: //expects SMALL_RECT
            goto moreformat;
        case LINEIN | CENTER_FIELD: //expects 1 short for width

        case BOXIN | CENTER_FIELD: //expects COORD
            d.WxH = va_arg(c, typeof(d.WxH));
            e.Top = ++bottomrectheight;
            e.Left = 0;
            e.Right = d.WxH->X;
            e.Bottom = bottomrectheight - d.WxH->Y;
            bottomrectheight = e.Bottom;
            goto centerfield;
        case MORE_FORMAT | CENTER_FIELD: //expects SMALL_RECT
            formatcenterfield:
            d.rect = va_arg(c, typeof(d.rect));
            e.Bottom = d.rect->Bottom;
            e.Left = d.rect->Left;
            e.Right = d.rect->Right;
            e.Top = d.rect->Top;
            centerfield:
            int width = e.Right - e.Left;
            e.Left = consolesize.X / 2 - width / 2; //rounds left when parity does not match
            e.Right = e.Left + width;
            return 0;
        case BOXIN | MORE_FORMAT | CENTER_FIELD: //expects COORD
            goto formatcenterfield;
        case FILL: //expects a COORD for location
            d.fill = va_arg(c, typeof(d.fill));
        case LINEIN | FILL:
        case BOXIN | FILL:
        case MORE_FORMAT | FILL:
// I HATE THIS I HATE THIS I HATE THIS I HATE THIS I HATE THIS I HATE THIS
        case 0:
        case LINEIN | BOXIN:
        case LINEIN | BOXIN | MORE_FORMAT:
        case CENTER_FIELD:
        case LINEIN | BOXIN | CENTER_FIELD:
        case LINEIN | MORE_FORMAT | CENTER_FIELD:
        case LINEIN | BOXIN | MORE_FORMAT | CENTER_FIELD:
        case LINEIN | MORE_FORMAT | CENTER_FIELD | FILL:
        case BOXIN | MORE_FORMAT | CENTER_FIELD | FILL:
        case LINEIN | BOXIN | MORE_FORMAT | CENTER_FIELD | FILL:
        case LINEIN | BOXIN | FILL:
        case LINEIN | MORE_FORMAT | FILL:
        case BOXIN | MORE_FORMAT | FILL:
        case LINEIN | BOXIN | MORE_FORMAT | FILL:
        case CENTER_FIELD | FILL:
        case LINEIN | CENTER_FIELD | FILL:
        case BOXIN | CENTER_FIELD | FILL:
        case LINEIN | BOXIN | CENTER_FIELD | FILL:
        case MORE_FORMAT | CENTER_FIELD | FILL:
            return BAD_INPUT_FIELD_FORMAT;
        default:
            return BAD_INPUT_FIELD_FORMAT;
    } */
}
cife_t createInputField(_In_ inputfieldstyle_t a, _Out_ hinfield_t * b, ...){
    SMALL_RECT box = {0, 0, 0, 0};
    va_list d;
    va_start(d, b);
    formatBox(&a, &box, d);
    va_end(d);
    for(int i = 0; i < cinrects; i++){
        ;
    }
}
void deleteInputField(hinfield_t a){
    //find the feild to delete, a is just a pointer
    //delete it then repack the array
}
void clearAllFields(){
    memset(poutrects, 0, cmaxconsolecharacters);
    memset(pinrects, 0, cmaxconsolecharacters);
}
//make sure the box it's upside down or rightside left.
#define flip(a, b, temp) do{{temp = (a) ^ (b); a ^= temp; b ^= temp;}} while(0)
void correctRect(SMALL_RECT *a, SMALL_RECT *b){
    short temp = 0;
    if(a->Top < a->Bottom)
        flip(a->Top, a->Bottom, temp);
    if(a->Right < a->Left)
        flip(a->Right, a->Left, temp);
    if(b->Top < b->Bottom)
        flip(b->Top, b->Top, temp);
    if(b->Right < b->Left)
        flip(b->Right, b->Left, temp);
}
//switch two variables using a difference map i.e. XOR
//XOR shows the differences, then XOR applies those differences.
//it's imposible for the rectangles to be intersected if the lowest point
//of one box is above the highest point of the other box. 
//The boxes must not have inverted coordinates
int isIntersected(SMALL_RECT *a, SMALL_RECT *b){
    if(a->Top < b->Bottom || a->Left > b->Right || a->Right < b->Left || a->Bottom > b->Top){
        return 0;
    }
    return 1;
}

void editBitRect(SMALL_RECT * b, pixeltype c){ //just draws the four corners of a rect
    int a = consolesize.X * b->Top;
    *(bitmap + b->Left + a) = c;
    *(bitmap + b->Right + a) = c;
    a = consolesize.X * b->Bottom;
    *(bitmap + b->Left + a) = c;
    *(bitmap + b->Right + a) = c;
}
void drawBitRect(recttype_t recttype){ //redraws the entire bitmap, need to do this to remove a rect or resize the buffer
    static COORD size;
    static void * jumponce = &&initialize;
    goto *jumponce;
    initialize:
    size = E(GetLargestConsoleWindowSize(hconout));
    bitmap = malloc(cmaxconsolecharacters * sizeof(int8_t));
    tempmap = calloc(cmaxconsolecharacters, sizeof(int8_t));
    pinklist = calloc(2 * (consolesize.X  + consolesize.Y), sizeof(pinklist));
    jumponce = &&body;
    body:
    memset(bitmap, 0, ctotalconsolecharacters);
    typeof(pinrects) tprects = pinrects;
    for(int q = 0; q < 2; q++){
        int i = 0;
        while(tprects[i] != 0){ //assumes that pinrects and poutrects are a packed array of pointers to rect structures and all invalid pointers are null
            int a = consolesize.X * tprects[i]->coords.Top;
            *(bitmap + tprects[i]->coords.Left + a) = FILLED;
            *(bitmap + tprects[i]->coords.Right + a) = FILLED;
            a = consolesize.X * tprects[i]->coords.Bottom;
            *(bitmap + tprects[i]->coords.Left + a) = FILLED;
            *(bitmap + tprects[i]->coords.Right + a) = FILLED;
            i++;
        }
        tprects = poutrects;
    }
}
enum {UP = 0x1, DOWN = 0x2, LEFT = 0x4, RIGHT = 0x8};
void getBiggestRect(COORD * ina){
    memcpy(tempmap, bitmap, ctotalconsolecharacters);
    COORD pos;
    pos.X = ina->X; pos.Y = ina->Y;
    for(int y = 0; y < consolesize.Y; y++){
        for(int x = 0; x < consolesize.X; x++){
            if(*(tempmap + x + y * consolesize.X) != FILLED){ //if the pixel is not filled then don't do this
                continue;
            }
            void makeline(short xo, short yo){
                register int ty = y;
                register int tx = x;
                while(ty >= 0 && tx >= 0 && ty < consolesize.Y && tx < consolesize.X){
                    if(*(tempmap + tx + ty * consolesize.X) == EMPTY){
                        *(tempmap + tx + ty * consolesize.X) = GREY;
                    } //we use grey to avoid doing unnecessary stuff.
                    tx += xo;
                    ty += yo;
                }
                x = tx;
                y = ty;
            }
            if(y >= pos.Y){     //if pixel is inline then draw both directions
                makeline(0, 1);
            }
            if(y <= pos.Y){
                makeline(0, -1);
            }
            if(x >= pos.X){
                makeline(1, 0);
            }
            if(x <= pos.X){
                makeline(-1, 0);
            }
        }
    }
};
void getWidestLine();

